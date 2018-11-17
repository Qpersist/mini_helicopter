#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"
/********************************************************************************	 
 * 本程序只供学习使用，未经作者许可，不得用于其它任何用途
 * ALIENTEK MiniFly_Remotor
 * 按键驱动代码	
 * 正点原子@ALIENTEK
 * 技术论坛:www.openedv.com
 * 创建日期:2017/5/2
 * 版本：V1.0
 * 版权所有，盗版必究。
 * Copyright(C) 广州市星翼电子科技有限公司 2014-2024
 * All rights reserved
********************************************************************************/   	 

#define KEY_L  		GPIO_Pin_2
#define KEY_S2   	GPIO_Pin_3

#define READ_KEY_L()  	GPIO_ReadInputDataBit(GPIOA,KEY_L)	//读取左按键
#define READ_KEY_J2()  	GPIO_ReadInputDataBit(GPIOA,KEY_S2)	//读取摇杆2按键

//IO初始化
void keyInit(void);

 //按键扫描函数		
void KEY_Scan(void);

#endif



