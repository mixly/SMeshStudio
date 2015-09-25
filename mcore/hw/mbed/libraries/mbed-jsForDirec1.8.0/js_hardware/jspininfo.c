//auto-generated pin info file
#include "jspininfo.h"
#include "PinNames.h"


#ifdef TARGET_NRF51822
#include "pins_mbed_arch_ble_nrf51822.h"

#elif TARGET_LPC11U24
#include "pins_mbed_arch_lpc11u24.h"

#elif TARGET_ARCH_MAX
#include "pins_mbed_arch_max_stm32f407.h"

#elif TARGET_LPC1768 && defined(CONTIKI)  //just tell xbed1768 and mbed1768 apart by the flag of CONTIKI
#include "pins_xbed1.0_lpc1768.h"

#elif TARGET_LPC1768 && !defined(CONTIKI)
#include "pins_mbed_arch_pro_lpc1768.h"

#elif TARGET_NUCLEO_F103RB
#include "pins_mbed_f103rb.h"

#elif TARGET_NUCLEO_F401RE
#include "pins_mbed_f401re.h"

#elif TARGET_NUCLEO_F411RE
#include "pins_mbed_f411re.h"

#elif TARGET_NUCLEO_F334R8
#include "pins_mbed_f334r8.h"

#elif TARGET_NUCLEO_F302R8
#include "pins_mbed_f302r8.h"

#elif TARGET_NUCLEO_F303RE
#include "pins_mbed_f303r8.h"

#elif TARGET_NUCLEO_L152RE
#include "pins_mbed_l152re.h"

#elif TARGET_NUCLEO_L053R8
#include "pins_mbed_l053r8.h"

#elif TARGET_NUCLEO_F030R8
#include "pins_mbed_f030r8.h"

#elif TARGET_NUCLEO_F070RB
#include "pins_mbed_f070rb.h"

#elif TARGET_NUCLEO_F072RB
#include "pins_mbed_f072rb.h"

#elif TARGET_NUCLEO_F091RC
#include "pins_mbed_f091rc.h"

#elif TARGET_KL46Z
#include "pins_mbed_kl46z.h"

#elif TARGET_KL25Z
#include "pins_mbed_kl25z.h"

#elif TARGET_K20D50M
#include "pins_mbed_k20d50m.h"

#elif TARGET_K22F
#include "pins_mbed_k22f.h"

#elif TARGET_K64F
#include "pins_mbed_k64f.h"

#elif TARGET_KL05Z
#include "pins_mbed_kl05z.h"

#elif TARGET_lpc11u68
#include "pins_mbed_lpc11u68.h"

#elif TARGET_LPC1549
#include "pins_mbed_lpc1549.h"

#elif TARGET_LPC824
#include "pins_mbed_lpc824.h"

#elif TARGET_LPC11U24
#include "pins_mbed_lpc11u24.h"
#endif
/*
const int PINNUM = 217;
const pinStrEnum pinsInfo[] = {
    { "A0", A0 },
    { "A1", A1 },
    { "A2", A2 },
    { "A3", A3 },
    { "A4", A4 },
    { "A5", A5 },
    { "D0", D0 },
    { "D1", D1 },
    { "D10", D10 },
    { "D11", D11 },
    { "D12", D12 },
    { "D13", D13 },
    { "D14", D14 },
    { "D15", D15 },
    { "D2", D2 },
    { "D3", D3 },
    { "D4", D4 },
    { "D5", D5 },
    { "D6", D6 },
    { "D7", D7 },
    { "D8", D8 },
    { "D9", D9 },
    { "I2C_SCL", I2C_SCL },
    { "I2C_SDA", I2C_SDA },
    { "LED1", LED1 },
    { "LED2", LED2 },
    { "LED3", LED3 },
    { "LED4", LED4 },
    { "NC", NC },
    { "P0_0", P0_0 },
    { "P0_1", P0_1 },
    { "P0_10", P0_10 },
    { "P0_11", P0_11 },
    { "P0_12", P0_12 },
    { "P0_13", P0_13 },
    { "P0_14", P0_14 },
    { "P0_15", P0_15 },
    { "P0_16", P0_16 },
    { "P0_17", P0_17 },
    { "P0_18", P0_18 },
    { "P0_19", P0_19 },
    { "P0_2", P0_2 },
    { "P0_20", P0_20 },
    { "P0_21", P0_21 },
    { "P0_22", P0_22 },
    { "P0_23", P0_23 },
    { "P0_24", P0_24 },
    { "P0_25", P0_25 },
    { "P0_26", P0_26 },
    { "P0_27", P0_27 },
    { "P0_28", P0_28 },
    { "P0_29", P0_29 },
    { "P0_3", P0_3 },
    { "P0_30", P0_30 },
    { "P0_31", P0_31 },
    { "P0_4", P0_4 },
    { "P0_5", P0_5 },
    { "P0_6", P0_6 },
    { "P0_7", P0_7 },
    { "P0_8", P0_8 },
    { "P0_9", P0_9 },
    { "P1_0", P1_0 },
    { "P1_1", P1_1 },
    { "P1_10", P1_10 },
    { "P1_11", P1_11 },
    { "P1_12", P1_12 },
    { "P1_13", P1_13 },
    { "P1_14", P1_14 },
    { "P1_15", P1_15 },
    { "P1_16", P1_16 },
    { "P1_17", P1_17 },
    { "P1_18", P1_18 },
    { "P1_19", P1_19 },
    { "P1_2", P1_2 },
    { "P1_20", P1_20 },
    { "P1_21", P1_21 },
    { "P1_22", P1_22 },
    { "P1_23", P1_23 },
    { "P1_24", P1_24 },
    { "P1_25", P1_25 },
    { "P1_26", P1_26 },
    { "P1_27", P1_27 },
    { "P1_28", P1_28 },
    { "P1_29", P1_29 },
    { "P1_3", P1_3 },
    { "P1_30", P1_30 },
    { "P1_31", P1_31 },
    { "P1_4", P1_4 },
    { "P1_5", P1_5 },
    { "P1_6", P1_6 },
    { "P1_7", P1_7 },
    { "P1_8", P1_8 },
    { "P1_9", P1_9 },
    { "P2_0", P2_0 },
    { "P2_1", P2_1 },
    { "P2_10", P2_10 },
    { "P2_11", P2_11 },
    { "P2_12", P2_12 },
    { "P2_13", P2_13 },
    { "P2_14", P2_14 },
    { "P2_15", P2_15 },
    { "P2_16", P2_16 },
    { "P2_17", P2_17 },
    { "P2_18", P2_18 },
    { "P2_19", P2_19 },
    { "P2_2", P2_2 },
    { "P2_20", P2_20 },
    { "P2_21", P2_21 },
    { "P2_22", P2_22 },
    { "P2_23", P2_23 },
    { "P2_24", P2_24 },
    { "P2_25", P2_25 },
    { "P2_26", P2_26 },
    { "P2_27", P2_27 },
    { "P2_28", P2_28 },
    { "P2_29", P2_29 },
    { "P2_3", P2_3 },
    { "P2_30", P2_30 },
    { "P2_31", P2_31 },
    { "P2_4", P2_4 },
    { "P2_5", P2_5 },
    { "P2_6", P2_6 },
    { "P2_7", P2_7 },
    { "P2_8", P2_8 },
    { "P2_9", P2_9 },
    { "P3_0", P3_0 },
    { "P3_1", P3_1 },
    { "P3_10", P3_10 },
    { "P3_11", P3_11 },
    { "P3_12", P3_12 },
    { "P3_13", P3_13 },
    { "P3_14", P3_14 },
    { "P3_15", P3_15 },
    { "P3_16", P3_16 },
    { "P3_17", P3_17 },
    { "P3_18", P3_18 },
    { "P3_19", P3_19 },
    { "P3_2", P3_2 },
    { "P3_20", P3_20 },
    { "P3_21", P3_21 },
    { "P3_22", P3_22 },
    { "P3_23", P3_23 },
    { "P3_24", P3_24 },
    { "P3_25", P3_25 },
    { "P3_26", P3_26 },
    { "P3_27", P3_27 },
    { "P3_28", P3_28 },
    { "P3_29", P3_29 },
    { "P3_3", P3_3 },
    { "P3_30", P3_30 },
    { "P3_31", P3_31 },
    { "P3_4", P3_4 },
    { "P3_5", P3_5 },
    { "P3_6", P3_6 },
    { "P3_7", P3_7 },
    { "P3_8", P3_8 },
    { "P3_9", P3_9 },
    { "P4_0", P4_0 },
    { "P4_1", P4_1 },
    { "P4_10", P4_10 },
    { "P4_11", P4_11 },
    { "P4_12", P4_12 },
    { "P4_13", P4_13 },
    { "P4_14", P4_14 },
    { "P4_15", P4_15 },
    { "P4_16", P4_16 },
    { "P4_17", P4_17 },
    { "P4_18", P4_18 },
    { "P4_19", P4_19 },
    { "P4_2", P4_2 },
    { "P4_20", P4_20 },
    { "P4_21", P4_21 },
    { "P4_22", P4_22 },
    { "P4_23", P4_23 },
    { "P4_24", P4_24 },
    { "P4_25", P4_25 },
    { "P4_26", P4_26 },
    { "P4_27", P4_27 },
    { "P4_28", P4_28 },
    { "P4_29", P4_29 },
    { "P4_3", P4_3 },
    { "P4_30", P4_30 },
    { "P4_31", P4_31 },
    { "P4_4", P4_4 },
    { "P4_5", P4_5 },
    { "P4_6", P4_6 },
    { "P4_7", P4_7 },
    { "P4_8", P4_8 },
    { "P4_9", P4_9 },
    { "USBRX", USBRX },
    { "USBTX", USBTX },
    { "p10", p10 },
    { "p11", p11 },
    { "p12", p12 },
    { "p13", p13 },
    { "p14", p14 },
    { "p15", p15 },
    { "p16", p16 },
    { "p17", p17 },
    { "p18", p18 },
    { "p19", p19 },
    { "p20", p20 },
    { "p21", p21 },
    { "p22", p22 },
    { "p23", p23 },
    { "p24", p24 },
    { "p25", p25 },
    { "p26", p26 },
    { "p27", p27 },
    { "p28", p28 },
    { "p29", p29 },
    { "p30", p30 },
    { "p5", p5 },
    { "p6", p6 },
    { "p7", p7 },
    { "p8", p8 },
    { "p9", p9 },
};
*/
