/*
 Copyright (c) 2014 Arduino.  All right reserved.

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 See the GNU Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

 //

#include "wiring_analog.h"
#include "wiring_digital.h"

#ifdef __cplusplus
extern "C" {
#endif

static int _readResolution = 10;
static int _writeResolution = 10;
static int ADC_RESOLUTION = 12;
static int DAC_RESOLUTION = 10;

void analogReadResolution(int res) {
	_readResolution = res;
	while( ADC->STATUS.bit.SYNCBUSY == 1 )
  {
    // Waiting for synchroinization
  }
	if(res == 8) ADC->CTRLB.bit.RESSEL= ADC_CTRLB_RESSEL_8BIT_Val; 
	else if(res == 10) ADC->CTRLB.bit.RESSEL= ADC_CTRLB_RESSEL_10BIT_Val;   
	else ADC->CTRLB.bit.RESSEL= ADC_CTRLB_RESSEL_12BIT_Val; 
}

void analogWriteResolution(int res) {
	_writeResolution = res;
}

static inline uint32_t mapResolution(uint32_t value, uint32_t from, uint32_t to) {
	if (from == to)
		return value;
	if (from > to)
		return value >> (from-to);
	else
		return value << (to-from);
}

void analogReference( eAnalogReference ulMode )
{

  // ATTENTION : On this board the default is not 5volts or 3.3volts BUT 1.65 volt

   
   
   ADC->CTRLA.bit.ENABLE = 0; // Enable ADC
  while( ADC->STATUS.bit.SYNCBUSY == 1 )
  {
    // Waiting for synchroinization
  }
  switch(ulMode)
  {
    case AR_DEFAULT:
      //default:
      
	  ADC->INPUTCTRL.bit.GAIN = ADC_INPUTCTRL_GAIN_DIV2_Val;
	  ADC->REFCTRL.bit.REFSEL = ADC_REFCTRL_REFSEL_INTVCC1_Val;
	  break;
    case AR_INTERNAL:
	  ADC->INPUTCTRL.bit.GAIN = ADC_INPUTCTRL_GAIN_1X_Val; 
      ADC->REFCTRL.bit.REFSEL = ADC_REFCTRL_REFSEL_INT1V_Val;
      break;

    case AR_EXTERNAL:
	  ADC->INPUTCTRL.bit.GAIN = ADC_INPUTCTRL_GAIN_1X_Val; 
      ADC->REFCTRL.bit.REFSEL = ADC_REFCTRL_REFSEL_AREFA_Val;
      break;
  }ADC->CTRLA.bit.ENABLE = 1; // Enable ADC
  while( ADC->STATUS.bit.SYNCBUSY == 1 )
  {
    // Waiting for synchroinization
  }
}

uint32_t analogRead( uint32_t ulPin )
{
  uint32_t valueRead = 0;
  pinPeripheral(ulPin, g_APinDescription[ulPin].ulPinType);
  if ( ulPin == 24 )  // Only 1 DAC on A0 (PA02)
  {
    DAC->CTRLA.bit.ENABLE = 0; //disable DAC on A0
  }
  
  ADC->INPUTCTRL.bit.MUXPOS = g_APinDescription[ulPin].ulADCChannelNumber;

  // Start conversion
  ADC->SWTRIG.bit.START = 1;

  while( ADC->INTFLAG.bit.RESRDY == 0 || ADC->STATUS.bit.SYNCBUSY == 1 )
  {
    // Waiting for a complete conversion and complete synchronization
  }

  // Store the value
  valueRead = ADC->RESULT.reg;

  // Clear the Data Ready flag
  ADC->INTFLAG.bit.RESRDY = 1;

  // Flush the ADC for further conversions
  // ADC->SWTRIG.bit.FLUSH = 1;

  while( ADC->STATUS.bit.SYNCBUSY == 1 || ADC->SWTRIG.bit.FLUSH == 1 )
  {
    // Waiting for synchronization
  }
  
  //valueRead = mapResolution(valueRead, ADC_RESOLUTION, _readResolution);
  return valueRead;
  
}


// Right now, PWM output only works on the pins with
// hardware support.  These are defined in the appropriate
// pins_*.c file.  For the rest of the pins, we default
// to digital output.
void analogWrite( uint32_t ulPin, uint32_t ulValue )
{
  uint32_t attr = g_APinDescription[ulPin].ulPinAttribute ;
//   uint32_t pwm_name = g_APinDescription[ulPin].ulTCChannel ;
  uint8_t isTC = 0 ;
  uint8_t Channelx ;
  Tc* TCx ;
  Tcc* TCCx ;

  if ( (attr & PIN_ATTR_ANALOG) == PIN_ATTR_ANALOG )
  {
    if ( ulPin == 24 )  // Only 1 DAC on A0 (PA02)
    {
      	ulValue = mapResolution(ulValue, _writeResolution, DAC_RESOLUTION);
    	DAC->DATA.reg = ulValue & 0x3FF;  // Dac on 10 bits.
		DAC->CTRLA.bit.ENABLE = 1; // DAC Enabled
		syncDAC();
      	return;
    }
	

  }

  if ( (attr & PIN_ATTR_PWM) == PIN_ATTR_PWM )
  {
    if ( (g_APinDescription[ulPin].ulPinType == PIO_TIMER) || g_APinDescription[ulPin].ulPinType == PIO_TIMER_ALT )
    {
      pinPeripheral( ulPin, g_APinDescription[ulPin].ulPinType ) ;
    }

    switch ( g_APinDescription[ulPin].ulPWMChannel )
    {
      case PWM3_CH0 :
                TCx = TC3 ;
                Channelx = 0 ;
                isTC = 1 ;
            break;

      case  PWM3_CH1:
                TCx = TC3 ;
                Channelx = 1;
                isTC = 1;
            break;

      case  PWM0_CH0 :
                TCCx = TCC0;
                Channelx = 0;
            break;

      case  PWM0_CH1 :
                TCCx = TCC0;
                Channelx = 1;
            break;

      case  PWM0_CH4 :
                TCCx = TCC0;
                Channelx = 0;
            break;

      case  PWM0_CH5 :
                TCCx = TCC0;
                Channelx = 1;
            break;

      case  PWM0_CH6 :
                TCCx = TCC0;
                Channelx = 2;
            break;

      case  PWM0_CH7 :
                TCCx = TCC0;
                Channelx = 3;
            break;

      case  PWM1_CH0 :
                TCCx = TCC1;
                Channelx = 0;
            break;

      case  PWM1_CH1 :
                TCCx = TCC1;
                Channelx = 1;
            break;

      case  PWM2_CH0 :
                TCCx = TCC2;
                Channelx = 0;
            break;

      case  PWM2_CH1 :
                TCCx = TCC2;
                Channelx = 1;
            break;
    }


    // Enable clocks according to TCCx instance to use
    switch ( GetTCNumber( g_APinDescription[ulPin].ulPWMChannel ) )
    {
      case 0: // TCC0
                //Enable GCLK for TCC0 (timer counter input clock)
                GCLK->CLKCTRL.reg = (uint16_t) (GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_ID( GCM_TCC0_TCC1 )) ;
            break;

      case 1: // TCC1
                //Enable GCLK for TCC1 (timer counter input clock)
                GCLK->CLKCTRL.reg = (uint16_t) (GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_ID( GCM_TCC0_TCC1 )) ;
            break;

      case 2: // TCC2
                //Enable GCLK for TCC2 (timer counter input clock)
                GCLK->CLKCTRL.reg = (uint16_t) (GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_ID( GCM_TCC2_TC3 )) ;
            break;

      case 3: // TC3
                //Enable GCLK for TC3 (timer counter input clock)
                GCLK->CLKCTRL.reg = (uint16_t) (GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_ID( GCM_TCC2_TC3 ));
            break;

      case 4: // TC4
                //Enable GCLK for TC4 (timer counter input clock)
                GCLK->CLKCTRL.reg = (uint16_t) (GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_ID( GCM_TC4_TC5 ));
            break;

      case 5: // TC5
                //Enable GCLK for TC5 (timer counter input clock)
                GCLK->CLKCTRL.reg = (uint16_t) (GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_ID( GCM_TC4_TC5 )) ;
            break;
    }

    // Set PORT
    if ( isTC )
    {
      // -- Configure TC
      //DISABLE TCx
      TCx->COUNT8.CTRLA.reg &=~(TC_CTRLA_ENABLE);
      //Set Timer counter Mode to 8 bits
      TCx->COUNT8.CTRLA.reg |= TC_CTRLA_MODE_COUNT8;
      //Set TCx as normal PWM
      TCx->COUNT8.CTRLA.reg |= TC_CTRLA_WAVEGEN_NPWM;
      //Set TCx in waveform mode Normal PWM
      TCx->COUNT8.CC[Channelx].reg = (uint8_t) ulValue;
      //Set PER to maximum counter value (resolution : 0xFF)
      TCx->COUNT8.PER.reg = 0xFF;
      // Enable TCx
      TCx->COUNT8.CTRLA.reg |= TC_CTRLA_ENABLE;
    }
    else
    {
      // -- Configure TCC

      //DISABLE TCCx
      TCCx->CTRLA.reg &=~(TCC_CTRLA_ENABLE);
      //Set TCx as normal PWM
      TCCx->WAVE.reg |= TCC_WAVE_WAVEGEN_NPWM;
      //Set TCx in waveform mode Normal PWM
      TCCx->CC[Channelx].reg = (uint32_t)ulValue;
      //Set PER to maximum counter value (resolution : 0xFF)
      TCCx->PER.reg = 0xFF;
      //ENABLE TCCx
      TCCx->CTRLA.reg |= TCC_CTRLA_ENABLE ;
    }

    return ;
  }

  // -- Defaults to digital write
  pinMode( ulPin, OUTPUT ) ;

  if ( ulValue < 128 )
  {
    digitalWrite( ulPin, LOW ) ;
  }
  else
  {
    digitalWrite( ulPin, HIGH ) ;
  }
}

#ifdef __cplusplus
}
#endif
