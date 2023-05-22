#ifndef __NRF24L01_H
#define __NRF24L01_H

#include "stm32f10x.h"

#define TX_ADR_WIDTH 	5  	//发射地址宽度
#define TX_PLOAD_WIDTH  32   //发射数据通道有效数据宽度0~32Byte 

#define RX_ADR_WIDTH    5
#define RX_PLOAD_WIDTH  32 

#define CHANAL 0	//频道选择 

/***** nRF24L01寄存器操作命令，详见NRF功能使用文档 *****/
#define NRF_READ_REG    0x00  //读配置寄存器,低5位为寄存器地址,000X XXXX
#define NRF_WRITE_REG   0x20  //写配置寄存器,低5位为寄存器地址,001X XXXX
#define RD_RX_PLOAD    	0x61  //读RX有效数据,1~32字节
#define WR_TX_PLOAD    	0xA0  //写TX有效数据,1~32字节
#define FLUSH_TX        0xE1  //清除TX FIFO寄存器.发射模式下用
#define FLUSH_RX        0xE2  //清除RX FIFO寄存器.接收模式下用
#define REUSE_TX_PL     0xE3  //重新使用上一包数据,CE为高,数据包被不断发送.
#define NOP             0xFF  //空操作,可以用来读状态寄存器	 
/*————————————————————————————————————————————————————————*/

/***** SPI(nRF24L01)寄存器地址宏定义 *****/
#define CONFIG          0x00  //配置寄存器地址;bit0:1接收模式,0发射模式;bit1:电选择;bit2:CRC模式;bit3:CRC使能;
                              //bit4:中断MAX_RT(达到最大重发次数中断)使能;bit5:中断TX_DS使能;bit6:中断RX_DR使能
#define EN_AA           0x01  //使能自动应答功能  bit0~5,对应通道0~5
#define EN_RXADDR       0x02  //接收地址允许,bit0~5,对应通道0~5
#define SETUP_AW        0x03  //设置地址宽度(所有数据通道):bit1,0:00,3字节;01,4字节;02,5字节;
#define SETUP_RETR      0x04  //建立自动重发;bit3:0,自动重发计数器;bit7:4,自动重发延时 250*x+86us
#define RF_CH           0x05  //RF通道,bit6:0,工作通道频率;
#define RF_SETUP        0x06  //RF寄存器;bit3:传输速率(0:1Mbps,1:2Mbps);bit2:1,发射功率;bit0:低噪声放大器增益
#define STATUS          0x07  //状态寄存器;bit0:TX FIFO满标志;bit3:1,接收数据通道号(最大:6);bit4,达到最多次重发
                              //bit5:数据发送完成中断;bit6:接收数据中断;
#define OBSERVE_TX      0x08  //发送检测寄存器,bit7:4,数据包丢失计数器;bit3:0,重发计数器
#define CD              0x09  //载波检测寄存器,bit0,载波检测;							  
/*————————————————————————————————————————————————————————*/

#define RX_ADDR_P0      0x0A  //数据通道0接收地址,最大长度5个字节,低字节在前
#define RX_ADDR_P1      0x0B  //数据通道1接收地址,最大长度5个字节,低字节在前
#define RX_ADDR_P2      0x0C  //数据通道2接收地址,最低字节可设置,高字节,必须同RX_ADDR_P1[39:8]相等;
#define RX_ADDR_P3      0x0D  //数据通道3接收地址,最低字节可设置,高字节,必须同RX_ADDR_P1[39:8]相等;
#define RX_ADDR_P4      0x0E  //数据通道4接收地址,最低字节可设置,高字节,必须同RX_ADDR_P1[39:8]相等;
#define RX_ADDR_P5      0x0F  //数据通道5接收地址,最低字节可设置,高字节,必须同RX_ADDR_P1[39:8]相等;
#define TX_ADDR         0x10  //发送地址(低字节在前),ShockBurstTM模式下,RX_ADDR_P0与此地址相等
#define RX_PW_P0        0x11  //接收数据通道0有效数据宽度(1~32字节),设置为0则非法
#define RX_PW_P1        0x12  //接收数据通道1有效数据宽度(1~32字节),设置为0则非法
#define RX_PW_P2        0x13  //接收数据通道2有效数据宽度(1~32字节),设置为0则非法
#define RX_PW_P3        0x14  //接收数据通道3有效数据宽度(1~32字节),设置为0则非法
#define RX_PW_P4        0x15  //接收数据通道4有效数据宽度(1~32字节),设置为0则非法
#define RX_PW_P5        0x16  //接收数据通道5有效数据宽度(1~32字节),设置为0则非法
#define FIFO_STATUS     0x17  //FIFO状态寄存器;bit0,RX FIFO寄存器空标志;bit1,RX FIFO满标志;bit2,3,保留
                              //bit4,TX FIFO空标志;bit5,TX FIFO满标志;bit6,1,循环发送上一数据包.0,不循环;
/*————————————————————————————————————————————————————————*/

/***** STATUS寄存器bit位定义 *****/							  
#define MAX_RT  	    0x10  //达到最大发送次数中断标志位 0001 0000
#define TX_DS   	    0x20  //TX发送完成中断标志位 0010 0000
#define RX_DR   	    0x40  //RT接收到数据中断标志位 0110 0000
/*————————————————————————————————————————————————————————*/

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
#define NRF_CSN_LOW()       GPIO_ResetBits(NRF_CSN_GPIO_PORT, NRF_CSN_PIN)		        //csn置低
#define NRF_CE_HIGH()	      GPIO_SetBits(NRF_CE_GPIO_PORT,NRF_CE_PIN)
#define NRF_CE_LOW()	      GPIO_ResetBits(NRF_CE_GPIO_PORT,NRF_CE_PIN)			      //CE置低
#define NRF_Read_IRQ()		  GPIO_ReadInputDataBit(NRF_IRQ_GPIO_PORT, NRF_IRQ_PIN)  //中断引脚

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
