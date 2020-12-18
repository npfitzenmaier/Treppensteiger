#ifndef _ICM_20948_I2C_H_
#define _ICM_20948_I2C_H_

#include "ICM_20948.h"

class ICM_20948_I2C : public ICM_20948
{
public:
    int _i2cFd;
    uint8_t _addr;
    uint8_t _ad0;
    bool _ad0val;
    ICM_20948_Serif_t _serif;

    ICM_20948_I2C(); // Constructor

    virtual ICM_20948_Status_e  begin(int i2cFd, bool ad0val = true, uint8_t ad0pin = ICM_20948_ARD_UNUSED_PIN);
};

#endif /* _ICM_20948_I2C_H_ */
