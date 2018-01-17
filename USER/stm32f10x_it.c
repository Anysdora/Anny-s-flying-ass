#include "include.h"
#include "stm32f10x_it.h"


//ϵͳʱ���, Ƶ��100HZ
u32 system_time_tick = 0;


_distance distance = {0, 0, 0};//�����ǵ� distance
_speed speed = {0, 0, 0};      //������XYZ�����speed

u8 sentDateFlag = 0;
 
void TIM3_IRQHandler(void)		    //10ms�ж�һ��
{
	static u16 TIM3_expand_1=0;	//�жϴ���������1
	static u16 TIM3_expand_2=0;	//�жϴ���������2

	if(TIM3->SR & TIM_IT_Update)
	{
		TIM3->SR = ~TIM_FLAG_Update;//����жϱ�־		
		TIM3_expand_1++;
		TIM3_expand_2++;
		
		//ϵͳʱ�ӵ�, ÿ10ms����һ��
		system_time_tick++;
		
		
		X_CONTROL(angle.roll, angle.pitch, angle.yaw);// 10ms����һ����̬

	
//		ע�����º����Ⱥ�˳��
				
		if(TIM3_expand_2 == 4)				//10 * 4 = 40ms����һ�η���ģʽ
		{
			TIM3_expand_2 = 0;

			//����ģʽ�ж�
			business_mode_handle(&mode_machine);

			//�����ϴη���״̬, ��·״̬ �� ����x_times_call_wait_for_n_ms_func, system_time_tick
			update_last_mode_state(mode_machine, &mode_machine_last,
							   road_state_machine, &road_state_machine_last, 
							   &x_times_call_wait_for_n_ms_func); 
		}

		//������������
		if(TIM3_expand_1 == 2)				//10 * 2 = 20ms�ж�һ��
		{
			TIM3_expand_1 = 0;
			
			out_of_control_lock(&angle);//��ǹ�������
			
			sentDateFlag = 1;
		}

		//����������
		MOTO_output_handle();
	}
}

