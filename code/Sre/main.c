#include "stm32f10x.h"
#include "lcd.h"
#include "adc.h"
#include "usart.h"
#include "key.h"
#include "led.h"

u32 TimingDelay = 0;
char disbuff[100];
extern int time_s;
extern int time_ms;
char flag_UI;
float Vmax = 3,Vmin = 1;
extern char RxBuffer1[100];
extern char RXOVER;
float uvmax,uvmin;
float kvmax,kvmin;
int i;
char led1_flag,led2_flag,led3_flag;
float history_voltage[4];
char Tim_flag = 0;



void Delay_Ms(u32 nTime);
void UI_DATA();
void UI_PARA();
void UART_handle();
void LED_handle();
void to_obtain_voltage(); 
void determine_tim();
//Main Body
int main(void)
{
	
	SysTick_Config(SystemCoreClock/1000);

	Delay_Ms(200);
	Init_adc();
	Init_usart();
	Init_key();
	Init_led();
	STM3210B_LCD_Init();
	LCD_Clear(Black);
	LCD_SetBackColor(Black);
	LCD_SetTextColor(White);
	
	Ctro_led(ledall,0);
	history_voltage[0] = volt_adc();
	history_voltage[1] = volt_adc();
	
	while(1){
		if(flag_UI)	UI_PARA();
		else	UI_DATA();
	
		UART_handle();
		determine_tim();
		LED_handle();
	}
}

void EXTI0_IRQHandler(void)
{
	static char flag_panduan;
  if(EXTI_GetITStatus(EXTI_Line0) != RESET)
  {
		i= 0xffff;
		while(i--);
		if(flag_UI == 0){
			kvmax = Vmax;
			kvmin = Vmin;
		}
//		
		flag_UI=!flag_UI;
//    //LCD_Clear(Black);
		if(flag_UI == 0){			//需要在切换回数据界面时判断
			flag_panduan = 1;
		}
		if(flag_panduan == 1){
			if(Vmax-Vmin-1>0){
		if((Vmax>0)&&(Vmax<3.3)){
		if((Vmin>0)&&(Vmin<3.3)){
			led2_flag = 0;
//			Vmax = uvmax;
//			Vmin = uvmin;
		}
		else{
			  Vmax = kvmax;
			  Vmin = kvmin;
				led2_flag = 1;
		}	
		}
			else{
			  Vmax = kvmax;
			  Vmin = kvmin;
				led2_flag = 1;
		}	
			}
			else{
			  Vmax = kvmax;
			  Vmin = kvmin;
				led2_flag = 1;
		}	
			flag_panduan = 0;
		}
		
		
		
	  /* Clear the  EXTI line 0 pending bit */
    EXTI_ClearITPendingBit(EXTI_Line0);
  }
}

void EXTI9_5_IRQHandler(void)
{
	 if(EXTI_GetITStatus(EXTI_Line8) != RESET)
  {
		i= 0xffff;
		
		while(i--);
    
		if(flag_UI == 1) Vmax+=0.1;
		if(Vmax>3.3)	Vmax = 0;
    /* Clear the  EXTI line 0 pending bit */
    EXTI_ClearITPendingBit(EXTI_Line8);
  }
}
void EXTI1_IRQHandler(void)
{
  if(EXTI_GetITStatus(EXTI_Line1) != RESET)
  {
		i= 0xffff;
		while(i--);
    
		if(flag_UI == 1) Vmin+=0.1;
		if(Vmin>3.3)	Vmin = 0;
    /* Clear the  EXTI line 0 pending bit */
    EXTI_ClearITPendingBit(EXTI_Line1);
  }
}
void EXTI2_IRQHandler(void)
{
  if(EXTI_GetITStatus(EXTI_Line2) != RESET)
  {
    i= 0xffff;
		while(i--);
		
    /* Clear the  EXTI line 0 pending bit */
    EXTI_ClearITPendingBit(EXTI_Line2);
  }

}
//
void UART_handle(){
	if(RXOVER){
		
		int i;
		LCD_DisplayStringLine(Line6,(unsigned char *)RxBuffer1);
		uvmax = (RxBuffer1[0]-'0') + (RxBuffer1[2]-'0')*0.1;
		uvmin = (RxBuffer1[4]-'0') + (RxBuffer1[6]-'0')*0.1;
		if(uvmax-uvmin-1>0){
		if((uvmax>0)&&(uvmax<3.3)){
		if((uvmin>0)&&(uvmin<3.3)){
			Vmax = uvmax;
			Vmin = uvmin;
			led3_flag = 0;
		}
		else led3_flag = 1;
		}
		else led3_flag = 1;
		}
		else led3_flag = 1;
		for(i=0;i<100;i++)	RxBuffer1[i] = 0;
		USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	}
}
void Delay_Ms(u32 nTime)
{
	TimingDelay = nTime;
	while(TimingDelay != 0);	
}
void UI_DATA(){
		LCD_DisplayStringLine(Line0,(unsigned char *)"      Data");
		sprintf(disbuff," V:%.2fV      ", volt_adc());
		
		if(time_ms%50 > 40) LCD_DisplayStringLine(Line2,(unsigned char *)disbuff);
		sprintf(disbuff," T:%ds      ",time_s);
		LCD_DisplayStringLine(Line3,(unsigned char *)disbuff);
		to_obtain_voltage();

}
void UI_PARA(){
		LCD_DisplayStringLine(Line0,(unsigned char *)"      Para");
		sprintf(disbuff," Vmax:%.1fV  ", Vmax);
		LCD_DisplayStringLine(Line2,(unsigned char *)disbuff);
		sprintf(disbuff," Vmin:%.1fV  ", Vmin);
		LCD_DisplayStringLine(Line3,(unsigned char *)disbuff);
}
void LED_handle(){
	if(Tim_flag == 1)	Ctro_led(led1,1);
	else								Ctro_led(led1,0);
	
	if(led2_flag == 1)	Ctro_led(led2,1);
	else								Ctro_led(led2,0);
	
	if(led3_flag == 1)	Ctro_led(led3,1);
	else								Ctro_led(led3,0);
	
}
void to_obtain_voltage(){
	if(time_ms%10 == 0){
	history_voltage[1] = history_voltage[0];
	history_voltage[0] = volt_adc();}
}
void determine_tim(){
	
	if((history_voltage[0] > Vmin)&&(history_voltage[1] < Vmin)){			//实时数据在上面，历史数据在下面，这样就保证了是上升沿触发计时
		Tim_flag = 1;
		time_s = 0;
	}
	
	if((history_voltage[0] > Vmax)&&(history_voltage[1] < Vmax)){
		Tim_flag = 0;
	}
}