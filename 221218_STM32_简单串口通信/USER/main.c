#include "stm32f4xx.h"
#include "sys.h"
#include "delay.h"

	u8 USART_RX_BUF[200];
	u16 USART_RX_STA=0;

void My_Led_Init(void){
	GPIO_InitTypeDef Led_InitStruct;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
	
	Led_InitStruct.GPIO_Pin=GPIO_Pin_9;
	Led_InitStruct.GPIO_Mode=GPIO_Mode_OUT;
	Led_InitStruct.GPIO_OType=GPIO_OType_PP;
	Led_InitStruct.GPIO_PuPd=GPIO_PuPd_UP;
	Led_InitStruct.GPIO_Speed=GPIO_Speed_100MHz;
	
	GPIO_Init(GPIOF,&Led_InitStruct);
	GPIO_SetBits(GPIOF,GPIO_Pin_9);
}

void My_Usart1_Init(void){
	GPIO_InitTypeDef GPIO_InitStruct;
	USART_InitTypeDef USART1_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	//typedef
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	//clk enable
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1);
	//set AF
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_9;
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_100MHz;
	
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_10;
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_100MHz;
	//struct init
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	//gpio init
	
	USART1_InitStruct.USART_BaudRate=115200;
	USART1_InitStruct.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;
	USART1_InitStruct.USART_Parity=USART_Parity_No;
	USART1_InitStruct.USART_StopBits=USART_StopBits_1;
	USART1_InitStruct.USART_WordLength=USART_WordLength_8b;
	USART1_InitStruct.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	//usart1 init
	USART_Init(USART1,&USART1_InitStruct);
	USART_Cmd(USART1,ENABLE);
	
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
	//when received,interrupt.
	
	NVIC_InitStruct.NVIC_IRQChannel=USART1_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=1;
	
	NVIC_Init(&NVIC_InitStruct);
	//interruption enable
}

void USART1_IRQHandler(){
	u8 Res;
#if SYSTEM_SUPPORT_OS 		//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntEnter();    
#endif
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		Res =USART_ReceiveData(USART1);//(USART1->DR);	//读取接收到的数据
		//GPIO_ResetBits(GPIOF,GPIO_Pin_9);
		//delay_ms(60);
		//GPIO_SetBits(GPIOF,GPIO_Pin_9);
		if((USART_RX_STA&0x8000)==0)//接收未完成
		{
			if(USART_RX_STA&0x4000)//接收到了0x0d
			{
				if(Res!=0x0a)USART_RX_STA=0;//接收错误,重新开始
				else USART_RX_STA|=0x8000;	//接收完成了 
			}
			else //还没收到0X0D
			{	
				if(Res==0x0d)USART_RX_STA|=0x4000;
				else
				{
					USART_RX_BUF[USART_RX_STA&0X3FFF]=Res ;
					USART_RX_STA++;
					if(USART_RX_STA>(200-1))USART_RX_STA=0;//接收数据错误,重新开始接收	  
				}		 
			}
		}   		 
	}
}//interruption service

int main(void){	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	My_Usart1_Init();
	My_Led_Init();
	delay_init(168);
	
	while(1){
		if(USART_RX_STA&0x8000)
		{	
			GPIO_ResetBits(GPIOF,GPIO_Pin_9);
			delay_ms(60);
			GPIO_SetBits(GPIOF,GPIO_Pin_9);
			/*
			len=USART_RX_STA&0x3fff;//得到此次接收到的数据长度
			for(t=0;t<len;t++)
			{
				USART_SendData(USART1, USART_RX_BUF[t]);         //向串口1发送数据
				while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
			}
			*/
			USART_RX_STA=0;
		}
	}
}
