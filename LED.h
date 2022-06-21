#ifndef LED_H_
#define LED_H_
//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif

//extern int _i2c_LP5024_readByte(uint8_t reg);
extern void Mode_Select_Standby(void);
extern void Mode_Select_Normal(void);
extern int Mode_Set(bool Log_Scale_EN, bool Power_Save_EN, bool Auto_Incr_EN, bool PWM_Dithering_EN, bool Max_Current_Option, bool LED_Global_Off);
extern int Bank_Color_Set_R(char BANK_R_COLOR);
extern int Bank_Color_Set_G(char BANK_G_COLOR);
extern int Bank_Color_Set_B(char BANK_B_COLOR);
extern int Bank_Brightness_Set(char BANK_BRIGHTNESS);
extern int LED_CONFIG0(bool LED7_Bank_EN, bool LED6_Bank_EN, bool LED5_Bank_EN, bool LED4_Bank_EN, bool LED3_Bank_EN, bool LED2_Bank_EN, bool LED1_Bank_EN, bool LED0_Bank_EN);

//extern int _i2c_LP5024_readByte_D2(uint8_t reg);
extern int Mode_Select_Standby_D2(void);
extern int Mode_Select_Normal_D2(void);
extern int Mode_Set_D2(bool Log_Scale_EN, bool Power_Save_EN, bool Auto_Incr_EN, bool PWM_Dithering_EN, bool Max_Current_Option, bool LED_Global_Off);
extern int Bank_Color_Set_R_D2(char BANK_R_COLOR);
extern int Bank_Color_Set_G_D2(char BANK_G_COLOR);
extern int Bank_Color_Set_B_D2(char BANK_B_COLOR);
extern int Bank_Brightness_Set_D2(char BANK_BRIGHTNESS);
extern int LED_CONFIG0_D2(bool LED7_Bank_EN, bool LED6_Bank_EN, bool LED5_Bank_EN, bool LED4_Bank_EN, bool LED3_Bank_EN, bool LED2_Bank_EN, bool LED1_Bank_EN, bool LED0_Bank_EN);



extern int LED_OUT23_D2_T1(char OUT_Color);
extern int LED_OUT20_D2_T2(char OUT_Color);
extern int LED_OUT14_D2_T3(char OUT_Color);
extern int LED_OUT17_D2_T4(char OUT_Color);
extern int LED_OUT22_D2_T1(char OUT_Color);

extern int LED_Init_D1(void);
extern int LED_D1_OFF(void);
extern int LED_D1_OFF_G_01(void);
extern int LED_D1_OFF_G_02(void);
extern int LED_D1_OFF_R_01(void);
extern int LED_D1_OFF_R_02(void);
extern int LED_D1_ORANGE(void);
extern int LED_D1_Yellow(void);
extern int LED_D1_ON(void);
extern int LED_D1_RED(void);
extern int LED_D1_GREEN(void);
extern int LED_D1_BLUE(void);
extern int LED_D2_ON(void);
extern int LED_D2_ON_H(void);
extern int LED_D2_FIRE(void);

extern int LED_D2_IR14_ON(void);
extern int LED_D2_IR13_ON(void);
extern int LED_D2_IR12_ON(void);
extern int LED_D2_IR11_ON(void);

extern int LED_D2_IR14_OFF(void);
extern int LED_D2_IR13_OFF(void);
extern int LED_D2_IR12_OFF(void);
extern int LED_D2_IR11_OFF(void);

extern int LED_D2_T1(void);
extern int LED_D2_T2(void);
extern int LED_D2_T3(void);
extern int LED_D2_T4(void);
extern int EMI_LED_T1(void);
extern int EMI_LED_T1_RED(void);


extern int LED_D2_OFF(void);
extern int LED_D2_OFF_IR(void);

extern int IRLED_D2_Init(void);
extern int LED_D2_TEST_02(void);
extern int IR_LED_OUT0_D2_14(char OUT_Color);
extern int IR_LED_OUT0_D2_13(char OUT_Color);
extern int IR_LED_OUT0_D2_12(char OUT_Color);
extern int IR_LED_OUT0_D2_11(char OUT_Color);

extern int Ring_LED_D5_B(char OUT_Color);
extern int Ring_LED_D6_B(char OUT_Color);
extern int Ring_LED_D3_B(char OUT_Color);
extern int Ring_LED_D1_B(char OUT_Color);
extern int Ring_LED_D8_B(char OUT_Color);
extern int Ring_LED_D7_B(char OUT_Color);
extern int Ring_LED_D2_B(char OUT_Color);
extern int Ring_LED_D4_B(char OUT_Color);

extern int Ring_LED_D5_G(char OUT_Color);
extern int Ring_LED_D6_G(char OUT_Color);
extern int Ring_LED_D3_G(char OUT_Color);
extern int Ring_LED_D1_G(char OUT_Color);
extern int Ring_LED_D8_G(char OUT_Color);
extern int Ring_LED_D7_G(char OUT_Color);
extern int Ring_LED_D2_G(char OUT_Color);
extern int Ring_LED_D4_G(char OUT_Color);

extern int Ring_LED_D5_R(char OUT_Color);
extern int Ring_LED_D6_R(char OUT_Color);
extern int Ring_LED_D3_R(char OUT_Color);
extern int Ring_LED_D1_R(char OUT_Color);
extern int Ring_LED_D8_R(char OUT_Color);
extern int Ring_LED_D7_R(char OUT_Color);
extern int Ring_LED_D2_R(char OUT_Color);
extern int Ring_LED_D4_R(char OUT_Color);

extern int LED_D2_Touch1_Test(void);
extern int LED_Ring_Init(void);
 
extern int LED_Ring_01(void);
extern int LED_Ring_02(void);
extern int LED_Ring_03(void);
extern int LED_Ring_04(void);

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
#endif /* LED_H_ */
