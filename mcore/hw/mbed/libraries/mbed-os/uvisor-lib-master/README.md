# uvisor-lib

## Overview ##
uvisor-lib is a yotta module that provides a secure environment for modular applications on mbed, and the APIs that interact with it.

It consists of two main components:

1. the uVisor binaries, which set up a protected environment where isolated compartments are granted finely tuned permissions.
2. APIs to interact with the uVisor.

If you are building your application with yotta on a supported platform, then you will already be running on top of the uVisor. At this stage of development, though, it is disabled by default.

To learn more about the uVisor refer to its [documentation](https://github.com/ARMmbed/uvisor-private).

### Word of caution
This version of the uVisor is an early beta with an **incomplete implementation of the security features** of the final product. Future versions of uVisor will add these functions - starting with this year's ARM TechCon that will come with a security challenge firmware.

Some of the open uVisor issues in progress are listed here:
- [list of open issues](https://github.com/ARMmbed/uvisor/issues)
- [FIXMEs](https://github.com/ARMmbed/uvisor/search?utf8=%E2%9C%93&q=FIXME)

### Supported platforms:
- [Freescale FRDM-K64F](http://developer.mbed.org/platforms/FRDM-K64F/) ([GCC ARM Embedded toolchain](https://launchpad.net/gcc-arm-embedded)).
- [STMicorelectronics STM32F429I-DISCO](http://www.st.com/web/catalog/tools/FM116/SC959/SS1532/PF259090) ([GCC ARM Embedded toolchain](https://launchpad.net/gcc-arm-embedded)).

## Introduction

The security model of the uVisor is based on secure boxes. A *secure box* is a code domain for which finely grained permissions are granted, protecting data and functions from the rest of the system.

By default, each yotta module that is included in the build process of an mbed application ends up in a unique untrusted box, referred to as the *main box*. All code and data in the main box share the same level of permissions.

If you need to perform secure data access or function execution you may want to configure a secure box. Secure boxes all run in unprivileged mode but are granted protection and isolation from other boxes.

Access to protected resources is only possible when the context of the box they belong to is active. By default, the active context is the one of the main box. A special mechanism, called the *secure gateway*, allows you to switch context to your own secure box. You cannot switch context to a different box from yours.

Protecting a resource against other boxes means having exclusive access over the memory section the resource is mapped to. This can be in SRAM (for variables, objects), in a secure storage (for protected keys) or can be mapped to a peripheral. Access Control Lists (ACLs) are used to describe the list of resources you want to protect.

The following sections will describe all these features in greater detail.

## How to Configure a Secure Box

To configure a secure box:

- Add uvisor-lib as a dependency in `module.json`:
```
  "dependencies": {
    "uvisor-lib": "ARMmbed/uvisor-lib#~0.7.0",
    ...
  },
```

- Include its header file in your source code:
```c
#include "uvisor-lib/uvisor-lib.h"
```

- [Protect your resources](#protect-your-data), using Access Control Lists (ACLs) and the private *box context*.

- Use the [secure gateway](#secure-function-call) to ensure critical functions are executed from the context of the secure box.

- Use the [uvisor-lib APIs](#low-level-apis) to access restricted features (interrupts, system calls).

### Protect Your Resources

Resources can be both memory assets (SRAM, secure storage) and memory-mapped peripherals.

If you want to secure data/variables in SRAM, you can use what we call the *box context*, a private memory section that can be optionally coupled to the secure box. The box context generally provides 1KB of memory, and can be populated with a struct that encapsulates all your private resources:

```C
typedef struct {
    uint8_t secret[SECRET_SIZE];
    bool initialized;
    State_t current_state
} BoxContext;
```

`uvisor_ctx` is the pointer, maintained by uVisor, that allows to access the box context:

```
void main(void)
{
    ...
    /* only possible from the context of the secure box */
    /* access from other boxes causes a memory exception */
    if(!uvisor_ctx->initialised)
        initialise_secret(uvisor_ctx->secret);
    ...
}
```

If you want to gain exclusive access to peripherals, you need to specify an ACLs for each of them. ACLs can be used to specify the pointer, the size and the permission level associated with the resource they represent, providing a high level of granularity.

```C
static const UvisorBoxAclItem g_box_acl[] = {
    {PORTB,  sizeof(*PORTB),  UVISOR_TACLDEF_PERIPH},
    {RTC,    sizeof(*RTC),    UVISOR_TACLDEF_PERIPH},
    {LPTMR0, sizeof(*LPTMR0), UVISOR_TACLDEF_PERIPH},
    }
```

Now that all your reseources are set-up for protection, you only need to group them together to configure the secure box, also specifying the name of the box and the required stack size.

```C
#define BOX_STACK_SIZE 0x100

UVISOR_BOX_CONFIG(my_box_name, g_box_acl, BOX_STACK_SIZE, BoxContext);
```

Please note that configuring a secure box in a yotta module does not mean that the whole module itself is a secure box. A secure box is a domain in which several restrictions apply, so choose carefully what to include in it. All peripherals, constants, variables and object instantiations that are not explicitly included in the secure box through the mechanism just described are automatically included in the context of the main box, unprotected from the rest of the code.

### Secure Function Call

By default code execution and access to data and peripherals happen in the context of the main box. You are generally free to execute code designed for a secure box, but if that code accesses a protected resource a memory exception will be thrown and the uVisor will halt the system. Secure boxes then need to expose special entry points to other boxes so that a context switch can be triggered. We call these entry points *secure gateways*.

To create a secure gateway, we suggest you use a wrapper function that performs the secure function call:

```C
/* the box is configured here */
...

/* the actual function */
UVISOR_EXTERN uint32_t __secure_sum(uint32_t op1, uint32_t op2)
{
    return op1 + op2;
}

/* the gateway to the secure function */
uint32_t secure_sum(uint32_t op1, uint32_t op2)
{
    return secure_gateway(my_box_name, __secure_sum, op1, op2);
}
```

This means that you generally need to expose only the unsecure entry points to your secure box, which trigger a gateway to your protected environment. API users do not need to know the details of this implementation, and you are only responsible for the level of security of your own module.

Secure gateways may be called with a maximum of four 32bit arguments, and may
return a single 32bit value.

### Low Level APIs

Access to some system registers or features is only possible in privileged mode, so low level APIs are provided to interact with the uVisor to perform those accesses. Currently, only interrupt management is supported.

#### Interrupt Management

A box can register for interrupts and gain exclusive access to the corresponding IRQn slot, so that it is the only one in charge of modifying it. To register an interrupt routine, you have to set a handler for it and enable the corresponding IRQn slot, as you would do with regular NVIC registers:

```C

#define MY_HANDLER_IRQn 42

void my_handler(void)
{
    ...
}

int some_function(void)
{
    ...

    vIRQ_SetVector(MY_HANDLER_IRQn, (uint32_t) &my_handler);
    vIRQ_EnableIRQ(MY_HANDLER_IRQn);

    ...
}
```

After registration of an interrupt, its IRQn is bounded to the box that registered it.

Please note:

1. Currently, interrupt ownership is exclusive, meaning that multiple boxes cannot register for the same interrupt.
2. Registration works on a first-come-first-served basis.
3. Execution of an interrupt handler is always handled in unprivileged mode in the context of the box that registered for it.

## Even More Documentation?

For detailed documentation of all available macros and APIs, please refer to the [APIs documentation](DOCUMENTATION.md).

## Examples

We designed an example, called [uvisor-helloworld](https://github.com/ARMmbed/uvisor-helloworld), that shows how to configure and use a secure box.

## Debugging

Failures and faults captured by the uVisor will trigger a system halt. For some specific faults the uVisor will enable an LED blinking pattern which is specific to the error encountered. Please refer to the [APIs documentation](DOCUMENTATION.md) for a complete list of the available blinking patterns.

Further debugging messages are silenced by default. If you want to enable them, you need to build a new version of uvisor-lib starting from the uVisor source code, with the debug option enabled. All messages are always printed through the semihosting interface, hence a debugger must be connected to the board to observe them. Make sure you have the latest [uVisor source code](https://github.com/ARMmbed/uvisor) and proceede as follows:

```bash
# assuming this is your code tree:
# ~/code/
#   |
#   `- my_application/
#   |
#   `- uvisor/
#      |
#      `- release/
#      |
#      `- k64f/uvisor/
#      |
#      `- stm32f4/uvisor/

# cd to uvisor (using k64f in this example)
cd ~/code/uvisor/k64f/uvisor

# build a debug release
make OPT= clean release

# link the newly created release to yotta
cd ~/code/uvisor/release
yotta link

# link your project to the newly created release
cd ~/code/my_application
yotta link uvisor-lib
```

## Limitations

For this release, uvisor-lib comes with the following restrictions:

- It is disabled by default. Use `UVISOR_SET_MODE(2);` in the top level yotta executable to enable it.
