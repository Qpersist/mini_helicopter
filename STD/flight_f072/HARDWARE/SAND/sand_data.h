#ifndef __sand_data_H
#define __sand_data_H	 
 #include "stm32f0xx.h"



void sand_ACC_GYRO_data(void);
void sand_IMU_data(void);
void sand_RC_data(void);
void sand_Motor_data(void);
void sand_PID_shell_data(void);
void sand_PID_core_data(void);



void Data_Receive_Prepare(uint8_t data);
void Data_Receive_Anl(uint8_t *data_buf,uint8_t num);

#endif



