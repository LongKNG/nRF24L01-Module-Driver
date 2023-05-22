/*---------------一个简单的nRF24L01模块测试程序----------------*/

#include "nRF24L01.h"
#include "OLED.h"       //OLED用于显示调试，也可以用串口       
#include "Delay.h"

uint8_t status;         //用于判断接收/发送状态的标志位
uint8_t Mode = 1;       //用于模式选择，1为接收模式，2为发送模式

uint8_t Tx_Buf[32];		// 发送缓冲
uint8_t Rx_Buf[32];		// 接收缓冲

int main()
{
    NRF24L01_Init();    //nRF24L01初始化
    OLED_Init();        //OLED初始化
    	 
    if(Mode == 1)
    {
         NRF24L01_RX_Mode(); //接收模式
    }
    else if(Mode == 2)
    {
        NRF24L01_TX_Mode(); //发送模式   
        Tx_Buf[0] = 2;      //第0位用于设置需要发送数据的长度，最长不超过32
        Tx_Buf[1] = 'H';
        Tx_Buf[2] = 'i';
    }
    
    status = NRF_Check();   //模块自检
    if(status == SUCCESS)   
	{
		OLED_ShowString(1,1,"Successful");
	}
    
/*-----------------------------------------------------*/    
    while(1)
    {
        if(Mode == 1)
        {
            status = NRF24L01_Rx_Dat(Rx_Buf);
		    if(status == RX_DR)		//接收成功
		    {
                OLED_ShowString(3,1,"Rece Successful");
            }
        }
        
        Delay_ms(1000);
        if(Mode == 2)
        {
            status = NRF24L01_Tx_Dat(Tx_Buf)
            if(status == TX_DS)     //发送成功
            {
                OLED_ShowString(3,1,"Send Successful");
            }
        }
    }
}
/*-----------------------------------------------------*/
