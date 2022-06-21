 
#include "mlx90614.h"

MLX90614::MLX90614(I2C* i2c,int addr){

    this->i2caddress = addr;
    this->i2c = i2c; 
    
}


bool MLX90614::getTemp(float* temp_val){

    char data[3];
    float temp_thermo;

    i2c->stop();
    Cy_SysLib_DelayUs(2);
    i2c->start();
    //wait(0.01);
    Cy_SysLib_DelayUs(1);
    data[0] = 0x00;
    i2c->write((i2caddress<<1), data, 1, true);
    //wait(0.01);
    Cy_SysLib_DelayUs(1);
    data[0] = 0x07;
    i2c->write((i2caddress<<1), data, 1, true);
    //wait(0.01);
    Cy_SysLib_DelayUs(1);
    i2c->read(((i2caddress<<1)|0x01), data, 3, true);
    //wait(0.01);
    Cy_SysLib_DelayUs(1);
    i2c->stop();                            //stop condition


    temp_thermo=((((data[1]&0x007f)<<8)+data[0])*0.02)-0.01;      //degree centigrate conversion
    *temp_val=temp_thermo-273;                          //Convert kelvin to degree Celsius
    
    return true;                            //load data successfully, return true 
}