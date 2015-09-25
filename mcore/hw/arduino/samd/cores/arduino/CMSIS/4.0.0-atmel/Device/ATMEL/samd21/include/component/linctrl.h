/**
 * \file
 *
 * \brief Component description for LINCTRL
 *
 * Copyright (c) 2014 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */

#ifndef _SAMD21_LINCTRL_COMPONENT_
#define _SAMD21_LINCTRL_COMPONENT_

/* ========================================================================== */
/**  SOFTWARE API DEFINITION FOR LINCTRL */
/* ========================================================================== */
/** \addtogroup SAMD21_LINCTRL LIN control module */
/*@{*/

#define LINCTRL_U2255
#define REV_LINCTRL                 0x100

/* -------- LINCTRL_CTRL : (LINCTRL Offset: 0x0) (R/W 16) control register -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t WOCFG:2;          /*!< bit:  0.. 1  wkout observation select           */
    uint16_t :14;              /*!< bit:  2..15  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} LINCTRL_CTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define LINCTRL_CTRL_OFFSET         0x0          /**< \brief (LINCTRL_CTRL offset) control register */
#define LINCTRL_CTRL_RESETVALUE     0x0000       /**< \brief (LINCTRL_CTRL reset_value) control register */

#define LINCTRL_CTRL_WOCFG_Pos      0            /**< \brief (LINCTRL_CTRL) wkout observation select */
#define LINCTRL_CTRL_WOCFG_Msk      (0x3u << LINCTRL_CTRL_WOCFG_Pos)
#define LINCTRL_CTRL_WOCFG(value)   ((LINCTRL_CTRL_WOCFG_Msk & ((value) << LINCTRL_CTRL_WOCFG_Pos)))
#define LINCTRL_CTRL_MASK           0x0003u      /**< \brief (LINCTRL_CTRL) MASK Register */

/** \brief LINCTRL hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
  __IO LINCTRL_CTRL_Type         CTRL;        /**< \brief Offset: 0x0 (R/W 16) control register */
} Linctrl;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/*@}*/

#endif /* _SAMD21_LINCTRL_COMPONENT_ */
