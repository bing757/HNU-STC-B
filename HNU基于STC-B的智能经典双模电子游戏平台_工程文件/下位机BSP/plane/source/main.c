#include "STC15F2K60S2.H"        //必须。
#include "sys.H"                 //必须。
#include "displayer.H" 
#include "adc.h"
#include "key.h"
#include "stdio.h"
#include "stdlib.h"
#include "beep.h"
#include "uart1.h" 
#include "music.H"

code unsigned long SysClock=11059200;         //必须。定义系统工作时钟频率(Hz)，用户必须修改成与实际工作频率（下载时选择的）一致
#ifdef _displayer_H_                          //显示模块选用时必须。（数码管显示译码表，用戶可修改、增加等） 
code char decode_table[]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x00,0x08,0x40,0x01, 0x41, 0x48, 0x18,0x70, 0x21, 0x41,
	              /* 序号:   0   1    2	   3    4	    5    6	  7   8	   9	 10	   11		12   13    14     15    16     17   18    19*/
                /* 显示:   0   1    2    3    4     5    6    7   8    9  (无)   下-  中-  上-  上中-  中下- |-下移  |-  |-上移  上-中-*/ 
													0x48,     0x09,      0x49  ,0x6e , 0x3f, 0x3e,	0x38, 0X6d,	0x79 ,0x73, 0x77};
								/* 序号:   20   		 21        22      23    24			25			26	 27    28    29    30*/ 
								/* 显示:  下-中-    上-下-     三-     y      O			U				L		 S     E     P      A */
            
#endif

code unsigned char  song[]={0x16,0x10,0x23,0x20,
														0x22,0x20,0x16,0x10,
														0x23,0x20,0x22,0x20};//音乐编码
code unsigned char  sound[]={0x23 ,0x06 ,0x25 ,0x02 ,0x26 ,0x04 ,
														0x25 ,0x02 ,0x26 ,0x02 ,0x31 ,0x08 ,
														0x26 ,0x06 ,0x31 ,0x02 ,0x23 ,0x04 };//音乐编码
unsigned short int xdata shoot[8]={17,10,10,10,10,10,10,10};//初始化射击状态
unsigned short int xdata target0[8]={10,10,10,10,10,10,10,10};//上（表示最上面的轨道）
unsigned short int xdata target1[8]={10,10,10,10,10,10,10,10};//中（表示中间轨道）
unsigned short int xdata target2[8]={10,10,10,10,10,10,10,10};//下（表示最下面轨道）
unsigned short int xdata bullet[3]={0,0,0};//代表三行子弹
unsigned short int xdata num=0;//消灭目标数量
unsigned char f=0;//标志位，用于各种功能的区分和实现
unsigned short int xdata m=0;//飞机当前位置
unsigned short int xdata a=1;//子弹运动轨道
char buffer[2];//接收串口通信数据
char x=0;//用于自动导航功能的使用

void init(){ //初始化数码管显示
	int i=0;
	for(i=m+1;i<=7;i++){ //根据不同轨道情况（即敌人和目标的位置）来改变数码管的显示，避免显示混乱
	if(target0[i]==13){
		if(target1[i]==12){ 
			 if(target2[i]==10) shoot[i]=19;//上-中-
			 else shoot[i]=22;//三 -
		}
		else if(target2[i]==11) shoot[i]=21;//上-下-
		else shoot[i]=13;//上-
	}
	else{ 
	if(target1[i]==12){ 
	   if(target2[i]==11) shoot[i]=20; //下-中-
		 else shoot[i]=12;//中-
		}
	 else if(target2[i]==11) shoot[i]=11;//下-	
	 else shoot[i]=10;//无
	}
}

}
void myadc_callback()//导航键功能
{ 
	if( GetAdcNavAct(enumAdcNavKeyUp) == enumKeyPress ){//上按，上移
		if(shoot[m]<18) shoot[m]++;
	}
	if( GetAdcNavAct(enumAdcNavKeyDown) == enumKeyPress ){//下按，下移
		if(shoot[m]>16) shoot[m]--;
	}
	if( GetAdcNavAct(enumAdcNavKeyCenter) == enumKeyPress ){//中心按，射击
		if(shoot[m]==16) bullet[0]=11;
		if(shoot[m]==17) bullet[1]=12;
		if(shoot[m]==18) bullet[2]=13;
	}
	if( GetAdcNavAct(enumAdcNavKeyRight) == enumKeyPress ){ //右按，右移
		if(m<3) {shoot[m+1]=shoot[m],shoot[m++]=10;a=m+1;}
	}
	if( GetAdcNavAct(enumAdcNavKeyLeft) == enumKeyPress ){ //左按，左移
		if(m>0) {shoot[m-1]=shoot[m],shoot[m--]=10;a=m+1;}
	}
  if( GetAdcNavAct(enumAdcNavKey3) == enumKeyPress )//按下k3开启”外挂”自动精准打击
	{
		if(x==0) x=1;
		else x=0;
	}
}

void my_auto(){ //自动导航
   int i=0;
	 for(i=1;i<8;i++){ //根据目标位置移动和发射子弹
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

void myKey_callback() //按键功能
{ 
	char k1=0,k2=0;
	k1=GetKeyAct(enumKey1);//得到K1的动作
	if(f!=2){
	if( k1 == enumKeyPress )//按下
	{
		f=1;
		Seg7Print(10,10,10,10,10,num/100%10,num/10%10,num%10);//显示成绩
	}//松开
	else if( k1 == enumKeyRelease ){ 
	 f=0;
	}

	k2=GetKeyAct(enumKey2);
	 if( k2 == enumKeyPress )//按下k2暂停
	{
		if(f!=3) f=3;
		else f=0;
	}
}
}

//模拟子弹发射移动轨道，实现打击判断和音效和成绩记录以及调用my_auto()函数
void my100mS_callback(){ 
	int k=1;							
	if(f==0){
	init();
	if(bullet[0]==11) { //下
	
		for(k=m+1;k<=7;k++){
		if(target2[k]==11&&a==k){//击中判断
		num++;//成绩加一
		target2[a]=10;
		SetBeep(1000,20);//击中音效
		goto f0;
		}
		}
		//模拟子弹发射移动轨道
		if(shoot[a]==19) shoot[a++]=22;//上中下
		else if(shoot[a]==13) shoot[a++]=21;//上下
		else if(shoot[a]==12) shoot[a++]=20;//下中
		else shoot[a++]=bullet[0];
		
		Seg7Print(shoot[0],shoot[1],shoot[2],shoot[3],shoot[4],shoot[5],shoot[6],shoot[7]);	
		if(a>=8) { f0: a=m+1; 	bullet[0]=0;	init(); if(bullet[0]==0 && x==2) x=1;}
	 }
	if(bullet[1]==12) { //中
		for(k=m+1;k<=7;k++){
		if(target1[k]==12&&a==k){//击中判断
		num++;//成绩加一
		target1[a]=10;
		SetBeep(1000,20);//击中音效
		goto f1;
		}
		}
		if(shoot[a]==21) shoot[a++]=22;//上中下
		else if(shoot[a]==13) shoot[a++]=19;//上中
		else if(shoot[a]==11) shoot[a++]=20;//下中
		else shoot[a++]=bullet[1];
		
		Seg7Print(shoot[0],shoot[1],shoot[2],shoot[3],shoot[4],shoot[5],shoot[6],shoot[7]);	
		if(a>=8) {f1: a=m+1; 	bullet[1]=0; init();if(bullet[1]==0 && x==2) x=1;}
	 }
	 if(bullet[2]==13) { //上
		for(k=m+1;k<=7;k++){
		if(target0[k]==13&&a==k){//击中判断
		num++;//成绩加一
		target0[a]=10;
		SetBeep(1000,20);//击中音效
		goto f2;
		}
		}
		 
		if(shoot[a]==20) shoot[a++]=22;//上中下
		else if(shoot[a]==12) shoot[a++]=19;//上中
		else if(shoot[a]==11) shoot[a++]=21;//下上
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

char p=0;//标志位
char r=1;//标志位
unsigned char s=0x80;//用于led灯显示
int e=0;//标志位
void my10mS_callback() //主要用于游戏显示
{ 
	if(f==0){ //游戏显示
	Seg7Print(shoot[0],shoot[1],shoot[2],shoot[3],shoot[4],shoot[5],shoot[6],shoot[7]);
	if(e++%7==0)
	if(r==1){
	if(bullet[0]==11 || bullet[1]==12 || bullet[2]==13) r=2;}
	else{
	if(s==0x0) {s=0x80;r=1;}
	else s=s>>1;
	LedPrint(s); //led灯显示
	}
	}
	if(f==2) {//游戏失败，显示”YOU LOSE“
	Seg7Print(23,24,25,10,26,24,27,28); 
  if(p==0) {
	SetBeep(1000,40);//蜂鸣器发出声响
	p=1;
	}}
	
	if(num>=25){ //显示”YOU PASS“表示游戏成功,和发出音效
	Seg7Print(23,24,25,10,29,30,27,27);
	f=4;
	if(p==0){
	SetMusic(90,0xFA,sound,sizeof(sound),enumMscNull);
	SetPlayerMode(enumModePlay);
  p=1;}
	}
}

unsigned short int Rop;//保存光照值作为随机数种子
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
	if(t++%2==0){ //随机目标产生
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
	if(t%2==0){ //目标移动
	for(b=m;b<7;b++){ 
		target0[b]=target0[b+1]; 
		target1[b]=target1[b+1];
	  target2[b]=target2[b+1];
	}target0[b]=10;target1[b]=10;target2[b]=10;
  }
	//判定飞机与目标是否相遇
	if(target0[m]!=10 || target1[m]!=10 || target2[m]!=10 ) f=2; 
}
}

void uart1rxd_callback(){
	if(buffer[0] == 0x1 ){  //上移
		if(shoot[m]<18) shoot[m]++;
	}
	
	if(buffer[0] == 0x2 ){  //下移
		if(shoot[m]>16) shoot[m]--;
	}
	if( buffer[0] == 0x3 ){//射击
		if(shoot[m]==16) bullet[0]=11;
		if(shoot[m]==17) bullet[1]=12;
		if(shoot[m]==18) bullet[2]=13;
	}
	if( buffer[0] == 0x4 ){ //右移
		if(m<3) {shoot[m+1]=shoot[m],shoot[m++]=10;a=m+1;}
	}
	if( buffer[0] == 0x5 ){ //左移
		if(m>0) {shoot[m-1]=shoot[m],shoot[m--]=10;a=m+1;}
	}
	if(f!=2){
	if( buffer[0] == 0x6 ){ //暂停
		if(f!=3) f=3;
		else f=0;
	}
	else if( buffer[0] == 0x7 ){ //显示成绩
		if(f!=1) {f=1;Seg7Print(10,10,10,10,10,num/100%10,num/10%10,num%10);}
		else f=0;
	}
  }
	if( buffer[0] == 0x8 ){ //自动导航
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
	SetUart1Rxd(buffer, 2, buffer, 1);//设置接收条件
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