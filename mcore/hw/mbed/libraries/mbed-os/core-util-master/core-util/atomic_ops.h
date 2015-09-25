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

#ifndef __MBED_UTIL_ATOMIC_OPS_H__
#define __MBED_UTIL_ATOMIC_OPS_H__

#include <stdint.h>
#include "CriticalSectionLock.h"

namespace mbed {
namespace util {

/**
 * Atomic compare and set. It compares the contents of a memory location to a
 * given value and, only if they are the same, modifies the contents of that
 * memory location to a given new value. This is done as a single atomic
 * operation. The atomicity guarantees that the new value is calculated based on
 * up-to-date information; if the value had been updated by another thread in
 * the meantime, the write would fail due to a mismatched expectedCurrentValue.
 *
 * Refer to https://en.wikipedia.org/wiki/Compare-and-set [which may redirect
 * you to the article on compare-and swap].
 *
 * @param  ptr                  The target memory location.
 * @param[in,out] expectedCurrentValue A pointer to some location holding the
 *                              expected current value of the data being set atomically.
 *                              The computed 'desiredValue' should be a function of this current value.
 *                              @Note: This is an in-out parameter. In the
 *                              failure case of atomic_cas (where the
 *                              destination isn't set), the pointee of expectedCurrentValue is
 *                              updated with the current value.
 * @param[in] desiredValue      The new value computed based on '*expectedCurrentValue'.
 *
 * @return                      true if the memory location was atomically
 *                              updated with the desired value (after verifying
 *                              that it contained the expectedCurrentValue),
 *                              false otherwise. In the failure case,
 *                              exepctedCurrentValue is updated with the new
 *                              value of the target memory location.
 *
 * pseudocode:
 * function cas(p : pointer to int, old : pointer to int, new : int) returns bool {
 *     if *p != *old {
 *         *old = *p
 *         return false
 *     }
 *     *p = new
 *     return true
 * }
 *
 * @Note: In the failure case (where the destination isn't set), the value
 * pointed to by expectedCurrentValue is still updated with the current value.
 * This property helps writing concise code for the following incr:
 *
 * function incr(p : pointer to int, a : int) returns int {
 *     done = false
 *     *value = *p // This fetch operation need not be atomic.
 *     while not done {
 *         done = atomic_cas(p, &value, value + a) // *value gets updated automatically until success
 *     }
 *     return value + a
 * }
 *
 * The following is the generic implementation. It can be specialized using
 * load-store-exclusive primitives for architectures offering appropriate
 * instructions. The generic implementation applies for architectures lacking
 * load-store-exclusive primitives, or when matching against target types larger
 * than the word-size.
 */
template<typename T>
bool atomic_cas(T *ptr, T *expectedCurrentValue, T desiredValue)
{
    bool rc = true;

    CriticalSectionLock lock;

    T currentValue = *ptr;
    if (currentValue == *expectedCurrentValue) {
        *ptr = desiredValue;
    } else {
        *expectedCurrentValue = currentValue;
        rc = false;
    }

    return rc;
}

/**
 * Atomic increment.
 * @param  valuePtr Target memory location being incremented.
 * @param  delta    The amount being incremented.
 * @return          The new incremented value.
 */
template<typename T> T atomic_incr(T *valuePtr, T delta)
{
    T oldValue = *valuePtr;
    while (true) {
        const T newValue = oldValue + delta;
        if (atomic_cas(valuePtr, &oldValue, newValue)) {
            return newValue;
        }
    }
}

/**
 * Atomic decrement.
 * @param  valuePtr Target memory location being decremented.
 * @param  delta    The amount being decremented.
 * @return          The new decremented value.
 */
template<typename T> T atomic_decr(T *valuePtr, T delta)
{
    T oldValue = *valuePtr;
    while (true) {
        const T newValue = oldValue - delta;
        if (atomic_cas(valuePtr, &oldValue, newValue)) {
            return newValue;
        }
    }
}

} // namespace util
} // namespace mbed

#endif // #ifndef __MBED_UTIL_ATOMIC_OPS_H__
