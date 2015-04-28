#include "sx127x.h"

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

SX127x::SX127x(PinName mosi, PinName miso, PinName sclk, PinName cs, PinName rst, PinName dio_0, PinName dio_1, PinName fem_ctx, PinName fem_cps) :
                m_spi(mosi, miso, sclk),                  m_cs(cs), reset_pin(rst), dio0(dio_0), dio1(dio_1), femctx(fem_ctx), femcps(fem_cps)
{
    reset_pin.input();
    m_cs = 1;
    m_spi.format(8, 0);
    m_spi.frequency(1000000);
    
    init();
}

SX127x::~SX127x()
{
    set_opmode(RF_OPMODE_SLEEP);
}

void SX127x::init()
{
    type = SX_NONE;

    RegOpMode.octet = read_reg(REG_OPMODE);
    RegPaConfig.octet = read_reg(REG_PACONFIG);
    RegLna.octet = read_reg(REG_LNA);
    RegDioMapping1.octet = read_reg(REG_DIOMAPPING1);
    RegDioMapping2.octet = read_reg(REG_DIOMAPPING2);
    
    if (!RegOpMode.bits.LongRangeMode) {
        if (RegOpMode.bits.Mode != RF_OPMODE_SLEEP)
            set_opmode(RF_OPMODE_SLEEP);
        RegOpMode.bits.LongRangeMode = 1;
        write_reg(REG_OPMODE, RegOpMode.octet);
    }
    
    RegModemConfig.octet = read_reg(REG_LR_MODEMCONFIG);
    RegModemConfig2.octet = read_reg(REG_LR_MODEMCONFIG2);
    RegTest31.octet = read_reg(REG_LR_TEST31);
    
    get_type();
    
    // turn on PA BOOST, eval boards are wired for this connection
    RegPaConfig.bits.PaSelect = 1;
    write_reg(REG_PACONFIG, RegPaConfig.octet);
    
    // CRC for TX is disabled by default
    setRxPayloadCrcOn(true);
}

void SX127x::get_type()
{
    RegOpMode.octet = read_reg(REG_OPMODE);
    if (RegOpMode.sx1276LORAbits.LowFrequencyModeOn)
        type = SX1276;
    else {
        RegOpMode.sx1276LORAbits.LowFrequencyModeOn = 1;
        write_reg(REG_OPMODE, RegOpMode.octet);
        RegOpMode.octet = read_reg(REG_OPMODE);
        if (RegOpMode.sx1276LORAbits.LowFrequencyModeOn)
            type = SX1276;
        else
            type = SX1272;
    }
}
uint8_t SX127x::read_reg(uint8_t addr)
{
    uint8_t ret;
    // Select the device by seting chip select low
    m_cs = 0;

    m_spi.write(addr); // bit7 is low for reading from radio
 
    // Send a dummy byte to receive the contents of register
    ret = m_spi.write(0x00);
 
    // Deselect the device
    m_cs = 1;
    
    return ret;
}

uint16_t SX127x::read_u16(uint8_t addr)
{
    uint16_t ret;
    // Select the device by seting chip select low
    m_cs = 0;

    m_spi.write(addr); // bit7 is low for reading from radio
 
    // Send a dummy byte to receive the contents of register
    ret = m_spi.write(0x00);
    ret <<= 8;
    ret += m_spi.write(0x00);
 
    // Deselect the device
    m_cs = 1;
    
    return ret;
}

void SX127x::write_reg_u24(uint8_t addr, uint32_t data)
{
    m_cs = 0;   // Select the device by seting chip select low

    m_spi.write(addr | 0x80); // bit7 is high for writing to radio
    m_spi.write((data >> 16) & 0xff);
    m_spi.write((data >> 8) & 0xff);
    m_spi.write(data & 0xff);
 
    m_cs = 1;   // Deselect the device
    
    if (addr == REG_FRFMSB) {
        if (data < 0x8340000)   // < 525MHz
            HF = false;
        else
            HF = true;
    }
}

void SX127x::write_reg(uint8_t addr, uint8_t data)
{
    m_cs = 0;   // Select the device by seting chip select low

    m_spi.write(addr | 0x80); // bit7 is high for writing to radio
    m_spi.write(data);
 
    m_cs = 1;   // Deselect the device
}

void SX127x::lora_write_fifo(uint8_t len)
{
    int i;
    
    m_cs = 0;
    m_spi.write(REG_FIFO | 0x80); // bit7 is high for writing to radio
    for (i = 0; i < len; i++) {
        m_spi.write(tx_buf[i]);
    }
    m_cs = 1;
}

void SX127x::lora_read_fifo(uint8_t len)
{
    int i;
     
    m_cs = 0;
    m_spi.write(REG_FIFO); // bit7 is low for reading from radio
    for (i = 0; i < len; i++) {
        rx_buf[i] = m_spi.write(0);
    }
    m_cs = 1;
}

void SX127x::set_opmode(chip_mode_e mode)
{
    RegOpMode.bits.Mode = mode;
    write_reg(REG_OPMODE, RegOpMode.octet);
}

void SX127x::set_frf_MHz( float MHz )
{
    uint32_t frf;
    
    frf = MHz / FREQ_STEP_MHZ;
    write_reg_u24(REG_FRFMSB, frf);
    
    if (MHz < 525)
        HF = false;
    else
        HF = true;
}

float SX127x::get_frf_MHz(void)
{
    uint32_t frf;
    uint8_t lsb, mid, msb;
    float MHz;
    
    msb = read_reg(REG_FRFMSB);
    mid = read_reg(REG_FRFMID);
    lsb = read_reg(REG_FRFLSB);
    frf = msb;
    frf <<= 8;
    frf += mid;
    frf <<= 8;
    frf += lsb;
    
    MHz = frf * FREQ_STEP_MHZ;
    
    if (MHz < 525)
        HF = false;
    else
        HF = true;
        
    return MHz;
}

uint8_t SX127x::getCodingRate(bool from_rx)
{
    if (from_rx) {
        // expected RegModemStatus was read on RxDone interrupt
        return RegModemStatus.bits.RxCodingRate;    
    } else {    // transmitted coding rate...
        if (type == SX1276)
            return RegModemConfig.sx1276bits.CodingRate;
        else if (type == SX1272)
            return RegModemConfig.sx1272bits.CodingRate;
        else
            return 0;
    }
}

void SX127x::setCodingRate(uint8_t cr)
{
    if (type == SX1276)
        RegModemConfig.sx1276bits.CodingRate = cr;
    else if (type == SX1272)
        RegModemConfig.sx1272bits.CodingRate = cr;
    else
        return;
        
    write_reg(REG_LR_MODEMCONFIG, RegModemConfig.octet);
}

bool SX127x::getHeaderMode(void)
{
    if (type == SX1276)
        return RegModemConfig.sx1276bits.ImplicitHeaderModeOn;
    else if (type == SX1272)
        return RegModemConfig.sx1272bits.ImplicitHeaderModeOn;
    else
        return false;
}

void SX127x::setHeaderMode(bool hm)
{
    if (type == SX1276)
        RegModemConfig.sx1276bits.ImplicitHeaderModeOn = hm;
    else if (type == SX1272)
        RegModemConfig.sx1272bits.ImplicitHeaderModeOn = hm;
    else
        return;
        
    write_reg(REG_LR_MODEMCONFIG, RegModemConfig.octet);
}

uint8_t SX127x::getBw(void)
{
    if (type == SX1276)
        return RegModemConfig.sx1276bits.Bw;
    else if (type == SX1272)
        return RegModemConfig.sx1272bits.Bw;
    else
        return 0;
}

void SX127x::setBw(uint8_t bw)
{
    if (type == SX1276)
        RegModemConfig.sx1276bits.Bw = bw;
    else if (type == SX1272) {
        RegModemConfig.sx1272bits.Bw = bw;
        if (RegModemConfig2.sx1272bits.SpreadingFactor > 10)
            RegModemConfig.sx1272bits.LowDataRateOptimize = 1;
        else
            RegModemConfig.sx1272bits.LowDataRateOptimize = 0;
    } else
        return;
        
    write_reg(REG_LR_MODEMCONFIG, RegModemConfig.octet);
}

uint8_t SX127x::getSf(void)
{
    // spreading factor same between sx127[26]
    return RegModemConfig2.sx1276bits.SpreadingFactor;
}

void SX127x::set_nb_trig_peaks(int n)
{
    RegTest31.bits.detect_trig_same_peaks_nb = n;
    write_reg(REG_LR_TEST31, RegTest31.octet);
}

void SX127x::setSf(uint8_t sf)
{
    // false detections vs missed detections tradeoff
    switch (sf) {
        case 6:
            set_nb_trig_peaks(3);
            break;
        case 7:
            set_nb_trig_peaks(4);
            break;
        default:
            set_nb_trig_peaks(5);
            break;
    }
    
    // write register at 0x37 with value 0xc if at SF6
    if (sf < 7)
        write_reg(REG_LR_DETECTION_THRESHOLD, 0x0c);
    else
        write_reg(REG_LR_DETECTION_THRESHOLD, 0x0a);
    
    RegModemConfig2.sx1276bits.SpreadingFactor = sf; // spreading factor same between sx127[26]
    write_reg(REG_LR_MODEMCONFIG2, RegModemConfig2.octet);
    
    if (type == SX1272) {
        if (sf > 10 && RegModemConfig.sx1272bits.Bw == 0)   // if bw=125KHz and sf11 or sf12
            RegModemConfig.sx1272bits.LowDataRateOptimize = 1;
        else
            RegModemConfig.sx1272bits.LowDataRateOptimize = 0;
        write_reg(REG_LR_MODEMCONFIG, RegModemConfig.octet);
    } else if (type == SX1276) {
        if (sf > 10 && RegModemConfig.sx1272bits.Bw == 0)   // if bw=125KHz and sf11 or sf12
            RegModemConfig3.sx1276bits.LowDataRateOptimize = 1;
        else
            RegModemConfig3.sx1276bits.LowDataRateOptimize = 0;
        write_reg(REG_LR_MODEMCONFIG3, RegModemConfig3.octet);
    }
}
        
bool SX127x::getRxPayloadCrcOn(void)
{
    if (type == SX1276)
        return RegModemConfig2.sx1276bits.RxPayloadCrcOn;
    else if (type == SX1272)
        return RegModemConfig.sx1272bits.RxPayloadCrcOn;
    else
        return 0;
}

void SX127x::setRxPayloadCrcOn(bool on)
{
    if (type == SX1276) {
        RegModemConfig2.sx1276bits.RxPayloadCrcOn = on;
        write_reg(REG_LR_MODEMCONFIG2, RegModemConfig2.octet);
    } else if (type == SX1272) {
        RegModemConfig.sx1272bits.RxPayloadCrcOn = on;
        write_reg(REG_LR_MODEMCONFIG, RegModemConfig.octet);
    }   
}

bool SX127x::getAgcAutoOn(void)
{
    if (type == SX1276) {
        RegModemConfig3.octet = read_reg(REG_LR_MODEMCONFIG3);
        return RegModemConfig3.sx1276bits.AgcAutoOn;
    } else if (type == SX1272) {
        RegModemConfig2.octet = read_reg(REG_LR_MODEMCONFIG2);
        return RegModemConfig2.sx1272bits.AgcAutoOn;
    } else
        return 0;
}

void SX127x::setAgcAutoOn(bool on)
{
    if (type == SX1276) {
        RegModemConfig3.sx1276bits.AgcAutoOn = on;
        write_reg(REG_LR_MODEMCONFIG3, RegModemConfig3.octet);
    } else if (type == SX1272) {
        RegModemConfig2.sx1272bits.AgcAutoOn = on;
        write_reg(REG_LR_MODEMCONFIG2, RegModemConfig2.octet);
    }
    
}

void SX127x::lora_start_tx(uint8_t len)
{
    if (type == SX1276) {
        // PA_BOOST on LF, RFO on HF
        if (HF) {
            if (RegPaConfig.bits.PaSelect) {
                RegPaConfig.bits.PaSelect = 0;
                write_reg(REG_PACONFIG, RegPaConfig.octet);
            }                
        } else { // LF...
            if (!RegPaConfig.bits.PaSelect) {
                RegPaConfig.bits.PaSelect = 1;
                write_reg(REG_PACONFIG, RegPaConfig.octet);
            }        
        }
    } else if (type == SX1272) {
        // always PA_BOOST
        if (!RegPaConfig.bits.PaSelect) {
            RegPaConfig.bits.PaSelect = 1;
            write_reg(REG_PACONFIG, RegPaConfig.octet);
        }
    }
                    
                    
    // DIO0 to TxDone
    if (RegDioMapping1.bits.Dio0Mapping != 1) {
        RegDioMapping1.bits.Dio0Mapping = 1;
        write_reg(REG_DIOMAPPING1, RegDioMapping1.octet);
    }
    
    // set FifoPtrAddr to FifoTxPtrBase
    write_reg(REG_LR_FIFOADDRPTR, read_reg(REG_LR_FIFOTXBASEADDR));
    
    // write PayloadLength bytes to fifo
    lora_write_fifo(len);

    if (HF)
        femctx = 1;
    else
        femcps = 0;
        
    // radio doesnt provide FhssChangeChannel with channel=0 for TX    
    if (RegHopPeriod > 0)
        write_reg_u24(REG_FRFMSB, frfs[0]);
        
    set_opmode(RF_OPMODE_TRANSMITTER);
}



void SX127x::lora_start_rx()
{
    if (HF)
        femctx = 0;
    else
        femcps = 1;
        
    if (RegDioMapping1.bits.Dio0Mapping != 0) {
        RegDioMapping1.bits.Dio0Mapping = 0;    // DIO0 to RxDone
        write_reg(REG_DIOMAPPING1, RegDioMapping1.octet);
    }
    
    write_reg(REG_LR_FIFOADDRPTR, read_reg(REG_LR_FIFORXBASEADDR));
    
    // shouldn't be necessary, radio should provide FhssChangeChannel with channel=0 for RX  
    if (RegHopPeriod > 0)
        write_reg_u24(REG_FRFMSB, frfs[0]);
    
    set_opmode(RF_OPMODE_RECEIVER);
}


void SX127x::hw_reset()
{
    /* only a french-swiss design would have hi-Z deassert */
    reset_pin.output();
    reset_pin.write(1);
    wait(0.05);
    reset_pin.input();
    wait(0.05);
}

service_action_e SX127x::service()
{
    
    if (RegOpMode.bits.Mode == RF_OPMODE_RECEIVER) {
        if (poll_vh) {
            RegIrqFlags.octet = read_reg(REG_LR_IRQFLAGS);
            if (RegIrqFlags.bits.ValidHeader) {
                RegIrqFlags.octet = 0;
                RegIrqFlags.bits.ValidHeader = 1;
                write_reg(REG_LR_IRQFLAGS, RegIrqFlags.octet);
                printf("VH\r\n");
            }
        }
    }
    
    // FhssChangeChannel
    if (RegDioMapping1.bits.Dio1Mapping == 1) {
        if (dio1) {
            RegHopChannel.octet = read_reg(REG_LR_HOPCHANNEL);    
            write_reg_u24(REG_FRFMSB, frfs[RegHopChannel.bits.FhssPresentChannel]);
            printf("hopch:%d\r\n", RegHopChannel.bits.FhssPresentChannel);
            RegIrqFlags.octet = 0;
            RegIrqFlags.bits.FhssChangeChannel = 1;
            write_reg(REG_LR_IRQFLAGS, RegIrqFlags.octet);
            
        }
    }
    
    if (dio0 == 0)
        return SERVICE_NONE;
        
    switch (RegDioMapping1.bits.Dio0Mapping) {
        case 0: // RxDone
            /* user checks for CRC error in IrqFlags */
            RegIrqFlags.octet = read_reg(REG_LR_IRQFLAGS);  // save flags
            RegHopChannel.octet = read_reg(REG_LR_HOPCHANNEL);
            if (RegIrqFlags.bits.FhssChangeChannel) {
                write_reg_u24(REG_FRFMSB, frfs[RegHopChannel.bits.FhssPresentChannel]);
            }
            //printf("[%02x]", RegIrqFlags.octet);
            write_reg(REG_LR_IRQFLAGS, RegIrqFlags.octet); // clear flags in radio
            
            /* any register of interest on received packet is read(saved) here */        
            RegModemStatus.octet = read_reg(REG_LR_MODEMSTAT);          
            RegPktSnrValue = read_reg(REG_LR_PKTSNRVALUE);
            RegPktRssiValue = read_reg(REG_LR_PKTRSSIVALUE);
            RegRxNbBytes = read_reg(REG_LR_RXNBBYTES);
    
            write_reg(REG_LR_FIFOADDRPTR, read_reg(REG_LR_FIFORXCURRENTADDR));
            lora_read_fifo(RegRxNbBytes);
            return SERVICE_READ_FIFO;
        case 1: // TxDone
            if (HF)
                femctx = 0;
            else
                femcps = 1;

            RegIrqFlags.octet = 0;
            RegIrqFlags.bits.TxDone = 1;
            write_reg(REG_LR_IRQFLAGS, RegIrqFlags.octet);                  
            return SERVICE_TX_DONE;        
    } // ...switch (RegDioMapping1.bits.Dio0Mapping)
    
    return SERVICE_ERROR;
}
