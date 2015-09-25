/* mbed Microcontroller Library
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
#include "cmsis.h"
#include "fsl_clock_manager.h"
#include "fsl_lptmr_hal.h"
#include "fsl_osc_hal.h"
#include "fsl_rtc_hal.h"
#include "device.h"

#if DEVICE_LOWPOWERTIMER

#include "lp_ticker_api.h"
#include "sleep_api.h"
#include "objects.h"
#include "uvisor-lib/uvisor-lib.h"

/*
The Low power  timer for KSDK devices uses 2 timers. The reason: LPTMR contains only 16-bit timer.
In addition, RTC contains only seconds alarm interrupt.
 
When adding a new event, if the new time fits within LPTMR 16bit counter, its interrupt is set only,
otherwise RTC seconds alarm is used, and the leftover is for LPTMR timer.
*/

// We use a part of the RTC seconds register to provide a "virtual" 32 bit timer
// The lower RTC_TIMER_BITS part of the RTC seconds register are going to be part of the virtual timer

#define RTC_TIMER_BITS    (17)
#define RTC_TIMER_MASK    ((1UL << RTC_TIMER_BITS) - 1)
#define RTC_OVERFLOW_BITS (32 - RTC_TIMER_BITS)
#define RTC_OVERFLOW_MASK (((1UL << RTC_OVERFLOW_BITS) - 1) << RTC_TIMER_BITS)

#define RTC_PRESCALER_MAX_VALUE 0x7FFF
#define LPTMR_TIMER_MAX_VALUE   0xFFFF

static void lptmr_isr(void);
static void rct_isr(void);
static void lp_ticker_schedule_lptmr(void);

static uint32_t lp_ticker_inited = 0;
static uint32_t lp_lptmr_schedule_ticks = 0;
// Store compare-match as we use 2 timers, plus LPTMR is relative to 0
static uint32_t lp_compare_value = 0xFFFFFFFFu;
static volatile uint8_t lp_ticker_lptmr_scheduled_flag = 0;

static void lptmr_isr(void)
{
    // TODO 0xc0170: looks like this gets fired even before set_interrupt
    LPTMR_HAL_ClearIntFlag(LPTMR0_BASE);
    LPTMR_HAL_Disable(LPTMR0_BASE);
}

static void rct_isr(void)
{
    RTC_HAL_SetAlarmIntCmd(RTC_BASE, false);
    RTC_HAL_SetAlarmReg(RTC_BASE, 0);
    if (lp_lptmr_schedule_ticks) {
        lp_ticker_lptmr_scheduled_flag = 1;
        lp_ticker_schedule_lptmr();
    }
}

static void lp_ticker_schedule_lptmr(void)
{
    // schedule LPTMR, restart counter and set compare
    LPTMR_HAL_Disable(LPTMR0_BASE);
    LPTMR_HAL_SetCompareValue(LPTMR0_BASE, lp_lptmr_schedule_ticks);
    LPTMR_HAL_Enable(LPTMR0_BASE);
    lp_lptmr_schedule_ticks = 0;
}

void lp_ticker_init(void) {
    if (lp_ticker_inited) {
        return;
    }
    lp_ticker_inited = 1;

    // RTC might be configured already, don't reset it
    RTC_HAL_SetSupervisorAccessCmd(RTC_BASE, true);
    if (!RTC_HAL_IsCounterEnabled(RTC_BASE)) {
        // select RTC for OSC32KSEL
        CLOCK_HAL_SetSource(SIM_BASE, kClockOsc32kSel, 2);
        // configure RTC
        SIM_HAL_EnableRtcClock(SIM_BASE, 0U);
        RTC_HAL_Init(RTC_BASE);
        RTC_HAL_Enable(RTC_BASE);
        for (volatile uint32_t wait_count = 0; wait_count < 1000000; wait_count++);
        RTC_HAL_SetAlarmIntCmd(RTC_BASE, false);
        RTC_HAL_SetSecsIntCmd(RTC_BASE, false);
        RTC_HAL_SetAlarmReg(RTC_BASE, 0);
        RTC_HAL_EnableCounter(RTC_BASE, true);
    }
    vIRQ_ClearPendingIRQ(RTC_IRQn);
    vIRQ_SetVector(RTC_IRQn, (uint32_t)rct_isr);
    vIRQ_EnableIRQ(RTC_IRQn);

    // configure LPTMR
    CLOCK_SYS_EnableLptimerClock(0);
    LPTMR0_CSR = 0x00;
    LPTMR0_PSR = 0x00;
    LPTMR0_CMR = 0x00;
    LPTMR_HAL_SetTimerModeMode(LPTMR0_BASE, kLptmrTimerModeTimeCounter);
    LPTMR0_PSR |= LPTMR_PSR_PCS(0x2) | LPTMR_PSR_PBYP_MASK;
    LPTMR_HAL_SetIntCmd(LPTMR0_BASE, 1);
    LPTMR_HAL_SetFreeRunningCmd(LPTMR0_BASE, 0);
    IRQn_Type timer_irq[] = LPTMR_IRQS;
    vIRQ_SetVector(timer_irq[0], (uint32_t)lptmr_isr);
    vIRQ_EnableIRQ(timer_irq[0]);
}

uint32_t lp_ticker_read() {
    if (!lp_ticker_inited) {
        lp_ticker_init();
    }
    uint32_t temp_pre = RTC_HAL_GetPrescaler(RTC_BASE);
    // RTC sec might be GT 4095, therfore use modulo the max allowance
    uint32_t temp_sec = RTC_HAL_GetSecsReg(RTC_BASE) % (1 << RTC_TIMER_BITS);
    while ((temp_pre != RTC_HAL_GetPrescaler(RTC_BASE)) || (temp_sec != (RTC_HAL_GetSecsReg(RTC_BASE) & RTC_TIMER_MASK))) {
        temp_pre = RTC_HAL_GetPrescaler(RTC_BASE);
        temp_sec = RTC_HAL_GetSecsReg(RTC_BASE) & RTC_TIMER_MASK;
    }
    return (temp_sec << 15 | temp_pre);
}

uint32_t lp_ticker_get_overflows_counter(void)
{
    // TODO: is there a race condition below if the RTC value changes right after we read it?
    uint32_t temp = RTC_HAL_GetSecsReg(RTC_BASE) & RTC_OVERFLOW_MASK;
    while (temp != (RTC_HAL_GetSecsReg(RTC_BASE) & RTC_OVERFLOW_MASK)){
        temp = RTC_HAL_GetSecsReg(RTC_BASE) & RTC_OVERFLOW_MASK;
    }
    return temp >> RTC_TIMER_BITS;
}

uint32_t lp_ticker_get_compare_match(void)
{
    return lp_compare_value;
}

void lp_ticker_set_interrupt(uint32_t now, uint32_t time) {
    // On K64F, the compare value can be only set when the LPTMR is disabled
    // However, disabling the LPTMR will automatically clear the LPTMR counter
    // So we need to compensate for the time that already passed
    lp_compare_value = time;
    uint32_t ticks = time > now ? time - now : (uint32_t)((uint64_t)time + 0xFFFFFFFFu - now);
    lp_lptmr_schedule_ticks = 0;
    lp_ticker_lptmr_scheduled_flag = 0;

    RTC_HAL_EnableCounter(RTC_BASE, false);
    if (ticks > LPTMR_TIMER_MAX_VALUE) {
        ticks -= RTC_PRESCALER_MAX_VALUE + 1 - RTC_HAL_GetPrescaler(RTC_BASE);
        uint32_t seconds = RTC_HAL_GetSecsReg(RTC_BASE);
        while (ticks > LPTMR_TIMER_MAX_VALUE) {
            ticks -= RTC_PRESCALER_MAX_VALUE + 1;
            seconds++;
        }
        RTC_HAL_SetAlarmReg(RTC_BASE, seconds);
        RTC_HAL_SetAlarmIntCmd(RTC_BASE, true);
        // the lp timer will be triggered once RTC alarm is set
        lp_lptmr_schedule_ticks = ticks;
    } else {
        // restart counter, set compare
        LPTMR_HAL_Disable(LPTMR0_BASE);
        LPTMR_HAL_SetCompareValue(LPTMR0_BASE, ticks);
        LPTMR_HAL_Enable(LPTMR0_BASE);
    }
    RTC_HAL_EnableCounter(RTC_BASE, true);
}

void lp_ticker_sleep_until(uint32_t now, uint32_t time)
{
    lp_ticker_set_interrupt(now, time);
    sleep_t sleep_obj;
    mbed_enter_sleep(&sleep_obj);
    // if LPTMR is running and we scheduled it, we know that RTC was fired while
    // we were sleeping
    if (LPTMR_HAL_IsEnabled(LPTMR0_BASE) && lp_ticker_lptmr_scheduled_flag) {
        __WFI();
    }
    mbed_exit_sleep(&sleep_obj);
}

#endif
