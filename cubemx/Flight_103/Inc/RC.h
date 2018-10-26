#ifndef __rc_H
#define __rc_H	 
#include "stm32f1xx_hal.h"

typedef struct int16_rcget{
				int16_t ROLL;
				int16_t PITCH;
				int16_t THROTTLE;
				int16_t YAW;
	      int16_t pitch_offset;
	      int16_t roll_offset;
	      int16_t yaw_offset;
				int16_t AUX1;
				int16_t AUX2;
				int16_t AUX3;
				int16_t AUX4;
}T_RC_DATA;

				
extern T_RC_DATA Rc_Data;//1000~2000
extern T_RC_DATA RX_Data;;
extern uint16_t Rc_Pwm_In[8],Rc_Data_5;
extern uint8_t ARMED,mode;
void PWM_IN_Init(void);
void RC_Data_Refine(void);
void Deblocking(void);
void mode_contrl(void);
void Rc_AUX(void);
void RC_Receive_Anl(void);
				
#endif



