#include "timer3.h"
#include  "IMU.h"
#include "sand_data.h"
#include "led.h"
#include "PWM.h"
#include "mpu9250.h"
#include "mpu6050.h"
#include "RC.h"
#include "control.h"
#include "24l01.h"

void tim2_init(uint16_t arr,uint16_t psc)
{
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	
	//timer3�ж�����

	TIM_TimeBaseInitStruct.TIM_Period=arr;
	TIM_TimeBaseInitStruct.TIM_Prescaler=psc;
	TIM_TimeBaseInitStruct.TIM_ClockDivision= TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_CounterMode= TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStruct);
	
	//�ж����ȼ�����
	NVIC_InitStruct.NVIC_IRQChannel=TIM2_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE ;
  NVIC_InitStruct.NVIC_IRQChannelPriority = 0;
	NVIC_Init(&NVIC_InitStruct);

	
	//ʹ��timer3�ж�
	
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
//	TIM_Cmd(TIM2, ENABLE);

}


void tim7_init(uint16_t arr,uint16_t psc)
{
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7,ENABLE);
	
	//timer3�ж�����

	TIM_TimeBaseInitStruct.TIM_Period=arr;
	TIM_TimeBaseInitStruct.TIM_Prescaler=psc;
	TIM_TimeBaseInitStruct.TIM_ClockDivision= TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_CounterMode= TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM7,&TIM_TimeBaseInitStruct);
	
	//�ж����ȼ�����
	NVIC_InitStruct.NVIC_IRQChannel=TIM7_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE ;
	NVIC_InitStruct.NVIC_IRQChannelPriority=0x01;
	NVIC_Init(&NVIC_InitStruct);

	
	//ʹ��timer3�ж�
	
	TIM_ITConfig(TIM7,TIM_IT_Update,ENABLE);
	

}

int times= 0;
int Flag_att = 0;
int att_cnt = 0;
int TIM4_times = 0;
int PWM_cnt = 0;
int PWM_value = 0;
extern int IRQ_timeout;
extern uint8_t Rx_buff[33];
void TIM2_IRQHandler(void)
{
    uint8_t sta = 0;
	if( TIM_GetITStatus(TIM2 ,TIM_IT_Update)==SET)
	{
        
//ʵ�ʲ���ʱ��IRQ�����ж�û�����ã�����൱��ÿ10ms���һ��       
        IRQ_timeout++;
        if(IRQ_timeout > 10)
        {
            IRQ_timeout = 0;

            sta=NRF24L01_Read_Reg(STATUS);  //��ȡ״̬�Ĵ�����ֵ    	 
            NRF24L01_Write_Reg(NRF_WRITE_REG+STATUS,sta); //���TX_DS��MAX_RT�жϱ�־
            if(sta&RX_OK)//���յ�����
            {
                NRF24L01_Read_Buf(RD_RX_PLOAD,Rx_buff,RX_PLOAD_WIDTH);//��ȡ����
                NRF24L01_Write_Reg(FLUSH_RX,0xff);//���RX FIFO�Ĵ��� 
                ReceiveData(Rx_buff);      
            }
        }
        
#if USE_IMU_DEVICE
		PWM_cnt++;
	
		if(PWM_cnt<50)
		{
			PWM_value = 0;
		}
		else if(PWM_cnt>=50&&PWM_cnt<250)
		{
			PWM_value+=5;
			
		}
		else{
			PWM_cnt = 0;
			PWM_value = 0;
		}

#else
        
        times ++;
        Flag_att++;
        if(Flag_att == 1)
        {
            MPU6500_Dataanl(&imu_data.mpu6500_dataacc1,&imu_data.mpu6500_datagyr1);
        }
        else
        {
            Flag_att = 0;
            MPU6500_Dataanl(&imu_data.mpu6500_dataacc2,&imu_data.mpu6500_datagyr2);
        }

        
        if(times %2 == 0)
        {
            Prepare_Data();
            Get_Attitude();
        }
        else if(times % 3 == 0)
        {
            LED0 =!LED0;
        }
        else if(times % 5 == 0)
        {   
            sand_IMU_data();
            sand_ACC_GYRO_data(); 
        }
        if(times % 23 == 0)
            READ_MPU9250_MAG();
			
#endif
	}
    TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
	
}

void TIM7_IRQHandler(void)
{
	if( TIM_GetITStatus(TIM7 ,TIM_IT_Update)==SET)
	{
        
#if USE_IMU_DEVICE
        //��־λ�ı�
        TIM4_times ++;
        //ң��������
        Deblocking();
        mode_contrl();
        //��̬����
        READ_6050();
        Prepare_6050_Data();
        Get_Attitude();
        
        //PID���Ʋ���
        CONTROL(angle.pitch,angle.roll,angle.yaw);
        
        if(TIM4_times % 5==0)
        {

        }
        else if(TIM4_times % 7 == 0)
        {   
            sand_IMU_data();
            sand_ACC_GYRO_data(); 
        }
        else if(TIM4_times %13 == 0)
        {
            sand_RC_data();
            sand_Motor_data();
        }
        
#else
		PWM_cnt++;
	
		if(PWM_cnt<50)
		{
			PWM_value = 0;
		}
		else if(PWM_cnt>=50&&PWM_cnt<250)
		{
			PWM_value+=5;
			
		}
		else{
			PWM_cnt = 0;
			PWM_value = 0;
		}
		Moto_PwmRflash(PWM_value,PWM_value,PWM_value,PWM_value);
#endif
	}
	TIM_ClearITPendingBit(TIM7,TIM_IT_Update);
}