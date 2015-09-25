# uvisor-lib API Documentation

Here you can find detailed documentation for:

1. [Configuration macros](#configuration-macros), to configure a secure box and protect data and peripherals.
2. [Secure function call](#secure-function-call), to execute code in the context of a secure box.
3. [Low level APIs](#low-level-apis), to access uVisor functions that are not available to unprivileged code (interrupts, restricted system registers).
4. [Type definitions](#type-definitions).
5. [Error patterns](#error-patterns).

## Configuration Macros

```C
UVISOR_SECURE_DATA var_type var_name = var_value
```

<table>
  <tr>
    <td>Description</td>
    <td colspan="2">Initialised variables are made private for the module</td>
  </tr>
  <tr>
    <td>Type</td>
    <td colspan="3">C/C++ pre-processor macro (attribute)</td>
  </tr>
</table>

Example:
```C
#include "uvisor-lib/uvisor-lib.h"

/* create private global initialised variable */
UVISOR_SECURE_DATA char g_state[] = INITIAL_STATE;
```

**Note:**

1. Protection of single objects is discouraged; consider using the private box context instead;

2. This macro is only available on the Freescale FRDM-K64F board.

---

```C
UVISOR_SECURE_CONST var_type var_name = var_value
```

<table>
  <tr>
    <td>Description</td>
    <td colspan="2">Data in Flash is made private for the module</td>
  </tr>
  <tr>
    <td>Type</td>
    <td colspan="3">C/C++ pre-processor macro (attribute)</td>
  </tr>
</table>

Example:
```C
#include "uvisor-lib/uvisor-lib.h"

/* create private global constant */
UVISOR_SECURE_CONST char g_password[] = "password";
```

**Note:**

1. Protection of single objects is discouraged; consider using the private box context instead;

2. This macro is only available on the Freescale FRDM-K64F board.

---

```C
UVISOR_SECURE_BSS var_type var_name;
```

<table>
  <tr>
    <td>Description</td>
    <td colspan="2">Data in the BSS (SRAM) is made private for the module</td>
  </tr>
  <tr>
    <td>Type</td>
    <td colspan="3">C/C++ pre-processor macro (attribute)</td>
  </tr>
</table>

Example:
```C
#include "uvisor-lib/uvisor-lib.h"

/* create private global variable */
UVISOR_SECURE_BSS int g_counter;
```

**Note:**

1. Protection of single objects is discouraged; consider using the private box context instead;

---

```C
UVISOR_BOX_CONFIG(box_name
                  const UvBoxAclItem *module_acl_list,
                  uint32_t module_stack_size,
                  [struct __your_context])
```

<table>
  <tr>
    <td>Description</td>
    <td colspan="3">Secure box configuration</td>
  </tr>
  <tr>
    <td>Type</td>
    <td colspan="2">C/C++ pre-processor macro (pseudo-function)</td>
  </tr>
  <tr>
    <td rowspan="4">Parameters</td>
    <td><pre>box_name<code></td>
    <td>Secure box name</td>
  </tr>
  <tr>
    <td><pre>const UvBoxAclItem *module_acl_list<code></td>
    <td>List of ACLs for the module</td>
  </tr>
  <tr>
    <td><pre>uint32_t module_stack_size<code></td>
    <td>Required stack size for the secure box</td>
  </tr>
  <tr>
    <td><pre>struct __your_context<code></td>
    <td>[optional] Type definition of the struct hosting the box context data</td>
  </tr>
</table>

Example:
```C
#include "uvisor-lib/uvisor-lib.h"

/* required stack size */
#define BOX_STACK_SIZE 0x100

/* define box context */
typedef struct {
    uint8_t secret[SECRET_SIZE];
    bool initialized;
    State_t current_state
} BoxContext;

/* create ACLs for the module */
static const UvBoxAclItem g_box_acl[] = {
    {PORTB,  sizeof(*PORTB),  UVISOR_TACLDEF_PERIPH},
    {RTC,    sizeof(*RTC),    UVISOR_TACLDEF_PERIPH},
    {LPTMR0, sizeof(*LPTMR0), UVISOR_TACLDEF_PERIPH},
};

/* configure secure box compartment */
UVISOR_BOX_CONFIG(my_box_name, g_box_acl, BOX_STACK_SIZE, BoxContext);
```

---

```C
UVISOR_SET_MODE(int uvisor_mode);
```

<table>
  <tr>
    <td>Description</td>
    <td colspan="2">[temporary] Set mode for the uVisor</td>
  </tr>
  <tr>
    <td>Type</td>
    <td colspan="3">C/C++ pre-processor macro (object declaration)</td>
  </tr>
  <tr>
    <td rowspan="3">Parameters</td>
    <td rowspan="3"><pre>int uvisor_mode<code></td>
    <td>0 = disabled [default]</td>
  </tr>
    <tr>
    <td>1 = permissive [n.a.] [currently same as enabled]</td>
  </tr>
  <tr>
    <td>2 = enabled</td>
  </tr>
</table>

Example:
```C
#include "uvisor-lib/uvisor-lib.h"

/* set uvisor mode */
UVISOR_SET_MODE(UVISOR_ENABLED);
```

**Note:**

1. This macro is only needed temporarily (uVisor disabled by default) and will be removed in the future.

2. This macro must be used only once in the top level yotta executable.

---

```C
UVISOR_SET_MODE_ACL(int uvisor_mode, const UvBoxAcl *main_box_acl_list);
```

<table>
  <tr>
    <td>Description</td>
    <td colspan="2">[temporary] Set mode for the uVisor and provide background
                    ACLs for the main box
    </td>
  </tr>
  <tr>
    <td>Type</td>
    <td colspan="3">C/C++ pre-processor macro (object declaration)</td>
  </tr>
  <tr>
    <td rowspan="4">Parameters</td>
    <td rowspan="3"><pre>int uvisor_mode<code></td>
    <td>0 = disabled [default]</td>
  </tr>
    <tr>
    <td>1 = permissive [n.a.] [currently same as enabled]</td>
  </tr>
  <tr>
    <td>2 = enabled</td>
  </tr>
  <tr>
    <td><pre>const UvBoxAclItem *main_box_acl_list<code></td>
    <td>List of ACLs for the main box (background ACLs)</td>
  </tr>
</table>

Example:
```C
#include "uvisor-lib/uvisor-lib.h"

/* create background ACLs for the main box */
static const UvBoxAclItem g_background_acl[] = {
    {UART0,       sizeof(*UART0), UVISOR_TACL_PERIPHERAL},
    {UART1,       sizeof(*UART1), UVISOR_TACL_PERIPHERAL},
    {PIT,         sizeof(*PIT),   UVISOR_TACL_PERIPHERAL},
};

/* set uvisor mode */
UVISOR_SET_MODE_ACL(UVISOR_ENABLED, g_background_acl);
```

**Note:**

1. This macro is only needed temporarily (uVisor disabled by default) and will be removed in the future.

2. This macro must be used only once in the top level yotta executable.

## Secure Function Call

```C
uint32_t secure_gateway(box_name, uint32_t target_fn, ...)
```

<table>
  <tr>
    <td>Description</td>
    <td colspan="2">Call a function using the context of a secure box</td>
  </tr>
  <tr>
    <td>Type</td>
    <td colspan="3">C/C++ pre-processor macro (pseudo-function)</td>
  </tr>
  <tr>
    <td rowspan="6">Parameters</td>
    <td><pre>box_name<code></td>
    <td>Secure box name</td>
  </tr>
  <tr>
    <td><pre>uint32_t target_fn<code></td>
    <td>Function to execute in the secure context</td>
  </tr>
  <tr>
    <td><pre>...<code></td>
    <td>A maximum of four 32bit arguments</td>
  </tr>
</table>

Example:
```C
/* the box is configured here */
...

/* the actual function */
extern "C" uint32_t __secure_sum(uint32_t op1, uint32_t op2)
{
    return op1 + op2;op3 + op4;
}

/* the gateway to the secure function */
uint32_t secure_sum(uint32_t op1, uint32_t op2)
{
    return secure_gateway(my_box_name, __secure_sum, op1, op2)
}
```

## Low Level APIs

Currently the following low level operations are permitted:

1. Interrupt management.

### Interrupt Management

```C
void vIRQ_SetVectorX(uint32_t irqn, uint32_t vector, uint32_t flag)
```

<table>
  <tr>
    <td>Description</td>
    <td colspan="2">Register an ISR to the currently active box, with the ability to specify permissions associated with the IRQn slot</td>
  <tr>
    <td rowspan="3">Parameters</td>
    <td><pre>uint32_t irqn<code></td>
    <td>IRQn</td>
  </tr>
  <tr>
    <td><pre>uint32_t vector<code></td>
    <td>Interrupt handler; if 0 the IRQn slot is de-registered for the current
        box</td>
  </tr>
  <tr>
    <td><pre>uint32_t flag<code></td>
    <td>Permission flag (currently not implemented)</td>
  </tr>
</table>

---

```C
void vIRQ_SetVector(uint32_t irqn, uint32_t vector, uint32_t flag)
```

<table>
  <tr>
    <td>Description</td>
    <td colspan="2">Register an ISR to the currently active box</td>
  <tr>
    <td rowspan="3">Parameters</td>
    <td><pre>uint32_t irqn<code></td>
    <td>IRQn</td>
  </tr>
  <tr>
    <td><pre>uint32_t vector<code></td>
    <td>Interrupt handler; if 0 the IRQn slot is de-registered for the current
        box</td>
  </tr>
  <tr>
    <td><pre>uint32_t flag<code></td>
    <td>Permission flag (currently not implemented)</td>
  </tr>
</table>

**Note:**

1. Currently `vIRQ_SetVectorX` and `vIRQ_SetVector` are mapped to the same function because the argument `flag` is not yet used to determine the permissions associated with the IRQn slot

2. `vIRQ_SetVector` is especially kept for backward compatibility with `NVIC_SetVector`

---

```C
uint32_t vIRQ_GetVector(uint32_t irqn)
```

<table>
  <tr>
    <td>Description</td>
    <td colspan="2">Get the ISR registered for IRQn</td>
  </tr>
  <tr>
    <td>Return value</td>
    <td colspan="2">The ISR registered for IRQn, if present; 0 otherwise</td>
  </tr>
  <tr>
    <td rowspan="1">Parameters</td>
    <td><pre>uint32_t irqn<code></td>
    <td>IRQn</td>
  </tr>
</table>

---

```C
void vIRQ_EnableIRQ(uint32_t irqn)
```

<table>
  <tr>
    <td>Description</td>
    <td colspan="2">Enable IRQn for the currently active box</td>
  </tr>
  <tr>
    <td rowspan="1">Parameters</td>
    <td><pre>uint32_t irqn<code></td>
    <td>IRQn</td>
  </tr>
</table>

---

```C
void vIRQ_DisableIRQ(uint32_t irqn)
```

<table>
  <tr>
    <td>Description</td>
    <td colspan="2">Disable IRQn for the currently active box</td>
  </tr>
  <tr>
    <td rowspan="1">Parameters</td>
    <td><pre>uint32_t irqn<code></td>
    <td>IRQn</td>
  </tr>
</table>

---

```C
void vIRQ_ClearPendingIRQ(uint32_t irqn)
```

<table>
  <tr>
    <td>Description</td>
    <td colspan="2">Clear pending status of IRQn</td>
  </tr>
  <tr>
    <td rowspan="2">Parameters</td>
    <td><pre>uint32_t irqn<code></td>
    <td>IRQn</td>
  </tr>
</table>

---

```C
void vIRQ_SetPendingIRQ(uint32_t irqn)
```

<table>
  <tr>
    <td>Description</td>
    <td colspan="2">Set pending status of IRQn</td>
  </tr>
  <tr>
    <td rowspan="2">Parameters</td>
    <td><pre>uint32_t irqn<code></td>
    <td>IRQn</td>
  </tr>
</table>

---

```C
uint32_t vIRQ_GetPendingIRQ(uint32_t irqn)
```

<table>
  <tr>
    <td>Description</td>
    <td colspan="2">Get pending status of IRQn</td>
  </tr>
  <tr>
    <td rowspan="1">Parameters</td>
    <td><pre>uint32_t irqn<code></td>
    <td>IRQn</td>
  </tr>
</table>

---

```C
void vIRQ_SetPriority(uint32_t irqn, uint32_t priority)
```

<table>
  <tr>
    <td>Description</td>
    <td colspan="2">Set priority level of IRQn</td>
  </tr>
  <tr>
    <td rowspan="2">Parameters</td>
    <td><pre>uint32_t irqn<code></td>
    <td>IRQn</td>
  </tr>
  <tr>
    <td><pre>uint32_t priority<code></td>
    <td>Priority level (minimum: 1)</td>
  </tr>
</table>

---

```C
uint32_t vIRQ_GetPriority(uint32_t irqn)
```

<table>
  <tr>
    <td>Description</td>
    <td colspan="2">Get priority level of IRQn</td>
  </tr>
  <tr>
    <td>Return value</td>
    <td colspan="2">The priority level of IRQn, if available; 0 otherwise</td>
  </tr>
  <tr>
    <td rowspan="1">Parameters</td>
    <td><pre>uint32_t irqn<code></td>
    <td>IRQn</td>
  </tr>
</table>

## Type Definitions

```C
typedef uint32_t UvisroBoxAcl;    /* permssions mask */
```

---

```C
typedef struct
{
    const volatile void* start; /* start address of protected area */
    uint32_t length;            /* size of protected area */
    UvisorBoxAcl acl;           /* permissions for the protected area */
} UvisorBoxAclItem;
```
## Error Patterns

| Error reason          | RED LED blinks |
|-----------------------|----------------|
| `PERMISSION_DENIED`   | 1              |
| `SANITY_CHECK_FAILED` | 2              |
| `NOT_IMPLEMENTED`     | 3              |
| `NOT_ALLOWED`         | 4              |
| `FAULT_MEMMANAGE`     | 5              |
| `FAULT_BUS`           | 6              |
| `FAULT_USAGE`         | 7              |
| `FAULT_HARD`          | 8              |
| `FAULT_DEBUG`         | 9              |
