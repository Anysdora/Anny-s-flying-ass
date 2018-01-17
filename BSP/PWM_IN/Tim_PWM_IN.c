#include "TIM_PWM_IN.h"

T_RC_DATA Rc_Data;//1000~2000

void TIM4_Cap_Init(void)
{	 
	GPIO_InitTypeDef         GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_ICInitTypeDef  TIM4_ICInitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);	 //ʹ��TIM4ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);  //ʹ��GPIOBʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);  //ʹ��AFIO���ܵ�ʱ��
	GPIO_PinRemapConfig(GPIO_Remap_TIM4, ENABLE);  //������ӳ��

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;             //PB6 ���֮ǰ����  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;            
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOB,GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);		

	//��ʼ����ʱ��4 TIM4	 
	TIM_TimeBaseStructure.TIM_Period = 0XFFFF;                      //�趨�������Զ���װֵ 
	TIM_TimeBaseStructure.TIM_Prescaler =71; 	                 //Ԥ��Ƶ��   
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;      //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);              //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ

	//��ʼ��TIM4���벶�����
	TIM4_ICInitStructure.TIM_Channel = TIM_Channel_1;                //CC1S=01 	ѡ������� IC1ӳ�䵽TI1��
	TIM4_ICInitStructure.TIM_ICPolarity =TIM_ICPolarity_Rising;	   //�����ز���
	TIM4_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //ӳ�䵽TI1��
	TIM4_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	         //���������Ƶ,����Ƶ 
	TIM4_ICInitStructure.TIM_ICFilter = 0x00;                        //IC1F=0000 ���������˲��� ���˲�
	TIM_ICInit(TIM4, &TIM4_ICInitStructure);

	TIM4_ICInitStructure.TIM_Channel = TIM_Channel_2;                //CC1S=01 	ѡ������� IC1ӳ�䵽TI1��
	TIM4_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	   //�����ز���
	TIM4_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //ӳ�䵽TI1��
	TIM4_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	         //���������Ƶ,����Ƶ 
	TIM4_ICInitStructure.TIM_ICFilter = 0x00;                        //IC1F=0000 ���������˲��� ���˲�
	TIM_ICInit(TIM4, &TIM4_ICInitStructure);

	TIM4_ICInitStructure.TIM_Channel = TIM_Channel_3;                //CC1S=01 	ѡ������� IC1ӳ�䵽TI1��
	TIM4_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	   //�����ز���
	TIM4_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //ӳ�䵽TI1��
	TIM4_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	         //���������Ƶ,����Ƶ 
	TIM4_ICInitStructure.TIM_ICFilter = 0x00;                        //IC1F=0000 ���������˲��� ���˲�
	TIM_ICInit(TIM4, &TIM4_ICInitStructure);

	TIM4_ICInitStructure.TIM_Channel = TIM_Channel_4;                //CC1S=01 	ѡ������� IC1ӳ�䵽TI1��
	TIM4_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	   //�����ز���
	TIM4_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //ӳ�䵽TI1��
	TIM4_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	         //���������Ƶ,����Ƶ 
	TIM4_ICInitStructure.TIM_ICFilter = 0x00;                        //IC1F=0000 ���������˲��� ���˲�
	TIM_ICInit(TIM4, &TIM4_ICInitStructure);

	TIM_Cmd(TIM4,ENABLE ); 

	TIM_ITConfig(TIM4, TIM_IT_CC1, ENABLE);        //��������ж� ,����CC1IE�����ж�	
	TIM_ITConfig(TIM4, TIM_IT_CC2, ENABLE);
	TIM_ITConfig(TIM4, TIM_IT_CC3, ENABLE);
	TIM_ITConfig(TIM4, TIM_IT_CC4, ENABLE);	
	
/////////////////////////////////////////////////////////////////////////////////��ʼ����ʱ��8 TIM8
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);	 //ʹ��TIM8ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);  //ʹ��GPIOCʱ��
	
	
	/*  TIM8 channel 1 pin (PC.06) configuration */ 
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9; 
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
 	GPIO_Init(GPIOC, &GPIO_InitStructure); 
		
	 TIM_TimeBaseStructure.TIM_Period = 0XFFFF;                      //�趨�������Զ���װֵ 
	 TIM_TimeBaseStructure.TIM_Prescaler =71; 	                 //Ԥ��Ƶ��   
	 TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;      //����ʱ�ӷָ�:TDTS = Tck_tim
	 TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	 TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);              //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
   
	  //��ʼ��TIM8���벶�����

	TIM4_ICInitStructure.TIM_Channel = TIM_Channel_1;                //CC1S=01 	ѡ������� IC1ӳ�䵽TI1��
  	TIM4_ICInitStructure.TIM_ICPolarity =TIM_ICPolarity_Rising;	   //�����ز���
  	TIM4_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //ӳ�䵽TI1��
  	TIM4_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	         //���������Ƶ,����Ƶ 
  	TIM4_ICInitStructure.TIM_ICFilter = 0x00;                        //IC1F=0000 ���������˲��� ���˲�
  	TIM_ICInit(TIM8, &TIM4_ICInitStructure);
//	TIM_SelectMasterSlaveMode(TIM8,TIM_MasterSlaveMode_Enable);        //????????

	TIM4_ICInitStructure.TIM_Channel = TIM_Channel_2;                //CC1S=01 	ѡ������� IC1ӳ�䵽TI1��
  	TIM4_ICInitStructure.TIM_ICPolarity =TIM_ICPolarity_Rising;	   //�����ز���
  	TIM4_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //ӳ�䵽TI1��
  	TIM4_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	         //���������Ƶ,����Ƶ 
  	TIM4_ICInitStructure.TIM_ICFilter = 0x00;                        //IC1F=0000 ���������˲��� ���˲�
  	TIM_ICInit(TIM8, &TIM4_ICInitStructure);
//	TIM_SelectMasterSlaveMode(TIM8,TIM_MasterSlaveMode_Enable);        //??????????
		
	TIM4_ICInitStructure.TIM_Channel = TIM_Channel_3;                //CC1S=01 	ѡ������� IC1ӳ�䵽TI1��
  	TIM4_ICInitStructure.TIM_ICPolarity =TIM_ICPolarity_Rising;	   //�����ز���
  	TIM4_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //ӳ�䵽TI1��
  	TIM4_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	         //���������Ƶ,����Ƶ 
  	TIM4_ICInitStructure.TIM_ICFilter = 0x00;                        //IC1F=0000 ���������˲��� ���˲�
  	TIM_ICInit(TIM8, &TIM4_ICInitStructure);
//	TIM_SelectMasterSlaveMode(TIM8,TIM_MasterSlaveMode_Enable);        //?????????
		
	TIM4_ICInitStructure.TIM_Channel = TIM_Channel_4;                //CC1S=01 	ѡ������� IC1ӳ�䵽TI1��
  	TIM4_ICInitStructure.TIM_ICPolarity =TIM_ICPolarity_Rising;	   //�����ز���
  	TIM4_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //ӳ�䵽TI1��
  	TIM4_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	         //���������Ƶ,����Ƶ 
  	TIM4_ICInitStructure.TIM_ICFilter = 0x00;                        //IC1F=0000 ���������˲��� ���˲�
  	TIM_ICInit(TIM8, &TIM4_ICInitStructure);
//	TIM_SelectMasterSlaveMode(TIM8,TIM_MasterSlaveMode_Enable);        //??????????
		
    TIM_ITConfig(TIM8,TIM_IT_CC1, ENABLE);        //��������ж� ,����CC1IE�����ж�	
	TIM_ITConfig(TIM8,TIM_IT_CC2, ENABLE);   
	TIM_ITConfig(TIM8,TIM_IT_CC3, ENABLE); 
	TIM_ITConfig(TIM8,TIM_IT_CC4, ENABLE); 
	TIM_Cmd(TIM8,ENABLE ); 	
}


u8  TIM4CH1_CAPTURE_STA=0;	//���벶��״̬		    				
u16	TIM4CH1_CAPTURE_VAL;	  //���벶��ֵ

u8  TIM4CH2_CAPTURE_STA=0;	//���벶��״̬		    				
u16	TIM4CH2_CAPTURE_VAL;	  //���벶��ֵ

u8  TIM4CH3_CAPTURE_STA=0;	//���벶��״̬		    				
u16	TIM4CH3_CAPTURE_VAL;	  //���벶��ֵ

u8  TIM4CH4_CAPTURE_STA=0;	//���벶��״̬		    				
u16	TIM4CH4_CAPTURE_VAL;	  //���벶��ֵ

//��ʱ��4�жϷ������	 
void TIM4_IRQHandler(void)
{ 
    if (TIM_GetITStatus(TIM4, TIM_IT_CC1) != RESET)            //����1���������¼�
		{	
			TIM_ClearITPendingBit(TIM4, TIM_IT_CC1); //����жϱ�־λ
			if(TIM4CH1_CAPTURE_STA&0X40)		                       //����һ���½��� 		
			{	  			
				TIM4CH1_CAPTURE_STA|=0X80;		                       //��ǳɹ�����һ��������
				TIM4CH1_CAPTURE_VAL=TIM_GetCapture1(TIM4);
		   	TIM_OC1PolarityConfig(TIM4,TIM_ICPolarity_Rising); //CC1P=0 ����Ϊ�����ز���
			}
			else  								                                 //��δ��ʼ,��һ�β���������
			{
				TIM4CH1_CAPTURE_STA=0;			                         //���
				TIM4CH1_CAPTURE_VAL=0;
	 			TIM_SetCounter(TIM4,0);
				TIM4CH1_CAPTURE_STA|=0X40;		                        //��ǲ�����������
		    TIM_OC1PolarityConfig(TIM4,TIM_ICPolarity_Falling);		//CC1P=1 ����Ϊ�½��ز���
			}		    
		}	
	 
	  if (TIM_GetITStatus(TIM4, TIM_IT_CC2) != RESET)            //����1���������¼�
		{	
			TIM_ClearITPendingBit(TIM4, TIM_IT_CC2); //����жϱ�־λ
			if(TIM4CH2_CAPTURE_STA&0X40)		                       //����һ���½��� 		
			{	  			
				TIM4CH2_CAPTURE_STA|=0X80;		                       //��ǳɹ�����һ��������
				TIM4CH2_CAPTURE_VAL=TIM_GetCapture2(TIM4);
		   		TIM_OC2PolarityConfig(TIM4,TIM_ICPolarity_Rising); //CC1P=0 ����Ϊ�����ز���
			}
			else  								                                 //��δ��ʼ,��һ�β���������
			{
				TIM4CH2_CAPTURE_STA=0;			                         //���
				TIM4CH2_CAPTURE_VAL=0;
	 			TIM_SetCounter(TIM4,0);
				TIM4CH2_CAPTURE_STA|=0X40;		                        //��ǲ�����������
		    TIM_OC2PolarityConfig(TIM4,TIM_ICPolarity_Falling);		//CC1P=1 ����Ϊ�½��ز���
			}		    
		}	
  	
    if (TIM_GetITStatus(TIM4, TIM_IT_CC3) != RESET)            //����1���������¼�
		{	
			TIM_ClearITPendingBit(TIM4, TIM_IT_CC3); //����жϱ�־λ
			if(TIM4CH3_CAPTURE_STA&0X40)		                       //����һ���½��� 		
			{	  			
				TIM4CH3_CAPTURE_STA|=0X80;		                       //��ǳɹ�����һ��������
				TIM4CH3_CAPTURE_VAL=TIM_GetCapture3(TIM4);
		   	TIM_OC3PolarityConfig(TIM4,TIM_ICPolarity_Rising); //CC1P=0 ����Ϊ�����ز���
			}
			else  								                                 //��δ��ʼ,��һ�β���������
			{
				TIM4CH3_CAPTURE_STA=0;			                         //���
				TIM4CH3_CAPTURE_VAL=0;
	 			TIM_SetCounter(TIM4,0);
				TIM4CH3_CAPTURE_STA|=0X40;		                        //��ǲ�����������
		    TIM_OC3PolarityConfig(TIM4,TIM_ICPolarity_Falling);		//CC1P=1 ����Ϊ�½��ز���
			}		    
		}	

    if (TIM_GetITStatus(TIM4, TIM_IT_CC4) != RESET)            //����1���������¼�
		{	
			TIM_ClearITPendingBit(TIM4, TIM_IT_CC4); //����жϱ�־λ
			if(TIM4CH4_CAPTURE_STA&0X40)		                       //����һ���½��� 		
			{	  			
				TIM4CH4_CAPTURE_STA|=0X80;		                       //��ǳɹ�����һ��������
				TIM4CH4_CAPTURE_VAL=TIM_GetCapture4(TIM4);
		   		TIM_OC4PolarityConfig(TIM4,TIM_ICPolarity_Rising); //CC1P=0 ����Ϊ�����ز���
			}else  								                                 //��δ��ʼ,��һ�β���������
			{
				TIM4CH4_CAPTURE_STA=0;			                         //���
				TIM4CH4_CAPTURE_VAL=0;
	 			TIM_SetCounter(TIM4,0);
				TIM4CH4_CAPTURE_STA|=0X40;		                        //��ǲ�����������
		    TIM_OC4PolarityConfig(TIM4,TIM_ICPolarity_Falling);		//CC1P=1 ����Ϊ�½��ز���
			}		    
		}		
}

//��ȡ4·PWMֵ
void GET_FOUR_PWM(void)
{
   if(TIM4CH1_CAPTURE_STA&0X80)          //�ɹ�������һ��������
	 {
		Rc_Data.YAW = TIM4CH1_CAPTURE_VAL;        //�õ��ܵĸߵ�ƽʱ
		TIM4CH1_CAPTURE_STA=0;                //������һ�β���
	 }
			
	 if(TIM4CH2_CAPTURE_STA&0X80)          //�ɹ�������һ��������
	 {
		Rc_Data.THROTTLE = TIM4CH2_CAPTURE_VAL;  //�õ��ܵĸߵ�ƽʱ
		TIM4CH2_CAPTURE_STA=0;                //������һ�β���
	 }
			
	 if(TIM4CH3_CAPTURE_STA&0X80)          //�ɹ�������һ��������
	 {
		Rc_Data.PITCH = TIM4CH3_CAPTURE_VAL;        //�õ��ܵĸߵ�ƽʱ
		TIM4CH3_CAPTURE_STA=0;                //������һ�β���
	 }
			
	 if(TIM4CH4_CAPTURE_STA&0X80)          //�ɹ�������һ��������
	 {
		Rc_Data.ROLL = TIM4CH4_CAPTURE_VAL;        //�õ��ܵĸߵ�ƽʱ
		TIM4CH4_CAPTURE_STA=0;                //������һ�β���
	 }
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
u8  TIM8CH1_CAPTURE_STA=0;	//���벶��״̬		    				
u16	TIM8CH1_CAPTURE_VAL;	  //���벶��ֵ
u8  TIM8CH2_CAPTURE_STA=0;	//���벶��״̬		    				
u16	TIM8CH2_CAPTURE_VAL;	  //���벶��ֵ
u8  TIM8CH3_CAPTURE_STA=0;	//���벶��״̬		    				
u16	TIM8CH3_CAPTURE_VAL;	  //���벶��ֵ
u8  TIM8CH4_CAPTURE_STA=0;	//���벶��״̬		    				
u16	TIM8CH4_CAPTURE_VAL;	  //���벶��ֵ

u16 Rc_Data_T8_C1;	
u16 Rc_Data_T8_C2;
u16 Rc_Data_T8_C3;  //0--1087, 1--1487, 2--1887 ���¸�����1��2֮��
u16 Rc_Data_T8_C4;

//��ʱ��8cc�жϷ������
void TIM8_CC_IRQHandler(void)
{		
	if (TIM_GetITStatus(TIM8, TIM_IT_CC1) != RESET)            //����1���������¼�
	{
		TIM_ClearITPendingBit(TIM8, TIM_IT_CC1); //����жϱ�־λ
		if(TIM8CH1_CAPTURE_STA&0X40)		                       //����һ���½��� 		
		{
			TIM8CH1_CAPTURE_STA|=0X80;		                       //��ǳɹ�����һ��������
			TIM8CH1_CAPTURE_VAL=TIM_GetCapture1(TIM8);
		TIM_OC1PolarityConfig(TIM8,TIM_ICPolarity_Rising); //CC1P=0 ����Ϊ�����ز���
		}
		else  								                                 //��δ��ʼ,��һ�β���������
		{
			TIM8CH1_CAPTURE_STA=0;			                         //���
			TIM8CH1_CAPTURE_VAL=0;
			TIM_SetCounter(TIM8,0);
			TIM8CH1_CAPTURE_STA|=0X40;		                        //��ǲ�����������
		TIM_OC1PolarityConfig(TIM8,TIM_ICPolarity_Falling);		//CC1P=1 ����Ϊ�½��ز���
		}
	}

	if (TIM_GetITStatus(TIM8, TIM_IT_CC2) != RESET)            //����2���������¼�
	{
		TIM_ClearITPendingBit(TIM8, TIM_IT_CC2);               //����жϱ�־λ
		if(TIM8CH2_CAPTURE_STA&0X40)		                       //����һ���½��� 		
		{
			TIM8CH2_CAPTURE_STA|=0X80;		                       //��ǳɹ�����һ��������
			TIM8CH2_CAPTURE_VAL=TIM_GetCapture2(TIM8);
		TIM_OC2PolarityConfig(TIM8,TIM_ICPolarity_Rising); //CC2P=0 ����Ϊ�����ز���
		}
		else  								                                 //��δ��ʼ,��һ�β���������
		{
			TIM8CH2_CAPTURE_STA=0;			                         //���
			TIM8CH2_CAPTURE_VAL=0;
			TIM_SetCounter(TIM8,0);
			TIM8CH2_CAPTURE_STA|=0X40;		                        //��ǲ�����������
		TIM_OC2PolarityConfig(TIM8,TIM_ICPolarity_Falling);		//CC2P=1 ����Ϊ�½��ز���
		}
	}
	
	if (TIM_GetITStatus(TIM8, TIM_IT_CC3) != RESET)            //����3���������¼�
	{
		TIM_ClearITPendingBit(TIM8, TIM_IT_CC3);               //����жϱ�־λ
		if(TIM8CH3_CAPTURE_STA&0X40)		                       //����һ���½��� 		
		{
			TIM8CH3_CAPTURE_STA|=0X80;		                       //��ǳɹ�����һ��������
			TIM8CH3_CAPTURE_VAL=TIM_GetCapture3(TIM8); 
		TIM_OC3PolarityConfig(TIM8,TIM_ICPolarity_Rising); //CC2P=0 ����Ϊ�����ز���
		}
		else  								                                 //��δ��ʼ,��һ�β���������
		{
			TIM8CH3_CAPTURE_STA=0;			                         //���
			TIM8CH3_CAPTURE_VAL=0;
			TIM_SetCounter(TIM8,0);
			TIM8CH3_CAPTURE_STA|=0X40;		                        //��ǲ�����������
		TIM_OC3PolarityConfig(TIM8,TIM_ICPolarity_Falling);		//CC2P=1 ����Ϊ�½��ز���
		}
	}
	
	
	if (TIM_GetITStatus(TIM8, TIM_IT_CC4) != RESET)            //����1���������¼�
	{
		TIM_ClearITPendingBit(TIM8, TIM_IT_CC4); //����жϱ�־λ
		if(TIM8CH4_CAPTURE_STA&0X40)		                       //����һ���½��� 		
		{
			TIM8CH4_CAPTURE_STA|=0X80;		                       //��ǳɹ�����һ��������
			TIM8CH4_CAPTURE_VAL=TIM_GetCapture4(TIM8);
		TIM_OC4PolarityConfig(TIM8,TIM_ICPolarity_Rising); //CC1P=0 ����Ϊ�����ز���
		}
		else  								                                 //��δ��ʼ,��һ�β���������
		{
			TIM8CH4_CAPTURE_STA=0;			                         //���
			TIM8CH4_CAPTURE_VAL=0;
			TIM_SetCounter(TIM8,0);
			TIM8CH4_CAPTURE_STA|=0X40;		                        //��ǲ�����������
		TIM_OC4PolarityConfig(TIM8,TIM_ICPolarity_Falling);		//CC1P=1 ����Ϊ�½��ز���
		}
	}		
}

 void GET_TIM8_PWM(void)
{
  	  if(TIM8CH1_CAPTURE_STA&0X80)          //�ɹ�������һ��������
	 {
			Rc_Data_T8_C1 = TIM8CH1_CAPTURE_VAL;        //�õ��ܵĸߵ�ƽʱ
			TIM8CH1_CAPTURE_STA=0;  
			            //������һ�β���
	 }  
	 
	 if(TIM8CH2_CAPTURE_STA&0X80)          //�ɹ�������һ��������
	 {
			Rc_Data_T8_C2 = TIM8CH2_CAPTURE_VAL;        //�õ��ܵĸߵ�ƽʱ
			TIM8CH2_CAPTURE_STA=0;                //������һ�β���
	 }
	  if(TIM8CH3_CAPTURE_STA&0X80)          //�ɹ�������һ��������
	 {
			Rc_Data_T8_C3 = TIM8CH3_CAPTURE_VAL;        //�õ��ܵĸߵ�ƽʱ
			TIM8CH3_CAPTURE_STA=0;  
			            //������һ�β���
	 }  
	  if(TIM8CH4_CAPTURE_STA&0X80)          //�ɹ�������һ��������
	 {
			Rc_Data_T8_C4 = TIM8CH4_CAPTURE_VAL;        //�õ��ܵĸߵ�ƽʱ
			TIM8CH4_CAPTURE_STA=0;                //������һ�β���
	 }
}




