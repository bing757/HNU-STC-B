#include "STC15F2K60S2.H"        //���롣
#include "sys.H"                 //���롣
#include "displayer.H" 
#include "adc.h"
#include "key.h"
#include "stdio.h"
#include "stdlib.h"
#include "beep.h"
#include "uart1.h" 
#include "music.H"

code unsigned long SysClock=11059200;         //���롣����ϵͳ����ʱ��Ƶ��(Hz)���û������޸ĳ���ʵ�ʹ���Ƶ�ʣ�����ʱѡ��ģ�һ��
#ifdef _displayer_H_                          //��ʾģ��ѡ��ʱ���롣���������ʾ������Ñ����޸ġ����ӵȣ� 
code char decode_table[]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x00,0x08,0x40,0x01, 0x41, 0x48, 0x18,0x70, 0x21, 0x41,
	              /* ���:   0   1    2	   3    4	    5    6	  7   8	   9	 10	   11		12   13    14     15    16     17   18    19*/
                /* ��ʾ:   0   1    2    3    4     5    6    7   8    9  (��)   ��-  ��-  ��-  ����-  ����- |-����  |-  |-����  ��-��-*/ 
													0x48,     0x09,      0x49  ,0x6e , 0x3f, 0x3e,	0x38, 0X6d,	0x79 ,0x73, 0x77};
								/* ���:   20   		 21        22      23    24			25			26	 27    28    29    30*/ 
								/* ��ʾ:  ��-��-    ��-��-     ��-     y      O			U				L		 S     E     P      A */
            
#endif

code unsigned char  song[]={0x16,0x10,0x23,0x20,
														0x22,0x20,0x16,0x10,
														0x23,0x20,0x22,0x20};//���ֱ���
code unsigned char  sound[]={0x23 ,0x06 ,0x25 ,0x02 ,0x26 ,0x04 ,
														0x25 ,0x02 ,0x26 ,0x02 ,0x31 ,0x08 ,
														0x26 ,0x06 ,0x31 ,0x02 ,0x23 ,0x04 };//���ֱ���
unsigned short int xdata shoot[8]={17,10,10,10,10,10,10,10};//��ʼ�����״̬
unsigned short int xdata target0[8]={10,10,10,10,10,10,10,10};//�ϣ���ʾ������Ĺ����
unsigned short int xdata target1[8]={10,10,10,10,10,10,10,10};//�У���ʾ�м�����
unsigned short int xdata target2[8]={10,10,10,10,10,10,10,10};//�£���ʾ����������
unsigned short int xdata bullet[3]={0,0,0};//���������ӵ�
unsigned short int xdata num=0;//����Ŀ������
unsigned char f=0;//��־λ�����ڸ��ֹ��ܵ����ֺ�ʵ��
unsigned short int xdata m=0;//�ɻ���ǰλ��
unsigned short int xdata a=1;//�ӵ��˶����
char buffer[2];//���մ���ͨ������
char x=0;//�����Զ��������ܵ�ʹ��

void init(){ //��ʼ���������ʾ
	int i=0;
	for(i=m+1;i<=7;i++){ //���ݲ�ͬ�������������˺�Ŀ���λ�ã����ı�����ܵ���ʾ��������ʾ����
	if(target0[i]==13){
		if(target1[i]==12){ 
			 if(target2[i]==10) shoot[i]=19;//��-��-
			 else shoot[i]=22;//�� -
		}
		else if(target2[i]==11) shoot[i]=21;//��-��-
		else shoot[i]=13;//��-
	}
	else{ 
	if(target1[i]==12){ 
	   if(target2[i]==11) shoot[i]=20; //��-��-
		 else shoot[i]=12;//��-
		}
	 else if(target2[i]==11) shoot[i]=11;//��-	
	 else shoot[i]=10;//��
	}
}

}
void myadc_callback()//����������
{ 
	if( GetAdcNavAct(enumAdcNavKeyUp) == enumKeyPress ){//�ϰ�������
		if(shoot[m]<18) shoot[m]++;
	}
	if( GetAdcNavAct(enumAdcNavKeyDown) == enumKeyPress ){//�°�������
		if(shoot[m]>16) shoot[m]--;
	}
	if( GetAdcNavAct(enumAdcNavKeyCenter) == enumKeyPress ){//���İ������
		if(shoot[m]==16) bullet[0]=11;
		if(shoot[m]==17) bullet[1]=12;
		if(shoot[m]==18) bullet[2]=13;
	}
	if( GetAdcNavAct(enumAdcNavKeyRight) == enumKeyPress ){ //�Ұ�������
		if(m<3) {shoot[m+1]=shoot[m],shoot[m++]=10;a=m+1;}
	}
	if( GetAdcNavAct(enumAdcNavKeyLeft) == enumKeyPress ){ //�󰴣�����
		if(m>0) {shoot[m-1]=shoot[m],shoot[m--]=10;a=m+1;}
	}
  if( GetAdcNavAct(enumAdcNavKey3) == enumKeyPress )//����k3��������ҡ��Զ���׼���
	{
		if(x==0) x=1;
		else x=0;
	}
}

void my_auto(){ //�Զ�����
   int i=0;
	 for(i=1;i<8;i++){ //����Ŀ��λ���ƶ��ͷ����ӵ�
	 if(target0[i]==13 && x==1){
	   if(shoot[m]<18) shoot[m]++;
		 if(shoot[m]==18) {bullet[2]=13;x=2;}
		  
		}
	 else if(target1[i]==12 && x==1){
	    shoot[m]=17;
		  bullet[1]=12;
		  x=2;
		}
	 else if(target2[i]==11 && x==1){
	    if(shoot[m]>16) shoot[m]--;
		  if(shoot[m]==16) {bullet[0]=11;x=2;}
		}
		}
}

void myKey_callback() //��������
{ 
	char k1=0,k2=0;
	k1=GetKeyAct(enumKey1);//�õ�K1�Ķ���
	if(f!=2){
	if( k1 == enumKeyPress )//����
	{
		f=1;
		Seg7Print(10,10,10,10,10,num/100%10,num/10%10,num%10);//��ʾ�ɼ�
	}//�ɿ�
	else if( k1 == enumKeyRelease ){ 
	 f=0;
	}

	k2=GetKeyAct(enumKey2);
	 if( k2 == enumKeyPress )//����k2��ͣ
	{
		if(f!=3) f=3;
		else f=0;
	}
}
}

//ģ���ӵ������ƶ������ʵ�ִ���жϺ���Ч�ͳɼ���¼�Լ�����my_auto()����
void my100mS_callback(){ 
	int k=1;							
	if(f==0){
	init();
	if(bullet[0]==11) { //��
	
		for(k=m+1;k<=7;k++){
		if(target2[k]==11&&a==k){//�����ж�
		num++;//�ɼ���һ
		target2[a]=10;
		SetBeep(1000,20);//������Ч
		goto f0;
		}
		}
		//ģ���ӵ������ƶ����
		if(shoot[a]==19) shoot[a++]=22;//������
		else if(shoot[a]==13) shoot[a++]=21;//����
		else if(shoot[a]==12) shoot[a++]=20;//����
		else shoot[a++]=bullet[0];
		
		Seg7Print(shoot[0],shoot[1],shoot[2],shoot[3],shoot[4],shoot[5],shoot[6],shoot[7]);	
		if(a>=8) { f0: a=m+1; 	bullet[0]=0;	init(); if(bullet[0]==0 && x==2) x=1;}
	 }
	if(bullet[1]==12) { //��
		for(k=m+1;k<=7;k++){
		if(target1[k]==12&&a==k){//�����ж�
		num++;//�ɼ���һ
		target1[a]=10;
		SetBeep(1000,20);//������Ч
		goto f1;
		}
		}
		if(shoot[a]==21) shoot[a++]=22;//������
		else if(shoot[a]==13) shoot[a++]=19;//����
		else if(shoot[a]==11) shoot[a++]=20;//����
		else shoot[a++]=bullet[1];
		
		Seg7Print(shoot[0],shoot[1],shoot[2],shoot[3],shoot[4],shoot[5],shoot[6],shoot[7]);	
		if(a>=8) {f1: a=m+1; 	bullet[1]=0; init();if(bullet[1]==0 && x==2) x=1;}
	 }
	 if(bullet[2]==13) { //��
		for(k=m+1;k<=7;k++){
		if(target0[k]==13&&a==k){//�����ж�
		num++;//�ɼ���һ
		target0[a]=10;
		SetBeep(1000,20);//������Ч
		goto f2;
		}
		}
		 
		if(shoot[a]==20) shoot[a++]=22;//������
		else if(shoot[a]==12) shoot[a++]=19;//����
		else if(shoot[a]==11) shoot[a++]=21;//����
		else shoot[a++]=bullet[2];
		
		Seg7Print(shoot[0],shoot[1],shoot[2],shoot[3],shoot[4],shoot[5],shoot[6],shoot[7]);	
		 if(a>=8) { 
			f2: a=m+1; 	bullet[2]=0;	init();	if(bullet[2]==0 && x==2) x=1;}
	 }
   }
	 if(x==1){
	  my_auto();
	 }
}

char p=0;//��־λ
char r=1;//��־λ
unsigned char s=0x80;//����led����ʾ
int e=0;//��־λ
void my10mS_callback() //��Ҫ������Ϸ��ʾ
{ 
	if(f==0){ //��Ϸ��ʾ
	Seg7Print(shoot[0],shoot[1],shoot[2],shoot[3],shoot[4],shoot[5],shoot[6],shoot[7]);
	if(e++%7==0)
	if(r==1){
	if(bullet[0]==11 || bullet[1]==12 || bullet[2]==13) r=2;}
	else{
	if(s==0x0) {s=0x80;r=1;}
	else s=s>>1;
	LedPrint(s); //led����ʾ
	}
	}
	if(f==2) {//��Ϸʧ�ܣ���ʾ��YOU LOSE��
	Seg7Print(23,24,25,10,26,24,27,28); 
  if(p==0) {
	SetBeep(1000,40);//��������������
	p=1;
	}}
	
	if(num>=25){ //��ʾ��YOU PASS����ʾ��Ϸ�ɹ�,�ͷ�����Ч
	Seg7Print(23,24,25,10,29,30,27,27);
	f=4;
	if(p==0){
	SetMusic(90,0xFA,sound,sizeof(sound),enumMscNull);
	SetPlayerMode(enumModePlay);
  p=1;}
	}
}

unsigned short int Rop;//�������ֵ��Ϊ���������
void Myadc()
{ 
	struct_ADC adc ;
	adc = GetADC();
	Rop=adc.Rop;
}

short int t=0;
void my1S_callback() 
{ short int b=1;
	srand(Rop);
	if(f==0){ 
	if(t++%2==0){ //���Ŀ�����
	int rd1=rand()%7;
	int rd2=rand()%3+5;
	switch(rd1){
	case 0:
		target0[rd2]=13;
		break;
	case 1:
		target1[rd2]=12;
		break;
	case 2:
		target2[rd2]=11;
		break;
	case 3:
		target2[rd2]=11;
		target0[rd2]=13;
		break;
	case 4:
		target1[rd2]=12;
		target0[rd2]=13;
		break;
	case 5:
		target1[rd2]=12;
		target2[rd2]=11;
		break;
	case 6:
		target0[rd2]=13;
		target1[rd2]=12;
		target2[rd2]=11;
		break;
	}
	}
	if(t%2==0){ //Ŀ���ƶ�
	for(b=m;b<7;b++){ 
		target0[b]=target0[b+1]; 
		target1[b]=target1[b+1];
	  target2[b]=target2[b+1];
	}target0[b]=10;target1[b]=10;target2[b]=10;
  }
	//�ж��ɻ���Ŀ���Ƿ�����
	if(target0[m]!=10 || target1[m]!=10 || target2[m]!=10 ) f=2; 
}
}

void uart1rxd_callback(){
	if(buffer[0] == 0x1 ){  //����
		if(shoot[m]<18) shoot[m]++;
	}
	
	if(buffer[0] == 0x2 ){  //����
		if(shoot[m]>16) shoot[m]--;
	}
	if( buffer[0] == 0x3 ){//���
		if(shoot[m]==16) bullet[0]=11;
		if(shoot[m]==17) bullet[1]=12;
		if(shoot[m]==18) bullet[2]=13;
	}
	if( buffer[0] == 0x4 ){ //����
		if(m<3) {shoot[m+1]=shoot[m],shoot[m++]=10;a=m+1;}
	}
	if( buffer[0] == 0x5 ){ //����
		if(m>0) {shoot[m-1]=shoot[m],shoot[m--]=10;a=m+1;}
	}
	if(f!=2){
	if( buffer[0] == 0x6 ){ //��ͣ
		if(f!=3) f=3;
		else f=0;
	}
	else if( buffer[0] == 0x7 ){ //��ʾ�ɼ�
		if(f!=1) {f=1;Seg7Print(10,10,10,10,10,num/100%10,num/10%10,num%10);}
		else f=0;
	}
  }
	if( buffer[0] == 0x8 ){ //�Զ�����
		if(x==0) x=1;
		else x=0;
	}
}


void main() 
{ DisplayerInit();
	KeyInit();
	BeepInit();
	MusicPlayerInit();
	AdcInit(ADCexpEXT);
	Uart1Init(2400);
	LedPrint(0);
	SetUart1Rxd(buffer, 2, buffer, 1);//���ý�������
	SetPlayerMode(enumModePlay);
	SetMusic(200,0xFA,song,sizeof(song),enumMscNull);
	SetEventCallBack(enumEventUart1Rxd, uart1rxd_callback);
	SetEventCallBack(enumEventKey, myKey_callback);
	SetEventCallBack(enumEventNav,myadc_callback);
	SetEventCallBack(enumEventXADC,Myadc);
	SetEventCallBack(enumEventSys10mS,my10mS_callback);
	SetEventCallBack(enumEventSys100mS,my100mS_callback);
	SetEventCallBack(enumEventSys1S,my1S_callback);
	SetDisplayerArea(0,7);	
	init();
  Seg7Print(shoot[0],shoot[1],shoot[2],shoot[3],shoot[4],shoot[5],shoot[6],shoot[7]);	
  MySTC_Init();	    
	while(1)             	
		{ MySTC_OS();    
		}	             
}                 