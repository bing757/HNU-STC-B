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

#ifdef _displayer_H_                         
code char decode_table[] = {
    0x3f, 0x06, 0x5b, 0x4f, 0x66, 
    0x6d, 0x7d, 0x07, 0x7f, 0x6f, 
    0x00, 0x08, 0x40, 0x01, 0x76, 0x38
};   
#endif
 
unsigned char temp[] = {0xc0, 0x60, 0x30, 0x18, 0x0c, 0x06, 0x03};  // LED pattern
int i = 3;  // Start index
unsigned char key_press_flag = 0;  // Key press flag for debouncing

// ???????PC
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
        Seg7Print(10,10,10,10,10,10,10,10);
    LedPrint(0);
    
    
    Seg7Print(0, 0, 0, 0, 0, 0, 0, 0);  // ??????
    SetEventCallBack(enumEventXADC, mydh_callback);
    SetEventCallBack(enumEventUart1Rxd, uart1rxd_callback);
      SetEventCallBack(enumEventIrRxd,ir_callback);
			//key -->ADC
      SetEventCallBack(enumEventXADC,key_callback);

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
		}
        
    else if(GetAdcNavAct(enumAdcNavKeyRight) == enumKeyPress)
		{
			IrPrint(buf2, 1);
			send_key_signal(4);  // ??????
		}
        
    else if(GetAdcNavAct(enumAdcNavKeyUp) == enumKeyPress)
		{
			IrPrint(buf3, 1);
			send_key_signal(1);  // ??????
		}
        
    else if(GetAdcNavAct(enumAdcNavKeyDown) == enumKeyPress)
		{
			 send_key_signal(2);  // ??????
			IrPrint(buf4, 1);
			
		}
        
    else if(GetKeyAct(enumKey2) == enumKeyPress)
		{
				IrPrint(buf5, 1);
				send_key_signal(5);  // ??????
		}
        
    else if(GetKeyAct(enumKey1) == enumKeyPress)
		{
			 IrPrint(buf6, 1);
				send_key_signal(6);  // ??????
		}
		else if ((GetAdcNavAct(enumAdcNavKeyCenter) == enumKeyPress)||(GetHallAct() == enumHallGetClose) ||(GetVibAct()==enumVibQuake)) {
        
        Seg7Print(5, 0, 0, 0, 0, 0, 0, 0);  // ???????5
        send_key_signal(5);  // ??????
				IrPrint(buf5, 1);
    }
        
}