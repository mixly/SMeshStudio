/* SX127x driver
 * Copyright (c) 2013 Semtech
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
  
#ifndef SX127x_H
#define SX127x_H

#include "mbed.h"

#define XTAL_FREQ   32000000

#define FREQ_STEP_MHZ     61.03515625e-6
#define FREQ_STEP_KHZ     61.03515625e-3
#define FREQ_STEP_HZ      61.03515625

#define MHZ_TO_FRF(m)   (m / FREQ_STEP_MHZ)

/******************************************************************************/
/*!
 * SX127x Internal registers Address
 */
#define REG_FIFO                                    0x00
#define REG_OPMODE                                  0x01
#define REG_FRFMSB                                  0x06
#define REG_FRFMID                                  0x07
#define REG_FRFLSB                                  0x08
// Tx settings
#define REG_PACONFIG                                0x09
#define REG_PARAMP                                  0x0A
#define REG_OCP                                     0x0B 
// Rx settings
#define REG_LNA                                     0x0C

// LoRa registers
#define REG_LR_FIFOADDRPTR                          0x0d
#define REG_LR_FIFOTXBASEADDR                       0x0e
#define REG_LR_FIFORXBASEADDR                       0x0f
#define REG_LR_FIFORXCURRENTADDR /*REG_LR_RXDATAADDR*/  0x10
#define REG_LR_IRQFLAGSMASK                         0x11
#define REG_LR_IRQFLAGS                             0x12
#define REG_LR_RXNBBYTES                            0x13
#define REG_LR_RXHEADERCNTVALUE_MSB                 0x14
#define REG_LR_RXHEADERCNTVALUE_LSB                 0x15
#define REG_LR_RXPACKETCNTVALUE_MSB                 0x16
#define REG_LR_RXPACKETCNTVALUE_LSB                 0x17
#define REG_LR_MODEMSTAT                            0x18
#define REG_LR_PKTSNRVALUE                          0x19
#define REG_LR_PKTRSSIVALUE                         0x1a
#define REG_LR_RSSIVALUE                            0x1b
#define REG_LR_HOPCHANNEL                           0x1c
#define REG_LR_MODEMCONFIG                          0x1d
#define REG_LR_MODEMCONFIG2                         0x1e
#define REG_LR_SYMBTIMEOUTLSB                       0x1f
#define REG_LR_PREAMBLEMSB                          0x20
#define REG_LR_PREAMBLELSB                          0x21
#define REG_LR_PAYLOADLENGTH                        0x22 // and RX length for implicit
#define REG_LR_RX_MAX_PAYLOADLENGTH                 0x23 // length limit for explicit mode
#define REG_LR_HOPPERIOD                            0x24
#define REG_LR_RXBYTEADDR /*REG_LR_RXDATAADDR*/     0x25
#define REG_LR_MODEMCONFIG3                         0x26    // sx1272 REG_LR_PPM_CORRECTION_MSB
#define REG_LR_PPM_CORRECTION_LSB                   0x27
#define REG_LR_TEST28                               0x28  // est_freq_error
#define REG_LR_TEST29                               0x29    // est_freq_error
#define REG_LR_TEST2A                               0x2a    // est_freq_error
#define REG_LR_TEST2B                               0x2b    // 
#define REG_LR_WIDEBAND_RSSI                        0x2c 
#define REG_LR_AGCH_TH                              0x2d    // agc_upper_th
#define REG_LR_AGCL_TH                              0x2e    // agc_lower_th
#define REG_LR_IFFRQH                               0x2f    // if_freq(12:8)
#define REG_LR_IFFRQL                               0x30    // if_freq(7:0)
#define REG_LR_TEST31                               0x31    // if_freq_auto, ...
#define REG_LR_TEST32                               0x32    // 
#define REG_LR_TEST33                               0x33
#define REG_LR_DETECTION_THRESHOLD                  0x37
/***** registers above 0x40 are same as FSK/OOK page */

#define REG_DIOMAPPING1                             0x40
#define REG_DIOMAPPING2                             0x41
#define REG_VERSION                                 0x42


/******************************************************************************/

typedef enum {
    RF_OPMODE_SLEEP = 0,
    RF_OPMODE_STANDBY,          // 1
    RF_OPMODE_SYNTHESIZER_TX,   // 2
    RF_OPMODE_TRANSMITTER,      // 3
    RF_OPMODE_SYNTHESIZER_RX,   // 4
    RF_OPMODE_RECEIVER,         // 5
    RF_OPMODE_RECEIVER_SINGLE,  // 6
    RF_OPMODE_CAD               // 7
} chip_mode_e;

typedef enum {
    SX_NONE = 0,
    SX1272,
    SX1276
} type_e;

typedef enum {
    SERVICE_NONE = 0,
    SERVICE_ERROR,
    //! request to call read_fifo()
    SERVICE_READ_FIFO,
    //! notification to application of transmit complete
    SERVICE_TX_DONE
} service_action_e;

/******************************************************************************/

typedef union {
    struct {    // sx1272 register 0x01
        uint8_t Mode                : 3;    // 0,1,2
        uint8_t ModulationShaping   : 2;    // 3,4  FSK/OOK
        uint8_t ModulationType      : 2;    // 5,6  FSK/OOK
        uint8_t LongRangeMode       : 1;    // 7    change this bit only in sleep mode
    } bits;
    struct {    // sx1276 register 0x01
        uint8_t Mode                : 3;    // 0,1,2
        uint8_t LowFrequencyModeOn  : 1;    // 3    1=access to LF test registers (0=HF regs)
        uint8_t reserved            : 1;    // 4
        uint8_t ModulationType      : 2;    // 5,6  FSK/OOK
        uint8_t LongRangeMode       : 1;    // 7    change this bit only in sleep mode
    } sx1276FSKbits;
    struct {    // sx1276 register 0x01
        uint8_t Mode                : 3;    // 0,1,2
        uint8_t LowFrequencyModeOn  : 1;    // 3    1=access to LF test registers (0=HF regs)
        uint8_t reserved            : 2;    // 4,5
        uint8_t AccessSharedReg     : 1;    // 6    1=FSK registers while in LoRa mode
        uint8_t LongRangeMode       : 1;    // 7    change this bit only in sleep mode
    } sx1276LORAbits;
    uint8_t octet;
} RegOpMode_t;

typedef union {
    struct {    // sx12xx register 0x09
        uint8_t OutputPower : 4;    // 0,1,2,3
        uint8_t MaxPower    : 3;    // 4,5,6
        uint8_t PaSelect    : 1;    // 7        1=PA_BOOST
    } bits;
    uint8_t octet;
} RegPaConfig_t;

typedef union {
    struct {    // sx12xx register 0x0b
        uint8_t OcpTrim : 5;    // 0,1,2,3,4
        uint8_t OcpOn   : 1;    // 5
        uint8_t unused  : 2;    // 6,7
    } bits;
    uint8_t octet;
} RegOcp_t;

typedef union {
    struct {    // sx12xx register 0x0c
        uint8_t LnaBoostHF           : 2;    // 0,1
        uint8_t reserved             : 1;    // 2
        uint8_t LnaBoostLF           : 2;    // 3,4
        uint8_t LnaGain              : 3;    // 5,6,7
    } bits;
    uint8_t octet;
} RegLna_t; // RXFE

typedef union {
    struct {    // sx127x register 0x12
        uint8_t CadDetected         : 1;    // 0
        uint8_t FhssChangeChannel   : 1;    // 1
        uint8_t CadDone             : 1;    // 2
        uint8_t TxDone              : 1;    // 3
        uint8_t ValidHeader         : 1;    // 4
        uint8_t PayloadCrcError     : 1;    // 5
        uint8_t RxDone              : 1;    // 6
        uint8_t RxTimeout           : 1;    // 7
    } bits;
    uint8_t octet;
} RegIrqFlags_t;

typedef union {
    struct {    // sx127x register 0x18
        uint8_t detect          : 1;    // 0
        uint8_t sync            : 1;    // 1
        uint8_t rx_ongoing      : 1;    // 2
        uint8_t header_valid    : 1;    // 3
        uint8_t clear           : 1;    // 4
        uint8_t RxCodingRate    : 3;    // 5,6,7
    } bits;
    uint8_t octet;
} RegModemStatus_t;

typedef union {
    struct {    // sx127x register 0x1c
        uint8_t FhssPresentChannel  : 6;    // 0,1,2,3,4,5
        uint8_t RxPayloadCrcOn      : 1;    // 6
        uint8_t PllTimeout          : 1;    // 7
    } bits;
    uint8_t octet;
} RegHopChannel_t;

typedef union {
    struct {    // sx1276 register 0x1d
        uint8_t ImplicitHeaderModeOn    : 1;    // 0
        uint8_t CodingRate              : 3;    // 1,2,3
        uint8_t Bw                      : 4;    // 4,5,6,7
    } sx1276bits;
    struct {    // sx1272 register 0x1d
        uint8_t LowDataRateOptimize     : 1;    // 0  ppm_offset: number of cyclic shifts possible to encode to symbol
        uint8_t RxPayloadCrcOn          : 1;    // 1
        uint8_t ImplicitHeaderModeOn    : 1;    // 2
        uint8_t CodingRate              : 3;    // 3,4,5
        uint8_t Bw                      : 2;    // 6,7
    } sx1272bits;
    uint8_t octet;
} RegModemConfig_t;

typedef union {
    struct {    // sx1276 register 0x1e
        uint8_t SymbTimeoutMsb          : 2;    // 0,1
        uint8_t RxPayloadCrcOn          : 1;    // 2
        uint8_t TxContinuousMode        : 1;    // 3
        uint8_t SpreadingFactor         : 4;    // 4,5,6,7
    } sx1276bits;
    struct {    // sx1272 register 0x1e
        uint8_t SymbTimeoutMsb          : 2;    // 0,1
        uint8_t AgcAutoOn               : 1;    // 2
        uint8_t TxContinuousMode        : 1;    // 3
        uint8_t SpreadingFactor         : 4;    // 4,5,6,7
    } sx1272bits;
    uint8_t octet;
} RegModemConfig2_t;

typedef union {
    struct {    // sx127x register 0x26
        uint8_t reserved    : 2;    // 0,1
        uint8_t AgcAutoOn   : 1;    // 2
        uint8_t LowDataRateOptimize  : 1;    // 3   ppm_offset
        uint8_t unused      : 4;    // 4,5,6,7 
    } sx1276bits;
    uint8_t octet;
    uint8_t sx1272_ppm_correction_msb;
} RegModemConfig3_t;

typedef union {
    struct {    // sx127x register 0x31
        uint8_t detect_trig_same_peaks_nb  : 3;    // 0,1,2
        uint8_t disable_pll_timeout        : 1;    // 3
        uint8_t tracking_intergral         : 2;    // 4,5
        uint8_t frame_sync_gain            : 1;    // 6
        uint8_t if_freq_auto               : 1;    // 7
    } bits;
    uint8_t octet;
} RegTest31_t;


/*********************** ...end lora ****************************/

typedef union {
    struct {    // sx12xx register 0x40
        uint8_t Dio3Mapping     : 2;    // 0,1
        uint8_t Dio2Mapping     : 2;    // 2,3
        uint8_t Dio1Mapping     : 2;    // 4,5
        uint8_t Dio0Mapping     : 2;    // 6,7 
    } bits;
    uint8_t octet;
} RegDioMapping1_t;

typedef union {
    struct {    // sx12xx register 0x41
        uint8_t MapPreambleDetect : 1;    // 0      //DIO4 assign: 1b=preambleDet 0b=rssiThresh
        uint8_t io_mode           : 3;    // 1,2,3  //0=normal,1=debug,2=fpga,3=pll_tx,4=pll_rx,5=analog
        uint8_t Dio5Mapping       : 2;    // 4,5
        uint8_t Dio4Mapping       : 2;    // 6,7 
    } bits;
    uint8_t octet;
} RegDioMapping2_t;

/***************************************************/


/** FSK/LoRa radio transceiver.
 * see http://en.wikipedia.org/wiki/Chirp_spread_spectrum
 */

class SX127x {
    public:
            /** Create SX127x instance
         * @param mosi SPI master-out pin
         * @param miso SPI master-in pin
         * @param sclk SPI clock pin
         * @param cs SPI chip-select pin
         * @param rst radio hardware reset pin
         * @param dio_0 interrupt pin from radio
         * @param fem_ctx rx-tx switch for HF bands (800/900)
         * @param fem_cps rx-tx switch for LF bands (vhf/433)
         */

        SX127x(PinName mosi, PinName miso, PinName sclk, PinName cs, PinName rst, PinName dio_0, PinName dio_1, PinName fem_ctx, PinName fem_cps);
        
        ~SX127x();
        
        /** set center operating frequency
         * @param MHz operating frequency in MHz
         */
        void set_frf_MHz( float MHz );
        
        /** get center operating frequency
         * @returns operating frequency in MHz
         */
        float get_frf_MHz(void);

        void set_opmode(chip_mode_e mode);
        
        /** reset radio using pin
         */
        void hw_reset(void);
        /** initialise SX1232 class to radio
         * @note this is called from class instantiation, but must also be manually called after hardware reset
         */
        void init(void);
        void get_type(void); // identify radio chip
        
        /** read register from radio
         * @param addr register address
         * @returns the value read from the register
         */
        uint8_t read_reg(uint8_t addr);
        uint16_t read_u16(uint8_t addr);
        
        /** write register to radio
         * @param addr register address
         * @param data byte to write
         */
        void write_reg(uint8_t addr, uint8_t data);
        void write_reg_u24(uint8_t addr, uint32_t data);
        
        /** transmit a packet
         * @param len size of packet
         * @note Limited to (lora fifo size 256)
         */
        void lora_start_tx(uint8_t len);
        
        /** start receive mode
         * @note the variable service_action needs to be monitored to indicate read_fifo() needs to be called to pull packet from FIFO.
         */
        void lora_start_rx(void);
        
        /** Called by main program when indicated by service_action variable, to pull recevied packet from radio FIFO.
         * @returns count of bytes received
         * @note received packet in rx_buf[]
         */
        void lora_read_fifo(uint8_t len);
        
        void lora_write_fifo(uint8_t len);
        
        service_action_e service(void); // (SLIH) ISR bottom half 
        
        uint8_t getCodingRate(bool from_rx);    // false:transmitted, true:last recevied packet
        void setCodingRate(uint8_t cr);
        
        bool getHeaderMode(void);
        void setHeaderMode(bool hm);
        
        uint8_t getBw(void);
        void setBw(uint8_t bw);
        
        uint8_t getSf(void);
        void setSf(uint8_t sf);        
        
        bool getRxPayloadCrcOn(void);
        void setRxPayloadCrcOn(bool);
        
        bool getAgcAutoOn(void);
        void setAgcAutoOn(bool);
        
        /*****************************************************/
        
        //! RF transmit packet buffer
        uint8_t tx_buf[256];    // lora fifo size
        
        //! RF receive packet buffer
        uint8_t rx_buf[256];    // lora fifo size
       
        //! radio chip type plugged in
        type_e type;
        
        //! operating mode
        RegOpMode_t RegOpMode;
        
        //! transmitter power configuration
        RegPaConfig_t RegPaConfig;
        
        RegOcp_t RegOcp;            // 0x0b
        
        // receiver front-end
        RegLna_t RegLna;            // 0x0c
        
        RegIrqFlags_t       RegIrqFlags;            // 0x12
        uint8_t             RegRxNbBytes;           // 0x13
        RegModemStatus_t    RegModemStatus;         // 0x18
        int8_t              RegPktSnrValue;         // 0x19  signed, s/n can be negative
        uint8_t             RegPktRssiValue;        // 0x1a
        RegHopChannel_t     RegHopChannel;          // 0x1c
        RegModemConfig_t    RegModemConfig;         // 0x1d
        RegModemConfig2_t   RegModemConfig2;        // 0x1e
        uint16_t            RegPreamble;            // 0x20->0x21
        uint8_t             RegPayloadLength;       // 0x22
        uint8_t             RegRxMaxPayloadLength;  // 0x23
        uint8_t             RegHopPeriod;           // 0x24
        RegModemConfig3_t   RegModemConfig3;        // 0x26
        RegTest31_t         RegTest31;              // 0x31
        
        //! pin assignments
        RegDioMapping1_t RegDioMapping1;
        
        //! pin assignments
        RegDioMapping2_t RegDioMapping2;                                
        
        bool poll_vh;
        
        // frequency hopping table
        const uint32_t *frfs;
        
    private:
        SPI m_spi;
        DigitalOut m_cs;
        DigitalInOut reset_pin;
        DigitalOut femcps;  // LF rf switch
        DigitalOut femctx;  // HF rf switch
        //void dio0_callback(void);
        bool HF;    // sx1272 is always HF
        void set_nb_trig_peaks(int);
        
    protected:
        InterruptIn dio0;
        InterruptIn dio1;
        FunctionPointer _callback_rx;        
};

#endif /* SX127x_H */