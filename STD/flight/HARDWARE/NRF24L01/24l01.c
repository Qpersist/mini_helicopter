#include "24l01.h"
#include "delay.h"
#include "spi.h"
#include "usart.h"
#include "led.h"
#include "RC.h"


#define ReceiveFrameHeaderH	0xAA
#define ReceiveFrameHeaderL	0xAA

#define FrameHeaderH_Addr		0u
#define FrameHeaderL_Addr		1u
#define FuncWord_Addr	2u
#define THR_Addr     	2u
#define YAW_Addr	 	4u
#define ROL_Addr		8u
#define PIT_Addr		6u
    
const u8 TX_ADDRESS[TX_ADR_WIDTH]={0xAA,0xBB,0xCC,0x00,0x01}; //���͵�ַ
const u8 RX_ADDRESS[RX_ADR_WIDTH]={0xAA,0xBB,0xCC,0x00,0x01};

//��ʼ��24L01��IO��
void NRF24L01_Init(void)
{ 	
    GPIO_InitTypeDef GPIO_InitStructure;
    SPI_InitTypeDef  SPI_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
 	NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOA, ENABLE);	 //ʹ��PB,G�˿�ʱ��
    RCC_APB2PeriphClockCmd(	RCC_APB2Periph_SPI1,  ENABLE );//SPI2ʱ��ʹ�� 		
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	//ʹ�ܸ��ù���ʱ��
    
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;				 //PB12���� ��ֹW25X�ĸ���
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);	//��ʼ��ָ��IO
    GPIO_SetBits(GPIOA,GPIO_Pin_4);//����				


    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;	//PG8 7 ���� 	  
    GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��ָ��IO
    GPIO_ResetBits(GPIOA,GPIO_Pin_1);//PB0����	
    				 
    ////////////////// 
    SPI2_Init();    		//��ʼ��SPI	 

    SPI_Cmd(SPI1, DISABLE); // SPI���費ʹ��

    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //SPI����Ϊ˫��˫��ȫ˫��
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//SPI����
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		//���ͽ���8λ֡�ṹ
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;		//ʱ�����յ�
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;	//���ݲ����ڵ�1��ʱ����
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//NSS�ź�����������
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;		//���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ16
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//���ݴ����MSBλ��ʼ
    SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRCֵ����Ķ���ʽ
    SPI_Init(SPI1, &SPI_InitStructure);  //����SPI_InitStruct��ָ���Ĳ�����ʼ������SPIx�Ĵ���

    SPI_Cmd(SPI1, ENABLE); //ʹ��SPI����
         
    NRF24L01_CE=0; 			//ʹ��24L01
    NRF24L01_CSN=1;			//SPIƬѡȡ��  
	delay_ms(1000); 		 
    	
}

void NRF_IRQ_INIT(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
 	NVIC_InitTypeDef NVIC_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO,ENABLE);   //ʹ��GPIOB��ʱ�Ӳ���������ʱ��
    //GPIOE.2 �ж����Լ��жϳ�ʼ������   �½��ش���
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource0);
    
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;   
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //PB1 ���� 
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;   //����GPIO����  
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    
    EXTI_InitStructure.EXTI_Line=EXTI_Line0;	//KEY2
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);	 	//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���
    
    

    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;			//ʹ�ܰ���WK_UP���ڵ��ⲿ�ж�ͨ��
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;	//��ռ���ȼ�2�� 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;					//�����ȼ�3
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//ʹ���ⲿ�ж�ͨ��
  	NVIC_Init(&NVIC_InitStructure); 
    
    GPIO_SetBits(GPIOA,GPIO_Pin_0);//PB0����
}
//���24L01�Ƿ����
//����ֵ:0���ɹ�;1��ʧ��	
u8 NRF24L01_Check(void)
{
	u8 buf[5]={0XA5,0XA5,0XA5,0XA5,0XA5};
    u8 buf_check[5];
	u8 i;
	SPI2_SetSpeed(SPI_BaudRatePrescaler_8); //spi�ٶ�Ϊ9Mhz��24L01�����SPIʱ��Ϊ10Mhz��   	 
	NRF24L01_Write_Buf(NRF_WRITE_REG+TX_ADDR,buf,5);//д��5���ֽڵĵ�ַ.	
	NRF24L01_Read_Buf(TX_ADDR,buf_check,5); //����д��ĵ�ַ  
	for(i=0;i<5;i++)if(buf_check[i]!=0XA5)break;	 							   
	if(i!=5)return 1;//���24L01����	
	return 0;		 //��⵽24L01
}	 	 
//SPIд�Ĵ���
//reg:ָ���Ĵ�����ַ
//value:д���ֵ
u8 NRF24L01_Write_Reg(u8 reg,u8 value)
{
	u8 status;	
   	NRF24L01_CSN=0;                 //ʹ��SPI����
  	status =SPI2_ReadWriteByte(reg);//���ͼĴ����� 
  	SPI2_ReadWriteByte(value);      //д��Ĵ�����ֵ
  	NRF24L01_CSN=1;                 //��ֹSPI����	   
  	return(status);       			//����״ֵ̬
}
//��ȡSPI�Ĵ���ֵ
//reg:Ҫ���ļĴ���
u8 NRF24L01_Read_Reg(u8 reg)
{
	u8 reg_val;	    
 	NRF24L01_CSN = 0;          //ʹ��SPI����		
  	SPI2_ReadWriteByte(reg);   //���ͼĴ�����
  	reg_val=SPI2_ReadWriteByte(0XFF);//��ȡ�Ĵ�������
  	NRF24L01_CSN = 1;          //��ֹSPI����		    
  	return(reg_val);           //����״ֵ̬
}	
//��ָ��λ�ö���ָ�����ȵ�����
//reg:�Ĵ���(λ��)
//*pBuf:����ָ��
//len:���ݳ���
//����ֵ,�˴ζ�����״̬�Ĵ���ֵ 
u8 NRF24L01_Read_Buf(u8 reg,u8 *pBuf,u8 len)
{
	u8 status,u8_ctr;	       
  	NRF24L01_CSN = 0;           //ʹ��SPI����
  	status=SPI2_ReadWriteByte(reg);//���ͼĴ���ֵ(λ��),����ȡ״ֵ̬   	   
 	for(u8_ctr=0;u8_ctr<len;u8_ctr++)pBuf[u8_ctr]=SPI2_ReadWriteByte(0XFF);//��������
  	NRF24L01_CSN=1;       //�ر�SPI����
  	return status;        //���ض�����״ֵ̬
}
//��ָ��λ��дָ�����ȵ�����
//reg:�Ĵ���(λ��)
//*pBuf:����ָ��
//len:���ݳ���
//����ֵ,�˴ζ�����״̬�Ĵ���ֵ
u8 NRF24L01_Write_Buf(u8 reg, u8 *pBuf, u8 len)
{
	u8 status,u8_ctr;	    
 	NRF24L01_CSN = 0;          //ʹ��SPI����
  	status = SPI2_ReadWriteByte(reg);//���ͼĴ���ֵ(λ��),����ȡ״ֵ̬
  	for(u8_ctr=0; u8_ctr<len; u8_ctr++)SPI2_ReadWriteByte(*pBuf++); //д������	 
  	NRF24L01_CSN = 1;       //�ر�SPI����
  	return status;          //���ض�����״ֵ̬
}				   
//����NRF24L01����һ������
//txbuf:�����������׵�ַ
//����ֵ:�������״��
u8 NRF24L01_TxPacket(u8 *txbuf)
{
	u8 sta;
 	SPI2_SetSpeed(SPI_BaudRatePrescaler_8);//spi�ٶ�Ϊ9Mhz��24L01�����SPIʱ��Ϊ10Mhz��   
	NRF24L01_CE=0;
  	NRF24L01_Write_Buf(WR_TX_PLOAD,txbuf,TX_PLOAD_WIDTH);//д���ݵ�TX BUF  32���ֽ�
 	NRF24L01_CE=1;//��������
    delay_ms(10);
	while(NRF24L01_IRQ!=0);//�ȴ��������
	sta=NRF24L01_Read_Reg(NRF_READ_REG + STATUS);  //��ȡ״̬�Ĵ�����ֵ	   
	NRF24L01_Write_Reg(NRF_WRITE_REG+STATUS,sta); //���TX_DS��MAX_RT�жϱ�־
	if(sta&MAX_TX)//�ﵽ����ط�����
	{
		NRF24L01_Write_Reg(FLUSH_TX,0xff);//���TX FIFO�Ĵ��� 
		return MAX_TX; 
	}
	if(sta&TX_OK)//�������
	{
		return TX_OK;
	}
	return 0xff;//����ԭ����ʧ��
}
//����NRF24L01����һ������
//txbuf:�����������׵�ַ
//����ֵ:0��������ɣ��������������
u8 NRF24L01_RxPacket(u8 *rxbuf)
{
	u8 sta;		    							   
	SPI2_SetSpeed(SPI_BaudRatePrescaler_8); //spi�ٶ�Ϊ9Mhz��24L01�����SPIʱ��Ϊ10Mhz��   
	sta=NRF24L01_Read_Reg(STATUS);  //��ȡ״̬�Ĵ�����ֵ    	 
	NRF24L01_Write_Reg(NRF_WRITE_REG+STATUS,sta); //���TX_DS��MAX_RT�жϱ�־
	if(sta&RX_OK)//���յ�����
	{
//		NRF24L01_Read_Buf(RD_RX_PLOAD,rxbuf,RX_PLOAD_WIDTH);//��ȡ����
		NRF24L01_Write_Reg(FLUSH_RX,0xff);//���RX FIFO�Ĵ��� 
		return 0; 
	}	   
	return 1;//û�յ��κ�����
}					    
//�ú�����ʼ��NRF24L01��RXģʽ
//����RX��ַ,дRX���ݿ���,ѡ��RFƵ��,�����ʺ�LNA HCURR
//��CE��ߺ�,������RXģʽ,�����Խ���������		   
void NRF24L01_RX_Mode(void)
{
	NRF24L01_CE=0;	  
//  NRF24L01_Write_Buf(NRF_WRITE_REG+TX_ADDR,(u8*)TX_ADDRESS,TX_ADR_WIDTH);//дTX�ڵ��ַ 
//  NRF24L01_Write_Buf(NRF_WRITE_REG+RX_ADDR_P0,(u8*)RX_ADDRESS,RX_ADR_WIDTH);//дRX�ڵ��ַ

//  NRF24L01_Write_Reg(NRF_WRITE_REG+EN_AA,0x01);    //ʹ��ͨ��0���Զ�Ӧ��    
//  NRF24L01_Write_Reg(NRF_WRITE_REG+EN_RXADDR,0x01);//ʹ��ͨ��0�Ľ��յ�ַ  	 
//  NRF24L01_Write_Reg(NRF_WRITE_REG+RF_CH,40);	     //����RFͨ��Ƶ��		  
//  NRF24L01_Write_Reg(NRF_WRITE_REG+RX_PW_P0,RX_PLOAD_WIDTH);//ѡ��ͨ��0����Ч���ݿ��� 	    
//  NRF24L01_Write_Reg(NRF_WRITE_REG+RF_SETUP,0x0f);//����TX�������,0db����,2Mbps,���������濪��   
//  NRF24L01_Write_Reg(NRF_WRITE_REG+CONFIG, 0x0f);//���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC,����ģʽ 
 ///// 
  	NRF24L01_Write_Reg(NRF_WRITE_REG+SETUP_AW, 0x03); //����ͨ�ŵ�ַ�ĳ��ȣ�Ĭ��ֵʱ0x03,����ַ����Ϊ5�ֽ�
    NRF24L01_Write_Buf(NRF_WRITE_REG+TX_ADDR,(uint8_t*)TX_ADDRESS,TX_ADR_WIDTH); //дTX�ڵ��ַ 
    NRF24L01_Write_Buf(NRF_WRITE_REG+RX_ADDR_P0,(uint8_t*)RX_ADDRESS,RX_ADR_WIDTH); //����TX�ڵ��ַ,��ҪΪ��ʹ��ACK
    NRF24L01_Write_Reg(NRF_WRITE_REG+SETUP_RETR,0x1A); //�����Զ��ط����ʱ��:500us + 86us;����Զ��ط�����:10�� 0x1A
    
    NRF24L01_Write_Reg(NRF_WRITE_REG+EN_RXADDR,0x01);//ʹ��ͨ��0�Ľ��յ�ַ  
    NRF24L01_Write_Reg(NRF_WRITE_REG+EN_AA,0x01); //ʹ��ͨ��0�Զ�Ӧ��
    NRF24L01_Write_Reg(NRF_WRITE_REG+RX_PW_P0,RX_PLOAD_WIDTH);//ѡ��ͨ��0����Ч���ݿ���  
    NRF24L01_Write_Buf(NRF_WRITE_REG+RX_ADDR_P0,(uint8_t*)RX_ADDRESS,RX_ADR_WIDTH); //дRX�ڵ��ַ
    NRF24L01_Write_Reg(NRF_WRITE_REG+RF_CH,40); //����RFͨ��Ϊ40hz(1-64Hz������)
    NRF24L01_Write_Reg(NRF_WRITE_REG+RF_SETUP,0x0f); //����TX�������,0db����,2Mbps,����������ر� ��ע�⣺����������ر�/����ֱ��Ӱ��ͨ��,Ҫ������������Ҫ�رն��ر�0x0f��
    NRF24L01_Write_Reg(NRF_WRITE_REG+CONFIG,0x0F);//����Ϊ����ģʽ
		NRF24L01_Write_Reg(NRF_WRITE_REG+STATUS,0X7E); //��������ж�,��ֹһ��ȥ����ģʽ�ʹ����ж�
  	NRF24L01_CE = 1; //CEΪ��,�������ģʽ 
}						 
//�ú�����ʼ��NRF24L01��TXģʽ
//����TX��ַ,дTX���ݿ���,����RX�Զ�Ӧ��ĵ�ַ,���TX��������,ѡ��RFƵ��,�����ʺ�LNA HCURR
//PWR_UP,CRCʹ��
//��CE��ߺ�,������RXģʽ,�����Խ���������		   
//CEΪ�ߴ���10us,����������.	 
void NRF24L01_TX_Mode(void)
{														 
	NRF24L01_CE=0;	    
  	NRF24L01_Write_Buf(NRF_WRITE_REG+TX_ADDR,(u8*)TX_ADDRESS,TX_ADR_WIDTH);//дTX�ڵ��ַ 
  	NRF24L01_Write_Buf(NRF_WRITE_REG+RX_ADDR_P0,(u8*)RX_ADDRESS,RX_ADR_WIDTH); //����TX�ڵ��ַ,��ҪΪ��ʹ��ACK	  

  	NRF24L01_Write_Reg(NRF_WRITE_REG+EN_AA,0x01);     //ʹ��ͨ��0���Զ�Ӧ��    
  	NRF24L01_Write_Reg(NRF_WRITE_REG+EN_RXADDR,0x01); //ʹ��ͨ��0�Ľ��յ�ַ  
  	NRF24L01_Write_Reg(NRF_WRITE_REG+SETUP_RETR,0x1a);//�����Զ��ط����ʱ��:500us + 86us;����Զ��ط�����:10��
  	NRF24L01_Write_Reg(NRF_WRITE_REG+RF_CH,40);       //����RFͨ��Ϊ40
    NRF24L01_Write_Reg(NRF_WRITE_REG+RX_PW_P0,RX_PLOAD_WIDTH);
  	NRF24L01_Write_Reg(NRF_WRITE_REG+RF_SETUP,0x0f);  //����TX�������,0db����,2Mbps,���������濪��   
  	NRF24L01_Write_Reg(NRF_WRITE_REG+CONFIG,0x0e);    //���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC,����ģʽ,���������ж�
	NRF24L01_CE=1;//CEΪ��,10us����������
}
extern u8 Rx_buff[33];
int IRQ_timeout;
void NRF24L01_INT_RX_Mode(u8 *rxbuf)
{
    u8 sta;

  sta=NRF24L01_Read_Reg(STATUS);  //��ȡ״̬�Ĵ�����ֵ    	 
  
  if(sta&TX_OK)
  {

    NRF24L01_RX_Mode();
    NRF24L01_Write_Reg(NRF_WRITE_REG+STATUS,TX_OK); //���TX_DS��MAX_RT�жϱ�־
    NRF24L01_Write_Reg(FLUSH_TX,0xff);//���RX FIFO�Ĵ��� 
  }
  if(sta&RX_OK)//���յ�����
  {
    
    NRF24L01_Read_Buf(RD_RX_PLOAD,rxbuf,RX_PLOAD_WIDTH);//��ȡ����
    NRF24L01_Write_Reg(FLUSH_RX,0xff);//���RX FIFO�Ĵ��� 
    NRF24L01_Write_Reg(NRF_WRITE_REG+STATUS,RX_OK); //���TX_DS��MAX_RT�жϱ�־
    ReceiveData(rxbuf);
    LED1 =!LED1;
    IRQ_timeout = 0;
  }
  if(sta & MAX_TX)                                  
  {				
    NRF24L01_RX_Mode();
    NRF24L01_Write_Reg(NRF_WRITE_REG+STATUS,MAX_TX); //���TX_DS��MAX_RT�жϱ�־
    NRF24L01_Write_Reg(FLUSH_TX,0xff);//���RX FIFO�Ĵ��� 
  }
}

void EXTI0_IRQHandler(void)
{
  uint8_t PA0_status = 0;
  PA0_status = GPIO_ReadOutputDataBit(GPIOA,GPIO_Pin_0);
  if(EXTI_GetITStatus(EXTI_Line0) != RESET )
	{

    NRF24L01_INT_RX_Mode(Rx_buff);

    EXTI_ClearITPendingBit(EXTI_Line0); 
  }
    
}



void ReceiveData(u8 *rxbuf)
{
  if(*(rxbuf+11) != 0xa5)
    return;
  if(*rxbuf & 0x01)//�����ݰ�����ң������ADC�������ʱ��������ʱ 
  {
      RX_Data.THROTTLE = ((uint16_t)rxbuf[THR_Addr] << 8) \
								  + (uint16_t)rxbuf[THR_Addr + 1];	
			RX_Data.YAW      = ((uint16_t)rxbuf[YAW_Addr] << 8) \
								  + (uint16_t)rxbuf[YAW_Addr + 1];
			RX_Data.ROLL     = ((uint16_t)rxbuf[ROL_Addr] << 8) \
								  + (uint16_t)rxbuf[ROL_Addr + 1];
			RX_Data.PITCH    = ((uint16_t)rxbuf[PIT_Addr] << 8) \
								  + (uint16_t)rxbuf[PIT_Addr + 1];		
  }
}
