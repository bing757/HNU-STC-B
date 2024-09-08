#include "STC15F2K60S2.H"
#include "sys.H"
#include "adc.h"   
#include "displayer.H"
#include "key.H"
#include "uart1.h"  
#include "beep.h"// ?????????
#include "hall.h"
#include "vib.h"
#include "IR.h"
#include "FM_radio.h"

code unsigned long SysClock = 11059200L; // ????????

#ifdef _displayer_H_                          //??????????(????????,?????????) 
code char decode_table[]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x00,0x08,0x40,0x01,0x41,0x48,0x76,0x38,0x40,0x00,	
	              /* ??:   0   	1    2	   3    4	    5    6	  7   8	   	9	 		10	11	 12   13   14   15    16   17   18   19	 */
                /* ??:   0   	1    2     3    4     5    6    7   8    9  (?)   ?-  ?-  ?-  ??-  ??-  H    L    - 	(?)*/  
	                       0x3f|0x80,0x06|0x80,0x5b|0x80,0x4f|0x80,0x66|0x80,0x6d|0x80,0x7d|0x80,0x07|0x80,0x7f|0x80,0x6f|0x80 ,0x54,0x3e,0x39,0x79};  
             /* ????     20         21         22         23      24        25        26        27        28        29        30n  31U 32C 33E */
#endif
 
//unsigned char temp[] = {0xc0, 0x60, 0x30, 0x18, 0x0c, 0x06, 0x03};  // LED pattern
int i = 3;  // Start index
unsigned char key_press_flag = 0;  // Key press flag for debouncing

// send to PC
void send_key_signal(unsigned char key) {
    SBUF = key;  // ????????????
    while (!TI); // ??????
    TI = 0;      // ??????
}


// ??????
void mydh_callback() {
    // ?????????
    if (GetAdcNavAct(enumAdcNavKeyLeft) == enumKeyPress) {
        
            key_press_flag = 1;  // ????
            Seg7Print(3, 0, 0, 0, 0, 0, 0, 0);  // ???????3
            send_key_signal(3);  // ??????
        
    }
    // ?????????
    else if (GetAdcNavAct(enumAdcNavKeyRight) == enumKeyPress) {
        
           
            key_press_flag = 1;  // ????
            Seg7Print(4, 0, 0, 0, 0, 0, 0, 0);  // ???????4
            send_key_signal(4);  // ??????
        
    }
    // ?????????
    else if (GetAdcNavAct(enumAdcNavKeyUp) == enumKeyPress) {
        key_press_flag = 1;  // ????
        Seg7Print(1, 0, 0, 0, 0, 0, 0, 0);  // ???????1
        send_key_signal(1);  // ??????
    }
    // ?????????
    else if (GetAdcNavAct(enumAdcNavKeyDown) == enumKeyPress) {
        key_press_flag = 1;  // ????
        Seg7Print(2, 0, 0, 0, 0, 0, 0, 0);  // ???????2
        send_key_signal(2);  // ??????
    }
    // Center/Hall/Vib ==> tank stop
        else if ((GetAdcNavAct(enumAdcNavKeyCenter) == enumKeyPress)||(GetHallAct() == enumHallGetClose) ||(GetVibAct()==enumVibQuake)) {
        key_press_flag = 1;  // ????
        Seg7Print(5, 0, 0, 0, 0, 0, 0, 0);  // ???????5
        send_key_signal(5);  // ??????
    }
    else if (GetKeyAct(enumKey1) == enumKeyPress) {
        key_press_flag = 1;  // ????
        Seg7Print(6, 0, 0, 0, 0, 0, 0, 0);  // ???????5
        send_key_signal(6);  // ??????
    }

    // ????????????
    /*if (key_press_flag) {
        LedPrint(temp[i]);  // ??LED??
        key_press_flag = 0;  // ????
    }*/
}

char rxd[1];
char R[1];
char bee[1];
char buf1[1]={0x03};
char buf2[1]={0x04};
char buf3[1]={0x01};
char buf4[1]={0x02};
char buf5[1]={0x05};
char buf6[1]={0x06};
//char matchhead[2]={0xaa,0x55};
 
void uart1rxd_callback();
void ir_callback();
void key_callback();
void adc1ms_callback();
void displayer1s_callback();

unsigned int adc_10bit = 0;			//????adc 10bit
unsigned long sumt = 0;				//??
unsigned int data_temp = 0;			//???????
unsigned int t = 0;					//??
unsigned int flag;					//???
int temp = 0;
struct_ADC adc_data;			
unsigned int light=0;//Rop
int code tempdata[]={239,197,175,160,150,142,135,129,124,120,116,113,109,107,104,101, 
										  99, 97, 95, 93, 91, 90, 88, 86, 85, 84, 82, 81, 80, 78, 77, 76, 
										  75, 74, 73, 72, 71, 70, 69, 68, 67, 67, 66, 65, 64, 63, 63, 62, 
										  61, 61, 60, 59, 58, 58, 57, 57, 56, 55, 55, 54, 54, 53, 52, 52, 
										  51, 51, 50, 50, 49, 49, 48, 48, 47, 47, 46, 46, 45, 45, 44, 44, 
										  43, 43, 42, 42, 41, 41, 41, 40, 40, 39, 39, 38, 38, 38, 37, 37, 
										  36, 36, 36, 35, 35, 34, 34, 34, 33, 33, 32, 32, 32, 31, 31, 31, 
										  30, 30, 29, 29, 29, 28, 28, 28, 27, 27, 27, 26, 26, 26, 25, 25,
										  24, 24, 24, 23, 23, 23, 22, 22, 22, 21, 21, 21, 20, 20, 20, 19, 
										  19, 19, 18, 18, 18, 17, 17, 16, 16, 16, 15, 15, 15, 14, 14, 14, 
										  13, 13, 13, 12, 12, 12, 11, 11, 11, 10, 10, 9, 9, 9, 8, 8, 8, 7, 
										  7, 7, 6, 6,5, 5, 54,4, 3, 3,3, 2, 2, 1, 1, 1, 0, 0, -1, -1, -1, 
										  -2, -2, -3, -3, -4, -4, -5, -5, -6, -6, -7, -7, -8, -8, -9, -9, 
										  -10, -10, -11, -11, -12, -13, -13, -14, -14, -15, -16, -16, -17, 
										  -18, -19, -19, -20, -21, -22, -23, -24, -25, -26, -27, -28, -29, 
										  -30, -32, -33, -35, -36, -38, -40, -43, -46, -50, -55, -63, 361};
unsigned int ktwo = 0;
unsigned int move = 0;
//void beep_callback();
struct_FMRadio radio = {975,6,1,0,0};
void main() {
      HallInit(); 
      VibInit(); 
    DisplayerInit();
    KeyInit();
	
	 
		FMRadioInit(radio);

      IrInit(NEC_R05d);
      BeepInit();
    AdcInit(ADCexpEXT);
    SetDisplayerArea(0, 7);
    
    Uart1Init(9600);  // ?????,????9600
    
    SetUart1Rxd(rxd, 1, 0, 0);
      SetIrRxd(R,1);
    
      bee[0]=rxd[0];
        //Seg7Print(10,10,10,10,10,10,10,10);
    LedPrint(0);
    
    
   // Seg7Print(0, 0, 0, 0, 0, 0, 0, 0);  // ??????
		Seg7Print(16,30,31,12,32,5,33,33);//display "HnU-C5EE"
    SetEventCallBack(enumEventXADC, mydh_callback);
    SetEventCallBack(enumEventUart1Rxd, uart1rxd_callback);
      SetEventCallBack(enumEventIrRxd,ir_callback);
			//key -->ADC
      SetEventCallBack(enumEventXADC,key_callback);
			
			SetEventCallBack(enumEventSys1mS,adc1ms_callback);//get light and temperature
			SetEventCallBack(enumEventSys1S,displayer1s_callback);//display 0--7

    MySTC_Init();
    
    while (1) {
        MySTC_OS();  // ???????
    }
}
void uart1rxd_callback(){
    LedPrint(rxd[0]);
      if ((rxd[0] == 07 || rxd[0] == 03 || rxd[0] == 01) && bee[0] != rxd[0]) {
        SetBeep(1000, 20);
    }
    if (rxd[0] == 00 && bee[0] != rxd[0]) {
        SetBeep(200, 100);
    }
            
        bee[0]=rxd[0];
        
    return;
}

void ir_callback(){
      if (R[0]==3){
      key_press_flag = 1;  // ????
            Seg7Print(3, 0, 0, 0, 0, 0, 0, 0);  // ???????3
            send_key_signal(3);  // ??????
        
    }
    // ?????????
    else if (R[0]==4) {           
            key_press_flag = 1;  // ????
            Seg7Print(4, 0, 0, 0, 0, 0, 0, 0);  // ???????4
            send_key_signal(4);  // ??????
        
    }
    // ?????????
    else if (R[0]==1) {
        key_press_flag = 1;  // ????
        Seg7Print(1, 0, 0, 0, 0, 0, 0, 0);  // ???????1
        send_key_signal(1);  // ??????
    }
    // ?????????
    else if (R[0]==2) {
        key_press_flag = 1;  // ????
        Seg7Print(2, 0, 0, 0, 0, 0, 0, 0);  // ???????2
        send_key_signal(2);  // ??????
    }
    // ?????????
        else if (R[0]==5) {
        key_press_flag = 1;  // ????
        Seg7Print(5, 0, 0, 0, 0, 0, 0, 0);  // ???????5
        send_key_signal(5);  // ??????
    }
    else if (R[0]==6) {
        key_press_flag = 1;  // ????
        Seg7Print(6, 0, 0, 0, 0, 0, 0, 0);  // ???????5
        send_key_signal(6);  // ??????
    }
    }

void key_callback(){
    if(GetAdcNavAct(enumAdcNavKeyLeft) == enumKeyPress)
		{
			send_key_signal(3);  // ??????
			IrPrint(buf1, 1);
			move = 3;
		}
        
    else if(GetAdcNavAct(enumAdcNavKeyRight) == enumKeyPress)
		{
			IrPrint(buf2, 1);
			send_key_signal(4);  // ??????
			move =4 ;
		}
        
    else if(GetAdcNavAct(enumAdcNavKeyUp) == enumKeyPress)
		{
			IrPrint(buf3, 1);
			send_key_signal(1);  // ??????
			move =1 ;
		}
        
    else if(GetAdcNavAct(enumAdcNavKeyDown) == enumKeyPress)
		{
			 send_key_signal(2);  // ??????
			IrPrint(buf4, 1);
			move = 2;
			
		}
        
    /*else if(GetKeyAct(enumKey2) == enumKeyPress)
		{
				IrPrint(buf5, 1);
				send_key_signal(5);  // ??????
		}*/
        
    else if(GetKeyAct(enumKey1) == enumKeyPress)
		{
			 IrPrint(buf6, 1);
			move = 6;
				send_key_signal(6);  // ??????
		}
		else if ((GetAdcNavAct(enumAdcNavKeyCenter) == enumKeyPress)||(GetHallAct() == enumHallGetClose) ||(GetVibAct()==enumVibQuake)) {
        
        Seg7Print(5, 0, 0, 0, 0, 0, 0, 0);  // ???????5
			move = 5;
        send_key_signal(5);  // ??????
				IrPrint(buf5, 1);
    }
        
}
void adc1ms_callback(){
	t++;
	if(t==200){
			data_temp=(sumt+t/2)/t;					//????
			temp=tempdata[data_temp-1]; 			//?????AD????
			if(temp<0){
				flag = 12;							//?????,????
			}
			else{
				flag = 0;
			}
			sumt=0;
			t=0;
	}
	adc_data = GetADC();
	adc_10bit = adc_data.Rt;
	data_temp=adc_10bit>>2;							//?10?AD????8?AD?
	sumt+=data_temp;								//?t?AD???
	//light
	light = adc_data.Rop;
	if (ktwo == 0)
	{
		//light
		send_key_signal(light);
	}
		
	//send_key_signal(5);
	//Seg7Print(light/100,light/10%10,light%10,0,6,flag,temp/10,temp%10);
}
void displayer1s_callback(){
	if (GetKeyAct(enumKey2) == enumKeyPress && ktwo!=2)
		ktwo = 2;
	
	if (ktwo == 2)
	
	{
		Seg7Print(light/100,light/10%10,light%10,0,move,flag,temp/10,temp%10);
	}
		
	else
		Seg7Print(16,30,31,12,32,5,33,33);//display "HnU-C5EE"
}