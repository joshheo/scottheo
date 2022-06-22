#include "mbed.h"
#include <FlashIAPBlockDevice.h>

#include "VL53L0X.h" 
#include "LED.h"
#include "rtos.h" 
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include "mbed_stats.h"
#include "platform/CircularBuffer.h"
#include "USBSerial.h"
#include "stdlib.h" 
#include <stdio.h>
#include <ctype.h>


RawSerial   pc(USBTX, USBRX);
RawSerial   ap(AP_UART_TX, AP_UART_RX);
RawSerial   rs485(RS485_UART_TX, RS485_UART_RX);

//FlashIAP flash;


#define FIRST_DB_BLOCK (0x1008a400)
static FlashIAPBlockDevice full_flash(FIRST_DB_BLOCK, 256 * 1024);


#define USE_XSHUT   1

#if (MBED_MAJOR_VERSION == 2)
#define WAIT_MS(x)              wait_ms(x)
#define VL53L0X_OsDelay(...)    wait_ms(2)
#elif (MBED_MAJOR_VERSION == 5)
#define WAIT_MS(x)              ThisThread::sleep_for(x)
#define VL53L0X_OsDelay(...)    ThisThread::sleep_for(2)
#else
#warning "I cannot control wait_ms()!!"
#endif
 
I2C         i2c(I2C_SDA, I2C_SCL);    //TOF I2C
 
VL53L0X     sensor(i2c, P13_4);

DigitalOut relay(RELAY_11_0);
DigitalOut xShut(P13_4);
DigitalOut RS485_GPIO(test_t_11_1);
//DigitalOut AP_Reset(AP_Reset_11_4);
DigitalOut Powerfail(GPIO_10_3);
DigitalOut Fire_alarm_Out(Fire_a_9_1);
DigitalOut GPO(GPO_9_2);  // 0 -> 5V  1-> 0V
DigitalOut Service_Required(Service_7_7);
DigitalOut temperature(Temperature_10_6);
 

DigitalOut Wiegand_out_D0(Wiegand_out_D0_9_3); // 1 on, 0 off
DigitalOut Wiegand_out_D1(Wiegand_out_D1_9_4); // 1 on, 0 off

InterruptIn Buzzer_in(Buzzer_6_3);
InterruptIn touch_T1(TOUCH_7_2); 
InterruptIn touch_T2(TOUCH_7_4);
InterruptIn touch_T3(TOUCH_7_5);
InterruptIn touch_T4(TOUCH_7_6);
InterruptIn GPI(GPI_10_7);

InterruptIn Fire_alarm_in_A(Fire_a_8_7);
InterruptIn Release_fire_alarm(Fire_r_10_5);
InterruptIn Exit_Button(Exit_b_8_5); // D 9.2V  0->1
InterruptIn Fire_alarm_in_B(Fire_a_9_0);
InterruptIn InterruptIn_Door_status_01(Door_s_8_3); // D 9.2V  0->1  
InterruptIn InterruptIn_Door_status_02(Door_s_8_4); // D 9.2V  0->1 
  
DigitalIn Hold_input(Hold_10_4);
DigitalIn Tamper(Tamper_10_2); // 1(W) on, 0(B) off
DigitalIn Door_status_01(Door_s_8_3); // D 9.2V  0->1
DigitalIn Door_status_02(Door_s_8_4); // D 9.2V  0->1 
DigitalIn RTX(Rtx_8_6);
/*
DigitalIn Fire_B_status(Fire_a_9_0);
DigitalIn Fire_A_status(Fire_a_8_7);
DigitalIn Fire_release_status(Fire_r_10_5);
DigitalIn Exit_Button_status(Exit_b_8_5); // D 9.2V  0->1
*/
DigitalIn Wiegand_in_D0(Wiegand_in_D1_9_5); // 1 on, 0 off
DigitalIn Wiegand_in_D1(Wiegand_in_D1_9_6); // 1 on, 0 off

Timer       t_2;
Timer       Factory_D;
Timer       LED_T;
Timer       Watchdog_T;
Timer       Hotel_T;

Thread thread_TOF;
Thread thread_LED;

char RFID_Buzzer[8] = {0x01, 0x0A, 0x06, 0x00, 0x00, 0x00, 0x00, 0x03};

const char *const msg0  = "VL53L0X is running correctly!!\r\n";
const char *const msg1  = "VL53L0X -> something is wrong!!\r\n"; 

static int TOF_Start, Tamper_data, start_main, Start_Led_Flag;
static int led_flag; // 1 LED OFF, 0 LED ON
static int Tamper_flag, verify_init_flag, Relay_status_flag, ir_led_flag;
static int LED_Ring_stop_flag, LED_Ring_flag, LED_Ring_Tof_flag, LED_Ring_Tof_buzzer, success_flag; 
static int T_1,T_2,T_3,T_4 = 0;
static int string_possition;
static int pulse_width, pulse_interval;
static int Verify_buffer_count, string_485, tamper_led_flag, relay_delay_len, Relay_flag;

static int MLX90614_tof_data, TOF_flag,  MLX90614_tof_data_count, MLX90614_tof_data_flag, MLX90614_data_flag;
static int Watchdog_flag, Watchdog_reset_flag, relay_status, Hold_status, Hotel_T_flag;
static int Exit, Fire_Alarm_A, Fire_Alarm_B, Fire_Release_S; //IO_out 0->1 

int LED_Start_R = 70;
int LED_Start_G = 150;
int LED_Start_B = 200;
int Touch_led_flag = 0;
int Touch_flag_last = 0;
int Fire_A_Flag, Fire_Release_Flag, Exit_button_flag, Fire_B_flag, Door_status_01_flag, Door_status_02_flag;
int Door_status_02_flag_fall, Hotel_led_flag, RS485_flag;
int Door_status_open, Door_status_close, Door_status_open_01;

char pc_buffer[2]; 
char buffer_ch[60]; 
char AP_Buffer[60];
char rs485_message[10]; 
char rs485_Buffer[14] = {0x01, 0x0B, 0x00, 0x01, 0x00, 0x06};
char out_data_v[12] = {0x01, 0x0A, 0x01, 0x00, 0x00, 0x04, 0x38, 0x31, 0x39, 0x30, 0x00, 0x03};
char AP_start[10] = {0x01, 0x0A, 0x11, 0x00, 0x00, 0x01, 0x01, 0x00, 0x03};
char IO_Buffer[9] = {0x01, 0x0A, 0x07, 0x00, 0x00, 0x01};
char IO_Buffer_Close[9] = {0x01, 0x0A, 0x07, 0x01, 0x00, 0x01};
char Tamper_Buffer_ON[12] = {0x01, 0x0A, 0x09, 0x01, 0x00, 0x00, 0x00, 0x03};
char Tamper_Buffer_OFF[12] = {0x01, 0x0A, 0x09, 0x00, 0x00, 0x00, 0x00, 0x03};
char update_buffer[11];


char Idle_buffer[10] = {0x01, 0x0A, 0x02, 0x01, 0x00, 0x00, 0x00, 0x00, 0x03};
char Start_buffer[10] = {0x01, 0x0A, 0x02, 0x01, 0x00, 0x01, 0x01, 0x00, 0x03};
char Fail_buffer[10] = {0x01, 0x0A, 0x02, 0x01, 0x00, 0x01, 0x03, 0x00, 0x03};
char Fake_buffer[10] = {0x01, 0x0A, 0x02, 0x01, 0x00, 0x01, 0x04, 0x00, 0x03};
char AP_Watchdog[10] = {0x01, 0x0A, 0x11, 0x00, 0x00, 0x01, 0x02, 0x00, 0x03};

char Mbed_os_version_Buffer[12] = {0x01, 0x0a, 0x08, 0x00, 0x00, 0x03, 0x01, 0x01, 0x00, 0x00, 0x03};

uint32_t addr = 0x1004a400;

void uart_pc(){
    
    pc_buffer[0] = pc.getc();
}
 
// ---------------------------------- Start LED ----------------------------------

void ring_stop(void){
    Ring_LED_D1_R(0);
    Ring_LED_D1_G(0);
    Ring_LED_D1_B(0);

    Ring_LED_D4_R(0);
    Ring_LED_D4_B(0);
    Ring_LED_D4_G(0);

    Ring_LED_D2_R(0);
    Ring_LED_D2_B(0);
    Ring_LED_D2_G(0);

    Ring_LED_D3_R(0);
    Ring_LED_D3_B(0);
    Ring_LED_D3_G(0);

    Ring_LED_D5_R(0);
    Ring_LED_D5_B(0);
    Ring_LED_D5_G(0);

    Ring_LED_D6_R(0);
    Ring_LED_D6_B(0);
    Ring_LED_D6_G(0); 
    
    Ring_LED_D8_R(0);
    Ring_LED_D8_B(0);
    Ring_LED_D8_G(0);

    Ring_LED_D7_R(0);
    Ring_LED_D7_B(0);
    Ring_LED_D7_G(0);  
}

void ring_Start(void){

    ring_stop();
        Ring_LED_D5_R(LED_Start_R);
        Ring_LED_D5_B(LED_Start_B);
        Ring_LED_D5_G(LED_Start_G);  
        Ring_LED_D6_R(LED_Start_R);
        Ring_LED_D6_B(LED_Start_B);
        Ring_LED_D6_G(LED_Start_G); 
        Cy_SysLib_Delay(95);  
    ring_stop();  
        Ring_LED_D3_R(LED_Start_R);
        Ring_LED_D3_B(LED_Start_B);
        Ring_LED_D3_G(LED_Start_G);  
        Ring_LED_D1_R(LED_Start_R);
        Ring_LED_D1_B(LED_Start_B);
        Ring_LED_D1_G(LED_Start_G); 
        Cy_SysLib_Delay(95); 
    ring_stop(); 
        Ring_LED_D8_R(LED_Start_R);
        Ring_LED_D8_B(LED_Start_B);
        Ring_LED_D8_G(LED_Start_G);
        Ring_LED_D7_R(LED_Start_R);
        Ring_LED_D7_B(LED_Start_B);
        Ring_LED_D7_G(LED_Start_G); 
        Cy_SysLib_Delay(95); 
    ring_stop();
        Ring_LED_D4_R(LED_Start_R);
        Ring_LED_D4_B(LED_Start_B);
        Ring_LED_D4_G(LED_Start_G); 
        Ring_LED_D2_R(LED_Start_R);
        Ring_LED_D2_B(LED_Start_B);
        Ring_LED_D2_G(LED_Start_G); 
        Cy_SysLib_Delay(95);  

}
void ring_Start_TOF(void){

    ring_stop();
        Ring_LED_D5_R(LED_Start_R);
        Ring_LED_D5_B(LED_Start_B);
        Ring_LED_D5_G(LED_Start_G);  
        Ring_LED_D6_R(LED_Start_R);
        Ring_LED_D6_B(LED_Start_B);
        Ring_LED_D6_G(LED_Start_G); 
        Cy_SysLib_Delay(23);  
    ring_stop();  
        Ring_LED_D3_R(LED_Start_R);
        Ring_LED_D3_B(LED_Start_B);
        Ring_LED_D3_G(LED_Start_G);  
        Ring_LED_D1_R(LED_Start_R);
        Ring_LED_D1_B(LED_Start_B);
        Ring_LED_D1_G(LED_Start_G); 
        Cy_SysLib_Delay(23); 
    ring_stop(); 
        Ring_LED_D8_R(LED_Start_R);
        Ring_LED_D8_B(LED_Start_B);
        Ring_LED_D8_G(LED_Start_G);
        Ring_LED_D7_R(LED_Start_R);
        Ring_LED_D7_B(LED_Start_B);
        Ring_LED_D7_G(LED_Start_G); 
        Cy_SysLib_Delay(23); 
    ring_stop();
        Ring_LED_D4_R(LED_Start_R);
        Ring_LED_D4_B(LED_Start_B);
        Ring_LED_D4_G(LED_Start_G); 
        Ring_LED_D2_R(LED_Start_R);
        Ring_LED_D2_B(LED_Start_B);
        Ring_LED_D2_G(LED_Start_G); 
        Cy_SysLib_Delay(23);  

}
void ring_Start_TOF_01(void){
 
        Ring_LED_D4_R(LED_Start_R);
        Ring_LED_D4_B(LED_Start_B);
        Ring_LED_D4_G(LED_Start_G); 
        Cy_SysLib_Delay(20);  
 
        Ring_LED_D2_R(LED_Start_R);
        Ring_LED_D2_B(LED_Start_B);
        Ring_LED_D2_G(LED_Start_G); 
        Cy_SysLib_Delay(20);  
 

        Ring_LED_D3_R(LED_Start_R);
        Ring_LED_D3_B(LED_Start_B);
        Ring_LED_D3_G(LED_Start_G);  
        Cy_SysLib_Delay(20);
 
        Ring_LED_D1_R(LED_Start_R);
        Ring_LED_D1_B(LED_Start_B);
        Ring_LED_D1_G(LED_Start_G);
        Cy_SysLib_Delay(20);
        
        ring_stop();
        Cy_SysLib_Delay(50);


        Ring_LED_D1_R(LED_Start_R);
        Ring_LED_D1_B(LED_Start_B);
        Ring_LED_D1_G(LED_Start_G);
        Cy_SysLib_Delay(20);

        Ring_LED_D3_R(LED_Start_R);
        Ring_LED_D3_B(LED_Start_B);
        Ring_LED_D3_G(LED_Start_G);  
        Cy_SysLib_Delay(20);

        Ring_LED_D2_R(LED_Start_R);
        Ring_LED_D2_B(LED_Start_B);
        Ring_LED_D2_G(LED_Start_G); 
        Cy_SysLib_Delay(20);

        Ring_LED_D4_R(LED_Start_R);
        Ring_LED_D4_B(LED_Start_B);
        Ring_LED_D4_G(LED_Start_G); 
        Cy_SysLib_Delay(20);  

        ring_stop();

}


// ---------------------------------- buzzer LED ----------------------------------
void ring_S_TOF(void){

    if(LED_Ring_stop_flag == 1){  
        return;
    }
        Ring_LED_D5_B(100);
        Ring_LED_D5_G(100);
        Ring_LED_D5_R(100);

        Ring_LED_D1_B(100);
        Ring_LED_D1_G(100);
        Ring_LED_D1_R(100);

        Ring_LED_D7_B(100);
        Ring_LED_D7_G(100);
        Ring_LED_D7_R(100);

        Ring_LED_D4_B(100);
        Ring_LED_D4_G(100);
        Ring_LED_D4_R(100);
        Cy_SysLib_Delay(10);
    if(LED_Ring_stop_flag == 1){  
        return;
    }
        Ring_LED_D5_B(230);
        Ring_LED_D5_G(230);
        Ring_LED_D5_R(230);

        Ring_LED_D1_B(230);
        Ring_LED_D1_G(230);
        Ring_LED_D1_R(230);

        Ring_LED_D7_B(230);
        Ring_LED_D7_G(230);
        Ring_LED_D7_R(230);

        Ring_LED_D4_B(230);
        Ring_LED_D4_G(230);
        Ring_LED_D4_R(230);
        Cy_SysLib_Delay(10); 
    if(LED_Ring_stop_flag == 1){  
        return;
    }
        Ring_LED_D6_B(100);
        Ring_LED_D6_G(100);
        Ring_LED_D6_R(100);

        Ring_LED_D3_B(100);
        Ring_LED_D3_G(100);
        Ring_LED_D3_R(100);

        Ring_LED_D2_B(100);
        Ring_LED_D2_G(100);
        Ring_LED_D2_R(100);

        Ring_LED_D8_B(100);
        Ring_LED_D8_G(100);
        Ring_LED_D8_R(100);
        Cy_SysLib_Delay(10); 
    if(LED_Ring_stop_flag == 1){  
        return;
    }
        Ring_LED_D5_B(150);
        Ring_LED_D5_G(150);
        Ring_LED_D5_R(150);

        Ring_LED_D1_B(150);
        Ring_LED_D1_G(150);
        Ring_LED_D1_R(150);

        Ring_LED_D7_B(150);
        Ring_LED_D7_G(150);
        Ring_LED_D7_R(150);

        Ring_LED_D4_B(150);
        Ring_LED_D4_G(150);
        Ring_LED_D4_R(150);

        Ring_LED_D6_B(230);
        Ring_LED_D6_G(230);
        Ring_LED_D6_R(230);

        Ring_LED_D3_B(230);
        Ring_LED_D3_G(230);
        Ring_LED_D3_R(230);

        Ring_LED_D2_B(230);
        Ring_LED_D2_G(230);
        Ring_LED_D2_R(230);

        Ring_LED_D8_B(230);
        Ring_LED_D8_G(230);
        Ring_LED_D8_R(230);
        Cy_SysLib_Delay(10); 
    if(LED_Ring_stop_flag == 1){  
        return;
    }
        Ring_LED_D5_B(50);
        Ring_LED_D5_G(50);
        Ring_LED_D5_R(50);

        Ring_LED_D1_B(50);
        Ring_LED_D1_G(50);
        Ring_LED_D1_R(50);

        Ring_LED_D7_B(50);
        Ring_LED_D7_G(50);
        Ring_LED_D7_R(50);

        Ring_LED_D4_B(50);
        Ring_LED_D4_G(50);
        Ring_LED_D4_R(50);
        Cy_SysLib_Delay(10);
    if(LED_Ring_stop_flag == 1){  
        return;
    }
        Ring_LED_D6_B(100);
        Ring_LED_D6_G(100);
        Ring_LED_D6_R(100);

        Ring_LED_D3_B(100);
        Ring_LED_D3_G(100);
        Ring_LED_D3_R(100);

        Ring_LED_D2_B(100);
        Ring_LED_D2_G(100);
        Ring_LED_D2_R(100);

        Ring_LED_D8_B(100);
        Ring_LED_D8_G(100);
        Ring_LED_D8_R(100);
        Cy_SysLib_Delay(10);
    if(LED_Ring_stop_flag == 1){  
        return;
    }
        Ring_LED_D5_B(0);
        Ring_LED_D5_G(0);
        Ring_LED_D5_R(0);

        Ring_LED_D1_B(0);
        Ring_LED_D1_G(0);
        Ring_LED_D1_R(0);

        Ring_LED_D7_B(0);
        Ring_LED_D7_G(0);
        Ring_LED_D7_R(0);

        Ring_LED_D4_B(0);
        Ring_LED_D4_G(0);
        Ring_LED_D4_R(0);

        Ring_LED_D6_B(20);
        Ring_LED_D6_G(20);
        Ring_LED_D6_R(20);

        Ring_LED_D3_B(20);
        Ring_LED_D3_G(20);
        Ring_LED_D3_R(20);

        Ring_LED_D2_B(20);
        Ring_LED_D2_G(20);
        Ring_LED_D2_R(20);

        Ring_LED_D8_B(20);
        Ring_LED_D8_G(20);
        Ring_LED_D8_R(20);
        Cy_SysLib_Delay(10);

}

void ring_P_TOF(void){

    if(LED_Ring_stop_flag == 1){  
        return;
    }
        Ring_LED_D6_B(100);
        Ring_LED_D6_G(100);
        Ring_LED_D6_R(100);

        Ring_LED_D3_B(100);
        Ring_LED_D3_G(100);
        Ring_LED_D3_R(100);

        Ring_LED_D2_B(100);
        Ring_LED_D2_G(100);
        Ring_LED_D2_R(100);

        Ring_LED_D8_B(100);
        Ring_LED_D8_G(100);
        Ring_LED_D8_R(100);
        Cy_SysLib_Delay(10);
    if(LED_Ring_stop_flag == 1){  
        return;
    }
        Ring_LED_D6_B(230);
        Ring_LED_D6_G(230);
        Ring_LED_D6_R(230);

        Ring_LED_D3_B(230);
        Ring_LED_D3_G(230);
        Ring_LED_D3_R(230);

        Ring_LED_D2_B(230);
        Ring_LED_D2_G(230);
        Ring_LED_D2_R(230);

        Ring_LED_D8_B(230);
        Ring_LED_D8_G(230);
        Ring_LED_D8_R(230);
        Cy_SysLib_Delay(10); 
        
        Ring_LED_D5_B(100);
        Ring_LED_D5_G(100);
        Ring_LED_D5_R(100);

        Ring_LED_D1_B(100);
        Ring_LED_D1_G(100);
        Ring_LED_D1_R(100);

        Ring_LED_D7_B(100);
        Ring_LED_D7_G(100);
        Ring_LED_D7_R(100);

        Ring_LED_D4_B(100);
        Ring_LED_D4_G(100);
        Ring_LED_D4_R(100);
    if(LED_Ring_stop_flag == 1){  
        return;
    }
        Cy_SysLib_Delay(10); 
        Ring_LED_D6_B(150);
        Ring_LED_D6_G(150);
        Ring_LED_D6_R(150);

        Ring_LED_D3_B(150);
        Ring_LED_D3_G(150);
        Ring_LED_D3_R(150);

        Ring_LED_D2_B(150);
        Ring_LED_D2_G(150);
        Ring_LED_D2_R(150);

        Ring_LED_D8_B(150);
        Ring_LED_D8_G(150);
        Ring_LED_D8_R(150);

        Ring_LED_D5_B(230);
        Ring_LED_D5_G(230);
        Ring_LED_D5_R(230);

        Ring_LED_D1_B(230);
        Ring_LED_D1_G(230);
        Ring_LED_D1_R(230);

        Ring_LED_D7_B(230);
        Ring_LED_D7_G(230);
        Ring_LED_D7_R(230);

        Ring_LED_D4_B(230);
        Ring_LED_D4_G(230);
        Ring_LED_D4_R(230);
        Cy_SysLib_Delay(10);
    if(LED_Ring_stop_flag == 1){  
        return;
    }
        Ring_LED_D6_B(50);
        Ring_LED_D6_G(50);
        Ring_LED_D6_R(50);

        Ring_LED_D3_B(50);
        Ring_LED_D3_G(50);
        Ring_LED_D3_R(50);

        Ring_LED_D2_B(50);
        Ring_LED_D2_G(50);
        Ring_LED_D2_R(50);

        Ring_LED_D8_B(50);
        Ring_LED_D8_G(50);
        Ring_LED_D8_R(50);
        Cy_SysLib_Delay(10);
    if(LED_Ring_stop_flag == 1){  
        return;
    }
        Ring_LED_D5_B(100);
        Ring_LED_D5_G(100);
        Ring_LED_D5_R(100);

        Ring_LED_D1_B(100);
        Ring_LED_D1_G(100);
        Ring_LED_D1_R(100);

        Ring_LED_D7_B(100);
        Ring_LED_D7_G(100);
        Ring_LED_D7_R(100);

        Ring_LED_D4_B(100);
        Ring_LED_D4_G(100);
        Ring_LED_D4_R(100);
        Cy_SysLib_Delay(10);
    if(LED_Ring_stop_flag == 1){  
        return;
    }
        Ring_LED_D6_B(0);
        Ring_LED_D6_G(0);
        Ring_LED_D6_R(0);

        Ring_LED_D3_B(0);
        Ring_LED_D3_G(0);
        Ring_LED_D3_R(0);

        Ring_LED_D2_B(0);
        Ring_LED_D2_G(0);
        Ring_LED_D2_R(0);

        Ring_LED_D8_B(0);
        Ring_LED_D8_G(0);
        Ring_LED_D8_R(0);

        Ring_LED_D5_B(55);
        Ring_LED_D5_G(55);
        Ring_LED_D5_R(55);

        Ring_LED_D1_B(55);
        Ring_LED_D1_G(55);
        Ring_LED_D1_R(55);

        Ring_LED_D7_B(55);
        Ring_LED_D7_G(55);
        Ring_LED_D7_R(55);

        Ring_LED_D4_B(55);
        Ring_LED_D4_G(55);
        Ring_LED_D4_R(55);
        Cy_SysLib_Delay(10); 
}

void ring_stop_TOF(void){ 
        Ring_LED_D6_G(0);
        Ring_LED_D6_R(0);
 
        Ring_LED_D3_G(0);
        Ring_LED_D3_R(0);
 
        Ring_LED_D2_G(0);
        Ring_LED_D2_R(0);
 
        Ring_LED_D8_G(0);
        Ring_LED_D8_R(0);  
 
        Ring_LED_D5_G(0);
        Ring_LED_D5_R(0);
 
        Ring_LED_D1_G(0);
        Ring_LED_D1_R(0);
 
        Ring_LED_D7_G(0);
        Ring_LED_D7_R(0);
 
        Ring_LED_D4_G(0);
        Ring_LED_D4_R(0);

}

void ring_S(void){

    if(LED_Ring_stop_flag == 1){  
        return;
    }
        Ring_LED_D5_B(100);
        Ring_LED_D1_B(100);
        Ring_LED_D7_B(100);
        Ring_LED_D4_B(100);
        Cy_SysLib_Delay(10);
    if(LED_Ring_stop_flag == 1){  
        return;
    }
        Ring_LED_D5_B(230);
        Ring_LED_D1_B(230);
        Ring_LED_D7_B(230);
        Ring_LED_D4_B(230); 
        Cy_SysLib_Delay(10); 
    if(LED_Ring_stop_flag == 1){  
        return;
    }
        Ring_LED_D6_B(100);
        Ring_LED_D3_B(100);
        Ring_LED_D2_B(100);
        Ring_LED_D8_B(100);
        Cy_SysLib_Delay(10); 
    if(LED_Ring_stop_flag == 1){  
        return;
    }
        Ring_LED_D5_B(150);
        Ring_LED_D1_B(150);
        Ring_LED_D7_B(150);
        Ring_LED_D4_B(150);

        Ring_LED_D6_B(230);
        Ring_LED_D3_B(230);
        Ring_LED_D2_B(230);
        Ring_LED_D8_B(230);
        Cy_SysLib_Delay(10); 
    if(LED_Ring_stop_flag == 1){  
        return;
    }
        Ring_LED_D5_B(50);
        Ring_LED_D1_B(50);
        Ring_LED_D7_B(50);
        Ring_LED_D4_B(50);
        Cy_SysLib_Delay(10);
    if(LED_Ring_stop_flag == 1){  
        return;
    }
        Ring_LED_D6_B(100);
        Ring_LED_D3_B(100);
        Ring_LED_D2_B(100);
        Ring_LED_D8_B(100);
        Cy_SysLib_Delay(10);
    if(LED_Ring_stop_flag == 1){  
        return;
    }
        Ring_LED_D5_B(0);
        Ring_LED_D1_B(0);
        Ring_LED_D7_B(0);
        Ring_LED_D4_B(0);

        Ring_LED_D6_B(20);
        Ring_LED_D3_B(20);
        Ring_LED_D2_B(20);
        Ring_LED_D8_B(20); 
        Cy_SysLib_Delay(10);

}
void ring_P(void){

    if(LED_Ring_stop_flag == 1){  
        return;
    }
        Ring_LED_D6_B(100);
        Ring_LED_D3_B(100);
        Ring_LED_D2_B(100);
        Ring_LED_D8_B(100);
        Cy_SysLib_Delay(10);
    if(LED_Ring_stop_flag == 1){  
        return;
    }
        Ring_LED_D6_B(230);
        Ring_LED_D3_B(230);
        Ring_LED_D2_B(230);
        Ring_LED_D8_B(230);
        Cy_SysLib_Delay(10); 
        
        Ring_LED_D5_B(100);
        Ring_LED_D1_B(100);
        Ring_LED_D7_B(100);
        Ring_LED_D4_B(100);
    if(LED_Ring_stop_flag == 1){  
        return;
    }
        Cy_SysLib_Delay(10); 
        Ring_LED_D6_B(150);
        Ring_LED_D3_B(150);
        Ring_LED_D2_B(150);
        Ring_LED_D8_B(150);

        Ring_LED_D5_B(230);
        Ring_LED_D1_B(230);
        Ring_LED_D7_B(230);
        Ring_LED_D4_B(230);
        Cy_SysLib_Delay(10);
    if(LED_Ring_stop_flag == 1){  
        return;
    }
        Ring_LED_D6_B(50);
        Ring_LED_D3_B(50);
        Ring_LED_D2_B(50);
        Ring_LED_D8_B(50);
        Cy_SysLib_Delay(10);
    if(LED_Ring_stop_flag == 1){  
        return;
    }
        Ring_LED_D5_B(100);
        Ring_LED_D1_B(100);
        Ring_LED_D7_B(100);
        Ring_LED_D4_B(100);
        Cy_SysLib_Delay(10);
    if(LED_Ring_stop_flag == 1){  
        return;
    }
        Ring_LED_D6_B(0);
        Ring_LED_D3_B(0);
        Ring_LED_D2_B(0);
        Ring_LED_D8_B(0);

        Ring_LED_D5_B(55);
        Ring_LED_D1_B(55);
        Ring_LED_D7_B(55);
        Ring_LED_D4_B(55);
        Cy_SysLib_Delay(10); 
}
// ------------------------------------ LED Dimming -------------------------------
void LED_dimming(void){

    for(int i=30; i<160; i++){
        
        if(led_flag == 1){
            break;
        }
		Bank_Brightness_Set(i);
		Cy_SysLib_Delay(7);
    }
    
    for(int j=160; j<230; j++){

        if(led_flag == 1){
            break;
        }
		Bank_Brightness_Set(j);
		Cy_SysLib_Delay(10);
    }

    for(int l=230; l<250; l++){

        if(led_flag == 1){
            break;
        }
        Bank_Brightness_Set(l);
		Cy_SysLib_Delay(29);//20
    }
    
    for(int m=250; m>230; m--){

        if(led_flag == 1){
            break;
        }
		Bank_Brightness_Set(m);
		Cy_SysLib_Delay(29);
    }

    for(int n=230; n>160; n--){

        if(led_flag == 1){
            break;
        }
		Bank_Brightness_Set(n);
		Cy_SysLib_Delay(10);
    }

    for(int o=160; o>30; o--){ //30
        
        if(led_flag == 1){
            break;
        }
		Bank_Brightness_Set(o);
		Cy_SysLib_Delay(7);//5
    }
}
 
// ----------------------------------- RS485 arraycopy -------------------------------
void rs485_arrayCopy(char *dst, char *src) {
	int i;
	for (i = 0; i < 6; i++) {
		dst[i+6] = src[i];
	}
}

// ----------------------------------- RS485 Callback -------------------------------
void rx_rs485_Callback_01(){

    char R485;
    do
    {
        R485 = rs485.getc();
        rs485_message[string_485] = R485;
        string_485++; 
    } 
    while(rs485.readable());
    if(string_485 == 9){
        RS485_flag = 1;
    }

    if(string_485 > 18){

        memset(rs485_message, 0, sizeof(rs485_message));
        string_485 = 0;

    }
    /*
    pc.printf("   RS485 data :  ");
    
    for(int j=0; j<string_485 ; j++){  
        pc.putc(rs485_message[j]); 
    }
    */

}
void rx_rs485_Callback() {  

    //device.putc(rs485.getc());
    char R485; 
    RS485_GPIO = 0;
    do
    {
        R485 = rs485.getc();
        rs485_message[string_485] = R485;
        string_485++; 
    } 
    while(rs485.readable());  
    pc.printf("   RS485 data :  ");
    
    for(int j=0; j<string_485 ; j++){  
        pc.putc(rs485_message[j]); 
    }
     
    if(string_485 == 6){
        rs485_arrayCopy(rs485_Buffer, rs485_message);
        rs485_Buffer[12] = 0x00;
        rs485_Buffer[13] = 0x03;
        //device.printf("RS485 data :    ");
        for(int j=0; j<14 ; j++){  
            ap.putc(rs485_Buffer[j]); 
        } 
        string_485 = 0;
        RS485_GPIO = 1;
        TOF_Start = 1;
        memset(rs485_message, 0, sizeof(rs485_message)); 
        string_possition = 0;
    }
    if(string_485 > 6){
        RS485_GPIO = 1;
        string_485 = 0; 
        memset(rs485_message, 0, sizeof(rs485_message)); 
        string_possition = 0;
        pc.printf("  RS485 RESET  ");
    }
    
}
//----------------------------------- Verify --------------------------------------
void verify(){
    
    if( string_possition >= 70 ){
        memset(AP_Buffer, 0, sizeof(AP_Buffer));
        pc.printf(" AP_Buffer3 ReSet  ");
    }
    
    if((string_possition >= 2)){
        if( AP_Buffer[00] == 0x01){
            if(AP_Buffer[01] == 0x01){
                memset(AP_Buffer, 0, sizeof(AP_Buffer));
                pc.printf("AP_Buffer2 Reset");
            }
        }
    }
    if((string_possition >= 2)){
        if( AP_Buffer[00] == 0x02 ){
            if(AP_Buffer[01] == 0x00){
                memset(AP_Buffer, 0, sizeof(AP_Buffer));
                pc.printf(" AP_Buffer ReSet  ");
            }
        }
    }

    if( (string_possition >= 2) ){
        if( (AP_Buffer[00] != 0x01) ){
            if( (AP_Buffer[00] != 0x02) ){
                if( (AP_Buffer[01] != 0x0a) ||
                    (AP_Buffer[01] != 0x0b) ||
                    (AP_Buffer[01] != 0x01)   ){  
                    
                    pc.printf("  Verify ELSE  "); 
                    pc.printf(" Verify String : %d ",string_possition);

                    for(int o=0; o<string_possition ; o++){
                        pc.putc(AP_Buffer[o]);
                    }


                    for(int e=0; e<string_possition-1; e++){ 
                        buffer_ch[e] = AP_Buffer[e+1]; 
                        Verify_buffer_count++;
                    }
                    memset(AP_Buffer, 0, sizeof(AP_Buffer));
                    
                    for(int e=0; e<Verify_buffer_count; e++){
                        AP_Buffer[e] = buffer_ch[e];
                    }

                    pc.printf("  Verify next  : %d  ",Verify_buffer_count);
                    for(int o=0; o<Verify_buffer_count ; o++){
                        pc.putc(AP_Buffer[o]);
                    }
                    string_possition = Verify_buffer_count;
                    Verify_buffer_count = 0;
                    verify_init_flag = 0;
                    }
            }
        }
    }



    if(AP_Buffer[0] == 0x02){ 
        if(AP_Buffer[01] == 0x01){ 
            pulse_width = 0;
            pulse_interval = 0;
            int setting_length;
            
            for(int o=0; o<string_possition; o++){
                pc.putc(AP_Buffer[o]);
            }

            pulse_width = atoi(&AP_Buffer[8]);
            pc.printf("width %d  ",pulse_width);

            setting_length = AP_Buffer[7];
            pc.printf("setting len %d   ",setting_length);

            pulse_interval = atoi(&AP_Buffer[10 + setting_length]);
            pc.printf("pulse_interval %d  ",pulse_interval);


            memset(AP_Buffer, 0, sizeof(AP_Buffer)); 
            string_possition = 0;

        }
    } 
    if((AP_Buffer[00] == 0x01) && (AP_Buffer[01] == 0x0b)){
        pc.printf("  RS485  ");
 /*
        if(AP_Buffer[03] == 0x00){

            string_485 = 0;
            RS485_GPIO = 1;
            TOF_Start = 0;
            char RFID_Card_Number[15];
            
            for(int g=0; g<14; g++){
                RFID_Card_Number[g] = AP_Buffer[g+6];   
            }
            pc.printf("  ");
            for(int h=0; h<14; h++){
                pc.putc(RFID_Card_Number[h]);
                rs485.putc(RFID_Card_Number[h]);
            }
            pc.printf("  ");
        }
        */
        //if(AP_Buffer[03] == 0x01){
            unsigned char rs485_data[3] = {0x55, 0x01, 0xA8}; 
            string_485 = 0;
            RS485_GPIO = 1;
            TOF_Start = 0;
            
            Cy_SysLib_Delay(10);
            for(int g=0; g<3; g++){
                rs485.putc(rs485_data[g]);  
            } 
            Cy_SysLib_Delay(10);
            RS485_GPIO = 0;
        //}

        if((AP_Buffer[11] == 0x00) && (AP_Buffer[0] != 0x00)){
                    pc.printf("  RS485 RESET  "); 
                    memset(AP_Buffer, 0, sizeof(AP_Buffer));
                    string_possition = 0;
        }
        else{

            pc.printf("  RS485 ELSE  "); 
            for(int e=11; e<string_possition; e++){ 
                buffer_ch[Verify_buffer_count] = AP_Buffer[e];
                AP_Buffer[Verify_buffer_count] = buffer_ch[Verify_buffer_count];
                Verify_buffer_count++;
            }
            memset(AP_Buffer, 0, sizeof(AP_Buffer));

            for(int e=0; e<Verify_buffer_count; e++){
                AP_Buffer[e] = buffer_ch[e];
            }
            
            pc.printf("  RS485 next : %d  ",Verify_buffer_count);
            for(int o=0; o<Verify_buffer_count; o++){
                pc.putc(AP_Buffer[o]);
            }
            
            string_possition = string_possition - Verify_buffer_count;
            Verify_buffer_count = 0; 
        }
        memset(rs485_message, 0, sizeof(rs485_message));
        TOF_Start = 1;
    }
    Cy_SysLib_Delay(30);
    
    if( (AP_Buffer[00] == 0x01) && (AP_Buffer[01] == 0x0a) ){
        
        if( (AP_Buffer[02] == 0x02) || 
            (AP_Buffer[02] == 0x03) || 
            (AP_Buffer[02] == 0x05) ||
            (AP_Buffer[02] == 0x11) ||
            (AP_Buffer[02] == 0x09) ||
            (AP_Buffer[02] == 0xff) ||
            (AP_Buffer[02] == 0x07) ){
    //----------------------------update---------------------
            if((AP_Buffer[02] == 0xFF) && (AP_Buffer[06] == 0x01)){
                    
                    LED_D1_ORANGE();
                    Cy_SysLib_Delay(100); 
                    NVIC_SystemReset();
                    
            }

            if((AP_Buffer[02] == 0x11) && (AP_Buffer[06] == 0x02)){ 
                Watchdog_T.stop();
                Watchdog_T.reset();
               // Watchdog_T.start();
                Watchdog_reset_flag = 0;
                Watchdog_flag = 0;
                pc.printf("   Watchdog   "); 
                Cy_SysLib_Delay(5);
                for(int j=0; j<9 ; j++){
                    ap.putc(AP_Watchdog[j]);
                    //pc.putc(AP_Watchdog[j]);
                }
                if((AP_Buffer[9] == 0x00) && (AP_Buffer[0] != 0x00)){
                    //pc.printf("  GOGO RESET  "); 
                    memset(AP_Buffer, 0, sizeof(AP_Buffer));
                    string_possition = 0;
                }
                else{
                    //device.printf("  GOGO ELSE  "); 
                    for(int e=9; e<string_possition; e++){ 
                        buffer_ch[Verify_buffer_count] = AP_Buffer[e];
                        AP_Buffer[Verify_buffer_count] = buffer_ch[Verify_buffer_count];
                        Verify_buffer_count++;
                    }
                    memset(AP_Buffer, 0, sizeof(AP_Buffer)); 
                    for(int e=0; e<Verify_buffer_count; e++){
                        AP_Buffer[e] = buffer_ch[e];
                    }
                    pc.printf("  GOGO next : %d  ",Verify_buffer_count);
                    for(int o=0; o<Verify_buffer_count; o++){
                        pc.putc(AP_Buffer[o]);
                    }
                    string_possition = string_possition - Verify_buffer_count;
                    Verify_buffer_count=0; 
                }
            }

         
    // -----------------------Tamper status---------------------------
            if(AP_Buffer[06] == 0x11){
                if(AP_Buffer[03] == 0x00){
                    pc.printf("Tamper OFF");
                    Tamper_flag = 0;
                }
                else if(AP_Buffer[03] == 0x01){
                    pc.printf("Tamper ON");
                    Tamper_flag = 1;
                }
                
                if((AP_Buffer[9] == 0x00) && (AP_Buffer[0] != 0x00)){
                   // device.printf("  Tamper RESET  "); 
                    memset(AP_Buffer, 0, sizeof(AP_Buffer));
                    string_possition = 0;
                }
                else{
                    //device.printf("  Tamper ELSE  "); 
                    for(int e=9; e<string_possition; e++){ 
                        buffer_ch[Verify_buffer_count] = AP_Buffer[e];
                        AP_Buffer[Verify_buffer_count] = buffer_ch[Verify_buffer_count];
                        Verify_buffer_count++;
                    }
                    memset(AP_Buffer, 0, sizeof(AP_Buffer)); 
                    for(int e=0; e<Verify_buffer_count; e++){
                        AP_Buffer[e] = buffer_ch[e];
                    }
                    pc.printf("  Tamper next : %d  ",Verify_buffer_count);
                    for(int o=0; o<Verify_buffer_count; o++){
                        pc.putc(AP_Buffer[o]);
                    }
                    string_possition = string_possition - Verify_buffer_count;
                    Verify_buffer_count=0; 
                }
            }

    // -----------------------Relay status---------------------------
        else if(AP_Buffer[06] == 0x21){
            Relay_status_flag = 1;//NC-COM User connect(Fail-Safe)
            pc.printf("NC-COM"); 
            
            /*
            memset(AP_Buffer, 0, sizeof(AP_Buffer));
            string_possition = 0; 
            */
            if((AP_Buffer[9] == 0x00) && (AP_Buffer[0] != 0x00)){
                   // device.printf("  Tamper RESET  "); 
                    memset(AP_Buffer, 0, sizeof(AP_Buffer));
                    string_possition = 0;
            }
            else{
                //device.printf("  Tamper ELSE  "); 
                for(int e=9; e<string_possition; e++){ 
                    buffer_ch[Verify_buffer_count] = AP_Buffer[e];
                    AP_Buffer[Verify_buffer_count] = buffer_ch[Verify_buffer_count];
                    Verify_buffer_count++;
                }
                memset(AP_Buffer, 0, sizeof(AP_Buffer));

                for(int e=0; e<Verify_buffer_count; e++){
                    AP_Buffer[e] = buffer_ch[e];
                }
                pc.printf("  Fail-Safe next : %d  ",Verify_buffer_count);
                for(int o=0; o<Verify_buffer_count; o++){
                    pc.putc(AP_Buffer[o]);
                }
                string_possition = string_possition - Verify_buffer_count;
                Verify_buffer_count=0; 
            }

            
        }
        else if(AP_Buffer[06] == 0x25){
            Relay_status_flag = 0;//NO-COM User connect(Fail-Secure)
            pc.printf("NO-COM");
 
            /*
            memset(AP_Buffer, 0, sizeof(AP_Buffer));
            string_possition = 0; 
            */
            if((AP_Buffer[9] == 0x00) && (AP_Buffer[0] != 0x00)){
                   // device.printf("  Tamper RESET  "); 
                    memset(AP_Buffer, 0, sizeof(AP_Buffer));
                    string_possition = 0;
            }
            else{
                //device.printf("  Tamper ELSE  "); 
                for(int e=9; e<string_possition; e++){ 
                    buffer_ch[Verify_buffer_count] = AP_Buffer[e];
                    AP_Buffer[Verify_buffer_count] = buffer_ch[Verify_buffer_count];
                    Verify_buffer_count++;
                }
                memset(AP_Buffer, 0, sizeof(AP_Buffer)); 
                for(int e=0; e<Verify_buffer_count; e++){
                    AP_Buffer[e] = buffer_ch[e];
                }
                pc.printf("  Fail-Secure next : %d  ",Verify_buffer_count);
                for(int o=0; o<Verify_buffer_count; o++){
                    pc.putc(AP_Buffer[o]);
                }
                string_possition = string_possition - Verify_buffer_count;
                Verify_buffer_count=0; 
            }

        }
        else if(AP_Buffer[06] == 0x23){
            Relay_status_flag = 0;//NO-COM User connect(General)
            pc.printf("General"); 
            
            /*
            memset(AP_Buffer, 0, sizeof(AP_Buffer));
            string_possition = 0; 
            */
            if((AP_Buffer[9] == 0x00) && (AP_Buffer[0] != 0x00)){
                   // device.printf("  Tamper RESET  "); 
                    memset(AP_Buffer, 0, sizeof(AP_Buffer));
                    string_possition = 0;
            }
            else{
                //device.printf("  Tamper ELSE  "); 
                for(int e=9; e<string_possition; e++){ 
                    buffer_ch[Verify_buffer_count] = AP_Buffer[e];
                    AP_Buffer[Verify_buffer_count] = buffer_ch[Verify_buffer_count];
                    Verify_buffer_count++;
                }
                memset(AP_Buffer, 0, sizeof(AP_Buffer)); 
                for(int e=0; e<Verify_buffer_count; e++){
                    AP_Buffer[e] = buffer_ch[e];
                }
                pc.printf("  General next : %d  ",Verify_buffer_count);
                for(int o=0; o<Verify_buffer_count; o++){
                    pc.putc(AP_Buffer[o]);
                }
                string_possition = string_possition - Verify_buffer_count;
                Verify_buffer_count=0; 
            }

        }
    // -----------------------Idle ---------------------------
        if( (AP_Buffer[02] == 0x02) && (AP_Buffer[06] == 0x00) && (AP_Buffer[8] == 0x03) ){
            
            led_flag = 0; 
            LED_D2_OFF(); 
            LED_Ring_flag = 0;

            LED_Ring_Tof_flag = 0;
            LED_Ring_Tof_buzzer = 0;
            ir_led_flag = 0;
            TOF_Start = 1;
            LED_OUT23_D2_T1(0);
            LED_OUT20_D2_T2(0);
            LED_OUT14_D2_T3(0);
            LED_OUT17_D2_T4(0);
            
            //Door hotel
            if(Hotel_led_flag == 1){
                LED_D1_RED();
            }
            if(Hotel_led_flag == 0){
                LED_D1_ON();
            }
            led_flag = 0;
            pc.printf("  IDLE  ");

            for(int r=0; r<9; r++){
                //pc.putc(AP_Buffer[r]);
                ap.putc(AP_Buffer[r]);
            }

            if((AP_Buffer[9] == 0x00) && (AP_Buffer[0] != 0x00)){
                //device.printf("  Idle RESET  "); 
                memset(AP_Buffer, 0, sizeof(AP_Buffer)); 
                string_possition = 0;
            }
            else{
                //device.printf("  Idel ELSE  ");
                //device.printf(" string_Possition : %d  ",string_possition);
                for(int e=9; e<string_possition; e++){
                    
                    buffer_ch[Verify_buffer_count] = AP_Buffer[e];
                    AP_Buffer[Verify_buffer_count] = buffer_ch[Verify_buffer_count];
                    Verify_buffer_count++;
                }
                memset(AP_Buffer, 0, sizeof(AP_Buffer));
                
                for(int e=0; e<Verify_buffer_count; e++){
                    AP_Buffer[e] = buffer_ch[e];
                }

                pc.printf("  Idel next  : %d  ",Verify_buffer_count);
                for(int o=0; o<Verify_buffer_count; o++){
                    pc.putc(AP_Buffer[o]);
                }
                string_possition = string_possition - Verify_buffer_count;
                Verify_buffer_count=0; 
            }
            Touch_led_flag = 0;
 
        }
    // -----------------------Start ---------------------------
        if( (AP_Buffer[02] == 0x02) && (AP_Buffer[06] == 0x01) && (AP_Buffer[8] == 0x03) ){
            
            led_flag = 1;
            pc.printf("    Start    ");
            if(LED_Ring_Tof_buzzer == 1){
                LED_Ring_Tof_flag = 0;
            }
            if(LED_Ring_Tof_buzzer == 0){
                LED_Ring_Tof_flag = 1;
            } 
            if(Touch_led_flag == 0){
                LED_D2_Touch1_Test();
                //LED_D2_ON(); 
            }

            for(int r=0; r<9; r++){
                //pc.putc(AP_Buffer[r]);
                ap.putc(AP_Buffer[r]);
            }

            if( (AP_Buffer[9] == 0x00) && (AP_Buffer[0] != 0x00) ){
                //device.printf("  Start RESET  "); 
                memset(AP_Buffer, 0, sizeof(AP_Buffer)); 
                string_possition = 0;
            }
            else{
                //device.printf("  Start ELSE  ");

                for(int e=9; e<string_possition; e++){
                    
                    buffer_ch[Verify_buffer_count] = AP_Buffer[e];
                    AP_Buffer[Verify_buffer_count] = buffer_ch[Verify_buffer_count];
                    Verify_buffer_count++;
                }
                memset(AP_Buffer, 0, sizeof(AP_Buffer));
                
                for(int e=0; e<Verify_buffer_count; e++){
                    AP_Buffer[e] = buffer_ch[e];
                }

                pc.printf("  Start next  : %d  ",Verify_buffer_count);
                for(int o=0; o<Verify_buffer_count ; o++){
                    pc.putc(AP_Buffer[o]);
                }
                string_possition = string_possition - Verify_buffer_count;
                Verify_buffer_count=0; 
            }
            Touch_led_flag = 0;
            TOF_Start = 1;
        }  
    // ---------------------------- Wiegand ----------------------------------
        if(AP_Buffer[02] == 0x05){
            pc.printf("  Wiegand  ");
    
            TOF_Start = 0;
            int wiegand_count;

            wiegand_count = AP_Buffer[05]; 

            char wiegand_data[wiegand_count];
            pc.printf("  %d  ",wiegand_count);
            for(int r=0; r<8+wiegand_count; r++){
                pc.putc(AP_Buffer[r]);
                ap.putc(AP_Buffer[r]);
            }
            for(int a=0; a<wiegand_count; a++){
                wiegand_data[a] = (AP_Buffer[a+6]);
            }
 
            thread_LED.set_priority(osPriorityLow);
            thread_TOF.set_priority(osPriorityLow);

            led_flag = 1;
            LED_Ring_flag = 0;
            TOF_Start = 0;
            LED_Ring_stop_flag = 1; 
            LED_Ring_Tof_flag = 0;
            LED_Ring_Tof_buzzer = 0;

            LED_D1_GREEN();
            success_flag = 1;
            for (int i = 0; i <= wiegand_count-1; i++){
                //pc.printf("%d", wiegand_data[i]);
                if(wiegand_data[i] == 0x01){ 
                    Wiegand_out_D1 = 0;
                    Cy_SysLib_DelayUs(pulse_width);
                    Wiegand_out_D1 = 1;

                }
                if(wiegand_data[i] == 0x00){ 
                    Wiegand_out_D0 = 0;
                    Cy_SysLib_DelayUs(pulse_width);
                    Wiegand_out_D0 = 1;
                }
                Cy_SysLib_DelayUs(pulse_interval);
            }
            
            Cy_SysLib_Delay(150);
            
            if( ( (AP_Buffer[wiegand_count + 9] == 0x00) 
                || (AP_Buffer[wiegand_count + 10] == 0x00) 
                || (AP_Buffer[wiegand_count + 9] = 0x00) ) && (AP_Buffer[0] != 0x00) ){ 
                memset(AP_Buffer, 0, sizeof(AP_Buffer)); 
                string_possition = 0;
                pc.printf(" Wiegand Reset ");
            }
            else{ 
                for(int e=8+wiegand_count; e<string_possition; e++){ 
                    buffer_ch[Verify_buffer_count] = AP_Buffer[e];
                    AP_Buffer[Verify_buffer_count] = buffer_ch[Verify_buffer_count];
                    Verify_buffer_count++;
                }
                memset(AP_Buffer, 0, sizeof(AP_Buffer));
                
                for(int e=0; e<Verify_buffer_count; e++){
                    AP_Buffer[e] = buffer_ch[e];
                }

                pc.printf("  Wiegand next  : %d  ",Verify_buffer_count);
                for(int o=0; o<Verify_buffer_count ; o++){
                    pc.putc(AP_Buffer[o]);
                }
                string_possition = string_possition - Verify_buffer_count;
                Verify_buffer_count=0; 
            }

            memset(wiegand_data, 0, sizeof(wiegand_data)); 
            thread_LED.set_priority(osPriorityNormal);
            thread_TOF.set_priority(osPriorityNormal); 
            LED_Ring_stop_flag = 0; 
            Touch_led_flag = 0;
            success_flag = 0;
            TOF_Start = 1;
            /*
            for(int g=0; g<12; g++){
                ap.putc(out_data_v[g]);
            }
            */

        }

         // -------------------------------------- Success ------------------------------------------
        if( (AP_Buffer[02] == 0x02) && (AP_Buffer[06] == 0x02) && ( (AP_Buffer[11] == 0x03) 
                                                                        || (AP_Buffer[12] == 0x03) 
                                                                        || (AP_Buffer[10] = 0x03) )
                                                                        ) {
            pc.printf("  Success  ");
            
            led_flag = 1;
            LED_Ring_flag = 0;
            TOF_Start = 0;
            LED_Ring_stop_flag = 1; 
            
            Door_status_open = 0;

            LED_Ring_Tof_flag = 0;
            LED_Ring_Tof_buzzer = 0;

            //thread_LED.set_priority(osPriorityLow);
            thread_TOF.set_priority(osPriorityLow);

            int delay_count;
            Door_status_open_01 = 1;
            Cy_SysLib_Delay(10);
            relay_delay_len = atoi(&AP_Buffer[7]);
            Cy_SysLib_Delay(10);
            pc.printf(" %d ",relay_delay_len);

            delay_count = AP_Buffer[5];
            pc.printf(" Delay Len : %d ",delay_count);
            
            if(AP_Buffer[03] == 0x01){ 
                LED_D1_Yellow(); 
            }
            if(AP_Buffer[03] == 0x00){
                LED_D1_GREEN();
            }
            //Door Hotel
            temperature = 1;
            Cy_SysLib_Delay(100);
            Powerfail = 0;
            GPO = 1;
            Cy_SysLib_Delay(30);
            Powerfail = 0;
            GPO = 0;
            temperature = 0;
            Hotel_T_flag = 0;
            Hotel_T.reset();
            Hotel_T.start();

            success_flag = 0;
             
            if(Relay_status_flag == 1){
                relay = 0;
            }
            if(Relay_status_flag == 0){
                relay = 1;
            }
            Relay_flag = 1;
            
            /*
            if(relay_delay_len == 0){
                relay_delay_len = 1000;
            }


            Cy_SysLib_Delay(relay_delay_len); 
            //led_flag = 0;
            if(Relay_status_flag == 1){
                relay = 1;
            }
            if(Relay_status_flag == 0){
                relay = 0;
            }
            */

             
            LED_Ring_stop_flag = 0;
            //thread_LED.set_priority(osPriorityNormal);
            thread_TOF.set_priority(osPriorityNormal);

            Cy_SysLib_Delay(50);

            for(int r=0; r<8+delay_count; r++){
                //pc.putc(AP_Buffer[r]);
                ap.putc(AP_Buffer[r]);
            }

            if( ( (AP_Buffer[12] == 0x00) 
                || (AP_Buffer[13] == 0x00) 
                || (AP_Buffer[11] = 0x00) ) && (AP_Buffer[0] != 0x00) ){
                //device.printf("  Success RESET  "); 
                memset(AP_Buffer, 0, sizeof(AP_Buffer)); 
                string_possition = 0;
            }
            else{
               //device.printf("  Success ELSE  ");

                for(int e=8+delay_count; e<string_possition; e++){ 
                    buffer_ch[Verify_buffer_count] = AP_Buffer[e];
                    AP_Buffer[Verify_buffer_count] = buffer_ch[Verify_buffer_count];
                    Verify_buffer_count++;
                }
                memset(AP_Buffer, 0, sizeof(AP_Buffer));
                
                for(int e=0; e<Verify_buffer_count; e++){
                    AP_Buffer[e] = buffer_ch[e];
                }

                pc.printf("  Success next  : %d  ",Verify_buffer_count);
                for(int o=0; o<Verify_buffer_count ; o++){
                    pc.putc(AP_Buffer[o]);
                }
                string_possition = string_possition - Verify_buffer_count;
                Verify_buffer_count=0; 
            }
            TOF_Start = 1;
            //relay_delay_len = 0;
            Touch_led_flag = 0;
            /*
            for(int g=0; g<12; g++){
                ap.putc(out_data_v[g]);
            }
            */
            
            
        }
    //------------------------------------- Fail -------------------------------
        if((AP_Buffer[02] == 0x02) && (AP_Buffer[06] == 0x03)){

            pc.printf("  FAIL  ");
            
            led_flag = 1; 
            LED_Ring_flag = 0;
            TOF_Start = 0;
            LED_Ring_stop_flag = 1;
            LED_Ring_Tof_flag = 0;
            LED_Ring_Tof_buzzer = 0;
            LED_D1_RED();
            Cy_SysLib_Delay(400); 
            LED_OUT23_D2_T1(0);
            LED_OUT20_D2_T2(0);
            LED_OUT14_D2_T3(0);
            LED_OUT17_D2_T4(0);
            //LED_D2_ON();
            
            //Door Hotel
            if(Hotel_led_flag == 1){
                LED_D1_RED();
                led_flag = 0;
            }
            if(Hotel_led_flag == 0){ 
                LED_D1_OFF();
            }
             
            LED_Ring_stop_flag = 0;
            //led_flag = 0;

            for(int r=0; r<9; r++){
                //pc.putc(AP_Buffer[r]);
                ap.putc(AP_Buffer[r]);
            }

            if( (AP_Buffer[9] == 0x00) && (AP_Buffer[0] != 0x00) ){
                //device.printf("  Fail RESET  "); 
                memset(AP_Buffer, 0, sizeof(AP_Buffer)); 
                string_possition = 0;
            }
            else{
                //device.printf("  Fail ELSE  ");

                for(int e=9; e<string_possition; e++){
                    
                    buffer_ch[Verify_buffer_count] = AP_Buffer[e];
                    AP_Buffer[Verify_buffer_count] = buffer_ch[Verify_buffer_count];
                    Verify_buffer_count++;
                }
                memset(AP_Buffer, 0, sizeof(AP_Buffer));
                
                for(int e=0; e<Verify_buffer_count; e++){
                    AP_Buffer[e] = buffer_ch[e];
                }

                pc.printf("  Fail next  : %d  ",Verify_buffer_count);
                for(int o=0; o<Verify_buffer_count ; o++){
                    pc.putc(AP_Buffer[o]);
                }
                string_possition = string_possition - Verify_buffer_count;
                Verify_buffer_count=0; 
            }
             
            Cy_SysLib_Delay(50);
            /*
            for(int g=0; g<12; g++){
                ap.putc(out_data_v[g]);
            }*/
            TOF_Start = 1;
            Touch_led_flag = 0;
        }

        // --------------------------------------  Fake  -------------------------------
        if((AP_Buffer[02] == 0x02) && (AP_Buffer[06] == 0x04)){

            pc.printf("  Fake  ");

            led_flag = 1;
            TOF_Start = 0; 
            LED_Ring_flag = 0;
            LED_Ring_stop_flag = 1;
            LED_Ring_Tof_flag = 0;
            LED_Ring_Tof_buzzer = 0;
            LED_D1_ORANGE();
            Cy_SysLib_Delay(400); 

            LED_OUT23_D2_T1(0);
            LED_OUT20_D2_T2(0);
            LED_OUT14_D2_T3(0);
            LED_OUT17_D2_T4(0);
            //LED_D2_ON();
            LED_D2_Touch1_Test();
            LED_D1_OFF(); 
            //led_flag = 0;
            LED_Ring_stop_flag = 0;


            for(int r=0; r<9; r++){
               //pc.putc(AP_Buffer[r]);
                ap.putc(AP_Buffer[r]);
            }

            if( (AP_Buffer[9] == 0x00) && (AP_Buffer[0] != 0x00) ){
                //device.printf("  Fake RESET  "); 
                memset(AP_Buffer, 0, sizeof(AP_Buffer)); 
                string_possition = 0;
            }
            else{
                //device.printf("  Fake ELSE  ");

                for(int e=9; e<string_possition; e++){
                    
                    buffer_ch[Verify_buffer_count] = AP_Buffer[e];
                    AP_Buffer[Verify_buffer_count] = buffer_ch[Verify_buffer_count];
                    Verify_buffer_count++;
                }
                memset(AP_Buffer, 0, sizeof(AP_Buffer));
                
                for(int e=0; e<Verify_buffer_count; e++){
                    AP_Buffer[e] = buffer_ch[e];
                }

                pc.printf("  Fake next  : %d  ",Verify_buffer_count);
                for(int o=0; o<Verify_buffer_count ; o++){
                    pc.putc(AP_Buffer[o]);
                }
                string_possition = string_possition - Verify_buffer_count;
                Verify_buffer_count=0; 
            }
            Cy_SysLib_Delay(50);
            /*
            for(int g=0; g<12; g++){
                ap.putc(out_data_v[g]);
            }
            */
            TOF_Start = 1;
            Touch_led_flag = 0;
        }

        else if((AP_Buffer[02] == 0x02) && (AP_Buffer[06] == 0x05)){
            pc.printf("  LED OFF  ");
            led_flag = 1; 
            LED_D1_OFF(); 
            //led_flag = 0;
            //LED_Ring_flag = 0; 
            memset(AP_Buffer, 0, sizeof(AP_Buffer)); 
            string_possition = 0;
            Touch_led_flag = 0; 
        }
    
    /*
    if(AP_Buffer[01] == 0x0d){
        if(AP_Buffer[02] == 0x01){
            //facility_use X
            if(AP_Buffer[03] == 0x00){
                unsigned char facility_use[9] = {0x01, 0x20, 0xef, 0x00, 0x01, 0x00, 0x10, 0x03};

                for(int i=0; i<8; i++){
                    rfid.putc(facility_use[i]); 
                }
                memset(AP_Buffer, 0, sizeof(AP_Buffer));
                memset(message, 0, sizeof(message));
                string_possition = 0; 
            }
            
            if(AP_Buffer[03] == 0x01){
                unsigned char facility_hex[12] = {0x01, 0x20, 0xec, 0x00, 0x04,}; 

                unsigned char checksum;

                for(int i=0; i<4; i++){
                    facility_hex[5+i] = AP_Buffer[6+i];
                } 
                
                checksum = CalcChecksum(facility_hex,9);  
                facility_hex[9] = (0xff - checksum) -1;

                //device.putc(facility_hex[9]);
                facility_hex[10] = 0x03;

                for(int a=0;a<11; a++){
                    device.putc(facility_hex[a]);
                }
                for(int j=0; j<11; j++){
                    rfid.putc(facility_hex[j]);
                }

                Cy_SysLib_Delay(400);

                unsigned char facility_use_01[9] = {0x01, 0x20, 0xef, 0x00, 0x01, 0x01, 0x11, 0x03};
                for(int b=0; b<8; b++){
                    device.putc(facility_use_01[b]);
                }
                for(int i=0; i<8; i++){
                    rfid.putc(facility_use_01[i]); 
                }
                memset(AP_Buffer, 0, sizeof(AP_Buffer));
                memset(message, 0, sizeof(message));
                string_possition = 0; 
            }
        }
        if(AP_Buffer[02] == 0x02){
            if(AP_Buffer[03] == 0x01){
                unsigned char app_Readkey_use[39] = {0x01, 0x20, 0xea, 0x00, 0x1e, 0x01};
                unsigned char app_ReadKey_end[8] = {0x01, 0x00, 0x00, 0x00, 0xf9, 0x0d, 0x00, 0x02};

                app_Readkey_use[06] = AP_Buffer[06];
                app_Readkey_use[07] = AP_Buffer[07];
                app_Readkey_use[8] = AP_Buffer[8];
                app_Readkey_use[9] = 0x01;
                app_Readkey_use[10] = 0x00;

                for(int i=0; i<16; i++){
                    app_Readkey_use[11+i] = AP_Buffer[9+i];
                } 
                App_ReadKey_arrayCopy(app_Readkey_use, app_ReadKey_end, 8);

                unsigned char checksum;
                checksum = CalcChecksum(app_Readkey_use,35);
                app_Readkey_use[35] = (0xff - checksum) -1;
                //device.putc(app_Readkey_use[36]);

                app_Readkey_use[36] = 0x03;

                for(int a=0; a<37; a++){
                    device.putc(app_Readkey_use[a]);
                }

                for(int b=0; b<37; b++){
                    rfid.putc(app_Readkey_use[b]);
                }

                memset(AP_Buffer, 0, sizeof(AP_Buffer));
                memset(message, 0, sizeof(message));
                string_possition = 0; 

            } 

        }
    }
    */
        } 
    }   
}

// ---------------------------------- AP Ready ------------------------------------

void ap_ready(){

    if(AP_Buffer[0] == 0x01){
        if((AP_Buffer[02] == 0x11) && (AP_Buffer[06] == 0x01)){ 
            for(int j=0; j<9 ; j++){
                ap.putc(AP_start[j]);
            }
            for(int a=0; a<11; a++){
                ap.putc(Mbed_os_version_Buffer[a]);
            }
            pc.printf("  watchdog  ");
            memset(AP_Buffer, 0, sizeof(AP_Buffer));
            string_possition = 0;
        }
    }
}

//----------------------------------- AP Start ------------------------------------
void ap_start(){

    if(AP_Buffer[0] == 0x01){
        if((AP_Buffer[02] == 0x11) && (AP_Buffer[06] == 0x01)){ 
            for(int j=0; j<9 ; j++){
                ap.putc(AP_start[j]);
            }
            pc.printf("  start GOGO  ");
            memset(AP_Buffer, 0, sizeof(AP_Buffer));
            string_possition = 0;
        }
    }
}

//---------------------------------- Read AP Serial ----------------------

void read_serial() {
     
	char chr;
    chr = ap.getc(); 
    AP_Buffer[string_possition] = chr;
    string_possition++;
    //pc.putc(chr);
    
    if( string_possition >= 70 ){
        memset(AP_Buffer, 0, sizeof(AP_Buffer)); 
        string_possition = 0;
    }

}

//---------------------------------- buzzer ------------------------------
void buzzer(){ 
    for(int i=0; i<8; i++){
        ap.putc(RFID_Buzzer[i]);
    }
    pc.printf(" BUZZER ");
    led_flag = 1; 
    LED_Ring_flag = 1; 
    LED_Ring_Tof_flag = 0;
    LED_Ring_Tof_buzzer = 1;
    Touch_led_flag = 1;
    ir_led_flag = 1; 
}

//---------------------------------- TOF Thread ------------------------------

void arrayCopy(char *dst, char *src, int n) {
	int i;
	for (i = 0; i < n; i++) {
		dst[i+6] = src[i];
	}
}
void tof_thread(){
   

    int status = VL53L0X_ERROR_NONE;
    uint32_t data;
    int data_result,data_c, tof_out_flag; 
    uint32_t count_b = 0;
    uint32_t tm_sensor_2;
    char out_data[12] = {0x01, 0x0A, 0x01, 0x00, 0x00, 0x04, 0x38, 0x31, 0x39, 0x30, 0x00, 0x03};
    int out_data_flag = 1;
 
    
    TOF_flag = 1;
    while(1){

        
        while(TOF_flag){
                
            #if USE_XSHUT
            status = sensor.init_sensor(0x33);  // set new I2C address
            #else
            // no control XSHUT then set default address (no other way)
            status = sensor.init_sensor(VL53L0X_DEFAULT_ADDRESS);
            #endif
            if (status == VL53L0X_ERROR_NONE) {
                pc.printf("%s", msg0);
            } else {
                pc.printf("%s", msg1);
                pc.printf("error number -> %d\r\n", status);
            }
            //status = sensor.set_mode(range_long_distance_33ms_200cm);
            //status = sensor.set_mode(range_hi_accurate_200ms_120cm);
            status = sensor.set_mode(range_hi_speed_20ms_120cm);
            if (status == VL53L0X_ERROR_NONE) {
                pc.printf("%s", msg0);
            } else {
                pc.printf("%s", msg1);
                pc.printf("error number -> %d\r\n", status); 
            } 
            t_2.reset();
            t_2.start();
            TOF_flag = 0; 
        }

        
        status = sensor.get_distance(&data);
        //pc.printf(" %d ",data); 
        if(TOF_Start == 1){
            if (status == VL53L0X_ERROR_NONE) { 
                //Cy_SysLib_Delay(10); 
                count_b++;
                data_result += data; 
            }

            //Cy_SysLib_Delay(10);
            if( data == 0 ){
                count_b = 0; 
                if(tof_out_flag == 0){
                    tof_out_flag = 1;
                }
            }

            if(tof_out_flag == 1){
                count_b = 0; 
                for(int g=0; g<12; g++){
                    ap.putc(out_data_v[g]);
                } 
                tof_out_flag = 2;
            }

            if(Relay_flag == 1){ 
                Hotel_T_flag = Hotel_T.read();

                /*
                float relay_float;
                relay_float = (float)relay_delay_len/1000;
                pc.printf("   @%f@   ",relay_float);
                */
                if(relay_delay_len/1000 == 0){
                    pc.printf("  0under  ");
                    Cy_SysLib_Delay(relay_delay_len);
                    if(Relay_status_flag == 1){
                        relay = 1;
                    }
                    if(Relay_status_flag == 0){
                        relay = 0;
                    }
                    Hotel_T.reset();
                    Hotel_T.stop();
                    Relay_flag = 0;
                    relay_delay_len = 0;
                }
                else if(Hotel_T_flag  == relay_delay_len/1000){
                    pc.printf("  Relay OFF  ");
                    //Cy_SysLib_Delay(relay_delay_len); 
                    //led_flag = 0;
                    if(Relay_status_flag == 1){
                        relay = 1;
                    }
                    if(Relay_status_flag == 0){
                        relay = 0;
                    }
                    Hotel_T.reset();
                    Hotel_T.stop();
                    Relay_flag = 0;
                    relay_delay_len = 0;
                }
            }


            int i=1, tof_len=0;
            int tof_buffer_len;
            //int tof_write_buffer_len;
            char *data_c_buffer;
            char *tof_Buffer;
            
            if(count_b == 8){
                //pc.printf("    %d     ",data_result);
                data_c = data_result/8;
                while(data_c>i){ 
                    i *= 10; 
                    tof_len++; 
                }
                //pc.printf("   %d   ",data_c);

                if(MLX90614_data_flag == 1){
                    
                    MLX90614_tof_data = data_c;
                    //pc.printf("   ***************TOF_DATA_SET*********   ");

                }
                
                tof_out_flag = 0;
                if( (data_c > MLX90614_tof_data-20) && (data_c < MLX90614_tof_data+20) ){
                    
                    if(MLX90614_tof_data_flag == 1){
                        MLX90614_data_flag = 0;
                        MLX90614_tof_data_count ++;
                        //pc.printf("  count : %d  ",MLX90614_tof_data_count);
                        //pc.printf("  ******** MLX DATA : %d ***********", MLX90614_tof_data);

                        if(MLX90614_tof_data_count == 50){
 
                                for(int g=0; g<12; g++){
                                    ap.putc(out_data_v[g]);
                                }   
                            MLX90614_tof_data_flag = 0; 
                        }  
                    }
                    //pc.printf("------------------");
                }
                else if( data_c < MLX90614_tof_data-20 ){
                   // pc.printf("   TOF Change LOW  ");
                    MLX90614_data_flag = 1;
                    MLX90614_tof_data_flag = 1;
                    MLX90614_tof_data_count = 0; 
                }
                else if( data_c > MLX90614_tof_data+20 ){
                   //pc.printf("   TOF Change HIGH  ");
                    MLX90614_data_flag = 1;
                    MLX90614_tof_data_flag = 1;
                    MLX90614_tof_data_count = 0; 
                }

                if(MLX90614_tof_data_flag == 1){

                    tof_buffer_len = tof_len; 
                    
                    data_c_buffer = (char *)malloc(sizeof(int) * tof_buffer_len+2);
                    tof_Buffer = (char *)malloc(sizeof(int) * tof_buffer_len+8);

                    tof_Buffer[0]=0x01;
                    tof_Buffer[1]=0x0A;
                    tof_Buffer[2]=0x01;
                    tof_Buffer[3]=0x00;
                    tof_Buffer[4]=0x00;
                    tof_Buffer[5]=tof_buffer_len;

                    sprintf(data_c_buffer, "%d", data_c);
                    data_c_buffer[tof_buffer_len] = 0x00;
                    data_c_buffer[tof_buffer_len+1] = 0x03; 
                    arrayCopy(tof_Buffer, data_c_buffer, tof_buffer_len+2);

                    if(tof_buffer_len == 0x01){
                        //pc.printf("  TOF Low  ");
                    }
                    else{
                        for(int i=0; i<(tof_buffer_len+8); i++){
                        ap.putc(tof_Buffer[i]);
                        //pc.putc(tof_Buffer[i]);
                        } 
                    } 
                    
                    free(data_c_buffer);
                    free(tof_Buffer); 
                    Cy_SysLib_Delay(5);
                    count_b = 0; 
                    data_result = 0;
                }
            }
            
            tm_sensor_2 = t_2.read();
        
            if(tm_sensor_2 > 0){
                //device.printf("tof data_result = %d",data_result);
                data_result = 0; 
                count_b = 0;
                data_c = 0;
                tm_sensor_2 = 0;
                t_2.reset();
                t_2.stop();
                t_2.start();
            }
        }
    }
}


//----------------------------------LED Thread -------------------------------
void led_thread(){

    while(1){

        
        if(LED_Ring_Tof_flag == 1){
             led_flag = 1;   
            LED_Ring_Init();

            //ring_P_TOF();
            //ring_S_TOF();
            ring_Start_TOF_01();
            
            LED_T.start();
            int LED_T_Read;
            LED_T_Read = LED_T.read(); 
            //device.printf("   LED_T_READ %d   ",LED_T_Read);
            if(LED_T_Read == 7){
                
                led_flag = 0;
                LED_D2_OFF(); 
                LED_Ring_flag = 0;
                TOF_Start = 1; 
                LED_OUT23_D2_T1(0);
                LED_OUT20_D2_T2(0);
                LED_OUT14_D2_T3(0);
                LED_OUT17_D2_T4(0);
                LED_D1_ON();
                
                memset(AP_Buffer, 0, sizeof(AP_Buffer)); 
                string_possition = 0;
                LED_T.stop();
                LED_T.reset();
            }
        }
        
        
        if(LED_Ring_flag == 1) {
            pc.printf(" Ring LED IN ");
            led_flag = 1;
            ring_stop();
            LED_Ring_Init();
            ring_P();
            ring_S();
            
            LED_T.start();
            int LED_T_Read;
            LED_T_Read = LED_T.read();
            //device.printf("   LED_T_READ %d   ",LED_T_Read);
            if(LED_T_Read == 7){
                
                led_flag = 0;
                LED_D2_OFF(); 
                LED_Ring_flag = 0;
                TOF_Start = 1; 
                LED_OUT23_D2_T1(0);
                LED_OUT20_D2_T2(0);
                LED_OUT14_D2_T3(0);
                LED_OUT17_D2_T4(0);
                LED_D1_ON();
                
                memset(AP_Buffer, 0, sizeof(AP_Buffer)); 
                string_possition = 0;
                LED_T.stop();
                LED_T.reset();
            }
        }
        
        if(success_flag == 1){
            
            Cy_SysLib_Delay(400); 
            LED_OUT23_D2_T1(0);
            LED_OUT20_D2_T2(0);
            LED_OUT14_D2_T3(0);
            LED_OUT17_D2_T4(0);
            //LED_D2_ON();
            LED_D2_Touch1_Test();
            LED_D1_OFF();
            success_flag = 0;
            TOF_Start = 1;
            /*
            for(int g=0; g<12; g++){
                ap.putc(out_data_v[g]);
            }
            */
        }
        LED_dimming();
    }
}
// --------------------------------- calcChecksum ----------------------------
unsigned char CalcChecksum(unsigned char *data, int leng){
  unsigned char csum;

    csum = 0;
    for (;leng > 0;leng--)
        csum += *data++; 
    return ~csum;
}
// ---------------------------------- myatoi (wiegand)---------------------------
long long int myatoi(char* str){
    int sign = 1;
    if (*str == '-'){
        sign = -1;
        str++;
    }
 
    long long int value = 0;
    while (*str){
        if ((*str >= '0') && (*str <= '9')){
            value = value * 10 + *str - '0';
            str++;
        }
        else{
            break;
        }        
    }
    return value * sign;
}


//----------------------------------- TOUCH ----------------------------------

void T_1_Touch(){

    T_1 = 1; 

}
void T_2_Touch(){

    T_2 = 1; 

}
void T_3_Touch(){

    T_3 = 1; 

}
void T_4_Touch(){

    T_4 = 1;

}

void Touch(){
 
    char touch_buffer[9] = {0x01, 0x0A, 0x04, 0x00, 0x00, 0x01};
    char T1[3];

    if(Touch_led_flag == 1){
        if(Touch_flag_last == 1){
                
            pc.printf("  T1T1T1  ");
            Mode_Select_Standby_D2();
            Mode_Select_Normal_D2();
            Mode_Set_D2(1,1,1,1,0,0);
            LED_CONFIG0_D2(1,0,1,1,1,1,1,1);
            Bank_Color_Set_R_D2(0);
            Bank_Color_Set_G_D2(0);
            Bank_Color_Set_B_D2(0);
            Bank_Brightness_Set_D2(0);

            LED_CONFIG0_D2(1,0,1,1,1,1,1,1); 
            LED_OUT23_D2_T1(200);
            Bank_Brightness_Set_D2(230); 
            Touch_flag_last = 0;
            Touch_led_flag = 0;
        }
        else if(Touch_flag_last == 2){
            pc.printf("  T2T2T2  ");
            Mode_Select_Standby_D2();
            Mode_Select_Normal_D2();
            Mode_Set_D2(1,1,1,1,0,0);
            LED_CONFIG0_D2(1,1,0,1,1,1,1,1);
            Bank_Color_Set_R_D2(0);
            Bank_Color_Set_G_D2(0);
            Bank_Color_Set_B_D2(0);
            Bank_Brightness_Set_D2(0);

            LED_CONFIG0_D2(1,1,0,1,1,1,1,1);
            LED_OUT20_D2_T2(200);
            Bank_Brightness_Set_D2(230); 
            Touch_flag_last = 0;
            Touch_led_flag = 0; 
        }

        else if(Touch_flag_last == 3){
            pc.printf("  T3T3T3  ");
            Mode_Select_Standby_D2();
            Mode_Select_Normal_D2();
            Mode_Set_D2(1,1,1,1,0,0);
            LED_CONFIG0_D2(1,1,1,1,1,1,0,1);
            Bank_Color_Set_R_D2(0);
            Bank_Color_Set_G_D2(0);
            Bank_Color_Set_B_D2(0);
            Bank_Brightness_Set_D2(0);

            LED_CONFIG0_D2(1,1,1,1,1,1,0,1);
            LED_OUT14_D2_T3(200);
            Bank_Brightness_Set_D2(230); 
            Touch_flag_last = 0;
            Touch_led_flag = 0; 
        }

        else if(Touch_flag_last == 4){
            pc.printf("  T4T4T4  ");
            Mode_Select_Standby_D2();
            Mode_Select_Normal_D2();
            Mode_Set_D2(1,1,1,1,0,0);
            LED_CONFIG0_D2(1,1,1,1,1,0,1,1);
            Bank_Color_Set_R_D2(0);
            Bank_Color_Set_G_D2(0);
            Bank_Color_Set_B_D2(0);
            Bank_Brightness_Set_D2(0);

            LED_CONFIG0_D2(1,1,1,1,1,0,1,1);
            LED_OUT17_D2_T4(200);
            Bank_Brightness_Set_D2(230); 
            Touch_flag_last = 0;
            Touch_led_flag = 0; 
        }
        else{
            pc.printf("   RERRERERE TEST   ");
            Mode_Select_Standby_D2();
            Mode_Select_Normal_D2();
            Mode_Set_D2(1,1,1,1,0,0);
            LED_CONFIG0_D2(1,1,1,1,1,1,1,1);
            Bank_Color_Set_R_D2(0);
            Bank_Color_Set_G_D2(0);
            Bank_Color_Set_B_D2(0);
            Bank_Brightness_Set_D2(0); 
            Touch_led_flag = 0; 
        }
    }

 
    if(T_1 == 1){
        //LED_D2_ON();
        LED_D2_Touch1_Test();
        T1[0] = 0x01;
        T1[1] = 0x00;
        T1[2] = 0x03;
        arrayCopy(touch_buffer,T1,3);
        for(int i=0; i<9; i++){
            ap.putc(touch_buffer[i]);
        } 
        LED_D2_T1(); 
        Touch_flag_last = 1; 
        Cy_SysLib_Delay(100);
        T_1 = 0; 
    }
        
    
    else if(T_2 == 1){
        //LED_D2_ON();
        LED_D2_Touch1_Test();
        T1[0] = 0x03;
        T1[1] = 0x00;
        T1[2] = 0x03;
        arrayCopy(touch_buffer,T1,3);
        for(int i=0; i<9; i++){
            ap.putc(touch_buffer[i]);
        }
        
        LED_D2_T2();
        Touch_flag_last = 2;
        Cy_SysLib_Delay(100); 
        T_2 = 0; 

    }
    else if(T_3 == 1){
        //LED_D2_ON();
        LED_D2_Touch1_Test();
        T1[0] = 0x02;
        T1[1] = 0x00;
        T1[2] = 0x03;
        arrayCopy(touch_buffer,T1,3);
        for(int i=0; i<9; i++){
            ap.putc(touch_buffer[i]);
        }
        LED_D2_T3();
        
        Touch_flag_last = 3;
        Cy_SysLib_Delay(100);
        T_3 = 0;
    }
    
    else if(T_4 == 1){
        //LED_D2_ON();
        LED_D2_Touch1_Test();
        T1[0] = 0x04;
        T1[1] = 0x00;
        T1[2] = 0x03;
        arrayCopy(touch_buffer,T1,3);
        for(int i=0; i<9; i++){
            ap.putc(touch_buffer[i]);
        }
        LED_D2_T4();
        
        Touch_flag_last = 4;
        Cy_SysLib_Delay(100);
        T_4 = 0;
    }
}

void GPI_rise(){
    /*
    device.printf("RISE"); 
    GPI_flag = 1;
    */  
    pc.printf("RISE");
    IO_Buffer[3] = 0x01;
    IO_Buffer[6] = 0x30;
    IO_Buffer[7] = 0x00;
    IO_Buffer[8] = 0x03;
    for(int i=0; i<9; i++){
        ap.putc(IO_Buffer[i]);
        pc.putc(IO_Buffer[i]);
    } 
    Cy_SysLib_Delay(3);
    
}
void GPI_fall(){
    /*
    device.printf("fall");
    Factory_D.reset();
    Factory_D.stop();
    GPI_flag = 0;
    */
    Cy_SysLib_Delay(3);  
    pc.printf("fall");
    IO_Buffer[3] = 0x00;
    IO_Buffer[6] = 0x30;
    IO_Buffer[7] = 0x00;
    IO_Buffer[8] = 0x03;
    for(int i=0; i<9; i++){
        ap.putc(IO_Buffer[i]);
        pc.putc(IO_Buffer[i]);
    } 
    Cy_SysLib_Delay(3);
}
 
void Fire_B(){
    Fire_B_flag = 1;
}
void Fire_A(){
    Fire_A_Flag = 1;
}
void Exit_B(){
    Exit_button_flag = 1;
}
void Fire_Release(){
    Fire_Release_Flag = 1;
}

void Door_S_01_01(){
    Door_status_01_flag = 2;
}
void Door_S_01(){
    Door_status_01_flag = 1;
}
void Door_S_02(){
    Door_status_02_flag = 1;
}

//Door Hotel
void Door_S_02_fall(){
    Door_status_02_flag_fall = 1;
}

void io_IN(){
    
    if(Tamper_flag == 1){
            
        Tamper_data = Tamper.read();
        if(Tamper_data == 0){
            led_flag = 1;        
                LED_D1_RED();
                Cy_SysLib_Delay(70);
                LED_D1_OFF(); 
                Cy_SysLib_Delay(70);
                Tamper_data = Tamper.read();
                Cy_SysLib_Delay(10);
                for(int i=0; i<9; i++){ 
                    pc.putc(Tamper_Buffer_ON[i]);
                    ap.putc(Tamper_Buffer_ON[i]); 
                } 
                Powerfail = 0;
                tamper_led_flag = 1;
        }
        if( (Tamper_data == 1) && (tamper_led_flag == 1) ){
            for(int i=0; i<9; i++){
                pc.putc(Tamper_Buffer_OFF[i]);
                ap.putc(Tamper_Buffer_OFF[i]); 
            }
            tamper_led_flag = 0; 
            Powerfail = 1;
            led_flag = 0;
            LED_D1_ON(); 
        }
    }
    /*
    if(GPI_flag == 1){

        Factory_D.start();
        factory_t = Factory_D.read();
        device.printf("%d",factory_t);

        if(factory_t > 5){
            device.printf("fall");
            IO_Buffer[3] = 0x00;
            IO_Buffer[6] = 0x30;
            IO_Buffer[7] = 0x00;
            IO_Buffer[8] = 0x03;
            for(int i=0; i<9; i++){
                ap.putc(IO_Buffer[i]);
                device.putc(IO_Buffer[i]);
            }
            Cy_SysLib_Delay(3);
            Factory_D.reset();
            Factory_D.stop();
            GPI_flag = 0;
        }
    }
    */


    if(Exit_button_flag == 1){ 
        IO_Buffer[6] = 0x0F;
        IO_Buffer[7] = 0x00;
        IO_Buffer[8] = 0x03;
        for(int i=0; i<9; i++){
            ap.putc(IO_Buffer[i]);
        }
        Cy_SysLib_Delay(100); 
        Exit_button_flag = 0;
    }

    if(Fire_Release_Flag == 1){ // Fire Unlock

        Fire_alarm_Out = 1; //Low
        
        IO_Buffer[6] = 0x1A;
        IO_Buffer[7] = 0x00;
        IO_Buffer[8] = 0x03;
        for(int i=0; i<9; i++){
            ap.putc(IO_Buffer[i]);
        }  
        Cy_SysLib_Delay(100);
        Fire_Release_Flag = 0;
        LED_D1_ON();
        led_flag = 0; 
    }

    if(Fire_A_Flag == 1){ // control
        if(Relay_status_flag == 1){
            relay = 0;
        }
        if(Relay_status_flag == 0){
            relay = 1;
        }

        IO_Buffer[6] = 0x13;
        IO_Buffer[7] = 0x00;
        IO_Buffer[8] = 0x03;
        for(int i=0; i<9; i++){
            ap.putc(IO_Buffer[i]);
        }  
        Cy_SysLib_Delay(100);
        Fire_alarm_Out = 0; // High
        led_flag = 1; 

        while(Fire_Release_Flag == 0){
            LED_D1_RED();
            LED_D2_FIRE();
            Cy_SysLib_Delay(70);
            LED_D1_OFF();
            LED_D2_OFF();
            Cy_SysLib_Delay(70);
        }
        Fire_A_Flag = 0;
    }

    if(Fire_B_flag == 1){ // panel

        if(Relay_status_flag == 1){
            relay = 0;
        }
        if(Relay_status_flag == 0){
            relay = 1;
        }

        IO_Buffer[6] = 0x15;
        IO_Buffer[7] = 0x00;
        IO_Buffer[8] = 0x03;
        for(int i=0; i<9; i++){
            ap.putc(IO_Buffer[i]);
        }
        
        Cy_SysLib_Delay(100);
        led_flag = 1; 
        Fire_alarm_Out = 0; // High
        while(Fire_Release_Flag == 0){
            LED_D1_RED();
            LED_D2_FIRE();
            Cy_SysLib_Delay(70);
            LED_D1_OFF();
            LED_D2_OFF();
            Cy_SysLib_Delay(70);
        }

        Fire_B_flag = 0;
    } 
    if(Door_status_01_flag == 2){
        pc.printf("  Door Close  ");
        Cy_SysLib_Delay(100); 
        Door_status_01_flag = 0;
        Door_status_open = 1; 
    }
    if(Door_status_02_flag_fall == 1){
        IO_Buffer[6] = 0x0D;
        IO_Buffer[7] = 0x00;
        IO_Buffer[8] = 0x03;
        for(int i=0; i<9; i++){
            ap.putc(IO_Buffer[i]);
        }
        pc.printf("Interlock open");
        Hotel_led_flag = 0;
        Cy_SysLib_Delay(100);
        led_flag = 0;
        LED_D1_ON();
        Door_status_02_flag_fall = 0;
    }


    //TEST_HEO
    if(Door_status_01_flag == 1){ 
        IO_Buffer[6] = 0x0B;
        IO_Buffer[7] = 0x00;
        IO_Buffer[8] = 0x03;
        for(int i=0; i<9; i++){
            ap.putc(IO_Buffer[i]);
        }
        pc.printf("  INDIA Door Close  ");
        Cy_SysLib_Delay(50);  
        Door_status_01_flag = 0;
        Door_status_close = 1;
    }
    if(Door_status_02_flag == 1){ 
        IO_Buffer_Close[6] = 0x0D;
        IO_Buffer_Close[7] = 0x00;
        IO_Buffer_Close[8] = 0x03;
        for(int i=0; i<9; i++){
            ap.putc(IO_Buffer_Close[i]);
        }
        pc.printf("  INDIA Door OPEN  "); 
        Hotel_led_flag = 0;  
        Cy_SysLib_Delay(50);
        Door_status_open = 1;
        Door_status_02_flag = 0;
    }


}
 
int main(){
    
	pc.baud(115200);
    ap.baud(460800);
    rs485.baud(9600);

    LED_Init_D1();

    LED_D2_OFF();

    pc.attach(&uart_pc, Serial::RxIrq);
    ap.attach(&read_serial, Serial::RxIrq);
    //rs485.attach(&rx_rs485_Callback_01, Serial::RxIrq); 

    start_main = 0;
    relay = 0;
    int Start_Led_Flag = 1;
    RS485_GPIO = 1;
    Relay_status_flag = 0;


    Watchdog_T.start();

     while(!start_main){

        //if(Start_Led_Flag == 1){
            LED_Ring_Init();
            ring_Start();
        //}
        Start_Led_Flag = 0;  
        for(int s=0; s<9 ; s++){
            pc.putc(AP_Buffer[s]);
        }
        
        Watchdog_flag = Watchdog_T.read();

        if(Watchdog_flag > 120){
            
            Watchdog_flag = 0;
            Watchdog_T.reset();
            Watchdog_T.stop();

            led_flag = 1;
            LED_D1_RED();
            DigitalOut AP_Reset(AP_Reset_11_4);
            touch_T1.fall(&T_1_Touch);
            touch_T2.fall(&T_2_Touch);
            touch_T3.fall(&T_3_Touch);
            touch_T4.fall(&T_4_Touch);
            AP_Reset = 0;

            while(1){

                if((T_1 == 1) || (T_2 == 1) || (T_3 == 1)){
                     
                    if(Relay_status_flag == 1){ 
                        relay = 1;
                    }         
                    if(Relay_status_flag == 0){ 
                        relay = 0;
                    }
                    LED_D1_GREEN();
                    Cy_SysLib_Delay(500);
                    if(Relay_status_flag == 1){ 
                        relay = 0;
                    }         
                    if(Relay_status_flag == 0){ 
                        relay = 1;
                    }  
                    LED_D1_RED(); 
                    T_1 = 0;
                    T_2 = 0;
                    T_3 = 0;
                }
                if(T_4 == 1){
                    pc.printf("%d",relay_status);
                    relay_status += 1;
                    if(relay_status == 1){
                        Relay_status_flag = 1;
                    }
                    if(relay_status == 2){
                        Relay_status_flag = 0;
                        relay_status = 0;
                    }
                    T_4 = 0;
                }
            }
                     
        }

        if((AP_Buffer[02] == 0x11) && (AP_Buffer[06] == 0x01)){
            //pin_mode(AP_UART_TX,PullUp); 
            Cy_SysLib_Delay(200);
            start_main = 1; 
            for(int j=0; j<9 ; j++){
                ap.putc(AP_start[j]);
            }
            ring_stop();
            for(int a=0; a<11; a++){
                ap.putc(Mbed_os_version_Buffer[a]);
            }
            memset(AP_Buffer, 0, sizeof(AP_Buffer)); 
            string_possition = 0; 
            break;
        }
        if(string_possition > 10){
            pc.printf(" RESET GO  ");
            memset(AP_Buffer, 0, sizeof(AP_Buffer)); 
            string_possition = 0; 
        }
        if(RS485_flag == 1){

            Watchdog_flag = 0;
            Watchdog_T.reset();
            Watchdog_T.stop();

            start_main = 1;
            memset(AP_Buffer, 0, sizeof(AP_Buffer)); 
            string_possition = 0; 
        }
    }
    
    
    
/*
    flash.init(); 
    full_flash.init(); 
    printf("Flash block device size: %llu\n",         full_flash.size());
    printf("Flash block device read size: %llu\n",    full_flash.get_read_size());
    printf("Flash block device program size: %llu\n", full_flash.get_program_size());
    printf("Flash block device erase size: %llu\n",   full_flash.get_erase_size());

    char *buffer = (char *)malloc(full_flash.get_erase_size());
    sprintf(buffer, "Hello World!\n");
    full_flash.erase(0, full_flash.get_erase_size());
    full_flash.program(buffer, 0, full_flash.get_erase_size());
    full_flash.read(buffer, 0, full_flash.get_erase_size());
    pc.printf("%s", buffer);
    pc.printf("   Application Start   ");
    // Deinitialize the device and buffer
    free(buffer);
    full_flash.deinit();
*/ 
    
    pc.printf(" APPLICATION START "); 
// ------------------------------- Thread ----------------------
    thread_TOF.start(tof_thread);
    thread_LED.start(led_thread);
//---------------------------- RS485 Callback ---------------------- 
    rs485.attach(&rx_rs485_Callback, Serial::RxIrq);
    //rs485.attach(&rx_rs485_Callback_01, Serial::RxIrq);  

//---------------------------- TOUCH --------------------------

    touch_T1.fall(&T_1_Touch); 
    touch_T2.fall(&T_2_Touch); 
    touch_T3.fall(&T_3_Touch); 
    touch_T4.fall(&T_4_Touch); 

//---------------------------- RFID Buzzer ----------------------
    Buzzer_in.fall(&buzzer);

// ---------------------------- IO -----------------------------
    GPI.rise(&GPI_rise);
    GPI.fall(&GPI_fall);
    Fire_alarm_in_A.rise(Fire_A);
    Release_fire_alarm.rise(Fire_Release);
    Exit_Button.rise(Exit_B);
    Fire_alarm_in_B.fall(Fire_B);
    InterruptIn_Door_status_01.fall(Door_S_01);

    //push
    //InterruptIn_Door_status_01.rise(Door_S_01_01);

    InterruptIn_Door_status_02.rise(Door_S_02);

    //push
    //InterruptIn_Door_status_02.fall(Door_S_02_fall);
    
    LED_D1_ON();
    
    LED_Ring_flag = 0;
    led_flag = 0;
    TOF_Start = 1;
    Relay_status_flag = 0;
    tamper_led_flag = 0;
    Watchdog_flag = 0;
    Watchdog_reset_flag = 0;

    //IO IN,OUT Init
    //------------------------------OUT-------------------------
    //han Door Powerfail = 1 , 
    Powerfail = 0;  
    Fire_alarm_Out = 1; // 1 - 12V OFF, 0 - 12V ON
    Wiegand_out_D1 = 1;
    Wiegand_out_D0 = 1;
    //------------------------------IN------------------------- 
    Door_status_01_flag = 0;
    Door_status_02_flag = 0;
    Door_status_close = 0;
    Door_status_open = 0;
    Fire_Alarm_B = 0;
    Exit = 0;
    Fire_Alarm_A = 0; 
    Fire_Release_S = 0;
    GPO = 1; //0 - 5V ON, 1 - 5V OFF
    Tamper_flag = 0;
    temperature = 1;


    int Hold_flag = 0;

    if(Relay_status_flag == 1){ 
        relay = 0;
    }         
    if(Relay_status_flag == 0){ 
        relay = 1;
    } 


    // Door Motor
    // Powerfail(22ping) 1 -> 0V , 0 -> 5V
    // GPO(28pin)        1 -> 0V, 0 -> 5V
    //22->1 / 28->0 : A+,B-
    //22->0 / 28->1 : A-,B+

    temperature = 1;
    Cy_SysLib_Delay(100);
    Powerfail = 1;
    GPO = 0;
    MLX90614_tof_data_flag = 1;
    MLX90614_data_flag = 1;

    while(1){

        Touch();
        ap_start();
        io_IN();
        verify(); 
     
        Powerfail = 0;
        GPO = 0;
        temperature = 1;
/*
        int MCU_Test = 1;

        RS485_GPIO = 1;
        if(RS485_flag == 1){
            
            for(int j=0; j<string_485 ; j++){  
                pc.putc(rs485_message[j]); 
            }
            
            Cy_SysLib_Delay(100);
            if((rs485_message[00] == 0x01) && (rs485_message[01] == 0xFF)){

                pc.printf("    MCU TEST Start    ");
                Fire_alarm_Out = 0;
                LED_D1_RED();
                LED_D2_FIRE();
                led_flag = 1;

                while(MCU_Test){

                    io_IN();

                    if(Hotel_status_flag == 1){
                        Fire_alarm_Out = 1;
                        pc.printf("  Door Status 01  ");
                        LED_D1_OFF();
                        LED_D2_OFF();

                        LED_D1_ON();
                        led_flag = 0;
                        MCU_Test = 0;
                    }
                }
            }
            memset(rs485_message, 0, sizeof(rs485_message));
            string_485 = 0;
            RS485_flag = 0;
        }
*/
        if(ir_led_flag == 1){
            TOF_Start = 0; 
            LED_D2_Touch1_Test();
            ir_led_flag = 0;
        }
    //TEST_HEO
        if(Door_status_open == 1){
            if(Relay_status_flag == 1){
                relay = 1;
            }
            if(Relay_status_flag == 0){
                relay = 0;
            }
            Door_status_close = 0;
            Door_status_open = 0;
            Hotel_T.reset();
            Hotel_T_flag = 0;
            Hotel_T.stop();
            pc.printf("#########Door open########");
        }

        if(Door_status_close == 1){
            if(Hotel_T_flag > 15){
                temperature = 1;
                Cy_SysLib_Delay(100);
                Powerfail = 1;
                GPO = 0;
                Cy_SysLib_Delay(30);
                Powerfail = 0;
                GPO = 0;
                Cy_SysLib_Delay(100);
                temperature = 0; 
                Hotel_T.reset();
                Hotel_T_flag = 0;
                Hotel_T.stop();
                pc.printf("@@@@@Door Close@@@@@");
                if(Relay_status_flag == 1){
                    relay = 1;
                }
                if(Relay_status_flag == 0){
                    relay = 0;
                }
                Door_status_close = 0;
                Door_status_open = 0;
                Door_status_open_01 = 1;

            }

            Hotel_T_flag = Hotel_T.read();
        }
        Watchdog_flag = Watchdog_T.read();
        Hold_status = Hold_input.read(); 

        if(Watchdog_flag > 80){
            
            Cy_SysLib_Delay(5);
            Watchdog_T.reset();
            Watchdog_flag = 0;
            Watchdog_reset_flag++;

            if(Watchdog_reset_flag == 3){

                pc.printf("reset");
                DigitalOut AP_Reset(AP_Reset_11_4);
                AP_Reset = 0;
                Cy_SysLib_Delay(50);
                led_flag = 1;
                LED_D1_RED();
                AP_Reset = 1;
               
            }
        }
        
        
        if(Hold_status == 1){
            if(Hold_flag == 1){
                led_flag = 1; 
                LED_D1_GREEN();
            }
            if(Relay_status_flag == 1){
                relay = 0;
            }
            if(Relay_status_flag == 0){
                relay = 1;
            }
            Hold_flag = 0;
        }

        if(Hold_status == 0){
            if(Hold_flag == 0){
                led_flag = 0; 
                LED_D1_ON(); 
            }
            if(Door_status_open_01 == 0){
                
                Door_status_open = 0;
                if(Relay_status_flag == 1){
                    relay = 1;
                }
                if(Relay_status_flag == 0){
                    relay = 0;
                }
                Hold_flag = 1;
            }
        }
 
        if(pc_buffer[0] == 0x0a){
            pc.printf(" Alive ");
            memset(pc_buffer, 0, 2);
        } 
        if(pc_buffer[0] == 0x0c){
            pc.printf("   Application Reset   ");
            Cy_SysLib_Delay(100); 
            NVIC_SystemReset();
        }
    }
}