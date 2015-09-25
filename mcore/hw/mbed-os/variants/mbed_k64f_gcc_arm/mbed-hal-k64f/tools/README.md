`uvisor_replace.py`
=========================

tl;dr
-----
A script replaces every macro containing direct memory accesses (`*addr = val`) with a memory access macro (`ADDRESS_WRITE(addr, val)`). In this way memory accesses can be redefined for special requirements.

Background
-----------
In uVisor we use ACLs to tell if a resource can be granted access to. Peripheral ACLs specify permission levels for whole peripheral slots, but some registers in those slots may still enforce higher restrictions. For example, RTC registers require superuser access unless a certain RTC-specific bit is set.

Accessing these registers might incur in a bus fault. uVisor is able to recover from bus faults, perform the r/w operation if an ACL covers it, and continue execution regularly, but this requires the memory access to be well defined, that is, parse-able.

The Problem
-----------
All functions in the Freescale K64F HAL ultimately use macros to perform r/w operations on peripheral registers. These macros have usually a structure like the following:
```C
#define BW_PERIPH_REG_BIT(x, n, v) (BITBAND_ACCESS32(x, n) = (v))
```
where `v` is the value to write to the bit `BIT` in the register `REG` of the peripheral `PERIPH` at the address `x`. `BITBAND_ACCESS(x, n)` is a macro that de-references the aliased bitband address obtained from `x` and `n`.

With this style we are not able to centrally re-define different behaviours for memory accesses. Every single macro implements a memory access explicitly (`*addr = val`), which cannot be replaced by a uVisor counterpart.

The Solution
------------
The solution is to replace every direct access to memory in the header files with a macro that implements memory access indirectly, so that if uVisor is used it turns into a [dedicated function](https://github.com/ARMmbed/uvisor/blob/change_bitband_support/core/mbed/uvisor-lib/secure_access.h), otherwise the straightforward pointer de-referencing is used.

This is done using a script, `uvisor_replace.py`, that scans header files using a total of 5 regular expressions. These capture all the combinations of possible macros, classified as follows:
- regular read/write (8, 16, 32 bits)
- bitbanded read/write (8, 16, 32 bits)
- union bitfield read

Why Do I Care?
--------------
*If* you use Freescale K64F HAL functions you are fine - this all happens under the hood. If, instead, you sometimes access registers or their bitfields singularly with a direct memory access then you have 3 options:

1. Use HAL functions; if you are already using some of them, this is anyway suggested for style consistency;

2. Use macros underlying the HAL functions: `B{W,R}_PERIPH_REG_BIT(x, n, v)` (Freescale specific)

3. Use memory access macros: `ADDRESS_{READ,WRITE}{32,16,8}(addr(, val))`; the `addr` field can also be replaced by a bitband translation with `BITBAND_ADDRESS{32,16,8}(addr, bit)` (uVisor specific)
