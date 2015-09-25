## compiler-polyfill

A [yotta](http://github.com/ARMmbed/yotta) module to smooth over some of the
differences between compilers.

### API

#### Attributes
##### `__packed`
Pack a structure, preventing any padding from being added between fields.

```C
#include "compiler-polyfill/attributes.h"

struct foo{
   char x;
   int y;
} __packed;
```

##### `__align(N)`
Declare a type to be aligned on an N-byte boundary.

```C
#include "compiler-polyfill/attributes.h"

struct bar{
   int x;
} __align(16);
```

##### `__unused`
Declare a function argument to be unused, suppressing compiler warnings.

```C
#include "compiler-polyfill/attributes.h"

void foo(int __unused arg){
    
}

```


