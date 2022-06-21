#include "mbed.h"

//Melexis Infrared Thermometer MLX90614 Library

//*****************************************************************
//  Build : 2011-06-08 Hikaru Sugiura
//          Only read thermo data.
//  
//  This program is based on Mr.Mitesh Patel's "mlx90614".
//  http://mbed.org/users/mitesh2patel/programs/mlx90614/lqnetj
//
//  This program does not check CRC.
//  If you want to check CRC, please do it your self :)
//****************************************************************//

/**An Interface for MLX90614
* 
* @code
* //Print temperature data
* #include "mbed.h"
* #include "mlx90614.h"
*
* I2C i2c(PB_9, PB_8);   //sda,scl
* MLX90614 thermometer(&i2c, 0x5A);
* float temp;
*
* void main(void){
*   if(thermometer.getTemp(&temp)){
*       printf("Temperature : %f \r\n",temp);
*   }
*   wait(0.5);
*
* }
* @endcode
*/

#pragma once

namespace gy906 {
//const int default_addr = 0x5a;
const int default_addr = 0x00;
namespace opcode {
    const int eeprom_access = 0x20;
    const int ram_access = 0x00;
    const int read_flags = 0xf0;
    const int sleep = 0xff;
    const int read_mask = 0x80;
    const int write_mask = 0x00;
}
namespace eeprom {
    const int T0_max = 0x00;
    const int T0_min = 0x01;
    const int PWMCTRL = 0x02;
    const int Ta_range = 0x03;
    const int EC_coef = 0x04;
    const int config_reg1 = 0x05;
    const int SMBus_addr = 0x0e;
    const int id1 = 0x1c;
    const int id2 = 0x1d;
    const int id3 = 0x1e;
    const int id4 = 0x1f;
}
namespace ram {
    const int ir1 = 0x04;
    const int ir2 = 0x05;
    const int T_ambient = 0x06;
    const int T_obj1 = 0x07;
    const int T_obj2 = 0x08;
}
}


class MLX90614{

    public:
        /** Create MLX90614 interface, initialize with selected I2C port and address.
        *
        * @param i2c I2C device pointer
        * @param addr Device address(default=0x5A)  
        */    
        MLX90614(I2C* i2c,int addr=0x5A);
        
        /** Get Temperature data from MLX90614. 
        *
        * @param temp_val return valiable pointer
        * @return 0 on success (ack), or non-0 on failure (nack)
        */
        bool getTemp(float* temp_val);
        
    private:
       I2C* i2c;
       int i2caddress;

};