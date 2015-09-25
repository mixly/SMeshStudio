/*
 * PackageLicenseDeclared: Apache-2.0
 * Copyright (c) 2015 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __MBED_UTIL_CRITICAL_SECTION_LOCK_H__
#define __MBED_UTIL_CRITICAL_SECTION_LOCK_H__

#include <stdint.h>
#ifndef TARGET_LIKE_POSIX
#include "cmsis-core/core_generic.h"
#ifdef TARGET_NORDIC
#include "nrf_soc.h"
#endif /* #ifdef TARGET_NORDIC */
#else  /* #ifdef TARGET_LIKE_POSIX */
#include <assert.h>
#include <unistd.h>
#include <signal.h>
#endif /* #ifdef TARGET_LIKE_POSIX */

namespace mbed {
namespace util {

/** RAII object for disabling, then restoring, interrupt state
  * Usage:
  * @code
  *
  * void f() {
  *     // some code here
  *     {
  *         CriticalSectionLock lock;
  *         // Code in this block will run with interrupts disabled
  *     }
  *     // interrupts will be restored to their previous state
  * }
  * @endcode
  */
class CriticalSectionLock {
public:
    CriticalSectionLock() {
#ifdef TARGET_NORDIC
        sd_nvic_critical_region_enter(&_state);
#elif defined(TARGET_LIKE_POSIX)
        if (++IRQNestingDepth > 1) {
            return;
        }

        int rc;
        sigset_t fullSet;
        rc = sigfillset(&fullSet);
        assert(rc == 0);
        rc = sigprocmask(SIG_BLOCK, &fullSet, &oldSigSet);
        assert(rc == 0);
#else
        _state = __get_PRIMASK();
        __disable_irq();
#endif
    }

    ~CriticalSectionLock() {
#ifdef TARGET_NORDIC
        sd_nvic_critical_region_exit(_state);
#elif defined(TARGET_LIKE_POSIX)
        assert(IRQNestingDepth > 0);
        if (--IRQNestingDepth == 0) {
            int rc = sigprocmask(SIG_SETMASK, &oldSigSet, NULL);
            assert(rc == 0);
        }
#else
        __set_PRIMASK(_state);
#endif
    }

private:
#ifdef TARGET_NORDIC
    uint8_t  _state;
#elif defined(TARGET_LIKE_POSIX)
    unsigned IRQNestingDepth;
    sigset_t oldSigSet;
#else
    uint32_t _state;
#endif
};

} // namespace util
} // namespace mbed

#endif // #ifndef __MBED_UTIL_CRITICAL_SECTION_LOCK_H__

