#include "moto.h"


void Tim2_init(void)
{
	TIM_TimeBaseInitTypeDef		TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  				TIM_OCInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	/**********************************************************
	72 000 000/72=1M
	1000 000/2500=400Hz
	���Բ�����PWMΪ400Hz
	����Ϊ2.5ms����Ӧ2500�ļ���ֵ��1ms~2ms��Ӧ�ļ���ֵΪ1000~2000��
	**********************************************************/
	TIM_TimeBaseStructure.TIM_Period = 2499;		//��������	
	TIM_TimeBaseStructure.TIM_Prescaler = 71;	//pwmʱ�ӷ�Ƶ
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;	
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;		//���ϼ���
	TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;       //��ʼռ�ձ�Ϊ0
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	
	TIM_OC1Init(TIM2, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);
	TIM_OC2Init(TIM2, &TIM_OCInitStructure);
	TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);
	TIM_OC3Init(TIM2, &TIM_OCInitStructure);
	TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);
	TIM_OC4Init(TIM2, &TIM_OCInitStructure);
	TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable);
	
	TIM_ARRPreloadConfig(TIM2, ENABLE);
	TIM_Cmd(TIM2, ENABLE);
}

void Moto_Init(void)   //PA0-3
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* GPIOA and GPIOC clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); 

	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_3 | GPIO_Pin_2 | GPIO_Pin_1 | GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		    // �����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	Tim2_init();	  //�ɶ�ʱ��2���ĸ�ͨ��������
}

//s8 gesture_feedforward_moto_0 = 0;      ǰ������ ��֤��ֱ���
//s8 gesture_feedforward_moto_1 = -10;
//s8 gesture_feedforward_moto_2 = -25;
//s8 gesture_feedforward_moto_3 = -15;

const u16 langyu_start_throttle = 750;     //���ֵ����ʼ����
const u16 xinxida_start_throttle = 750;
const u16 yinyan_start_throttle = 1050;

/* MOTO1-PA3 MOTO2-PA2 MOTO3-PA1 MOTO4-PA0 */
void Moto_PwmRflash(uint16_t MOTO1_PWM,uint16_t MOTO2_PWM,uint16_t MOTO3_PWM,uint16_t MOTO4_PWM)
{		
	if(MOTO1_PWM>Moto_PwmMax)	MOTO1_PWM = Moto_PwmMax;   //�����޷� MAX==950
	if(MOTO2_PWM>Moto_PwmMax)	MOTO2_PWM = Moto_PwmMax;
	if(MOTO3_PWM>Moto_PwmMax)	MOTO3_PWM = Moto_PwmMax;
	if(MOTO4_PWM>Moto_PwmMax)	MOTO4_PWM = Moto_PwmMax;	
	
	TIM2->CCR1 = MOTO1_PWM + yinyan_start_throttle + gesture_feedforward_moto_0;
	TIM2->CCR2 = MOTO2_PWM + yinyan_start_throttle + gesture_feedforward_moto_1;
	TIM2->CCR3 = MOTO3_PWM + yinyan_start_throttle + gesture_feedforward_moto_2;
	TIM2->CCR4 = MOTO4_PWM + yinyan_start_throttle + gesture_feedforward_moto_3;
}
