#include "ICM_20948_I2C.h"

#include <linux/i2c-dev.h>
#include <i2c/smbus.h>
#include <sys/ioctl.h>

// Forward Declarations
ICM_20948_Status_e ICM_20948_write_I2C(uint8_t reg, uint8_t* data, uint32_t len, void* user);
ICM_20948_Status_e ICM_20948_read_I2C(uint8_t reg, uint8_t* buff, uint32_t len, void* user);


ICM_20948_I2C::ICM_20948_I2C()
{
}

ICM_20948_Status_e ICM_20948_I2C::begin(int i2cFd, bool ad0val, uint8_t ad0pin){
    // Associate
    _ad0 = ad0pin;
    _i2cFd = i2cFd;
    _ad0val = ad0val;

    _addr = ICM_20948_I2C_ADDR_AD0;
    if (_ad0val)
    {
        _addr = ICM_20948_I2C_ADDR_AD1;
    }

    // Set pinmodes
    if (_ad0 != ICM_20948_ARD_UNUSED_PIN)
    {
        //pinMode(_ad0, OUTPUT);
    }

    // Set pins to default positions
    if (_ad0 != ICM_20948_ARD_UNUSED_PIN)
    {
        //digitalWrite(_ad0, _ad0val);
    }

    // _i2c->begin(); // Moved into user's sketch

    // Set up the serif
    _serif.write = ICM_20948_write_I2C;
    _serif.read = ICM_20948_read_I2C;
    _serif.user = static_cast<void*>(this); // refer to yourself in the user field

    // Link the serif
    _device._serif = &_serif;

    // Perform default startup
    status = startupDefault();
    if( status != ICM_20948_Stat_Ok )
    {
        return status;
    }
    return status;
}

// serif functions for the I2C and SPI classes
ICM_20948_Status_e ICM_20948_write_I2C(uint8_t reg, uint8_t* data, uint32_t len, void* user)
{
    if (user == nullptr)
    {
    return ICM_20948_Stat_ParamErr;
    }
    int i2cFd = static_cast<ICM_20948_I2C*>(user)->_i2cFd; // Cast user field to ICM_20948_I2C type and extract the I2C interface pointer
    if (i2cFd < 0)
    {
    return ICM_20948_Stat_ParamErr;
    }
    uint8_t addr = static_cast<ICM_20948_I2C*>(user)->_addr;

    if (ioctl(i2cFd, I2C_SLAVE, addr) < 0)
    {
        // ERROR
        return ICM_20948_Stat_Err;
    }

	i2c_smbus_write_i2c_block_data(i2cFd, reg, static_cast<__u8>(len), data);

    return ICM_20948_Stat_Ok;
}

ICM_20948_Status_e ICM_20948_read_I2C(uint8_t reg, uint8_t* buff, uint32_t len, void* user)
{
    if (user == nullptr)
    {
    return ICM_20948_Stat_ParamErr;
    }
    int i2cFd = static_cast<ICM_20948_I2C*>(user)->_i2cFd;
    if (i2cFd < 0)
    {
    return ICM_20948_Stat_ParamErr;
    }
    uint8_t addr = static_cast<ICM_20948_I2C*>(user)->_addr;

    if (ioctl(i2cFd, I2C_SLAVE, addr) < 0)
    {
        // ERROR
        return ICM_20948_Stat_Err;
    }

    int32_t num_received = i2c_smbus_read_i2c_block_data(i2cFd, reg, static_cast<__u8>(len), buff);

    if (num_received == static_cast<int32_t>(len))
    {
    return ICM_20948_Stat_Ok;
    }
    else
    {
    return ICM_20948_Stat_NoData;
    }
}
