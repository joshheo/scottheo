#include "LED.h"
#include "mbed.h"


const uint16_t LP5024_SLAVE_ADDRESS_D2 = ((uint16_t)0x29 << (int)1);
const uint16_t LP5024_SLAVE_ADDRESS = ((uint16_t)0x28 << (int)1);


I2C i2_c_LP5024_handle(P8_1, P8_0);

struct reg_default
{
    unsigned int reg;
    unsigned int def;
};
 typedef union {
    uint8_t c[1];
    uint16_t d;
}WORD;

uint8_t _i2c_LP5024_readByte(uint8_t reg)
{
    WORD _reg;
    char data[1];
    _reg.d = 0;
    _reg.c[0] = reg;
    //printf("[%s:%d] I2C handle=%x , slaveaddr=0x%x,reg=0x%x \n", __FUNCTION__, __LINE__, &i2_c_LP5024_handle, LP5024_SLAVE_ADDRESS, _reg.d);
    i2_c_LP5024_handle.write((int)LP5024_SLAVE_ADDRESS, (const char *)&_reg.c[0], 1, false);
    i2_c_LP5024_handle.read((int)LP5024_SLAVE_ADDRESS, (char *)&data[0], 1, false);
    return data[0];
}
uint8_t _i2c_LP5024_readByte_D2(uint8_t reg)
{
    WORD _reg;
    char data[1];
    _reg.d = 0;
    _reg.c[0] = reg;
    //printf("[%s:%d] I2C handle=%x , slaveaddr=0x%x,reg=0x%x \n", __FUNCTION__, __LINE__, &i2_c_LP5024_handle, LP5024_SLAVE_ADDRESS, _reg.d);
    i2_c_LP5024_handle.write((int)LP5024_SLAVE_ADDRESS_D2, (const char *)&_reg.c[0], 1, false);
    i2_c_LP5024_handle.read((int)LP5024_SLAVE_ADDRESS_D2, (char *)&data[0], 1, false);
    return data[0];
}

void Mode_Select_Standby(void)
{
    int return_Standby;
    char buff[2];
    buff[0] = 0x00;
    buff[1] = 0x00;
    return_Standby = i2_c_LP5024_handle.write(LP5024_SLAVE_ADDRESS, buff, 2, false);
    //uint8_t data = _i2c_LP5024_readByte(0x00);
    //printf("[%s:%d] Mode_Select_Standby read data(0x%x)=0x%x\n", __FUNCTION__, __LINE__, 0x00, data);
    //printf("return_Standby = %d\n", return_Standby);
}
void Mode_Select_Normal(void)
{
    int Mode_Select_Normal;
    char buff_01[2];
    buff_01[0] = 0x00;
    buff_01[1] = 0x40;
    Mode_Select_Normal = i2_c_LP5024_handle.write(LP5024_SLAVE_ADDRESS, buff_01, 2, false);
    //uint8_t data = _i2c_LP5024_readByte(0x00);
    //printf("[%s:%d] Mode_Select_Normal read data(0x%x)=0x%x\n", __FUNCTION__, __LINE__, 0x00, data);
    //printf("Mode_Select_Normal = %d\n", Mode_Select_Normal);
}
int Mode_Set(bool Log_Scale_EN, bool Power_Save_EN, bool Auto_Incr_EN, bool PWM_Dithering_EN, bool Max_Current_Option, bool LED_Global_Off)
{
    int Mode_Set;
    char buff_03[2];
	buff_03[0] = 0x01;
	buff_03[1] = (char)Log_Scale_EN*32+Power_Save_EN*16+Auto_Incr_EN*8+PWM_Dithering_EN*4+Max_Current_Option*2+LED_Global_Off;
	Mode_Set = i2_c_LP5024_handle.write(LP5024_SLAVE_ADDRESS, buff_03, 2, false);
    //uint8_t data = _i2c_LP5024_readByte(0x00);
    //printf("[%s:%d] Mode_Set read data(0x%x)=0x%x\n", __FUNCTION__, __LINE__, 0x01, data);
    //printf("Mode_Set = %d\n", Mode_Set);
    return 0;
}
int Bank_Color_Set_R(char BANK_R_COLOR)
{
    int Bank_Color_Set;
    char buff_02[2];
    buff_02[0] = 0x04;
    buff_02[1] = (char)BANK_R_COLOR;
    Bank_Color_Set = i2_c_LP5024_handle.write(LP5024_SLAVE_ADDRESS, buff_02, 2, false);
    //uint8_t data = _i2c_LP5024_readByte(0x00);
    //printf("[%s:%d] Bank_Color_Set read data(0x%x)=0x%x\n", __FUNCTION__, __LINE__, 0x04, data);
    //printf("Bank_Color_Set = %d\n", Bank_Color_Set);
    return 0;
}
int Bank_Color_Set_G(char BANK_G_COLOR)
{
    int Bank_Color_Set;
    char buff_02[2];
    buff_02[0] = 0x05;
    buff_02[1] = (char)BANK_G_COLOR;
    Bank_Color_Set = i2_c_LP5024_handle.write(LP5024_SLAVE_ADDRESS, buff_02, 2, false);
    //uint8_t data = _i2c_LP5024_readByte(0x00);
    //printf("[%s:%d] Bank_Color_Set read data(0x%x)=0x%x\n",__FUNCTION__, __LINE__, 0x05, data);
    //printf("Bank_Color_Set = %d\n", Bank_Color_Set);
    return 0;
}
int Bank_Color_Set_B(char BANK_B_COLOR)
{
    int Bank_Color_Set;
    char buff_02[2];
    buff_02[0] = 0x06;
    buff_02[1] = (char)BANK_B_COLOR;
    Bank_Color_Set = i2_c_LP5024_handle.write(LP5024_SLAVE_ADDRESS, buff_02, 2, false);
    //uint8_t data = _i2c_LP5024_readByte(0x00);
    //printf("[%s:%d] Bank_Color_Set read data(0x%x)=0x%x\n", __FUNCTION__, __LINE__, 0x06, data);
    //printf("Bank_Color_Set = %d\n", Bank_Color_Set);
    return 0;
}
int Bank_Brightness_Set(char BANK_BRIGHTNESS)
{
    int Bank_Brightness_Set;
    char buff_02[2];
    buff_02[0] = 0x03;
    buff_02[1] = (char)BANK_BRIGHTNESS;
    Bank_Brightness_Set = i2_c_LP5024_handle.write(LP5024_SLAVE_ADDRESS, buff_02, 2, false);
    //uint8_t data = _i2c_LP5024_readByte(0x00);
    //printf("[%s:%d] Bank_Brightness_Set read data(0x%x)=0x%x\n", __FUNCTION__, __LINE__, 0x03, data);
    //printf("Bank_Brightness_Set = %d\n", Bank_Brightness_Set);
    return 0;
}
int LED_CONFIG0(bool LED7_Bank_EN, bool LED6_Bank_EN, bool LED5_Bank_EN, bool LED4_Bank_EN, bool LED3_Bank_EN, bool LED2_Bank_EN, bool LED1_Bank_EN, bool LED0_Bank_EN)
{
    int LED_CONFIG0;

    char buff_02[2];
    buff_02[0] = 0x02;
    buff_02[1] = (char)LED7_Bank_EN*128+LED6_Bank_EN*64+LED5_Bank_EN*32+LED4_Bank_EN*16+LED3_Bank_EN*8+LED2_Bank_EN*4+LED1_Bank_EN*2+LED0_Bank_EN;
    LED_CONFIG0 = i2_c_LP5024_handle.write(LP5024_SLAVE_ADDRESS, buff_02, 2, false);
    //uint8_t data = _i2c_LP5024_readByte(0x00);
    //printf("[%s:%d] LED_CONFIG0 read data(0x%x)=0x%x\n", __FUNCTION__, __LINE__, 0x02, data);
    //printf("LED_CONFIG0 = %d\n", LED_CONFIG0);
    return 0;
}



int Mode_Select_Standby_D2(void)
{
    int return_Standby;
    char buff[2];
    buff[0] = 0x00;
    buff[1] = 0x00;
    return_Standby = i2_c_LP5024_handle.write(LP5024_SLAVE_ADDRESS_D2, buff, 2, false);
    //uint8_t data = _i2c_LP5024_readByte_D2(0x00);
   // printf("[%s:%d] Mode_Select_Standby read data(0x%x)=0x%x\n",  __FUNCTION__, __LINE__, 0x00, data);
    //printf("return_Standby = %d\n", return_Standby);
    return 0;
}
int Mode_Select_Normal_D2(void)
{
    int Mode_Select_Normal;
    char buff_01[2];
    buff_01[0] = 0x00;
    buff_01[1] = 0x40;
    Mode_Select_Normal = i2_c_LP5024_handle.write(LP5024_SLAVE_ADDRESS_D2, buff_01, 2, false);
    //uint8_t data = _i2c_LP5024_readByte_D2(0x00);
  // printf("[%s:%d] Mode_Select_Normal read data(0x%x)=0x%x\n",  __FUNCTION__, __LINE__, 0x00, data);
    //printf("Mode_Select_Normal = %d\n", Mode_Select_Normal);
    return 0;
}
int Mode_Set_D2(bool Log_Scale_EN, bool Power_Save_EN, bool Auto_Incr_EN, bool PWM_Dithering_EN, bool Max_Current_Option, bool LED_Global_Off)
{
    int Mode_Set;
    char buff_03[2];
	buff_03[0] = 0x01;
	buff_03[1] = (char)Log_Scale_EN*32+Power_Save_EN*16+Auto_Incr_EN*8+PWM_Dithering_EN*4+Max_Current_Option*2+LED_Global_Off;
	Mode_Set = i2_c_LP5024_handle.write(LP5024_SLAVE_ADDRESS_D2, buff_03, 2, false);
    //uint8_t data = _i2c_LP5024_readByte_D2(0x00);
    //printf("[%s:%d] Mode_Set read data(0x%x)=0x%x\n", __FUNCTION__, __LINE__, 0x01, data);
    //printf("Mode_Set = %d\n", Mode_Set);
    return 0;
}
int Bank_Color_Set_R_D2(char BANK_R_COLOR)
{
    int Bank_Color_Set;
    char buff_02[2];
    buff_02[0] = 0x04;
    buff_02[1] = (char)BANK_R_COLOR;
    Bank_Color_Set = i2_c_LP5024_handle.write(LP5024_SLAVE_ADDRESS_D2, buff_02, 2, false);
    //uint8_t data = _i2c_LP5024_readByte_D2(0x00);
   // printf("[%s:%d] Bank_Color_Set read data(0x%x)=0x%x\n", __FUNCTION__, __LINE__, 0x04, data);
    //printf("Bank_Color_Set = %d\n", Bank_Color_Set);
    return 0;
}
int Bank_Color_Set_G_D2(char BANK_G_COLOR)
{
    int Bank_Color_Set;
    char buff_02[2];
    buff_02[0] = 0x05;
    buff_02[1] = (char)BANK_G_COLOR;
    Bank_Color_Set = i2_c_LP5024_handle.write(LP5024_SLAVE_ADDRESS_D2, buff_02, 2, false);
    //uint8_t data = _i2c_LP5024_readByte_D2(0x05);
    //printf("[%s:%d] Bank_Color_Set read data(0x%x)=0x%x\n", __FUNCTION__, __LINE__, 0x05, data);
    //printf("Bank_Color_Set = %d\n", Bank_Color_Set);
    return 0;
}
int Bank_Color_Set_B_D2(char BANK_B_COLOR)
{
    int Bank_Color_Set;
    char buff_02[2];
    buff_02[0] = 0x06;
    buff_02[1] = (char)BANK_B_COLOR;
    Bank_Color_Set = i2_c_LP5024_handle.write(LP5024_SLAVE_ADDRESS_D2, buff_02, 2, false);
    //uint8_t data = _i2c_LP5024_readByte_D2(0x00);
    //printf("[%s:%d] Bank_Color_Set read data(0x%x)=0x%x\n", __FUNCTION__, __LINE__, 0x06, data);
    //printf("Bank_Color_Set = %d\n", Bank_Color_Set);
    return 0;
}
int Bank_Brightness_Set_D2(char BANK_BRIGHTNESS)
{
    int Bank_Brightness_Set;
    char buff_02[2];
    buff_02[0] = 0x03;
    buff_02[1] = (char)BANK_BRIGHTNESS;
    Bank_Brightness_Set = i2_c_LP5024_handle.write(LP5024_SLAVE_ADDRESS_D2, buff_02, 2, false);
    //uint8_t data = _i2c_LP5024_readByte(0x00);
   // printf("[%s:%d] Bank_Brightness_Set read data(0x%x)=0x%x\n", __FUNCTION__, __LINE__, 0x03, data);
    //printf("Bank_Brightness_Set = %d\n", Bank_Brightness_Set);
    return 0;
}


int LED_CONFIG0_D2(bool LED7_Bank_EN, bool LED6_Bank_EN, bool LED5_Bank_EN, bool LED4_Bank_EN, bool LED3_Bank_EN, bool LED2_Bank_EN, bool LED1_Bank_EN, bool LED0_Bank_EN)
{
    int LED_CONFIG0;

    char buff_02[2];
    buff_02[0] = 0x02;
    buff_02[1] = (char)LED7_Bank_EN*128+LED6_Bank_EN*64+LED5_Bank_EN*32+LED4_Bank_EN*16+LED3_Bank_EN*8+LED2_Bank_EN*4+LED1_Bank_EN*2+LED0_Bank_EN;
    LED_CONFIG0 = i2_c_LP5024_handle.write(LP5024_SLAVE_ADDRESS_D2, buff_02, 2, false);
    //uint8_t data = _i2c_LP5024_readByte_D2(0x00);
   // printf("[%s:%d] LED_CONFIG0 read data(0x%x)=0x%x\n", __FUNCTION__, __LINE__, 0x02, data);
    //printf("LED_CONFIG0 = %d\n", LED_CONFIG0);
    return 0;
}


int Ring_LED_D5_R(char OUT_Color)
{
    int LED_OUT0;

    char buff_02[2];
    buff_02[0] = 0x0f;
    buff_02[1] = (char)OUT_Color;
    LED_OUT0 = i2_c_LP5024_handle.write(LP5024_SLAVE_ADDRESS, buff_02, 2, false); 
    return 0;
}
int Ring_LED_D6_R(char OUT_Color)
{
    int LED_OUT0;

    char buff_02[2];
    buff_02[0] = 0x21;
    buff_02[1] = (char)OUT_Color;
    LED_OUT0 = i2_c_LP5024_handle.write(LP5024_SLAVE_ADDRESS, buff_02, 2, false); 
    return 0;
}
int Ring_LED_D1_R(char OUT_Color)
{
    int LED_OUT0;

    char buff_02[2];
    buff_02[0] = 0x15;
    buff_02[1] = (char)OUT_Color;
    LED_OUT0 = i2_c_LP5024_handle.write(LP5024_SLAVE_ADDRESS, buff_02, 2, false); 
    return 0;
}
int Ring_LED_D3_R(char OUT_Color)
{
    int LED_OUT0;

    char buff_02[2];
    buff_02[0] = 0x12;
    buff_02[1] = (char)OUT_Color;
    LED_OUT0 = i2_c_LP5024_handle.write(LP5024_SLAVE_ADDRESS, buff_02, 2, false); 
    return 0;
}
int Ring_LED_D8_R(char OUT_Color)
{
    int LED_OUT0;

    char buff_02[2];
    buff_02[0] = 0x1b;
    buff_02[1] = (char)OUT_Color;
    LED_OUT0 = i2_c_LP5024_handle.write(LP5024_SLAVE_ADDRESS, buff_02, 2, false); 
    return 0;
}
int Ring_LED_D7_R(char OUT_Color)
{
    int LED_OUT0;

    char buff_02[2];
    buff_02[0] = 0x18;
    buff_02[1] = (char)OUT_Color;
    LED_OUT0 = i2_c_LP5024_handle.write(LP5024_SLAVE_ADDRESS, buff_02, 2, false); 
    return 0;
}
int Ring_LED_D2_R(char OUT_Color)
{
    int LED_OUT0;

    char buff_02[2];
    buff_02[0] = 0x24;
    buff_02[1] = (char)OUT_Color;
    LED_OUT0 = i2_c_LP5024_handle.write(LP5024_SLAVE_ADDRESS, buff_02, 2, false); 
    return 0;
}
int Ring_LED_D4_R(char OUT_Color)
{
    int LED_OUT0;

    char buff_02[2];
    buff_02[0] = 0x1e;
    buff_02[1] = (char)OUT_Color;
    LED_OUT0 = i2_c_LP5024_handle.write(LP5024_SLAVE_ADDRESS, buff_02, 2, false); 
    return 0;
}



int Ring_LED_D5_G(char OUT_Color)
{
    int LED_OUT0;

    char buff_02[2];
    buff_02[0] = 0x10;
    buff_02[1] = (char)OUT_Color;
    LED_OUT0 = i2_c_LP5024_handle.write(LP5024_SLAVE_ADDRESS, buff_02, 2, false); 
    return 0;
}
int Ring_LED_D6_G(char OUT_Color)
{
    int LED_OUT0;

    char buff_02[2];
    buff_02[0] = 0x22;
    buff_02[1] = (char)OUT_Color;
    LED_OUT0 = i2_c_LP5024_handle.write(LP5024_SLAVE_ADDRESS, buff_02, 2, false); 
    return 0;
}
int Ring_LED_D1_G(char OUT_Color)
{
    int LED_OUT0;

    char buff_02[2];
    buff_02[0] = 0x16;
    buff_02[1] = (char)OUT_Color;
    LED_OUT0 = i2_c_LP5024_handle.write(LP5024_SLAVE_ADDRESS, buff_02, 2, false); 
    return 0;
}
int Ring_LED_D3_G(char OUT_Color)
{
    int LED_OUT0;

    char buff_02[2];
    buff_02[0] = 0x13;
    buff_02[1] = (char)OUT_Color;
    LED_OUT0 = i2_c_LP5024_handle.write(LP5024_SLAVE_ADDRESS, buff_02, 2, false); 
    return 0;
}
int Ring_LED_D8_G(char OUT_Color)
{
    int LED_OUT0;

    char buff_02[2];
    buff_02[0] = 0x1c;
    buff_02[1] = (char)OUT_Color;
    LED_OUT0 = i2_c_LP5024_handle.write(LP5024_SLAVE_ADDRESS, buff_02, 2, false); 
    return 0;
}
int Ring_LED_D7_G(char OUT_Color)
{
    int LED_OUT0;

    char buff_02[2];
    buff_02[0] = 0x19;
    buff_02[1] = (char)OUT_Color;
    LED_OUT0 = i2_c_LP5024_handle.write(LP5024_SLAVE_ADDRESS, buff_02, 2, false); 
    return 0;
}
int Ring_LED_D2_G(char OUT_Color)
{
    int LED_OUT0;

    char buff_02[2];
    buff_02[0] = 0x25;
    buff_02[1] = (char)OUT_Color;
    LED_OUT0 = i2_c_LP5024_handle.write(LP5024_SLAVE_ADDRESS, buff_02, 2, false); 
    return 0;
}
int Ring_LED_D4_G(char OUT_Color)
{
    int LED_OUT0;

    char buff_02[2];
    buff_02[0] = 0x1f;
    buff_02[1] = (char)OUT_Color;
    LED_OUT0 = i2_c_LP5024_handle.write(LP5024_SLAVE_ADDRESS, buff_02, 2, false); 
    return 0;
}




int Ring_LED_D5_B(char OUT_Color)
{
    int LED_OUT0;

    char buff_02[2];
    buff_02[0] = 0x11;
    buff_02[1] = (char)OUT_Color;
    LED_OUT0 = i2_c_LP5024_handle.write(LP5024_SLAVE_ADDRESS, buff_02, 2, false); 
    return 0;
}
int Ring_LED_D6_B(char OUT_Color)
{
    int LED_OUT0;

    char buff_02[2];
    buff_02[0] = 0x23;
    buff_02[1] = (char)OUT_Color;
    LED_OUT0 = i2_c_LP5024_handle.write(LP5024_SLAVE_ADDRESS, buff_02, 2, false); 
    return 0;
}
int Ring_LED_D1_B(char OUT_Color)
{
    int LED_OUT0;

    char buff_02[2];
    buff_02[0] = 0x17;
    buff_02[1] = (char)OUT_Color;
    LED_OUT0 = i2_c_LP5024_handle.write(LP5024_SLAVE_ADDRESS, buff_02, 2, false); 
    return 0;
}
int Ring_LED_D3_B(char OUT_Color)
{
    int LED_OUT0;

    char buff_02[2];
    buff_02[0] = 0x14;
    buff_02[1] = (char)OUT_Color;
    LED_OUT0 = i2_c_LP5024_handle.write(LP5024_SLAVE_ADDRESS, buff_02, 2, false); 
    return 0;
}
int Ring_LED_D8_B(char OUT_Color)
{
    int LED_OUT0;

    char buff_02[2];
    buff_02[0] = 0x1d;
    buff_02[1] = (char)OUT_Color;
    LED_OUT0 = i2_c_LP5024_handle.write(LP5024_SLAVE_ADDRESS, buff_02, 2, false); 
    return 0;
}
int Ring_LED_D7_B(char OUT_Color)
{
    int LED_OUT0;

    char buff_02[2];
    buff_02[0] = 0x1a;
    buff_02[1] = (char)OUT_Color;
    LED_OUT0 = i2_c_LP5024_handle.write(LP5024_SLAVE_ADDRESS, buff_02, 2, false); 
    return 0;
}
int Ring_LED_D2_B(char OUT_Color)
{
    int LED_OUT0;

    char buff_02[2];
    buff_02[0] = 0x26;
    buff_02[1] = (char)OUT_Color;
    LED_OUT0 = i2_c_LP5024_handle.write(LP5024_SLAVE_ADDRESS, buff_02, 2, false); 
    return 0;
}
int Ring_LED_D4_B(char OUT_Color)
{
    int LED_OUT0;

    char buff_02[2];
    buff_02[0] = 0x20;
    buff_02[1] = (char)OUT_Color;
    LED_OUT0 = i2_c_LP5024_handle.write(LP5024_SLAVE_ADDRESS, buff_02, 2, false); 
    return 0;
}



int IR_LED_OUT0_D2_13(char OUT_Color)
{
    int LED_OUT0;

    char buff_02[2];
    buff_02[0] = 0x0F;
    buff_02[1] = (char)OUT_Color;
    LED_OUT0 = i2_c_LP5024_handle.write(LP5024_SLAVE_ADDRESS_D2, buff_02, 2, false);
   // uint8_t data = _i2c_LP5024_readByte_D2(0x13);
   // printf("[%s:%d] LED_OUT0 read data(0x%x)=0x%x\n", __FUNCTION__, __LINE__, 0x02, data);
  //  printf("LED_OUT0 = %d\n", LED_OUT0);
    return 0;
}

int IR_LED_OUT0_D2_14(char OUT_Color)
{
    int LED_OUT0;

    char buff_02[2];
    buff_02[0] = 0x10;
    buff_02[1] = (char)OUT_Color;
    LED_OUT0 = i2_c_LP5024_handle.write(LP5024_SLAVE_ADDRESS_D2, buff_02, 2, false);
  //  uint8_t data = _i2c_LP5024_readByte_D2(0x13);
  // printf("[%s:%d] LED_OUT0 read data(0x%x)=0x%x\n", __FUNCTION__, __LINE__, 0x02, data);
  //  printf("LED_OUT0 = %d\n", LED_OUT0);
    return 0;
}

int IR_LED_OUT0_D2_11(char OUT_Color)
{
    int LED_OUT0;

    char buff_02[2];
    buff_02[0] = 0x25;
    buff_02[1] = (char)OUT_Color;
    LED_OUT0 = i2_c_LP5024_handle.write(LP5024_SLAVE_ADDRESS_D2, buff_02, 2, false);
  //  uint8_t data = _i2c_LP5024_readByte_D2(0x13);
  //  printf("[%s:%d] LED_OUT0 read data(0x%x)=0x%x\n", __FUNCTION__, __LINE__, 0x02, data);
  //  printf("LED_OUT0 = %d\n", LED_OUT0);
    return 0;
}


int IR_LED_OUT0_D2_12(char OUT_Color)
{
    int LED_OUT0;

    char buff_02[2];
    buff_02[0] = 0x26;
    buff_02[1] = (char)OUT_Color;
    LED_OUT0 = i2_c_LP5024_handle.write(LP5024_SLAVE_ADDRESS_D2, buff_02, 2, false);
  //  uint8_t data = _i2c_LP5024_readByte_D2(0x13);
  //  printf("[%s:%d] LED_OUT0 read data(0x%x)=0x%x\n", __FUNCTION__, __LINE__, 0x02, data);
  //  printf("LED_OUT0 = %d\n", LED_OUT0);
    return 0;
}

int LED_OUT23_D2_T1(char OUT_Color)
{
    int LED_OUT0;

    char buff_02[2];
    buff_02[0] = 0x23;
    buff_02[1] = (char)OUT_Color;
    LED_OUT0 = i2_c_LP5024_handle.write(LP5024_SLAVE_ADDRESS_D2, buff_02, 2, false);
  //  uint8_t data = _i2c_LP5024_readByte_D2(0x13);
  //  printf("[%s:%d] LED_OUT0 read data(0x%x)=0x%x\n", __FUNCTION__, __LINE__, 0x02, data);
  //  printf("LED_OUT0 = %d\n", LED_OUT0);
    return 0;
}

int LED_OUT22_D2_T1(char OUT_Color)
{
    int LED_OUT0;

    char buff_02[2];
    buff_02[0] = 0x22;
    buff_02[1] = (char)OUT_Color;
    LED_OUT0 = i2_c_LP5024_handle.write(LP5024_SLAVE_ADDRESS_D2, buff_02, 2, false);
  //  uint8_t data = _i2c_LP5024_readByte_D2(0x13);
  //  printf("[%s:%d] LED_OUT0 read data(0x%x)=0x%x\n", __FUNCTION__, __LINE__, 0x02, data);
  //  printf("LED_OUT0 = %d\n", LED_OUT0);
    return 0;
}

int LED_OUT21_D2_T1(char OUT_Color){
    int LED_OUT0;

    char buff_02[2];
    buff_02[0] = 0x21;
    buff_02[1] = (char)OUT_Color;
    LED_OUT0 = i2_c_LP5024_handle.write(LP5024_SLAVE_ADDRESS_D2, buff_02, 2, false);
  //  uint8_t data = _i2c_LP5024_readByte_D2(0x13);
  //  printf("[%s:%d] LED_OUT0 read data(0x%x)=0x%x\n", __FUNCTION__, __LINE__, 0x02, data);
  //  printf("LED_OUT0 = %d\n", LED_OUT0);
    return 0;
}

int LED_OUT20_D2_T2(char OUT_Color)
{
    int LED_OUT0;

    char buff_02[2];
    buff_02[0] = 0x20;
    buff_02[1] = (char)OUT_Color;
    LED_OUT0 = i2_c_LP5024_handle.write(LP5024_SLAVE_ADDRESS_D2, buff_02, 2, false);
  //  uint8_t data = _i2c_LP5024_readByte_D2(0x13);
  //  printf("[%s:%d] LED_OUT0 read data(0x%x)=0x%x\n", __FUNCTION__, __LINE__, 0x02, data);
  //  printf("LED_OUT0 = %d\n", LED_OUT0);
    return 0;
}
int LED_OUT14_D2_T3(char OUT_Color)
{
    int LED_OUT0;

    char buff_02[2];
    buff_02[0] = 0x14;
    buff_02[1] = (char)OUT_Color;
    LED_OUT0 = i2_c_LP5024_handle.write(LP5024_SLAVE_ADDRESS_D2, buff_02, 2, false);
  //  uint8_t data = _i2c_LP5024_readByte_D2(0x13);
  //  printf("[%s:%d] LED_OUT0 read data(0x%x)=0x%x\n", __FUNCTION__, __LINE__, 0x02, data);
  //  printf("LED_OUT0 = %d\n", LED_OUT0);
    return 0;
}

int LED_OUT17_D2_T4(char OUT_Color)
{
    int LED_OUT0;

    char buff_02[2];
    buff_02[0] = 0x17;
    buff_02[1] = (char)OUT_Color;
    LED_OUT0 = i2_c_LP5024_handle.write(LP5024_SLAVE_ADDRESS_D2, buff_02, 2, false);
  //  uint8_t data = _i2c_LP5024_readByte_D2(0x13);
  //  printf("[%s:%d] LED_OUT0 read data(0x%x)=0x%x\n", __FUNCTION__, __LINE__, 0x02, data);
  //  printf("LED_OUT0 = %d\n", LED_OUT0);
    return 0;
}



int LED_OUT0_D2_IRD13(char OUT_Color)
{
    int IR13;

    char buff_02[2];
    buff_02[0] = 0x0f;
    buff_02[1] = (char)OUT_Color;
    IR13 = i2_c_LP5024_handle.write(LP5024_SLAVE_ADDRESS_D2, buff_02, 2, false);
  //  uint8_t data = _i2c_LP5024_readByte_D2(0x13);
  //  printf("[%s:%d] LED_OUT0 read data(0x%x)=0x%x\n", __FUNCTION__, __LINE__, 0x02, data);
  //  printf("LED_OUT0 = %d\n", LED_OUT0);
    return 0;
}

int LED_OUT0_D2_IRD14(char OUT_Color)
{
    int IR14;

    char buff_02[2];
    buff_02[0] = 0x10;
    buff_02[1] = (char)OUT_Color;
    IR14 = i2_c_LP5024_handle.write(LP5024_SLAVE_ADDRESS_D2, buff_02, 2, false);
  //  uint8_t data = _i2c_LP5024_readByte_D2(0x13);
  //  printf("[%s:%d] LED_OUT0 read data(0x%x)=0x%x\n", __FUNCTION__, __LINE__, 0x02, data);
  //  printf("LED_OUT0 = %d\n", LED_OUT0);
    return 0;
}


int LED_OUT0_D2_IRD11(char OUT_Color)
{
    int IR11;

    char buff_02[2];
    buff_02[0] = 0x25;
    buff_02[1] = (char)OUT_Color;
    IR11 = i2_c_LP5024_handle.write(LP5024_SLAVE_ADDRESS_D2, buff_02, 2, false);
  //  uint8_t data = _i2c_LP5024_readByte_D2(0x13);
  //  printf("[%s:%d] LED_OUT0 read data(0x%x)=0x%x\n", __FUNCTION__, __LINE__, 0x02, data);
  //  printf("LED_OUT0 = %d\n", LED_OUT0);
    return 0;
}

int LED_OUT0_D2_IRD12(char OUT_Color)
{
    int IR12;

    char buff_02[2];
    buff_02[0] = 0x26;
    buff_02[1] = (char)OUT_Color;
    IR12 = i2_c_LP5024_handle.write(LP5024_SLAVE_ADDRESS_D2, buff_02, 2, false);
  //  uint8_t data = _i2c_LP5024_readByte_D2(0x13);
  //  printf("[%s:%d] LED_OUT0 read data(0x%x)=0x%x\n", __FUNCTION__, __LINE__, 0x02, data);
  //  printf("LED_OUT0 = %d\n", LED_OUT0);
    return 0;
}


int LED_Ring_Init(void){
 
    Mode_Select_Standby();
    Mode_Select_Normal();
    Mode_Set(1,1,1,1,0,0);
    LED_CONFIG0(0,0,0,0,0,0,0,0);  
     
    return 0 ;

}
int LED_Ring_01(void){

    Mode_Select_Standby();
    Mode_Select_Normal();
    Mode_Set(1,1,1,1,0,0);
    LED_CONFIG0(0,1,0,0,0,0,0,1);

    Bank_Color_Set_R(0);
    Bank_Color_Set_G(0);
    Bank_Color_Set_B(240);
    Bank_Brightness_Set(250);

    return 0;
}

int LED_Ring_02(void){

    Mode_Select_Standby();
    Mode_Select_Normal();
    Mode_Set(1,1,1,1,0,0);
    LED_CONFIG0(0,0,0,0,0,1,1,0);

    Bank_Color_Set_R(0);
    Bank_Color_Set_G(0);
    Bank_Color_Set_B(240);
    Bank_Brightness_Set(250);

    return 0;
}

int LED_Ring_03(void){

    Mode_Select_Standby();
    Mode_Select_Normal();
    Mode_Set(1,1,1,1,0,0);
    LED_CONFIG0(0,0,0,1,1,0,0,0);

    Bank_Color_Set_R(0);
    Bank_Color_Set_G(0);
    Bank_Color_Set_B(240);
    Bank_Brightness_Set(250);

    return 0;
}

int LED_Ring_04(void){

    Mode_Select_Standby();
    Mode_Select_Normal();
    Mode_Set(1,1,1,1,0,0);
    LED_CONFIG0(1,0,1,0,0,0,0,0);

    Bank_Color_Set_R(0);
    Bank_Color_Set_G(0);
    Bank_Color_Set_B(240);
    Bank_Brightness_Set(250);

    return 0;
}

int LED_Init_D1(void){
    
    Mode_Select_Standby();
    Mode_Select_Normal();
    Mode_Set(1,1,1,1,0,0);
    LED_CONFIG0(1,1,1,1,1,1,1,1);

    return 0 ;
}

int LED_D1_OFF(void){
    
    LED_Init_D1();
    Bank_Color_Set_R(0);
    Bank_Color_Set_G(0);
    Bank_Color_Set_B(0);
    Bank_Brightness_Set(0);

    return 0 ;
}
int LED_D1_OFF_G_01(void){
    
    LED_Init_D1();
    Bank_Color_Set_R(0);
    Bank_Color_Set_G(190);
    Bank_Color_Set_B(0);
    Bank_Brightness_Set(190);

    return 0 ;
}
int LED_D1_OFF_G_02(void){
    
    LED_Init_D1();
    Bank_Color_Set_R(0);
    Bank_Color_Set_G(140);
    Bank_Color_Set_B(0);
    Bank_Brightness_Set(140);

    return 0 ;
}

int LED_D1_OFF_R_01(void){
    
    LED_Init_D1();
    Bank_Color_Set_R(190);
    Bank_Color_Set_G(0);
    Bank_Color_Set_B(0);
    Bank_Brightness_Set(190);

    return 0 ;
}
int LED_D1_OFF_R_02(void){
    
    LED_Init_D1();
    Bank_Color_Set_R(140);
    Bank_Color_Set_G(0);
    Bank_Color_Set_B(0);
    Bank_Brightness_Set(140);

    return 0 ;
}


int LED_D1_ON(void){
    
    LED_Init_D1();
    Bank_Color_Set_R(140);
    Bank_Color_Set_G(250);
    Bank_Color_Set_B(190);
    Bank_Brightness_Set(250);

    return 0 ;
}


int LED_D1_RED(void){
    
    LED_Init_D1();
    Bank_Color_Set_R(250);
    Bank_Color_Set_G(0);
    Bank_Color_Set_B(0);
    Bank_Brightness_Set(250);
    return 0 ;
}

int LED_D1_ORANGE(void){
    
    LED_Init_D1();
    Bank_Color_Set_R(250);
    Bank_Color_Set_G(80);
    Bank_Color_Set_B(20);
    Bank_Brightness_Set(250);
    return 0 ;
}

int LED_D1_Yellow(void){
    
    LED_Init_D1();
    Bank_Color_Set_R(250);
    Bank_Color_Set_G(250);
    Bank_Color_Set_B(45);
    Bank_Brightness_Set(250);
    return 0 ;
}



int LED_D1_BLUE(void){
    
    LED_Init_D1();
    Bank_Color_Set_R(0);
    Bank_Color_Set_G(0);
    Bank_Color_Set_B(250);
    Bank_Brightness_Set(250);
    return 0 ;
}


int LED_D1_GREEN(void){
    
    LED_Init_D1();
    Bank_Color_Set_R(0);
    Bank_Color_Set_G(250);
    Bank_Color_Set_B(0);
    Bank_Brightness_Set(250);
    return 0 ;
}

int LED_D2_ON(void){
    
    Mode_Select_Standby_D2();
    Mode_Select_Normal_D2();
    Mode_Set_D2(1,1,1,1,0,0);
    LED_CONFIG0_D2(0,1,1,1,1,1,1,0);
    Bank_Color_Set_R(140);
    Bank_Color_Set_G(250);
    Bank_Color_Set_B(190);
    Bank_Brightness_Set_D2(230);
    return 0 ;
}

int LED_D2_FIRE(void){
    Mode_Select_Standby_D2();
    Mode_Select_Normal_D2();
    Mode_Set_D2(1,1,1,1,0,0);
    LED_CONFIG0_D2(0,1,1,1,1,1,1,0);
    Bank_Color_Set_R_D2(240);
    Bank_Color_Set_G_D2(0);
    Bank_Color_Set_B_D2(0);
    Bank_Brightness_Set_D2(200);
    return 0 ;
}

int LED_D2_Touch1_Test(void){
    Mode_Select_Standby_D2();
    Mode_Select_Normal_D2();
    Mode_Set_D2(1,1,1,1,0,0);
    LED_CONFIG0_D2(0,1,1,1,1,1,1,0);
    Bank_Color_Set_R_D2(90);
    Bank_Color_Set_G_D2(190);
    Bank_Color_Set_B_D2(250);
    Bank_Brightness_Set_D2(200);
    return 0 ;
}



int LED_D2_IR12_OFF(void){
    
    Mode_Select_Standby_D2();
    Mode_Select_Normal_D2();
    Mode_Set_D2(1,1,1,1,0,0);
    LED_CONFIG0_D2(0,1,1,1,1,1,1,0);
    LED_OUT0_D2_IRD12(0);
    Bank_Brightness_Set_D2(0);
    return 0 ;
}

int LED_D2_IR11_OFF(void){
    
    Mode_Select_Standby_D2();
    Mode_Select_Normal_D2();
    Mode_Set_D2(1,1,1,1,0,0);
    LED_CONFIG0_D2(0,1,1,1,1,1,1,0);
    LED_OUT0_D2_IRD11(0);
    Bank_Brightness_Set_D2(0);
    return 0 ;
}

int LED_D2_IR13_OFF(void){
    
    Mode_Select_Standby_D2();
    Mode_Select_Normal_D2();
    Mode_Set_D2(1,1,1,1,0,0);
    LED_CONFIG0_D2(0,1,1,1,1,1,1,0);
    LED_OUT0_D2_IRD13(200);
    Bank_Brightness_Set_D2(200);
    return 0 ;
}

int LED_D2_IR14_OFF(void){
    
    Mode_Select_Standby_D2();
    Mode_Select_Normal_D2();
    Mode_Set_D2(1,1,1,1,0,0);
    LED_CONFIG0_D2(0,1,1,1,1,1,1,0);
    LED_OUT0_D2_IRD14(0);
    Bank_Brightness_Set_D2(0);
    return 0 ;
}


int LED_D2_IR12_ON(void){
    
    Mode_Select_Standby_D2();
    Mode_Select_Normal_D2();
    Mode_Set_D2(1,1,1,1,0,0);
    LED_CONFIG0_D2(0,1,1,1,1,1,1,0);
    LED_OUT0_D2_IRD12(0);
    Bank_Brightness_Set_D2(0);
    return 0 ;
}

int LED_D2_IR11_ON(void){
    
    Mode_Select_Standby_D2();
    Mode_Select_Normal_D2();
    Mode_Set_D2(1,1,1,1,0,0);
    LED_CONFIG0_D2(0,1,1,1,1,1,1,0);
    LED_OUT0_D2_IRD11(0);
    Bank_Brightness_Set_D2(0);
    return 0 ;
}

int LED_D2_IR13_ON(void){
    
    Mode_Select_Standby_D2();
    Mode_Select_Normal_D2();
    Mode_Set_D2(1,1,1,1,0,0);
    LED_CONFIG0_D2(0,1,1,1,1,1,1,0);
    LED_OUT0_D2_IRD13(200);
    Bank_Brightness_Set_D2(200);
    return 0 ;
}

int LED_D2_IR14_ON(void){
    
    Mode_Select_Standby_D2();
    Mode_Select_Normal_D2();
    Mode_Set_D2(1,1,1,1,0,0);
    LED_CONFIG0_D2(0,1,1,1,1,1,1,0);
    LED_OUT0_D2_IRD14(0);
    Bank_Brightness_Set_D2(0);
    return 0 ;
}


int LED_D2_T1(void){
    
    Mode_Select_Standby_D2();
    Mode_Select_Normal_D2();
    Mode_Set_D2(1,1,1,1,0,0);
    LED_CONFIG0_D2(0,0,1,1,1,1,1,0);
    LED_OUT23_D2_T1(230);
    Bank_Brightness_Set_D2(230);
    return 0 ;
}


int EMI_LED_T1(void){
    
    Mode_Select_Standby_D2();
    Mode_Select_Normal_D2();
    Mode_Set_D2(1,1,1,1,0,0);
    LED_CONFIG0_D2(0,0,1,1,1,1,1,0);
    LED_OUT22_D2_T1(230);
    Bank_Brightness_Set_D2(230);
    return 0 ;
}

int EMI_LED_T1_RED(void){

    Mode_Select_Standby_D2();
    Mode_Select_Normal_D2();
    Mode_Set_D2(1,1,1,1,0,0);
    LED_CONFIG0_D2(0,0,1,1,1,1,1,0);
    LED_OUT21_D2_T1(230);
    Bank_Brightness_Set_D2(230);
    return 0 ;
}

int LED_D2_T2(void){
    
    Mode_Select_Standby_D2();
    Mode_Select_Normal_D2();
    Mode_Set_D2(1,1,1,1,0,0);
    LED_CONFIG0_D2(0,1,0,1,1,1,1,0);
    LED_OUT20_D2_T2(230);
    Bank_Brightness_Set_D2(230);
    return 0 ;
}

int LED_D2_T3(void){
    
    Mode_Select_Standby_D2();
    Mode_Select_Normal_D2();
    Mode_Set_D2(1,1,1,1,0,0);
    LED_CONFIG0_D2(0,1,1,1,1,1,0,0);
    LED_OUT14_D2_T3(230);
    Bank_Brightness_Set_D2(230);
    return 0 ;
}
int LED_D2_T4(void){
    
    Mode_Select_Standby_D2();
    Mode_Select_Normal_D2();
    Mode_Set_D2(1,1,1,1,0,0);
    LED_CONFIG0_D2(0,1,1,1,1,0,1,0);
    LED_OUT17_D2_T4(230);
    Bank_Brightness_Set_D2(230);
    return 0 ;
}

int LED_D2_OFF(void){
    
    Mode_Select_Standby_D2();
    Mode_Select_Normal_D2();
    Mode_Set_D2(1,1,1,1,0,0);
    LED_CONFIG0_D2(1,0,0,0,0,0,0,1);
    Bank_Color_Set_R_D2(250);
    Bank_Color_Set_G_D2(250);
    Bank_Color_Set_B_D2(250);
    Bank_Brightness_Set_D2(250);
    return 0 ;
}
  


int IRLED_D2_Init(void){
    Mode_Select_Standby_D2();
    Mode_Select_Normal_D2();
    Mode_Set_D2(1,1,1,1,0,0);
    LED_CONFIG0_D2(0,0,0,0,0,0,0,0);
    return 0 ;
}
int LED_D2_TEST_02(void){
    Mode_Select_Standby_D2();
    Mode_Select_Normal_D2();
    Mode_Set_D2(1,1,1,1,0,0);
    LED_CONFIG0_D2(0,0,0,0,0,0,0,0);
    IR_LED_OUT0_D2_11(250);
    IR_LED_OUT0_D2_12(250);
    IR_LED_OUT0_D2_13(250);
    IR_LED_OUT0_D2_14(250);

    return 0 ;
} 
