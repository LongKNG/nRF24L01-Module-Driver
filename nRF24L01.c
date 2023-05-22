#include "nRF24L01.h"
//#include "Serial.h"

uint8_t RX_BUF[RX_PLOAD_WIDTH];									//�������ݻ���
uint8_t TX_BUF[TX_PLOAD_WIDTH];									//�������ݻ���
uint8_t TX_ADDRESS[TX_ADR_WIDTH] = {0x11,0x22,0x33,0x44,0x55};	//����һ����̬���͵�ַ
uint8_t RX_ADDRESS[RX_ADR_WIDTH] = {0x11,0x22,0x33,0x44,0x55}; 	//����һ����̬���յ�ַ

void Delay(__IO u32 nCount)
{
	for(; nCount != 0; nCount--);
} 

/*������������������������������������������������������������������������
  * @brief  SPI�� I/O����
  * @param  ��
  * @retval ��
������������������������������������������������������������������������*/
void NRF24L01_Init(void)
{
	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	/*������ӦIO�˿ڵ�ʱ��*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA
							 |RCC_APB2Periph_GPIOB
							 |NRF_IRQ_GPIO_CLK
							 |NRF_CSN_GPIO_CLK
							 |NRF_CE_GPIO_CLK,ENABLE);

//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);//ʹ��SPI1ʱ��
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);//ʹ��SPI2ʱ��
	
	/*����SPI1 SPI_NRF_SPI�� SCK,MISO,MOSI���ţ�GPIOA^5,GPIOA^6,GPIOA^7 */
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;

	/*����SPI2 SPI_NRF_SPI�� SCK,MISO,MOSI���ţ�GPIOB^13,GPIOB^14,GPIOA^15 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //���ù���
	GPIO_Init(GPIOB, &GPIO_InitStructure);  

	/*����SPI_NRF_SPI��CE����,��SPI_NRF_SPI�� CSN ����*/
	GPIO_InitStructure.GPIO_Pin = NRF_CSN_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(NRF_CSN_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = NRF_CE_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(NRF_CE_GPIO_PORT, &GPIO_InitStructure);

	/*����SPI_NRF_SPI��IRQ����*/
	GPIO_InitStructure.GPIO_Pin = NRF_IRQ_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU ;  //��������
	GPIO_Init(NRF_IRQ_GPIO_PORT, &GPIO_InitStructure); 
		  
	
	NRF_CSN_HIGH();//�Զ���ĺ꣬��������csn���ţ�NRF�������״̬

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; 	//˫��ȫ˫��
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;	 					//��ģʽ
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;	 				//���ݴ�С8λ
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;		 					//ʱ�Ӽ��ԣ�����ʱΪ��
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;						//��1��������Ч��������Ϊ����ʱ��
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		   					//NSS�ź����������
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;	//8��Ƶ��9MHz
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;  				//��λ��ǰ
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	
//	SPI_Init(SPI1, &SPI_InitStructure);
	
	SPI_Init(SPI2, &SPI_InitStructure);
	
//	SPI_Cmd(SPI1, ENABLE);//ʹ��SPI1
	SPI_Cmd(SPI2, ENABLE);//ʹ��SPI2
}

/*������������������������������������������������������������������������
  * @brief   ������NRF��/дһ�ֽ�����
  * @param   д�������
  *		@arg dat 
  * @retval  ��ȡ�õ�����
������������������������������������������������������������������������*/
uint8_t NRF24L01_RW(uint8_t dat)
{  	

	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);//�� SPI���ͻ������ǿ�ʱ�ȴ�

	SPI_I2S_SendData(SPI2, dat); //ͨ�� SPI2����һ�ֽ�����		

	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);//��SPI���ջ�����Ϊ��ʱ,�ȴ�

	return SPI_I2S_ReceiveData(SPI2);//���ش�SPI���߶�ȡ���ֽ�
}

/*������������������������������������������������������������������������
  * @brief   ������NRF�ض��ļĴ���д������
  * @param   
  *		@arg reg:NRF������+�Ĵ�����ַ
  *		@arg dat:��Ҫ��Ĵ���д�������
  * @retval  NRF��status�Ĵ�����״̬
������������������������������������������������������������������������*/
uint8_t NRF24L01_WriteReg(uint8_t reg,uint8_t dat)
{
	uint8_t status;
	
	NRF_CE_LOW();	
	NRF_CSN_LOW();//�õ�CSN��ʹ��SPI����
				
	status = NRF24L01_RW(reg);//��������Ĵ�����
	 
	NRF24L01_RW(dat);//��Ĵ���д������
			   
	NRF_CSN_HIGH();//CSN���ߣ����
		
	return(status);//����״̬�Ĵ�����ֵ
}

/*������������������������������������������������������������������������
  * @brief   ���ڴ�NRF�ض��ļĴ�����������
  * @param   
  *		@arg reg:NRF������+�Ĵ�����ַ
  * @retval  �Ĵ����е�����
������������������������������������������������������������������������*/
uint8_t NRF24L01_ReadReg(uint8_t reg)
{
 	uint8_t reg_val;

	NRF_CE_LOW();	
 	NRF_CSN_LOW();//�õ�CSN��ʹ��SPI����
				  	 
	NRF24L01_RW(reg);//���ͼĴ�����
	
	reg_val = NRF24L01_RW(NOP);//��ȡ�Ĵ�����ֵ
	             	
	NRF_CSN_HIGH();//CSN���ߣ����	
   	
	return reg_val;
}	

/*������������������������������������������������������������������������
  * @brief   ������NRF�ļĴ�����д��һ������
  * @param   
  *		@arg reg : NRF������+�Ĵ�����ַ
  *		@arg pBuf�����ڴ洢���������ļĴ������ݵ����飬�ⲿ����
  * 	@arg bytes: pBuf�����ݳ���
  * @retval  NRF��status�Ĵ�����״̬
������������������������������������������������������������������������*/
uint8_t NRF24L01_ReadBuf(uint8_t reg,uint8_t *pBuf,uint8_t bytes)
{
	uint8_t status, byte_cnt;

	NRF_CE_LOW();
	NRF_CSN_LOW();//�õ�CSN��ʹ��SPI����
			
	status = NRF24L01_RW(reg);//���ͼĴ����� 

	/*��ȡ����������*/
	for(byte_cnt=0;byte_cnt<bytes;byte_cnt++)
	{	
		pBuf[byte_cnt] = NRF24L01_RW(NOP); //��NRF24L01��ȡ����  
	}
	
	NRF_CSN_HIGH();//CSN���ߣ����

	return status;		//���ؼĴ���״ֵ̬
}

/*������������������������������������������������������������������������
  * @brief   ������NRF�ļĴ�����д��һ������
  * @param   
  *		@arg reg : NRF������+�Ĵ�����ַ
  *		@arg pBuf���洢�˽�Ҫд��д�Ĵ������ݵ����飬�ⲿ����
  * 	@arg bytes: pBuf�����ݳ���
  * @retval  NRF��status�Ĵ�����״̬
������������������������������������������������������������������������*/
uint8_t NRF24L01_WriteBuf(uint8_t reg ,uint8_t *pBuf,uint8_t bytes)
{
	uint8_t status,byte_cnt;
	
	NRF_CE_LOW();	
	NRF_CSN_LOW();//�õ�CSN��ʹ��SPI����			

	status = NRF24L01_RW(reg);//���ͼĴ�����

	/*�򻺳���д������*/
	for(byte_cnt=0;byte_cnt<bytes;byte_cnt++)
	{
		NRF24L01_RW(*pBuf++);//д���ݵ�������
	}  
	
	NRF_CSN_HIGH();//CSN���ߣ����		

	return (status);//����NRF24L01��״̬ 		
}

/*������������������������������������������������������������������������
  * @brief  ���ò��������ģʽ
  * @param  ��
  * @retval ��
������������������������������������������������������������������������*/
void NRF24L01_RX_Mode(void)
{
	NRF_CE_LOW();	

	NRF24L01_WriteBuf(NRF_WRITE_REG+RX_ADDR_P0,RX_ADDRESS,RX_ADR_WIDTH);//дRX�ڵ��ַ

	NRF24L01_WriteReg(NRF_WRITE_REG+EN_AA,0x01);    		 //ʹ��ͨ��0���Զ�Ӧ��    
	NRF24L01_WriteReg(NRF_WRITE_REG+EN_RXADDR,0x01);		 //ʹ��ͨ��0�Ľ��յ�ַ    
	NRF24L01_WriteReg(NRF_WRITE_REG+RF_CH,CHANAL);      	 //����RFͨ��Ƶ��    
	NRF24L01_WriteReg(NRF_WRITE_REG+RX_PW_P0,RX_PLOAD_WIDTH);//ѡ��ͨ��0����Ч���ݿ��      
	NRF24L01_WriteReg(NRF_WRITE_REG+RF_SETUP,0x0f); 		 //����TX�������,0db����,2Mbps,���������濪��   
	NRF24L01_WriteReg(NRF_WRITE_REG+CONFIG, 0x0f);  		 //���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC,����ģʽ 
	NRF24L01_WriteReg(NRF_WRITE_REG+STATUS,0xff);			 //������״̬�Ĵ�������
	
	NRF24L01_WriteReg(FLUSH_TX,NOP);
	NRF24L01_WriteReg(FLUSH_RX,NOP);
	
	NRF_CE_HIGH();//CE���ߣ��������ģʽ	
}    

/*������������������������������������������������������������������������
  * @brief  ���÷���ģʽ
  * @param  ��
  * @retval ��
������������������������������������������������������������������������*/
void NRF24L01_TX_Mode(void)
{  
	NRF_CE_LOW();		

	NRF24L01_WriteBuf(NRF_WRITE_REG+TX_ADDR,TX_ADDRESS,TX_ADR_WIDTH);    //дTX�ڵ��ַ 
	NRF24L01_WriteBuf(NRF_WRITE_REG+RX_ADDR_P0,RX_ADDRESS,RX_ADR_WIDTH); //����TX�ڵ��ַ,��ҪΪ��ʹ��ACK   

	NRF24L01_WriteReg(NRF_WRITE_REG+EN_AA,0x01);     //ʹ��ͨ��0���Զ�Ӧ��    
	NRF24L01_WriteReg(NRF_WRITE_REG+EN_RXADDR,0x01); //ʹ��ͨ��0�Ľ��յ�ַ  
	NRF24L01_WriteReg(NRF_WRITE_REG+SETUP_RETR,0x1a);//�����Զ��ط����ʱ��:500us + 86us;����Զ��ط�����:10��
	NRF24L01_WriteReg(NRF_WRITE_REG+RF_CH,CHANAL);   //����RFͨ��ΪCHANAL
	NRF24L01_WriteReg(NRF_WRITE_REG+RF_SETUP,0x0f);  //����TX�������,0db����,2Mbps,���������濪��   
	NRF24L01_WriteReg(NRF_WRITE_REG+CONFIG,0x0e);    //���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC,����ģʽ,���������ж�
	
	NRF24L01_WriteReg(FLUSH_TX,NOP);
	NRF24L01_WriteReg(FLUSH_RX,NOP);
		
	NRF_CE_HIGH();//CE���ߣ����뷢��ģʽ
	Delay(0xffff);//CEҪ����һ��ʱ��Ž��뷢��ģʽ
}

/*������������������������������������������������������������������������
  * @brief  ��Ҫ����NRF��MCU�Ƿ���������
  * @param  ��
  * @retval SUCCESS/ERROR ��������/����ʧ��
������������������������������������������������������������������������*/
uint8_t NRF24L01_Check(void)
{
	uint8_t buf[5]={0xC2,0xC2,0xC2,0xC2,0xC2};
	uint8_t buf1[5];
	uint8_t i; 
	 
	NRF24L01_WriteBuf(NRF_WRITE_REG+TX_ADDR,buf,5);	//д��5���ֽڵĵ�ַ 	
	NRF24L01_ReadBuf(TX_ADDR,buf1,5);				//����д��ĵ�ַ 
	 
	/*�Ƚ�*/
	for(i=0;i<5;i++)
	{
		if(buf1[i]!=0xC2)
		break;
	} 
	       
	if(i==5)
		return SUCCESS;//MCU��NRF�ɹ�����         
	else
		return ERROR;//MCU��NRF����������        
}

/*������������������������������������������������������������������������
  * @brief   ������NRF�ķ��ͻ�������д������
  * @param   
  *		@arg txBuf���洢�˽�Ҫ���͵����ݵ����飬�ⲿ����	
  * @retval  ���ͽ�����ɹ�����TXDS,ʧ�ܷ���MAXRT��ERROR
������������������������������������������������������������������������*/
uint8_t NRF24L01_Tx_Dat(uint8_t *txbuf)
{
	uint8_t state;  	 
	NRF_CE_LOW();//ceΪ�ͣ��������ģʽ1					
	NRF24L01_WriteBuf(WR_TX_PLOAD,txbuf,TX_PLOAD_WIDTH);//д���ݵ�TX BUF ��� 32���ֽ�		

	NRF_CE_HIGH();//CEΪ�ߣ�txbuf�ǿգ��������ݰ�   
	                           
	while(NRF_Read_IRQ()!=0);//�ȴ���������ж� 	
	                             
	state = NRF24L01_ReadReg(STATUS);//��ȡ״̬�Ĵ�����ֵ
                 
	NRF24L01_WriteReg(NRF_WRITE_REG+STATUS,state);	//���TX_DS��MAX_RT�жϱ�־  	
	NRF24L01_WriteReg(FLUSH_TX,NOP);    			//���TX FIFO�Ĵ��� 

	 /*�ж��ж�����*/    
	if(state&MAX_RT)                     
		return MAX_RT;//�ﵽ����ط����� 
	else if(state&TX_DS)                  
		return TX_DS;//�������
	 else						  
		return ERROR;//����ԭ����ʧ��                 
} 

/*������������������������������������������������������������������������
  * @brief   ���ڴ�NRF�Ľ��ջ������ж�������
  * @param   
  *		@arg rxBuf �����ڽ��ո����ݵ����飬�ⲿ����	
  * @retval 
  *		@arg ���ս��
������������������������������������������������������������������������*/
uint8_t NRF24L01_Rx_Dat(uint8_t *rxbuf)
{
	uint8_t state; 
	NRF_CE_HIGH();	 //�������״̬
	/*��ѯ��־*/
	while(NRF_Read_IRQ()==0)
	{
		NRF_CE_LOW();//�������״̬
	               
		state=NRF24L01_ReadReg(STATUS);//��ȡstatus�Ĵ�����ֵ

	     
		NRF24L01_WriteReg(NRF_WRITE_REG+STATUS,state);//����жϱ�־

		/*�ж��Ƿ���յ�����*/
		if(state&RX_DR)                                 
		{														//���յ�����
			NRF24L01_ReadBuf(RD_RX_PLOAD,rxbuf,RX_PLOAD_WIDTH);	//��ȡ����
			NRF24L01_WriteReg(FLUSH_RX,NOP);          			//���RX FIFO�Ĵ���
			return RX_DR; 
		}
		else    
			return ERROR;//û�յ��κ�����
	}
	
	return ERROR;//û�յ��κ�����                    
}
/*********************************************END OF FILE**********************/
