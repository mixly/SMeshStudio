#include "Ultrasonic.h"

Ultrasonic::Ultrasonic(BricktronicsMegashield* b, uint8_t port)
{
    bms = b;

    //TODO: do this a better way
    switch (port)
    {
        case 1:
            scl_pin = S1_DA;
            sda_pin = S1_DB;
            break;
        case 2:
            scl_pin = S2_DA;
            sda_pin = S2_DB;
            break;
        case 3:
            scl_pin = S3_DA;
            sda_pin = S3_DB;
            break;
        case 4:
            scl_pin = S4_DA;
            sda_pin = S4_DB;
            break;
    }

    SoftI2cMaster i2c;
}

void Ultrasonic::begin(void)
{
    i2c.init(scl_pin, sda_pin);
}

char* Ultrasonic::readString(uint8_t startRegister, uint8_t bytes)
{
    readBytes(startRegister, bytes, b_buf);
    b_buf[BUFF_LEN-1] = 0;
    return (char*) b_buf;
}

uint8_t Ultrasonic::readBytes(uint8_t startRegister, uint8_t bytes, uint8_t* buf)
{
    if (!(i2c.start(ULTRASONIC_ADDRESS | I2C_WRITE))) // Try to start, with a write address
    {
        return false; // if it fails, return false.
    }

    if (!(i2c.write(startRegister))) // Try to start a write
    {
        return false; // if it fails, return false.
    }

    // Do an i2c restart.  See HDK for details.
    if (!(i2c.restart(ULTRASONIC_ADDRESS | I2C_READ)))
    {
        return false;
    }


    for (uint8_t i = 0; i < bytes-1; i++)
    {
        buf[i] = i2c.read(false); // read, and then send ack
    }
    buf[bytes-1] = i2c.read(true); // read the last byte, then send nak

    i2c.stop();
    return true;
}

uint8_t Ultrasonic::readByte(uint8_t address)
{
    readBytes(address, 1, b_buf);
    return b_buf[0];
}

bool Ultrasonic::writeBytes(uint8_t first_register_address,
                            uint8_t num_of_bytes,
                            uint8_t* buffer)
{
    if (!buffer)
    {
        buffer = b_buf;
    }

    if (!i2c.start(ULTRASONIC_ADDRESS | I2C_WRITE))
    {
        return false;
    }

    if (!i2c.write(first_register_address))
    {
        return false;
    }

    for (int i = 0; i < num_of_bytes; i++)
    {
        if (!i2c.write(buffer[i]))
        {
            return false;
        }
    }
    i2c.stop();
    return true;

}

uint8_t Ultrasonic::writeByte(uint8_t address, uint8_t data)
{
    return writeBytes(address, 1, &data);
}

char* Ultrasonic::getVersion(void)
{
    return readString(GET_VERSION, 8);
}

char* Ultrasonic::getProductID(void)
{
    return readString(GET_PRODUCT_ID, 8);
}

char* Ultrasonic::getSensorType(void)
{
    return readString(GET_SENSOR_TYPE, 8);
}

uint8_t Ultrasonic::getDistance(void)
{
    writeByte(READ_COMMAND, ULTRASONIC_ADDRESS);
    delay(20);
    return readByte(READ_MEASUREMENT_BYTE_ZERO);
}

