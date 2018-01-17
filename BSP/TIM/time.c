#include "time.h"
#include "stm32f10x.h"


/**************************ʵ�ֺ���********************************************
*����ԭ��:		
*��������:1ms�ж�һ��,������Ϊ1000		
*******************************************************************************/
void TIM3_Init(u16 period_num)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	//�������ã�ʱ���ͱȽ�������ã���������ֻ�趨ʱ�����Բ���OC�Ƚ����
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	
	TIM_DeInit(TIM3);

	TIM_TimeBaseStructure.TIM_Period=period_num;//װ��ֵ
	
	//prescaler is 72,that is 72000000/72 = 1000000Hz �� 1us;
	TIM_TimeBaseStructure.TIM_Prescaler=72-1;//��Ƶϵ��
	
	//set clock division 
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; //or TIM_CKD_DIV2 or TIM_CKD_DIV4
	
	//count up
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
	
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);
	//clear the TIM3 overflow interrupt flag
	TIM_ClearFlag(TIM3,TIM_FLAG_Update);
	//TIM3 overflow interrupt enable
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
	//enable TIM3
	TIM_Cmd(TIM3,DISABLE);
}


/**************************ʵ�ֺ���******************************************************			
*��������:1ms����һ�Σ�ͨ��get_ms_for_6050dmp(unsigned long *count)���ݼ���ֵ��MPU6050DMP
*˵    ��:û���ж�
*****************************************************************************************/
void TIM1_Init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	//�������ã�ʱ���ͱȽ�������ã���������ֻ�趨ʱ�����Բ���OC�Ƚ����
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);
	
	TIM_DeInit(TIM1);

	TIM_TimeBaseStructure.TIM_Period = 0XFFFF;//װ��ֵ
	
	//prescaler is 35999,that is 72000000/(35999 + 1)/2=1000Hz;
	TIM_TimeBaseStructure.TIM_Prescaler=35999;//��Ƶϵ��
	
	//set clock division 
	//�˴�����Ϊ 2ʱ�ӷ�Ƶ�� ӦΪprescaler����Ϊ0xffff��Ȼ�޷��ﵽ1ms����һ��
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV2; //or TIM_CKD_DIV1 or TIM_CKD_DIV4
	//count up
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
	
	TIM_TimeBaseInit(TIM1,&TIM_TimeBaseStructure);
	
	//clear the TIM3 overflow interrupt flag
	TIM_ClearFlag(TIM1,TIM_FLAG_Update);
	
	//TIM3 overflow interrupt disable
	TIM_ITConfig(TIM1,TIM_IT_Update,DISABLE);
	
	//enable TIM1
	TIM_Cmd(TIM1,ENABLE);
}

//�˺�������DMP6050��DMP��ʱ�����Ȼ����û��ʲô����
void  get_ms_gansha(unsigned long *count)
{
	*count = TIM1->CNT;
	
	TIM1->CNT = 0;
}
