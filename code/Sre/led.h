#ifndef __LED_H
#define __LED_H

#define led1 GPIO_Pin_8
#define led2 GPIO_Pin_9
#define led3 GPIO_Pin_10
#define led4 GPIO_Pin_11
#define led5 GPIO_Pin_12
#define led6 GPIO_Pin_13
#define led7 GPIO_Pin_14
#define led8 GPIO_Pin_15
#define ledall GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15


void Init_led();
void Ctro_led(uint16_t led,char statu);
#endif