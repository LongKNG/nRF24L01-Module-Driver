#ifndef __NRF24L01_H
#define __NRF24L01_H

#include "stm32f10x.h"

#define TX_ADR_WIDTH 	5  	//�����ַ���
#define TX_PLOAD_WIDTH  32   //��������ͨ����Ч���ݿ��0~32Byte 

#define RX_ADR_WIDTH    5
#define RX_PLOAD_WIDTH  32 

#define CHANAL 0	//Ƶ��ѡ�� 

/***** nRF24L01�Ĵ�������������NRF����ʹ���ĵ� *****/
#define NRF_READ_REG    0x00  //�����üĴ���,��5λΪ�Ĵ�����ַ,000X XXXX
#define NRF_WRITE_REG   0x20  //д���üĴ���,��5λΪ�Ĵ�����ַ,001X XXXX
#define RD_RX_PLOAD    	0x61  //��RX��Ч����,1~32�ֽ�
#define WR_TX_PLOAD    	0xA0  //дTX��Ч����,1~32�ֽ�
#define FLUSH_TX        0xE1  //���TX FIFO�Ĵ���.����ģʽ����
#define FLUSH_RX        0xE2  //���RX FIFO�Ĵ���.����ģʽ����
#define REUSE_TX_PL     0xE3  //����ʹ����һ������,CEΪ��,���ݰ������Ϸ���.
#define NOP             0xFF  //�ղ���,����������״̬�Ĵ���	 
/*����������������������������������������������������������������������������������������������������������������*/

/***** SPI(nRF24L01)�Ĵ�����ַ�궨�� *****/
#define CONFIG          0x00  //���üĴ�����ַ;bit0:1����ģʽ,0����ģʽ;bit1:��ѡ��;bit2:CRCģʽ;bit3:CRCʹ��;
                              //bit4:�ж�MAX_RT(�ﵽ����ط������ж�)ʹ��;bit5:�ж�TX_DSʹ��;bit6:�ж�RX_DRʹ��
#define EN_AA           0x01  //ʹ���Զ�Ӧ����  bit0~5,��Ӧͨ��0~5
#define EN_RXADDR       0x02  //���յ�ַ����,bit0~5,��Ӧͨ��0~5
#define SETUP_AW        0x03  //���õ�ַ���(��������ͨ��):bit1,0:00,3�ֽ�;01,4�ֽ�;02,5�ֽ�;
#define SETUP_RETR      0x04  //�����Զ��ط�;bit3:0,�Զ��ط�������;bit7:4,�Զ��ط���ʱ 250*x+86us
#define RF_CH           0x05  //RFͨ��,bit6:0,����ͨ��Ƶ��;
#define RF_SETUP        0x06  //RF�Ĵ���;bit3:��������(0:1Mbps,1:2Mbps);bit2:1,���书��;bit0:�������Ŵ�������
#define STATUS          0x07  //״̬�Ĵ���;bit0:TX FIFO����־;bit3:1,��������ͨ����(���:6);bit4,�ﵽ�����ط�
                              //bit5:���ݷ�������ж�;bit6:���������ж�;
#define OBSERVE_TX      0x08  //���ͼ��Ĵ���,bit7:4,���ݰ���ʧ������;bit3:0,�ط�������
#define CD              0x09  //�ز����Ĵ���,bit0,�ز����;							  
/*����������������������������������������������������������������������������������������������������������������*/

#define RX_ADDR_P0      0x0A  //����ͨ��0���յ�ַ,��󳤶�5���ֽ�,���ֽ���ǰ
#define RX_ADDR_P1      0x0B  //����ͨ��1���յ�ַ,��󳤶�5���ֽ�,���ֽ���ǰ
#define RX_ADDR_P2      0x0C  //����ͨ��2���յ�ַ,����ֽڿ�����,���ֽ�,����ͬRX_ADDR_P1[39:8]���;
#define RX_ADDR_P3      0x0D  //����ͨ��3���յ�ַ,����ֽڿ�����,���ֽ�,����ͬRX_ADDR_P1[39:8]���;
#define RX_ADDR_P4      0x0E  //����ͨ��4���յ�ַ,����ֽڿ�����,���ֽ�,����ͬRX_ADDR_P1[39:8]���;
#define RX_ADDR_P5      0x0F  //����ͨ��5���յ�ַ,����ֽڿ�����,���ֽ�,����ͬRX_ADDR_P1[39:8]���;
#define TX_ADDR         0x10  //���͵�ַ(���ֽ���ǰ),ShockBurstTMģʽ��,RX_ADDR_P0��˵�ַ���
#define RX_PW_P0        0x11  //��������ͨ��0��Ч���ݿ��(1~32�ֽ�),����Ϊ0��Ƿ�
#define RX_PW_P1        0x12  //��������ͨ��1��Ч���ݿ��(1~32�ֽ�),����Ϊ0��Ƿ�
#define RX_PW_P2        0x13  //��������ͨ��2��Ч���ݿ��(1~32�ֽ�),����Ϊ0��Ƿ�
#define RX_PW_P3        0x14  //��������ͨ��3��Ч���ݿ��(1~32�ֽ�),����Ϊ0��Ƿ�
#define RX_PW_P4        0x15  //��������ͨ��4��Ч���ݿ��(1~32�ֽ�),����Ϊ0��Ƿ�
#define RX_PW_P5        0x16  //��������ͨ��5��Ч���ݿ��(1~32�ֽ�),����Ϊ0��Ƿ�
#define FIFO_STATUS     0x17  //FIFO״̬�Ĵ���;bit0,RX FIFO�Ĵ����ձ�־;bit1,RX FIFO����־;bit2,3,����
                              //bit4,TX FIFO�ձ�־;bit5,TX FIFO����־;bit6,1,ѭ��������һ���ݰ�.0,��ѭ��;
/*����������������������������������������������������������������������������������������������������������������*/

/***** STATUS�Ĵ���bitλ���� *****/							  
#define MAX_RT  	    0x10  //�ﵽ����ʹ����жϱ�־λ 0001 0000
#define TX_DS   	    0x20  //TX��������жϱ�־λ 0010 0000
#define RX_DR   	    0x40  //RT���յ������жϱ�־λ 0110 0000
/*����������������������������������������������������������������������������������������������������������������*/

//stm32f103vet6 CSN PC6
//stm32f103c8t6 CSN PB12
#define NRF_CSN_GPIO_PORT    GPIOB
#define NRF_CSN_PIN          GPIO_Pin_12
#define NRF_CSN_GPIO_CLK     RCC_APB2Periph_GPIOB

//stm32f103vet6 CE PC5
//stm32f103c8t6 CE PB0
#define NRF_CE_GPIO_PORT    GPIOA
#define NRF_CE_PIN          GPIO_Pin_15
#define NRF_CE_GPIO_CLK     RCC_APB2Periph_GPIOA
//stm32f103vet6 IRQ PC4
//stm32f103c8t6 IRQ PC13
#define NRF_IRQ_GPIO_PORT    GPIOB
#define NRF_IRQ_PIN          GPIO_Pin_11
#define NRF_IRQ_GPIO_CLK     RCC_APB2Periph_GPIOC

#define NRF_CSN_HIGH()      GPIO_SetBits(NRF_CSN_GPIO_PORT, NRF_CSN_PIN)
#define NRF_CSN_LOW()       GPIO_ResetBits(NRF_CSN_GPIO_PORT, NRF_CSN_PIN)		        //csn�õ�
#define NRF_CE_HIGH()	      GPIO_SetBits(NRF_CE_GPIO_PORT,NRF_CE_PIN)
#define NRF_CE_LOW()	      GPIO_ResetBits(NRF_CE_GPIO_PORT,NRF_CE_PIN)			      //CE�õ�
#define NRF_Read_IRQ()		  GPIO_ReadInputDataBit(NRF_IRQ_GPIO_PORT, NRF_IRQ_PIN)  //�ж�����

//void NRF24L01_Init(void)
//u8 SPI_NRF_RW(u8 dat);
//u8 SPI_NRF_ReadReg(u8 reg );
//u8 SPI_NRF_WriteReg(u8 reg,u8 dat);

//u8 SPI_NRF_ReadBuf(u8 reg,u8 *pBuf,u8 bytes);
//u8 SPI_NRF_WriteBuf(u8 reg ,u8 *pBuf,u8 bytes);	

//void NRF_TX_Mode(void);
//void NRF_RX_Mode(void);
//u8 NRF_Rx_Dat(u8 *rxbuf);
//u8 NRF_Tx_Dat(u8 *txbuf);
//u8 NRF_Check(void);
////--------------------------------
void NRF24L01_Init(void);
uint8_t NRF24L01_Check(void);
uint8_t NRF24L01_RW(uint8_t dat);

uint8_t NRF24L01_ReadReg(uint8_t reg);
uint8_t NRF24L01_WriteReg(uint8_t reg,uint8_t dat);

uint8_t NRF24L01_ReadBuf(uint8_t reg,uint8_t *pBuf,uint8_t bytes);
uint8_t NRF24L01_WriteBuf(uint8_t reg ,uint8_t *pBuf,uint8_t bytes);


void NRF24L01_TX_Mode(void);
void NRF24L01_RX_Mode(void);
uint8_t NRF24L01_Tx_Dat(uint8_t *txbuf);
uint8_t NRF24L01_Rx_Dat(uint8_t *rxbuf);





#endif /* __SPI_NRF_H */ 
