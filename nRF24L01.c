#include "nRF24L01.h"
//#include "Serial.h"

uint8_t RX_BUF[RX_PLOAD_WIDTH];									//接收数据缓存
uint8_t TX_BUF[TX_PLOAD_WIDTH];									//发射数据缓存
uint8_t TX_ADDRESS[TX_ADR_WIDTH] = {0x11,0x22,0x33,0x44,0x55};	//定义一个静态发送地址
uint8_t RX_ADDRESS[RX_ADR_WIDTH] = {0x11,0x22,0x33,0x44,0x55}; 	//定义一个静态接收地址

void Delay(__IO u32 nCount)
{
	for(; nCount != 0; nCount--);
} 

/*————————————————————————————————————
  * @brief  SPI的 I/O配置
  * @param  无
  * @retval 无
————————————————————————————————————*/
void NRF24L01_Init(void)
{
	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	/*开启相应IO端口的时钟*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA
							 |RCC_APB2Periph_GPIOB
							 |NRF_IRQ_GPIO_CLK
							 |NRF_CSN_GPIO_CLK
							 |NRF_CE_GPIO_CLK,ENABLE);

//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);//使能SPI1时钟
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);//使能SPI2时钟
	
	/*配置SPI1 SPI_NRF_SPI的 SCK,MISO,MOSI引脚，GPIOA^5,GPIOA^6,GPIOA^7 */
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;

	/*配置SPI2 SPI_NRF_SPI的 SCK,MISO,MOSI引脚，GPIOB^13,GPIOB^14,GPIOA^15 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 		//复用功能
	GPIO_Init(GPIOB, &GPIO_InitStructure);  

	/*配置SPI_NRF_SPI的CE引脚,和SPI_NRF_SPI的 CSN 引脚*/
	GPIO_InitStructure.GPIO_Pin = NRF_CSN_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(NRF_CSN_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = NRF_CE_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(NRF_CE_GPIO_PORT, &GPIO_InitStructure);

	/*配置SPI_NRF_SPI的IRQ引脚*/
	GPIO_InitStructure.GPIO_Pin = NRF_IRQ_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU ;  		//上拉输入
	GPIO_Init(NRF_IRQ_GPIO_PORT, &GPIO_InitStructure); 
		  
	
	NRF_CSN_HIGH();//自定义的宏，用于拉高csn引脚，NRF进入空闲状态

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; 	//双线全双工
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;	 			//主模式
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;	 		//数据大小8位
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;		 		//时钟极性，空闲时为低
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;				//第1个边沿有效，上升沿为采样时刻
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		   		//NSS信号由软件产生
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;	//8分频，9MHz
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;  			//高位在前
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	
//	SPI_Init(SPI1, &SPI_InitStructure);
	
	SPI_Init(SPI2, &SPI_InitStructure);
	
//	SPI_Cmd(SPI1, ENABLE);		//使能SPI1
	SPI_Cmd(SPI2, ENABLE);		//使能SPI2
}

/*————————————————————————————————————
  * @brief   用于向NRF读/写一字节数据
  * @param   写入的数据
  *		@arg dat 
  * @retval  读取得的数据
————————————————————————————————————*/
uint8_t NRF24L01_RW(uint8_t dat)
{  	

	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);//当 SPI发送缓冲器非空时等待

	SPI_I2S_SendData(SPI2, dat); //通过 SPI2发送一字节数据		

	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);//当SPI接收缓冲器为空时,等待

	return SPI_I2S_ReceiveData(SPI2);//返回从SPI总线读取的字节
}

/*————————————————————————————————————
  * @brief   用于向NRF特定的寄存器写入数据
  * @param   
  *		@arg reg:NRF的命令+寄存器地址
  *		@arg dat:将要向寄存器写入的数据
  * @retval  NRF的status寄存器的状态
————————————————————————————————————*/
uint8_t NRF24L01_WriteReg(uint8_t reg,uint8_t dat)
{
	uint8_t status;
	
	NRF_CE_LOW();	
	NRF_CSN_LOW();			//置低CSN，使能SPI传输
				
	status = NRF24L01_RW(reg);	//发送命令及寄存器号
	 
	NRF24L01_RW(dat);		//向寄存器写入数据
			   
	NRF_CSN_HIGH();			//CSN拉高，完成
		
	return(status);			//返回状态寄存器的值
}

/*————————————————————————————————————
  * @brief   用于从NRF特定的寄存器读出数据
  * @param   
  *		@arg reg:NRF的命令+寄存器地址
  * @retval  寄存器中的数据
————————————————————————————————————*/
uint8_t NRF24L01_ReadReg(uint8_t reg)
{
 	uint8_t reg_val;

	NRF_CE_LOW();	
 	NRF_CSN_LOW();			//置低CSN，使能SPI传输
				  	 
	NRF24L01_RW(reg);		//发送寄存器号
	
	reg_val = NRF24L01_RW(NOP);	//读取寄存器的值
	             	
	NRF_CSN_HIGH();			//CSN拉高，完成	
   	
	return reg_val;
}	

/*————————————————————————————————————
  * @brief   用于向NRF的寄存器中写入一串数据
  * @param   
  *		@arg reg : NRF的命令+寄存器地址
  *		@arg pBuf：用于存储将被读出的寄存器数据的数组，外部定义
  * 	@arg bytes: pBuf的数据长度
  * @retval  NRF的status寄存器的状态
————————————————————————————————————*/
uint8_t NRF24L01_ReadBuf(uint8_t reg,uint8_t *pBuf,uint8_t bytes)
{
	uint8_t status, byte_cnt;

	NRF_CE_LOW();
	NRF_CSN_LOW();			//置低CSN，使能SPI传输
			
	status = NRF24L01_RW(reg);	//发送寄存器号 

	/*读取缓冲区数据*/
	for(byte_cnt=0;byte_cnt<bytes;byte_cnt++)
	{	
		pBuf[byte_cnt] = NRF24L01_RW(NOP); //从NRF24L01读取数据  
	}
	
	NRF_CSN_HIGH();			//CSN拉高，完成

	return status;			//返回寄存器状态值
}

/*————————————————————————————————————
  * @brief   用于向NRF的寄存器中写入一串数据
  * @param   
  *		@arg reg : NRF的命令+寄存器地址
  *		@arg pBuf：存储了将要写入写寄存器数据的数组，外部定义
  * 	@arg bytes: pBuf的数据长度
  * @retval  NRF的status寄存器的状态
————————————————————————————————————*/
uint8_t NRF24L01_WriteBuf(uint8_t reg ,uint8_t *pBuf,uint8_t bytes)
{
	uint8_t status,byte_cnt;
	
	NRF_CE_LOW();	
	NRF_CSN_LOW();			//置低CSN，使能SPI传输			

	status = NRF24L01_RW(reg);	//发送寄存器号

	/*向缓冲区写入数据*/
	for(byte_cnt=0;byte_cnt<bytes;byte_cnt++)
	{
		NRF24L01_RW(*pBuf++);	//写数据到缓冲区
	}  
	
	NRF_CSN_HIGH();			//CSN拉高，完成		

	return (status);	//返回NRF24L01的状态 		
}

/*————————————————————————————————————
  * @brief  配置并进入接收模式
  * @param  无
  * @retval 无
————————————————————————————————————*/
void NRF24L01_RX_Mode(void)
{
	NRF_CE_LOW();	

	NRF24L01_WriteBuf(NRF_WRITE_REG+RX_ADDR_P0,RX_ADDRESS,RX_ADR_WIDTH);//写RX节点地址

	NRF24L01_WriteReg(NRF_WRITE_REG+EN_AA,0x01);			//使能通道0的自动应答    
	NRF24L01_WriteReg(NRF_WRITE_REG+EN_RXADDR,0x01);		//使能通道0的接收地址    
	NRF24L01_WriteReg(NRF_WRITE_REG+RF_CH,CHANAL);      	 	//设置RF通信频率    
	NRF24L01_WriteReg(NRF_WRITE_REG+RX_PW_P0,RX_PLOAD_WIDTH);	//选择通道0的有效数据宽度      
	NRF24L01_WriteReg(NRF_WRITE_REG+RF_SETUP,0x0f);			//设置TX发射参数,0db增益,2Mbps,低噪声增益开启   
	NRF24L01_WriteReg(NRF_WRITE_REG+CONFIG, 0x0f);			//配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,接收模式 
	NRF24L01_WriteReg(NRF_WRITE_REG+STATUS,0xff);			//将所有状态寄存器清零
	
	NRF24L01_WriteReg(FLUSH_TX,NOP);
	NRF24L01_WriteReg(FLUSH_RX,NOP);
	
	NRF_CE_HIGH();//CE拉高，进入接收模式	
}    

/*————————————————————————————————————
  * @brief  配置发送模式
  * @param  无
  * @retval 无
————————————————————————————————————*/
void NRF24L01_TX_Mode(void)
{  
	NRF_CE_LOW();		

	NRF24L01_WriteBuf(NRF_WRITE_REG+TX_ADDR,TX_ADDRESS,TX_ADR_WIDTH);    //写TX节点地址 
	NRF24L01_WriteBuf(NRF_WRITE_REG+RX_ADDR_P0,RX_ADDRESS,RX_ADR_WIDTH); //设置TX节点地址,主要为了使能ACK   

	NRF24L01_WriteReg(NRF_WRITE_REG+EN_AA,0x01);     //使能通道0的自动应答    
	NRF24L01_WriteReg(NRF_WRITE_REG+EN_RXADDR,0x01); //使能通道0的接收地址  
	NRF24L01_WriteReg(NRF_WRITE_REG+SETUP_RETR,0x1a);//设置自动重发间隔时间:500us + 86us;最大自动重发次数:10次
	NRF24L01_WriteReg(NRF_WRITE_REG+RF_CH,CHANAL);   //设置RF通道为CHANAL
	NRF24L01_WriteReg(NRF_WRITE_REG+RF_SETUP,0x0f);  //设置TX发射参数,0db增益,2Mbps,低噪声增益开启   
	NRF24L01_WriteReg(NRF_WRITE_REG+CONFIG,0x0e);    //配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,发射模式,开启所有中断
	
	NRF24L01_WriteReg(FLUSH_TX,NOP);
	NRF24L01_WriteReg(FLUSH_RX,NOP);
		
	NRF_CE_HIGH();//CE拉高，进入发送模式
	Delay(0xffff);//CE要拉高一段时间才进入发送模式
}

/*————————————————————————————————————
  * @brief  主要用于NRF与MCU是否正常连接
  * @param  无
  * @retval SUCCESS/ERROR 连接正常/连接失败
————————————————————————————————————*/
uint8_t NRF24L01_Check(void)
{
	uint8_t buf[5]={0xC2,0xC2,0xC2,0xC2,0xC2};
	uint8_t buf1[5];
	uint8_t i; 
	 
	NRF24L01_WriteBuf(NRF_WRITE_REG+TX_ADDR,buf,5);	//写入5个字节的地址 	
	NRF24L01_ReadBuf(TX_ADDR,buf1,5);		//读出写入的地址 
	 
	/*比较*/
	for(i=0;i<5;i++)
	{
		if(buf1[i]!=0xC2)
		break;
	} 
	       
	if(i==5)
		return SUCCESS;	//MCU与NRF成功连接         
	else
		return ERROR;	//MCU与NRF不正常连接        
}

/*————————————————————————————————————
  * @brief   用于向NRF的发送缓冲区中写入数据
  * @param   
  *		@arg txBuf：存储了将要发送的数据的数组，外部定义	
  * @retval  发送结果，成功返回TXDS,失败返回MAXRT或ERROR
————————————————————————————————————*/
uint8_t NRF24L01_Tx_Dat(uint8_t *txbuf)
{
	uint8_t state;  	 
	NRF_CE_LOW();			//ce为低，进入待机模式1					
	NRF24L01_WriteBuf(WR_TX_PLOAD,txbuf,TX_PLOAD_WIDTH);//写数据到TX BUF 最大 32个字节		

	NRF_CE_HIGH();					//CE为高，txbuf非空，发送数据包   
	                           
	while(NRF_Read_IRQ()!=0);			//等待发送完成中断 	
	                             
	state = NRF24L01_ReadReg(STATUS);		//读取状态寄存器的值
                 
	NRF24L01_WriteReg(NRF_WRITE_REG+STATUS,state);	//清除TX_DS或MAX_RT中断标志  	
	NRF24L01_WriteReg(FLUSH_TX,NOP);    		//清除TX FIFO寄存器 

	 /*判断中断类型*/    
	if(state&MAX_RT)                     
		return MAX_RT;	//达到最大重发次数 
	else if(state&TX_DS)                  
		return TX_DS;	//发送完成
	 else						  
		return ERROR;	//其他原因发送失败                 
} 

/*————————————————————————————————————
  * @brief   用于从NRF的接收缓冲区中读出数据
  * @param   
  *		@arg rxBuf ：用于接收该数据的数组，外部定义	
  * @retval 
  *		@arg 接收结果
————————————————————————————————————*/
uint8_t NRF24L01_Rx_Dat(uint8_t *rxbuf)
{
	uint8_t state; 
	NRF_CE_HIGH();	 //进入接收状态
	/*轮询标志*/
	while(NRF_Read_IRQ()==0)
	{
		NRF_CE_LOW();		//进入待机状态
	               
		state=NRF24L01_ReadReg(STATUS);			//读取status寄存器的值

	     
		NRF24L01_WriteReg(NRF_WRITE_REG+STATUS,state);	//清除中断标志

		/*判断是否接收到数据*/
		if(state&RX_DR)                                 
		{								//接收到数据
			NRF24L01_ReadBuf(RD_RX_PLOAD,rxbuf,RX_PLOAD_WIDTH);	//读取数据
			NRF24L01_WriteReg(FLUSH_RX,NOP);          		//清除RX FIFO寄存器
			return RX_DR; 
		}
		else    
			return ERROR;	//没收到任何数据
	}
	
	return ERROR;		//没收到任何数据                    
}
/*********************************************END OF FILE**********************/
