#include "include.h"
#include "business_mode.h"


//@brief :	ϵͳ��n�ε���wait_for_n_ms����, 
//			�ñ��� ����+����+����(��Ҫ������Ҫ˵����) 
//			�� ��"update_last_mode_state()����(������)" ���Լ� ��"ÿ�εȴ�ʱ�䵽���"(������) ����
//
u16 x_times_call_wait_for_n_ms_func = 0;

u8 Flag=0;
//
//@brief :	�ȴ� n * 10(ms), ��Ҫ�������̿����е�ʱ��ȴ�
//
//@param :	param_0 system_time_tick, ϵͳʱ�ӵ�, Ƶ��100HZ
//			param_1 n_ms,
//			param_2 �˴��ǵڼ��ε��øú���
//
//@retval:	�ȴ�ʱ���Ƿ��ѵ������� return 1, ���� return 0.
//
//@note  :	param_2 ��Ҫ����������ʱ�䣬���¼�ʱ�� ÿ�εȴ�ʱ�䵽���������
//			�˺������ܽ�����ã����� n_ms ��ʱδ��ʱ�������������������ã���������
//
u8 wait_for_n_ms(u32 *system_time_tick, u32 n_ms, u16 *x_times)
{
	//��ʼ��Ϊ65535, ��ֹ��һ�ε���ʱ x_times_last == x_times, �Ӷ�����bug
	static u16 x_times_last = 65535;

	//���ٴε��õ��˺���ʱ����¼�µ���˲���system_time_tick, ������ n_ms ���Ƚ�, ���Ƿ��ѵ���ȴ�ʱ�䡣
	
	//��ʼ�ȴ�ʱ�� "system_time_tick" ����
	static u32 start_wait_system_time_tick_latch;

	if(*x_times == x_times_last) 
	{		
		if(n_ms < (*system_time_tick - start_wait_system_time_tick_latch))
		{
			//ʱ���ѵ�, ������±���--->x_times_call_wait_for_n_ms_func
			(*x_times)++; 
	
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		x_times_last = *x_times;
		
		//ʱ������
		start_wait_system_time_tick_latch = *system_time_tick;
				
		return 0;
	}
}




//

int16_t MODE=0;
//
void A_to_B_fire_B_to_A_control(mode_flow_step_type *current_mode_flow_steps)
{
	u8 Suddenly_flag = 1;
  
	float track_pitch_offset_max = 2.3f;
	
	if(g_height == 300) g_height =110;
	
	switch(*current_mode_flow_steps)
	{
		case mode_step_none:
		{
			MODE=0;
			*current_mode_flow_steps = mode_step_1;
			
			//��ƫ���ǵ�����ֵ�������˲���yawֵ, �����Ͳ��ᵼ���ϵ�����ᱻ������ת����ɵ���ʧ��
			yaw_offset = angle.yaw;
			
			set_height_height_grab.flag = 0;	
			set_height_throttle_grab.flag = 0;
			set_height_throttle_grab.current_throttle = 0 ;
			fly_direction = forward;
			magnet_ON;
		
			break;
		}
		
		case mode_step_1:
		{
			MODE=1;
			//�ȴ�8s
			if(wait_for_n_ms(&system_time_tick, 1000, &x_times_call_wait_for_n_ms_func))
			{
				*current_mode_flow_steps = mode_step_2;
				increment_set_to_zero();
			}
			
			break;
		}
		
		case mode_step_2:
		{
			MODE=2;
//			set_height_control_expect_height = delivery_param_to_control.expect_height_start_circle;
			set_height_control_expect_height =110;
			set_height_cascade_control_2(set_height_control_expect_height, set_height_control_expect_height/5, set_height_control_expect_height/6);
			
	
			if(g_height > 35)
			{
				locate_single_loop_pid_control(g_position_circle_adjust.x, g_position_circle_adjust.y);
				
				if(stady_state_judge(g_position_circle_adjust, set_point_expect_position, 5, 80))
				{
					*current_mode_flow_steps = mode_step_3;
					
					//ȡyaw����ֵ
//					yaw_offset_tmp = get_yaw_expect(g_position_circle_adjust.x, g_position_circle_adjust.y, 
//													g_position_line_1_adjust.x, g_position_line_1_adjust.y);
				}
				

			}
			else
			{
				roll_offset = 0;
				pitch_offset = 0;
			}
			
			break;
		}
		
		case mode_step_3:
		{
			MODE=3;
//			if(yaw_expect_wave(yaw_offset_tmp, &yaw_offset))
			{
				*current_mode_flow_steps = mode_step_4;
			}
			
			set_height_cascade_control_2(set_height_control_expect_height, set_height_control_expect_height/5, set_height_control_expect_height/6);
			
			if(g_height > 35)
			{
				locate_single_loop_pid_control(g_position_circle_adjust.x, g_position_circle_adjust.y);
			}
			else
			{
				roll_offset = 0;
				pitch_offset = 0;
			}
			
			break;
		}
		
		case mode_step_4:
		{
			MODE=4;
			set_height_cascade_control_2(set_height_control_expect_height, set_height_control_expect_height/5, set_height_control_expect_height/6);
			
			if(g_height > 35)
			{
				locate_single_loop_pid_control(g_position_circle_adjust.x, g_position_circle_adjust.y);
			}
			else
			{
				roll_offset = 0;
				pitch_offset = 0;
			}
			
			//����ѭ����
//			if(g_height > set_height_control_expect_height - 5 && g_height < set_height_control_expect_height + 5)
//			{
//				*current_mode_flow_steps = mode_step_5;
//			}
			if(stady_state_judge(g_position_circle_adjust, set_point_expect_position, 10, 80))
			{
					*current_mode_flow_steps = mode_step_5;
			}
				
			
			break;
		}
		
		//ѭ��, ֻ�������ĵ㲻һ��
		case mode_step_5:
		{
			MODE=5;
			set_height_cascade_control_2(set_height_control_expect_height, set_height_control_expect_height/5, set_height_control_expect_height/6);
			
			if(fly_direction == forward)
			{
				//���������¼���·��״̬
				if(road_state_machine == circle_connect_line)
				{
					//��ͷ�����ǳ���η�, ����λ line_2.end, ������F405���Ѿ��������
					locate_single_loop_pid_control(g_position_line_2_adjust.x, g_position_circle_adjust.y);		
					pitch_offset=-2.7;
//					if(roll_offset>1.2)
//						roll_offset=1.2;
//					if(roll_offset<-1.2)
//						roll_offset=-1.2;
				}
				else if(road_state_machine == straight_line || road_state_machine == line_connect_circle_connect_line )
				{
				
					locate_single_loop_pid_control(g_position_line_1_adjust.x, g_position_line_1_adjust.y );
					pitch_offset=-2.7;
//					if(roll_offset>1.2)
//						roll_offset=1.2;
//					if(roll_offset<-1.2)
//						roll_offset=-1.2;
				}
				else if(road_state_machine == one_circle)
				{
					locate_single_loop_pid_control(g_position_circle_adjust.x, g_position_circle_adjust.y);				
					pitch_offset=-2.7;
//					if(roll_offset>1.2)
//						roll_offset=1.2;
//					if(roll_offset<-1.2)
//						roll_offset=-1.2;
				}
				else if(road_state_machine == line_connect_circle)
				{

					
				 {	
						//�Ѿ������յ�
					locate_single_loop_pid_control(g_position_circle_adjust.x, g_position_circle_adjust.y);
					
//						if(g_position_circle_adjust.x <100 && g_position_circle_adjust.x > -100 
//						&& g_position_circle_adjust.y <100 && g_position_circle_adjust.y > -100)
//						{
//					//������һ�� 
								*current_mode_flow_steps = mode_step_6;
								locate_single_loop_pid_x.increment = 1000;
//								locate_single_loop_pid_y.increment = 0;
//						}
					}

				}
	
				
			}
			else if(fly_direction == backward)
			{
				//���������¼���·��״̬
				if(road_state_machine == line_connect_circle)
				{
					//��ͷ�����ǳ���η�, ����λ line_2.end, ������F405���Ѿ��������
					locate_single_loop_pid_control(g_position_line_2_adjust.x, g_position_line_2_adjust.y);
						pitch_offset=1.5;
					if(roll_offset>1)
						roll_offset=1;
					if(roll_offset<-1)
						roll_offset=-1;
				}
				else if(road_state_machine == straight_line || road_state_machine == two_stripes || road_state_machine == one_stripes)
				{				
					locate_single_loop_pid_control(g_position_line_2_adjust.x, (g_position_line_1_adjust.y + g_position_line_2_adjust.y) / 2.0f);
						pitch_offset=1.5;
					if(roll_offset>1)
						roll_offset=1;
					if(roll_offset<-1)
						roll_offset=-1;
				}
				else if(road_state_machine == circle_connect_line)
				{
					//�Ѿ������յ�
					locate_single_loop_pid_control(g_position_circle_adjust.x, g_position_circle_adjust.y);
					
					
					if(g_position_circle_adjust.x <150 && g_position_circle_adjust.x > -150 
						&& g_position_circle_adjust.y <150 && g_position_circle_adjust.y > -150)
					{
					//������һ�� 
						*current_mode_flow_steps = mode_step_6;
						locate_single_loop_pid_x.increment = 0;
//						locate_single_loop_pid_y.increment = 0;
					}

				}
				else
				{
					//���������е�·��״̬, ��������
				}
	
			}
			else
			{
				//fuck
			}		

			//������һ���ܵ�ʱ��, ����
			if(road_state_machine == one_stripes)
			{
				track_pitch_offset_max = 1.8f;
			}
			
/**********************************************************************************************************************/
			//
			//ʵ��������ܵ��ڽ�Բ��ʱ��ǳ���, ����Ч��������
			//
			
			//����������޷�, ��ֹ�ٶȹ���
			track_output_limit(track_pitch_offset_max, &pitch_offset);
			
			break; 
		}		
		
		case mode_step_6:
		{
			if((road_state_machine == line_connect_circle_connect_line))
			{
						
				*current_mode_flow_steps = mode_step_5;
				
			}
			MODE=6;
			locate_single_loop_pid_control(g_position_circle_adjust.x, g_position_circle_adjust.y);
			set_height_cascade_control_2(set_height_control_expect_height, set_height_control_expect_height/5, set_height_control_expect_height/6);
			
			//���յ�Բ�ϴﵽ��̬, ����LED3, ����ȴ���
			if(stady_state_judge(g_position_circle_adjust, set_point_expect_position, 10, 80))
			{
				*current_mode_flow_steps = mode_step_7;
				
//				magnet_OFF;
//				set_height_throttle_grab.current_throttle -= 20;
			}
			
				
			
			

			break;
			
		}		
		
		case mode_step_7:
		{
			MODE=7;
			locate_single_loop_pid_control(g_position_circle_adjust.x, g_position_circle_adjust.y);
			set_height_cascade_control_2(set_height_control_expect_height, set_height_control_expect_height/5, set_height_control_expect_height/6);
			
			//LED3��3s, ���뷵�ز�
//			if(wait_for_n_ms(&system_time_tick, 300, &x_times_call_wait_for_n_ms_func))
			{
				*current_mode_flow_steps = mode_step_8;
				
				LED3(OFF);
				
				fly_direction = (enum _direction)(1 - fly_direction);
				
				track_pitch_offset_max = 2.0f;
			}

			break;
			
		}		
		
		//����ѭ��
		case mode_step_8:
		{
			set_height_cascade_control_2(set_height_control_expect_height, set_height_control_expect_height/5, set_height_control_expect_height/6);
			
			if(fly_direction == forward)
			{
				//���������¼���·��״̬
				if(road_state_machine == circle_connect_line)
				{
					//��ͷ�����ǳ���η�, ����λ line_2.end, ������F405���Ѿ��������
					locate_single_loop_pid_control(g_position_line_2_adjust.x, g_position_line_2_adjust.y);
						pitch_offset=1;
					if(roll_offset>1)
						roll_offset=1;
					if(roll_offset<-1)
						roll_offset=-1;
				}
				else if(road_state_machine == straight_line || road_state_machine == two_stripes || road_state_machine == one_stripes)
				{						
					locate_single_loop_pid_control(g_position_line_1_adjust.x, (g_position_line_1_adjust.y + g_position_line_2_adjust.y) / 2.0f);
						pitch_offset=1;
					if(roll_offset>1)
						roll_offset=1;
					if(roll_offset<-1)
						roll_offset=-1;
				}
				else if(road_state_machine == line_connect_circle)
				{
					//�Ѿ������յ�
					locate_single_loop_pid_control(g_position_circle_adjust.x, g_position_circle_adjust.y);
					
					if(g_position_circle_adjust.x <100 && g_position_circle_adjust.x > -100 
					&& g_position_circle_adjust.y <100 && g_position_circle_adjust.y > -100)
					{
					//������һ�� 
					*current_mode_flow_steps = mode_step_9;
						locate_single_loop_pid_x.increment = 0;
//						locate_single_loop_pid_y.increment = 0;
					}
				}
				else
				{
					//���������е�·��״̬, ��������
				}
				
			}
			else if(fly_direction == backward)
			{
							
				//���������¼���·��״̬
				if(road_state_machine == line_connect_circle)
				{
					if(g_position_circle_adjust.x <150 && g_position_circle_adjust.x > -150) 
					{
						locate_single_loop_pid_control(g_position_line_2_adjust.x, g_position_circle_adjust.y);
							pitch_offset=1.8;
//					if(roll_offset>1.2)
//						roll_offset=1.2;
//					if(roll_offset<-1.2)
//						roll_offset=-1.2;
					}
					//��ͷ�����ǳ���η�, ����λ line_2.end, ������F405���Ѿ��������
					locate_single_loop_pid_control(g_position_line_2_adjust.x, g_position_circle_adjust.y);
						pitch_offset=1.8;
//					if(roll_offset>1.2)
//						roll_offset=1.2;
//					if(roll_offset<-1.2)
//						roll_offset=-1.2;
				}
				else if(road_state_machine == line_connect_circle_connect_line || road_state_machine == straight_line )
				{					
					locate_single_loop_pid_control(g_position_line_2_adjust.x, g_position_line_1_adjust.y );
						pitch_offset=1.8;
//					if(roll_offset>1.2)
//						roll_offset=1.2;
//					if(roll_offset<-1.2)
//						roll_offset=-1.2;
				}
				else if(road_state_machine == one_circle)
				{
					locate_single_loop_pid_control(g_position_circle_adjust.x, g_position_circle_adjust.y);				
					pitch_offset=1.8;
//					if(roll_offset>1.2)
//						roll_offset=1.2;
//					if(roll_offset<-1.2)
//						roll_offset=-1.2;
				}
				else if(road_state_machine == circle_connect_line)
				{
				
					{
					//�Ѿ������յ�
					locate_single_loop_pid_control(g_position_circle_adjust.x, g_position_circle_adjust.y);
					
					//������һ�� 
//					if(g_position_circle_adjust.x <100 && g_position_circle_adjust.x > -100
//					&& g_position_circle_adjust.y <100 && g_position_circle_adjust.y > -100)
//					{
					//������һ�� 
					*current_mode_flow_steps = mode_step_9;
						locate_single_loop_pid_x.increment = -1000;
//						locate_single_loop_pid_y.increment = 0;
//					}
					}
				}
				else
				{
					//���������е�·��״̬, ��������
				}
	
			}
			else
			{
				//fuck
			}		

			//������һ���ܵ�ʱ��, ����
			if(road_state_machine == two_stripes)
			{
				track_pitch_offset_max = 1.8f;
			}
			
/**********************************************************************************************************************/
			//
			//ʵ��������ܵ��ڽ�Բ��ʱ��ǳ���, ����Ч��������
			//
			
			//����������޷�, ��ֹ�ٶȹ���
			
			track_output_limit(track_pitch_offset_max, &pitch_offset);
			
			break; 
		}		
		
		//���յ�Բ�Ϸ�, ������̬����Խ���
		case mode_step_9:
		{
			if(road_state_machine == line_connect_circle_connect_line)
			{	
					*current_mode_flow_steps = mode_step_8;
			}
			
			locate_single_loop_pid_control(g_position_circle_adjust.x, g_position_circle_adjust.y);
			set_height_cascade_control_2(set_height_control_expect_height, set_height_control_expect_height/5, set_height_control_expect_height/6);
			
			//���յ�Բ�ϴﵽ��̬
			if(stady_state_judge(g_position_circle_adjust, set_point_expect_position, 10, 80))
			{
				*current_mode_flow_steps = mode_step_10;
			}

			break;
		}
		
		//����
		case mode_step_10:
		{
			MODE=10;
			*current_mode_flow_steps = mode_step_11;
			
			set_height_height_grab.flag = 0;	
			
			set_height_control_expect_height = -60;
			
			Suddenly_flag = 1;
			
			break;
		}		
		
		case mode_step_11:
		{
			MODE=11;
			set_height_cascade_control_2(set_height_control_expect_height, 
										 (g_height-set_height_control_expect_height) * 0.1, 
										 (g_height-set_height_control_expect_height) * 0.1);


			
			if(g_height > 30)
			{
				locate_single_loop_pid_control(g_position_circle_adjust.x, g_position_circle_adjust.y);
				//�߶Ȳ���, ��������
			}
			else
			{
				//���䡢������������
				pitch_offset = 0;
				roll_offset = 0;
				locate_single_loop_pid_x.increment = 0;
				locate_single_loop_pid_y.increment = 0;	
				
				//ֱ�Ӽ�����, ���ٽ���
				set_height_throttle_grab.current_throttle -= 10;
				
				if(g_height < 20 && Suddenly_flag == 1)
				{
					set_height_throttle_grab.current_throttle -= 50;
					Suddenly_flag = 0;
				}
				
				if(set_height_throttle_grab.current_throttle <= 100)
				{
					set_height_throttle_grab.current_throttle = 100;
				}

			}

			if(g_height < 10)
			{
				*current_mode_flow_steps = mode_step_12;		
				
				ctrl.height.core.pid_out = 0;
				ctrl.height.shell.pid_out = 0;
				ctrl.height.shell.increment = 0;
				ctrl.height.core.increment = 0;
			
				ARMED = 0;
			}
			break;
		}
		
		case mode_step_12:
		{
			MODE=12;
			*current_mode_flow_steps = mode_step_none;
		
			mode_machine = mode_none;			
			
			break;
		}
		
		default:
		{
			
			break;
		}
	}
}

void fly_on_rectangle_control(mode_flow_step_type *current_mode_flow_steps)
{
	float track_pitch_offset_max = 2.2f;
	
	u8 Suddenly_flag = 1;
	

		
	static road_state_machine_type last_road_state_machine = one_stripes;	//��ѭ���ﲻ����ֵ�״̬

	switch(*current_mode_flow_steps)
	{
		case mode_step_none:
		{
			MODE=0;
			*current_mode_flow_steps = mode_step_1;
			
			//��ƫ���ǵ�����ֵ�������˲���yawֵ, �����Ͳ��ᵼ���ϵ�����ᱻ������ת����ɵ���ʧ��
			yaw_offset = angle.yaw;
			
			set_height_height_grab.flag = 0;	
			set_height_throttle_grab.flag = 0;
			set_height_throttle_grab.current_throttle = 0 ;
			fly_direction = forward;
			magnet_ON;
		
			break;
		}
		
		case mode_step_1:
		{
			MODE=1;
			//�ȴ�8s
			if(wait_for_n_ms(&system_time_tick, 1000, &x_times_call_wait_for_n_ms_func))
			{
				*current_mode_flow_steps = mode_step_2;
				increment_set_to_zero();
			}
			
			break;
		}
		
		case mode_step_2:
		{
			MODE=2;
//			set_height_control_expect_height = delivery_param_to_control.expect_height_start_circle;
			
			set_height_control_expect_height =90;//�߶ȸĵ�һ��ͼ������ӭ�ж���
			set_height_cascade_control_2(set_height_control_expect_height, set_height_control_expect_height/5, set_height_control_expect_height/6);
			
	
			if(g_height > 30)
			{
				locate_single_loop_pid_control(g_position_circle_adjust.x, g_position_circle_adjust.y);
				
				if(stady_state_judge(g_position_circle_adjust, set_point_expect_position, 3, 100))
				{
					*current_mode_flow_steps = mode_step_3;
					
					//ȡyaw����ֵ
//					yaw_offset_tmp = get_yaw_expect(g_position_circle_adjust.x, g_position_circle_adjust.y, 
//													g_position_line_1_adjust.x, g_position_line_1_adjust.y);
				}
				
			}
			else
			{
				roll_offset = 0;
				pitch_offset = 0;
			}
			
			break;
		}
		
		case mode_step_3:
		{
			MODE=3;
//			if(yaw_expect_wave(yaw_offset_tmp, &yaw_offset))
			{
				*current_mode_flow_steps = mode_step_4;
			}
			
			set_height_cascade_control_2(set_height_control_expect_height, set_height_control_expect_height/5, set_height_control_expect_height/6);
			
			if(g_height > 30)
			{
				locate_single_loop_pid_control(g_position_circle_adjust.x, g_position_circle_adjust.y);
			}
			else
			{
				roll_offset = 0;
				pitch_offset = 0;
			}
			
			break;
		}
		
		case mode_step_4:
		{
			MODE=4;
			set_height_cascade_control_2(set_height_control_expect_height, set_height_control_expect_height/5, set_height_control_expect_height/6);
			
			if(g_height > 30)
			{
				locate_single_loop_pid_control(g_position_circle_adjust.x, g_position_circle_adjust.y);
			}
			else
			{
				roll_offset = 0;
				pitch_offset = 0;
			}
			
			//����ѭ����
//			if(g_height > set_height_control_expect_height - 5 && g_height < set_height_control_expect_height + 5)
//			{
//				*current_mode_flow_steps = mode_step_5;
//			}
			if(stady_state_judge(g_position_circle_adjust, set_point_expect_position, 8, 80))//���Ⱥ󷢳�
			{
					*current_mode_flow_steps = mode_step_5;
			}
				
			
			break;
		}
		
		//ѭ��, ֻ�������ĵ㲻һ��
		case mode_step_5:
		{
			MODE=5;
			set_height_cascade_control_2(set_height_control_expect_height, set_height_control_expect_height/5, set_height_control_expect_height/6);
					
			
			
			if( road_state_machine == straight_line)		
			{				
				locate_single_loop_pid_control(g_position_line_1_adjust.x, g_position_line_1_adjust.y );
				pitch_offset=-2.0;
//				if(roll_offset>1.2)
//					roll_offset=1.2;
//				if(roll_offset<-1.2)
//					roll_offset=-1.2;
			}
			else if(road_state_machine == line_connect_circle_connect_line ||road_state_machine == one_circle||road_state_machine == Three_line_cricle)
			{
				locate_single_loop_pid_control(g_position_line_1_adjust.x, g_position_circle_adjust.y);
				pitch_offset=-2.0;
//				if(roll_offset>1.2)
//					roll_offset=1.2;
//				if(roll_offset<-1.0)
//					roll_offset=-1.0;
			}
			else if(road_state_machine == juxingjiao)		//��⵽ת�ǽ�����һ�� ��һ��ת��
			{
				*current_mode_flow_steps = mode_step_6;
			}
		

			
			break; 
		}

		case mode_step_6://////////////////////////ת��һ�����ν�
		{
			MODE=6;
			pitch_offset=0;
			
			if(yaw_expect_wave(90, &yaw_offset))//��ת
			{
				*current_mode_flow_steps = mode_step_7;
			}
			
			if(yaw_offset>60&&yaw_offset<90)
			{
				locate_single_loop_pid_control(g_position_line_1_adjust.x, g_position_line_1_adjust.y);
				pitch_offset=-1;
			}
			
			set_height_cascade_control_2(set_height_control_expect_height, set_height_control_expect_height/5, set_height_control_expect_height/6);
			
//			if(g_height > 35)
//			{
//				locate_single_loop_pid_control(g_position_circle_adjust.x, g_position_circle_adjust.y);
//			}
//			else
			{
//				roll_offset = 0;
				pitch_offset = 0;
			}
			
		
	
				break;
		}
		
		case mode_step_7:
		{
			MODE=7;
			set_height_cascade_control_2(set_height_control_expect_height, set_height_control_expect_height/5, set_height_control_expect_height/6);
					
			pitch_offset=-1.6;
			
			if( road_state_machine == straight_line)		
			{				
				locate_single_loop_pid_control(g_position_line_1_adjust.x, g_position_line_1_adjust.y );
				pitch_offset=-2.0;
//				if(roll_offset>1.2)
////					roll_offset=1.2;
				if(roll_offset<-1.2)
					roll_offset=-1.2;
			}
			
			else if(road_state_machine == line_connect_circle_connect_line ||road_state_machine == one_circle||road_state_machine == Three_line_cricle)
			{
				locate_single_loop_pid_control(g_position_line_1_adjust.x, g_position_circle_adjust.y);
				pitch_offset=-2.0;
////				if(roll_offset>1.2)
////					roll_offset=1.2;
				if(roll_offset<-1.2)
					roll_offset=-1.2;
			}
			
			else if(road_state_machine == juxingjiao )
			{
				*current_mode_flow_steps = mode_step_8;
			}
			
			
			
			
			break;
		}
		case mode_step_8:   //����ֱ�ӷɷɵ�Բ����  ROLL����
		{
			MODE=8;

			set_height_cascade_control_2(set_height_control_expect_height, set_height_control_expect_height/5, set_height_control_expect_height/6);
			
			
			if( road_state_machine == Three_line_cricle )
			{
				*current_mode_flow_steps = mode_step_9;
			}
			else
			{
				locate_single_loop_pid_control(g_position_line_1_adjust.x, g_position_circle_adjust.y);
				
				roll_offset=-0.8;
				if(pitch_offset>1.0)
					pitch_offset=1.0;
				if(pitch_offset<-1.0)
					pitch_offset=-1.0;
			}
			
			break;
		}
//		case mode_step_8:
//		{
//			set_height_cascade_control_2(set_height_control_expect_height, set_height_control_expect_height/5, set_height_control_expect_height/6);

//			
//			if(yaw_expect_wave(90, &yaw_offset))
//			{
//					*current_mode_flow_steps = mode_step_9;
//			}
//			
//			
//			break;
//		}
	
//		case mode_step_9:
//		{
//			set_height_cascade_control_2(set_height_control_expect_height, set_height_control_expect_height/5, set_height_control_expect_height/6);

//			pitch_offset=-2.5;
//			roll_offset=0;
//			if(road_state_machine = straight_line)
//			{
//				pitch_offset=-2.0;
////				locate_single_loop_pid_control(g_position_line_1_adjust.x, (g_position_line_1_adjust.y + g_position_line_2_adjust.y) / 2.0f);
////				if(roll_offset>1.2)
////					roll_offset=1.2;
////				if(roll_offset<-0.6)
////					roll_offset=-0.6;
//			}
//			else if(road_state_machine == Three_line_cricle)
//			{
//				*current_mode_flow_steps = mode_step_11;
//			}
//			
//			
//			break;
//		}
		case mode_step_9:   //
		{
			MODE=9;
			set_height_cascade_control_2(set_height_control_expect_height, set_height_control_expect_height/5, set_height_control_expect_height/6);
			
			if(g_height > 30)
			{
				locate_single_loop_pid_control(g_position_circle_adjust.x, g_position_circle_adjust.y);
				
//				if(roll_offset>3)
//					roll_offset=3;
				if(roll_offset<-1.8)
					roll_offset=-1.8;
				
			}
			else
			{
				roll_offset = 0;
				pitch_offset = 0;
			}
			

			if(stady_state_judge(g_position_circle_adjust, set_point_expect_position, 20, 100))//���Ⱥ󷢳�
			{
					*current_mode_flow_steps = mode_step_10;
			}
			break;
		}
		
		
		case mode_step_10:   //�߶����ת��
		{
			MODE=10;
			roll_offset=0;
			set_height_cascade_control_2(set_height_control_expect_height, set_height_control_expect_height/5, set_height_control_expect_height/6);
			
				locate_single_loop_pid_control(g_position_circle_adjust.x, g_position_circle_adjust.y);
			
			
			if(fabs(g_position_circle_adjust.x<100) && fabs(g_position_circle_adjust.y)<100)//��ת
			{
				if(yaw_expect_wave(90, &yaw_offset))
				{
					*current_mode_flow_steps = mode_step_11;
				}
			}
			
			
			break;
		}
		
		case mode_step_11:   //
		{
			MODE=11;
			set_height_cascade_control_2(set_height_control_expect_height, set_height_control_expect_height/5, set_height_control_expect_height/6);
			if( road_state_machine == straight_line)		
			{				
				locate_single_loop_pid_control(g_position_line_1_adjust.x, g_position_line_1_adjust.y );
				pitch_offset=-1.8;
				if(roll_offset>1.2)
					roll_offset=1.2;
				if(roll_offset<-1.2)
					roll_offset=-1.2;
			}
			else if(road_state_machine == line_connect_circle_connect_line ||road_state_machine == one_circle||road_state_machine == Three_line_cricle)
			{
				locate_single_loop_pid_control(g_position_line_1_adjust.x, g_position_circle_adjust.y);
				pitch_offset=-1.8;
//				if(roll_offset>1.2)
//					roll_offset=1.2;
//				if(roll_offset<-1.0)
//					roll_offset=-1.0;
			}
			else if(road_state_machine == juxingjiao)		//��⵽ת�ǽ�����һ�� 
			{
				*current_mode_flow_steps = mode_step_13;
			}
			break;
		}
		
//		case mode_step_12:   //
//		{
//			MODE=12;
//			set_height_cascade_control_2(set_height_control_expect_height, set_height_control_expect_height/5, set_height_control_expect_height/6);
//					
//			
//			
//			if( road_state_machine == straight_line)		
//			{
//				locate_single_loop_pid_control(g_position_line_1_adjust.x, (g_position_line_1_adjust.y + g_position_line_2_adjust.y) / 2.0f);
//				pitch_offset=-1.7;
////				if(roll_offset>1.2)
////					roll_offset=1.2;
//				if(roll_offset<-1.2)
//					roll_offset=-1.2;
//			}
//			else if(road_state_machine == line_connect_circle_connect_line ||road_state_machine == one_circle||road_state_machine == Three_line_cricle)
//			{
//				locate_single_loop_pid_control(g_position_line_1_adjust.x, g_position_circle_adjust.y);
//				pitch_offset=-1.7;
////				if(roll_offset>1.2)
////					roll_offset=1.2;
//				if(roll_offset<-1.2)
//					roll_offset=-1.2;
//			}
//			else if(road_state_machine == juxingjiao )		
//			{
//				*current_mode_flow_steps = mode_step_13;
//			}
//		
//			break;
//		}
		
		case mode_step_13:    //��3�����ν�
		{
			MODE=13;
			pitch_offset=0;
			if(yaw_expect_wave(90, &yaw_offset))//��ת
			{
				*current_mode_flow_steps = mode_step_14;
			}
			if(yaw_offset>-120&&yaw_offset<-90&&road_state_machine==straight_line)
			{
				locate_single_loop_pid_control(g_position_line_1_adjust.x, g_position_line_1_adjust.y);
				pitch_offset=-1;
//				if(roll_offset>1.0)
//					roll_offset=1.0;
//				if(roll_offset<-1.0)
//					roll_offset=-1.0;
			}
			
			set_height_cascade_control_2(set_height_control_expect_height, set_height_control_expect_height/5, set_height_control_expect_height/6);
			
//			if(g_height > 35)
//			{
//				locate_single_loop_pid_control(g_position_circle_adjust.x, g_position_circle_adjust.y);
//			}
//			else
			{
//				roll_offset = 0;
				pitch_offset = 0;
			}
			
			break;
		}
		
		case mode_step_14:   //
		{
			MODE=14;
			set_height_cascade_control_2(set_height_control_expect_height, set_height_control_expect_height/5, set_height_control_expect_height/6);
			
			pitch_offset=-1.6;
			
			if( road_state_machine == straight_line)
			{				
				locate_single_loop_pid_control(g_position_line_1_adjust.x, g_position_line_1_adjust.y);
				pitch_offset=-2.0;
//				if(roll_offset>1.2)
//					roll_offset=1.2;
//				if(roll_offset<-1.2)
//					roll_offset=-1.2;
			}
			
			else if(road_state_machine == line_connect_circle_connect_line ||road_state_machine == one_circle||road_state_machine == Three_line_cricle)
			{
				locate_single_loop_pid_control(g_position_line_1_adjust.x, g_position_circle_adjust.y);
				pitch_offset=-2.0;
//				if(roll_offset>1.2)
//					roll_offset=1.2;
//				if(roll_offset<-1.2)
//					roll_offset=-1.2;
			}
			
			else if(road_state_machine == juxingjiao )
			{
				*current_mode_flow_steps = mode_step_15;
			}
		
			break;
		}
//		case mode_step_13:    //��4�����ν� �����ڲ�ת��
//		{
//				MODE=13;
////			pitch_offset=0;
////			if(yaw_expect_wave(90, &yaw_offset))//��ת
////			{
//				*current_mode_flow_steps = mode_step_14;
////			}
////			if(yaw_offset>-30&&yaw_offset<0)
////			{
////				locate_single_loop_pid_control(g_position_line_1_adjust.x, g_position_line_1_adjust.y);
////			}
//			
//			set_height_cascade_control_2(set_height_control_expect_height, set_height_control_expect_height/5, set_height_control_expect_height/6);
//			
////			if(g_height > 35)
////			{
////				locate_single_loop_pid_control(g_position_circle_adjust.x, g_position_circle_adjust.y);
////			}
////			else
//			{
//				roll_offset = 0;
//				pitch_offset = 0;
//			}
//			
//			break;
//		}
		
		case mode_step_15:   //
		{
			MODE=15;
			pitch_offset=0;
			
			set_height_cascade_control_2(set_height_control_expect_height, set_height_control_expect_height/5, set_height_control_expect_height/6);
					
			
			
			set_height_cascade_control_2(set_height_control_expect_height, set_height_control_expect_height/5, set_height_control_expect_height/6);
			
			
			if( road_state_machine == Three_line_cricle )
			{
				*current_mode_flow_steps = mode_step_16;
			}
			else 
			{
				locate_single_loop_pid_control(g_position_line_1_adjust.x, g_position_circle_adjust.y);
				if(pitch_offset>0.8)
					pitch_offset=0.8;
				if(pitch_offset<-0.8)
					pitch_offset=-0.8;
				
				roll_offset=-1.0;
			}
		
		
			break;
		}
		
		case mode_step_16:
		{
			MODE=16;
			locate_single_loop_pid_control(g_position_circle_adjust.x, g_position_circle_adjust.y);
			set_height_cascade_control_2(set_height_control_expect_height, set_height_control_expect_height/5, set_height_control_expect_height/6);
				if(roll_offset<-1.7)
					roll_offset=-1.7;		
			//���յ�Բ�ϴﵽ��̬
			if(stady_state_judge(g_position_circle_adjust, set_point_expect_position, 15, 100))
			{
				*current_mode_flow_steps = mode_step_17;
			}

			break;
		}
		
		//����
		case mode_step_17:
		{
			MODE=17;
			*current_mode_flow_steps = mode_step_18;
			
			set_height_height_grab.flag = 0;	
			
			set_height_control_expect_height = -60;
			
			Suddenly_flag = 1;
			
			break;
		}
		
		case mode_step_18:
		{
			MODE=18;
			set_height_cascade_control_2(set_height_control_expect_height, 
										 (g_height-set_height_control_expect_height) * 0.1, 
										 (g_height-set_height_control_expect_height) * 0.1);


			
			if(g_height > 30)
			{
				locate_single_loop_pid_control(g_position_circle_adjust.x, g_position_circle_adjust.y);
				//�߶Ȳ���, ��������
			}
			else
			{
				//���䡢������������
				pitch_offset = 0;
				roll_offset = 0;
				locate_single_loop_pid_x.increment = 0;
				locate_single_loop_pid_y.increment = 0;	
				
				//ֱ�Ӽ�����, ���ٽ���
				set_height_throttle_grab.current_throttle -= 10;
				
				if(g_height < 20 && Suddenly_flag == 1)
				{
					set_height_throttle_grab.current_throttle -= 80;
					Suddenly_flag = 0;
				}
				
				if(set_height_throttle_grab.current_throttle <= 100)
				{
					set_height_throttle_grab.current_throttle = 100;
				}

			}

			if(g_height < 10)
			{
				*current_mode_flow_steps = mode_step_19;		
				
				ctrl.height.core.pid_out = 0;
				ctrl.height.shell.pid_out = 0;
				ctrl.height.shell.increment = 0;
				ctrl.height.core.increment = 0;
			
				ARMED = 0;
			}
			break;
		}
		
		case mode_step_19:
		{
			MODE=19;

/***********************************************************************************************************************************/
			//�ָ���ԭ���� PID
			// The data of pitch
			
/***********************************************************************************************************************************/	
			
			*current_mode_flow_steps = mode_step_none;
		
			mode_machine = mode_none;			
			
			break;
		}
		
		default:
		{
			
			break;
		}
	}
}


void A_to_B_control(mode_flow_step_type *current_mode_flow_steps)			//����������Բ״̬
{
	u8 Suddenly_flag = 1;
  
	float track_pitch_offset_max = 2.5f;
	
	if(g_height == 300) g_height =100;
	
	switch(*current_mode_flow_steps)
	{
		case mode_step_none:
		{
			MODE=0;
			*current_mode_flow_steps = mode_step_1;
			
			//��ƫ���ǵ�����ֵ�������˲���yawֵ, �����Ͳ��ᵼ���ϵ�����ᱻ������ת����ɵ���ʧ��
			yaw_offset = angle.yaw;
			
			set_height_height_grab.flag = 0;	
			set_height_throttle_grab.flag = 0;
			set_height_throttle_grab.current_throttle = 0 ;
			fly_direction = forward;
			magnet_ON;
		
			break;
		}
		
		case mode_step_1:
		{
			MODE=1;
			//�ȴ�8s
			if(wait_for_n_ms(&system_time_tick, 1000, &x_times_call_wait_for_n_ms_func))
			{
				*current_mode_flow_steps = mode_step_2;
				increment_set_to_zero();
			}
			
			break;
		}
		
		case mode_step_2:
		{
			MODE=2;
//			set_height_control_expect_height = delivery_param_to_control.expect_height_start_circle;
			set_height_control_expect_height =90;
			set_height_cascade_control_2(set_height_control_expect_height, set_height_control_expect_height/5, set_height_control_expect_height/6);
			
	
			if(g_height > 30)
			{
				locate_single_loop_pid_control(g_position_circle_adjust.x, g_position_circle_adjust.y);
				
				if(stady_state_judge(g_position_circle_adjust, set_point_expect_position, 3, 80))
				{
					*current_mode_flow_steps = mode_step_3;
					
					//ȡyaw����ֵ
//					yaw_offset_tmp = get_yaw_expect(g_position_circle_adjust.x, g_position_circle_adjust.y, 
//													g_position_line_1_adjust.x, g_position_line_1_adjust.y);
				}
				
			}
			else
			{
				roll_offset = 0;
				pitch_offset = 0;
			}
			
			break;
		}
		
		case mode_step_3:
		{
			MODE=3;
//			if(yaw_expect_wave(yaw_offset_tmp, &yaw_offset))
			{
				*current_mode_flow_steps = mode_step_4;
			}
			
			set_height_cascade_control_2(set_height_control_expect_height, set_height_control_expect_height/5, set_height_control_expect_height/6);
			
			if(g_height > 30)
			{
				locate_single_loop_pid_control(g_position_circle_adjust.x, g_position_circle_adjust.y);
			}
			else
			{
				roll_offset = 0;
				pitch_offset = 0;
			}
			
			break;
		}
		
		case mode_step_4:
		{
			MODE=4;
			set_height_cascade_control_2(set_height_control_expect_height, set_height_control_expect_height/5, set_height_control_expect_height/6);
			
			if(g_height > 30)
			{
				locate_single_loop_pid_control(g_position_circle_adjust.x, g_position_circle_adjust.y);
			}
			else
			{
				roll_offset = 0;
				pitch_offset = 0;
			}
			
			//����ѭ����
//			if(g_height > set_height_control_expect_height - 5 && g_height < set_height_control_expect_height + 5)
//			{
//				*current_mode_flow_steps = mode_step_5;
//			}
			if(stady_state_judge(g_position_circle_adjust, set_point_expect_position, 7, 80))//���Ⱥ󷢳�
			{
					*current_mode_flow_steps = mode_step_5;
			}
				
			
			break;
		}
		
		//ѭ��, ֻ�������ĵ㲻һ��
		case mode_step_5:
		{
			MODE=5;
			set_height_cascade_control_2(set_height_control_expect_height, set_height_control_expect_height/5, set_height_control_expect_height/6);
			
			if(fly_direction == forward)
			{
				//���������¼���·��״̬
				if(road_state_machine == Three_line_cricle && wait_for_n_ms(&system_time_tick, 600, &x_times_call_wait_for_n_ms_func))
				{
					//�Ѿ������յ�
					locate_single_loop_pid_control(g_position_circle_adjust.x, g_position_circle_adjust.y);
				
//						if(g_position_circle_adjust.x <100 && g_position_circle_adjust.x > -100 
//						&& g_position_circle_adjust.y <100 && g_position_circle_adjust.y > -100)
//						{
//					//������һ�� 
								{
									*current_mode_flow_steps = mode_step_6;
									locate_single_loop_pid_x.increment = 1000;
								}
//								locate_single_loop_pid_y.increment = 0;
//						}
				}
				else if(road_state_machine == straight_line )
				{
				
					locate_single_loop_pid_control(g_position_line_1_adjust.x, g_position_line_1_adjust.y);
					pitch_offset=-2.7;
//					if(roll_offset>1.2)
//						roll_offset=1.2;
//					if(roll_offset<-1.2)
//						roll_offset=-1.2;
				}
				else if(road_state_machine == one_circle||road_state_machine == Three_line_cricle)
				{
					locate_single_loop_pid_control(g_position_circle_adjust.x, g_position_circle_adjust.y);				
					pitch_offset=-2.7;
//					if(roll_offset>1.2)
//						roll_offset=1.2;
//					if(roll_offset<-1.2)
//						roll_offset=-1.2;
				}

	
				
			}
			else if(fly_direction == backward)
			{
				//���������¼���·��״̬
				if(road_state_machine == line_connect_circle)
				{
					//��ͷ�����ǳ���η�, ����λ line_2.end, ������F405���Ѿ��������
					locate_single_loop_pid_control(g_position_line_2_adjust.x, g_position_line_2_adjust.y);
				}
				else if(road_state_machine == straight_line || road_state_machine == two_stripes || road_state_machine == one_stripes)
				{				
					locate_single_loop_pid_control(g_position_line_2_adjust.x, (g_position_line_1_adjust.y + g_position_line_2_adjust.y) / 2.0f);
				}
				else if(road_state_machine == circle_connect_line)
				{
					//�Ѿ������յ�
					locate_single_loop_pid_control(g_position_circle_adjust.x, g_position_circle_adjust.y);
					
					
					if(g_position_circle_adjust.x <100 && g_position_circle_adjust.x > -100 
						&& g_position_circle_adjust.y <100 && g_position_circle_adjust.y > -100)
					{
					//������һ�� 
						*current_mode_flow_steps = mode_step_6;
						locate_single_loop_pid_x.increment = 0;
//						locate_single_loop_pid_y.increment = 0;
					}
					

				}
				else
				{
					//���������е�·��״̬, ��������
				}
	
			}
			else
			{
				//fuck
			}		

			//������һ���ܵ�ʱ��, ����
			if(road_state_machine == one_stripes)
			{
				track_pitch_offset_max = 1.8f;
			}
			
/**********************************************************************************************************************/
			//
			//ʵ��������ܵ��ڽ�Բ��ʱ��ǳ���, ����Ч��������
			//
			
			//����������޷�, ��ֹ�ٶȹ���
			track_output_limit(track_pitch_offset_max, &pitch_offset);
			
			break; 
		}		
		
		case mode_step_6:
		{
//			if((road_state_machine == line_connect_circle_connect_line))
//			{
//				
//				*current_mode_flow_steps = mode_step_5;
//			}
		
			MODE=6;
			locate_single_loop_pid_control(g_position_circle_adjust.x, g_position_circle_adjust.y);
			set_height_cascade_control_2(set_height_control_expect_height, set_height_control_expect_height/5, set_height_control_expect_height/6);
			magnet_OFF;//������
			//���յ�Բ�ϴﵽ��̬, ����LED3, ����ȴ���
			if(stady_state_judge(g_position_circle_adjust, set_point_expect_position, 20, 100))
			{
				*current_mode_flow_steps = mode_step_7;
				
				magnet_OFF;
//				set_height_throttle_grab.current_throttle -= 20;
			}
			
				
			

			break;
			
		}		
		
		case mode_step_7:
		{
			MODE=7;
			locate_single_loop_pid_control(g_position_circle_adjust.x, g_position_circle_adjust.y);
			set_height_cascade_control_2(set_height_control_expect_height, set_height_control_expect_height/5, set_height_control_expect_height/6);
			
			//LED3��3s, ���뷵�ز�
//			if(wait_for_n_ms(&system_time_tick, 300, &x_times_call_wait_for_n_ms_func))
			{
				*current_mode_flow_steps = mode_step_9;
				
				LED3(OFF);
				
				fly_direction = (enum _direction)(1 - fly_direction);
				
				track_pitch_offset_max = 1.7f;
			}

			break;
			
		}		
		
//		//����ѭ��
//		case mode_step_8:
//		{
//			set_height_cascade_control_2(set_height_control_expect_height, set_height_control_expect_height/5, set_height_control_expect_height/6);
//			
//			if(fly_direction == forward)
//			{
//				//���������¼���·��״̬
//				if(road_state_machine == circle_connect_line)
//				{
//					//��ͷ�����ǳ���η�, ����λ line_2.end, ������F405���Ѿ��������
//					locate_single_loop_pid_control(g_position_line_2_adjust.x, g_position_line_2_adjust.y);					
//				}
//				else if(road_state_machine == straight_line || road_state_machine == two_stripes || road_state_machine == one_stripes)
//				{						
//					locate_single_loop_pid_control(g_position_line_1_adjust.x, (g_position_line_1_adjust.y + g_position_line_2_adjust.y) / 2.0f);
//				}
//				else if(road_state_machine == line_connect_circle)
//				{
//					//�Ѿ������յ�
//					locate_single_loop_pid_control(g_position_circle_adjust.x, g_position_circle_adjust.y);
//					
//					if(g_position_circle_adjust.x <100 && g_position_circle_adjust.x > -100 
//					&& g_position_circle_adjust.y <100 && g_position_circle_adjust.y > -100)
//					{
//					//������һ�� 
//					*current_mode_flow_steps = mode_step_9;
//						locate_single_loop_pid_x.increment = 0;
////						locate_single_loop_pid_y.increment = 0;
//					}
//				}
//				else
//				{
//					//���������е�·��״̬, ��������
//				}
//				
//			}
//			else if(fly_direction == backward)
//			{
//							
//				//���������¼���·��״̬
//				if(road_state_machine == line_connect_circle)
//				{
//					if(g_position_circle_adjust.x <100 && g_position_circle_adjust.x > -100) 
//					{
//						locate_single_loop_pid_control(g_position_line_2_adjust.x, g_position_circle_adjust.y);
//						
//					}
//					//��ͷ�����ǳ���η�, ����λ line_2.end, ������F405���Ѿ��������
//					locate_single_loop_pid_control(g_position_line_2_adjust.x, g_position_line_2_adjust.y);
//				}
//				else if(road_state_machine == straight_line || road_state_machine == two_stripes || road_state_machine == one_stripes)
//				{					
//					locate_single_loop_pid_control(g_position_line_2_adjust.x, (g_position_line_1_adjust.y + g_position_line_2_adjust.y) / 2.0f);
//				}
//				else if(road_state_machine == circle_connect_line)
//				{
//					//�Ѿ������յ�
//					locate_single_loop_pid_control(g_position_circle_adjust.x, g_position_circle_adjust.y);
//					
//					//������һ�� 
////					if(g_position_circle_adjust.x <100 && g_position_circle_adjust.x > -100
////					&& g_position_circle_adjust.y <100 && g_position_circle_adjust.y > -100)
////					{
//					//������һ�� 
//					*current_mode_flow_steps = mode_step_9;
//						locate_single_loop_pid_x.increment = -1000;
////						locate_single_loop_pid_y.increment = 0;
////					}

//				}
//				else
//				{
//					//���������е�·��״̬, ��������
//				}
//	
//			}
//			else
//			{
//				//fuck
//			}		

//			//������һ���ܵ�ʱ��, ����
//			if(road_state_machine == two_stripes)
//			{
//				track_pitch_offset_max = 1.7f;
//			}
//			
///**********************************************************************************************************************/
//			//
//			//ʵ��������ܵ��ڽ�Բ��ʱ��ǳ���, ����Ч��������
//			//
//			
//			//����������޷�, ��ֹ�ٶȹ���
//			
//			track_output_limit(track_pitch_offset_max, &pitch_offset);
//			
//			break; 
//		}		
//		
		//���յ�Բ�Ϸ�, ������̬����Խ���
		case mode_step_9:
		{
			locate_single_loop_pid_control(g_position_circle_adjust.x, g_position_circle_adjust.y);
			set_height_cascade_control_2(set_height_control_expect_height, set_height_control_expect_height/5, set_height_control_expect_height/6);
			
			//���յ�Բ�ϴﵽ��̬
			if(stady_state_judge(g_position_circle_adjust, set_point_expect_position, 10, 80))
			{
				*current_mode_flow_steps = mode_step_10;
			}

			break;
		}		
		
		//����
		case mode_step_10:
		{
			MODE=10;
			*current_mode_flow_steps = mode_step_11;
			
			set_height_height_grab.flag = 0;	
			
			set_height_control_expect_height = -60;   //
			
			Suddenly_flag = 1;
			
			break;
		}		
		
		case mode_step_11:
		{
			MODE=11;
			set_height_cascade_control_2(set_height_control_expect_height, 
										 (g_height-set_height_control_expect_height) * 0.1, 
										 (g_height-set_height_control_expect_height) * 0.1);


			
			if(g_height > 30)
			{
				locate_single_loop_pid_control(g_position_circle_adjust.x, g_position_circle_adjust.y);
				//�߶Ȳ���, ��������
			}
			else
			{
				//���䡢������������
				pitch_offset = 0;
				roll_offset = 0;
				locate_single_loop_pid_x.increment = 0;
				locate_single_loop_pid_y.increment = 0;	
				
				//ֱ�Ӽ�����, ���ٽ���
				set_height_throttle_grab.current_throttle -= 10;
				
				if(g_height < 20 && Suddenly_flag == 1)
				{
					set_height_throttle_grab.current_throttle -= 50;
					Suddenly_flag = 0;
				}
				
				if(set_height_throttle_grab.current_throttle <= 100)
				{
					set_height_throttle_grab.current_throttle = 100;
				}

			}

			if(g_height < 10)
			{
				*current_mode_flow_steps = mode_step_12;		
				magnet_ON;     //�ؼ���
				ctrl.height.core.pid_out = 0;
				ctrl.height.shell.pid_out = 0;
				ctrl.height.shell.increment = 0;
				ctrl.height.core.increment = 0;
			
				ARMED = 0;
			}
			break;
		}
		
		case mode_step_12:
		{
			MODE=12;
			*current_mode_flow_steps = mode_step_none;
		
			mode_machine = mode_none;			
			
			break;
		}
		
		default:
		{
			
			break;
		}
	}
	

}

void A_C_B_control(mode_flow_step_type *current_mode_flow_steps)
{
	u8 Suddenly_flag = 1;
  
	float track_pitch_offset_max = 2.5f;
	
	if(g_height == 300) g_height =110;
	
	switch(*current_mode_flow_steps)
	{
		case mode_step_none:
		{
			MODE=0;
			*current_mode_flow_steps = mode_step_1;
			
			//��ƫ���ǵ�����ֵ�������˲���yawֵ, �����Ͳ��ᵼ���ϵ�����ᱻ������ת����ɵ���ʧ��
			yaw_offset = angle.yaw;
			
			set_height_height_grab.flag = 0;	
			set_height_throttle_grab.flag = 0;
			set_height_throttle_grab.current_throttle = 0 ;
			fly_direction = forward;
			magnet_ON;
		
			break;
		}
		
		case mode_step_1:
		{
			MODE=1;
			//�ȴ�8s
			if(wait_for_n_ms(&system_time_tick, 1000, &x_times_call_wait_for_n_ms_func))
			{
				*current_mode_flow_steps = mode_step_2;
				increment_set_to_zero();
			}
			
			break;
		}
		
		case mode_step_2:
		{
			MODE=2;
//			set_height_control_expect_height = delivery_param_to_control.expect_height_start_circle;
			set_height_control_expect_height =110;
			set_height_cascade_control_2(set_height_control_expect_height, set_height_control_expect_height/5, set_height_control_expect_height/6);
			
	
			if(g_height > 30)
			{
				locate_single_loop_pid_control(g_position_circle_adjust.x, g_position_circle_adjust.y);
				
				if(stady_state_judge(g_position_circle_adjust, set_point_expect_position, 3, 80))
				{
					*current_mode_flow_steps = mode_step_3;
					
					//ȡyaw����ֵ
//					yaw_offset_tmp = get_yaw_expect(g_position_circle_adjust.x, g_position_circle_adjust.y, 
//													g_position_line_1_adjust.x, g_position_line_1_adjust.y);
				}
				
			}
			else
			{
				roll_offset = 0;
				pitch_offset = 0;
			}
			
			break;
		}
		
		case mode_step_3:
		{
			MODE=3;
//			if(yaw_expect_wave(yaw_offset_tmp, &yaw_offset))
			{
				*current_mode_flow_steps = mode_step_4;
			}
			
			set_height_cascade_control_2(set_height_control_expect_height, set_height_control_expect_height/5, set_height_control_expect_height/6);
			
			if(g_height > 30)
			{
				locate_single_loop_pid_control(g_position_circle_adjust.x, g_position_circle_adjust.y);
			}
			else
			{
				roll_offset = 0;
				pitch_offset = 0;
			}
			
			break;
		}
		
		case mode_step_4:
		{
			MODE=4;
			set_height_cascade_control_2(set_height_control_expect_height, set_height_control_expect_height/5, set_height_control_expect_height/6);
			
			if(g_height > 30)
			{
				locate_single_loop_pid_control(g_position_circle_adjust.x, g_position_circle_adjust.y);
			}
			else
			{
				roll_offset = 0;
				pitch_offset = 0;
			}
			
			//����ѭ����
//			if(g_height > set_height_control_expect_height - 5 && g_height < set_height_control_expect_height + 5)
//			{
//				*current_mode_flow_steps = mode_step_5;
//			}
			if(stady_state_judge(g_position_circle_adjust, set_point_expect_position, 7, 80))//���Ⱥ󷢳�
			{
					*current_mode_flow_steps = mode_step_5;
			}
				
			
			break;
		}
		
		//ѭ��, ֻ�������ĵ㲻һ��
		case mode_step_5:
		{
			MODE=5;
			set_height_cascade_control_2(set_height_control_expect_height, set_height_control_expect_height/5, set_height_control_expect_height/6);
			
			if(fly_direction == forward)
			{
				//���������¼���·��״̬
				if(road_state_machine == circle_connect_line)
				{
					//��ͷ�����ǳ���η�, ����λ line_2.end, ������F405���Ѿ��������
					locate_single_loop_pid_control(g_position_line_2_adjust.x, g_position_circle_adjust.y);		
					pitch_offset=-2.2;
//					if(roll_offset>2.2)
//						roll_offset=2.2;
//					if(roll_offset<-2.2)
//						roll_offset=-2.2;
				}
				else if(road_state_machine == straight_line )
				{
				
					locate_single_loop_pid_control(g_position_line_1_adjust.x, g_position_line_1_adjust.y );
					pitch_offset=-2.2;
//					if(roll_offset>2.2)
//						roll_offset=2.2;
//					if(roll_offset<-2.2)
//						roll_offset=-2.2;
				}
				else if(road_state_machine == one_circle)
				{
					locate_single_loop_pid_control(g_position_circle_adjust.x, g_position_circle_adjust.y);				
					pitch_offset=-2.2;
//					if(roll_offset>2.2)
//						roll_offset=2.2;
//					if(roll_offset<-2.2)
//						roll_offset=-2.2;
				}
				else if(road_state_machine == line_connect_circle_connect_line)//�����м��C�� ��Բ��
				{

					
					//�Ѿ������м��C��
					locate_single_loop_pid_control(g_position_circle_adjust.x, g_position_circle_adjust.y);
				
//						if(g_position_circle_adjust.x <100 && g_position_circle_adjust.x > -100 
//						&& g_position_circle_adjust.y <100 && g_position_circle_adjust.y > -100)
//						{
//					//������һ�� 
								{
									*current_mode_flow_steps = mode_step_6;
									locate_single_loop_pid_x.increment = 1000;
								}
//								locate_single_loop_pid_y.increment = 0;
//						}
					

				}
	
				
			}
			else if(fly_direction == backward)
			{
				//���������¼���·��״̬
				if(road_state_machine == line_connect_circle)
				{
					//��ͷ�����ǳ���η�, ����λ line_2.end, ������F405���Ѿ��������
					locate_single_loop_pid_control(g_position_line_2_adjust.x, g_position_line_2_adjust.y);
				}
				else if(road_state_machine == straight_line || road_state_machine == two_stripes || road_state_machine == one_stripes)
				{				
					locate_single_loop_pid_control(g_position_line_2_adjust.x, (g_position_line_1_adjust.y + g_position_line_2_adjust.y) / 2.0f);
				}
				else if(road_state_machine == circle_connect_line)
				{
					//�Ѿ������յ�
					locate_single_loop_pid_control(g_position_circle_adjust.x, g_position_circle_adjust.y);
					
					
					if(g_position_circle_adjust.x <100 && g_position_circle_adjust.x > -100 
						&& g_position_circle_adjust.y <100 && g_position_circle_adjust.y > -100)
					{
					//������һ�� 
						*current_mode_flow_steps = mode_step_6;
						locate_single_loop_pid_x.increment = 0;
//						locate_single_loop_pid_y.increment = 0;
					}
					

				}
				else
				{
					//���������е�·��״̬, ��������
				}
	
			}
			else
			{
				//fuck
			}		


			

			
			//����������޷�, ��ֹ�ٶȹ���
			track_output_limit(track_pitch_offset_max, &pitch_offset);
			
			break; 
		}		
		
		case mode_step_6:
		{
			MODE=6;
			locate_single_loop_pid_control(g_position_circle_adjust.x, g_position_circle_adjust.y);
			set_height_cascade_control_2(set_height_control_expect_height, set_height_control_expect_height/5, set_height_control_expect_height/6);
			
			if(road_state_machine == straight_line )
			{
				*current_mode_flow_steps = mode_step_5;
					
			}
			//���յ�Բ�ϴﵽ��̬, ����ת��
			if(stady_state_judge(g_position_circle_adjust, set_point_expect_position, 10, 100))
			{
				*current_mode_flow_steps = mode_step_7;
				
				magnet_OFF;
//				set_height_throttle_grab.current_throttle -= 20;
			}
			
				
			

			break;
			
		}
		
		case mode_step_7:
		{
			MODE=7;
			locate_single_loop_pid_control(g_position_circle_adjust.x, g_position_circle_adjust.y);
			set_height_cascade_control_2(set_height_control_expect_height, set_height_control_expect_height/5, set_height_control_expect_height/6);
			
			//ת�� ָ��B��
			if(fabs(g_position_circle_adjust.x)<100&&fabs(g_position_circle_adjust.y)<100)
			{
				if(yaw_expect_wave(-85, &yaw_offset))
				{
					*current_mode_flow_steps = mode_step_8;//ת�귽��������һ��
					fly_direction = forward;
				}
			}


			break;
			
		}		
		
		//ѭ��, ֻ�������ĵ㲻һ��
		case mode_step_8:
		{
			MODE=5;
			set_height_cascade_control_2(set_height_control_expect_height, set_height_control_expect_height/5, set_height_control_expect_height/6);
			
			if(fly_direction == forward)
			{
				//���������¼���·��״̬
				if(road_state_machine == line_connect_circle_connect_line || road_state_machine == one_circle)
				{
					//��ͷ�����ǳ���η�, ����λ line_2.end, ������F405���Ѿ��������
					locate_single_loop_pid_control(g_position_line_2_adjust.x, g_position_circle_adjust.y);		
					pitch_offset=-2.2;
//					if(roll_offset>2.2)
//						roll_offset=2.2;
//					if(roll_offset<-2.2)
//						roll_offset=-2.2;
				}
				else if(road_state_machine == straight_line )
				{
				
					locate_single_loop_pid_control(g_position_line_1_adjust.x, g_position_line_1_adjust.y);
					pitch_offset=-2.2;
//					if(roll_offset>1.2)
//						roll_offset=1.2;
//					if(roll_offset<-1.2)
//						roll_offset=-1.2;
				}
	
				else if(road_state_machine == line_connect_circle)//����B�� ����Բ
				{

					
					//�Ѿ������յ�
					locate_single_loop_pid_control(g_position_circle_adjust.x, g_position_circle_adjust.y);
				
//						if(g_position_circle_adjust.x <100 && g_position_circle_adjust.x > -100 
//						&& g_position_circle_adjust.y <100 && g_position_circle_adjust.y > -100)
//						{
//					//������һ�� 
								{
									*current_mode_flow_steps = mode_step_9;
									locate_single_loop_pid_x.increment = 1000;
								}
//								locate_single_loop_pid_y.increment = 0;
//						}
					

				}
	
				
			}
			else if(fly_direction == backward)
			{
				//���������¼���·��״̬
				if(road_state_machine == line_connect_circle)
				{
					//��ͷ�����ǳ���η�, ����λ line_2.end, ������F405���Ѿ��������
					locate_single_loop_pid_control(g_position_line_2_adjust.x, g_position_line_2_adjust.y);
				}
				else if(road_state_machine == straight_line || road_state_machine == two_stripes || road_state_machine == one_stripes)
				{				
					locate_single_loop_pid_control(g_position_line_2_adjust.x, (g_position_line_1_adjust.y + g_position_line_2_adjust.y) / 2.0f);
				}
				else if(road_state_machine == circle_connect_line)
				{
					//�Ѿ������յ�
					locate_single_loop_pid_control(g_position_circle_adjust.x, g_position_circle_adjust.y);
					
					
					if(g_position_circle_adjust.x <100 && g_position_circle_adjust.x > -100 
						&& g_position_circle_adjust.y <100 && g_position_circle_adjust.y > -100)
					{
					//������һ�� 
						*current_mode_flow_steps = mode_step_6;
						locate_single_loop_pid_x.increment = 0;
//						locate_single_loop_pid_y.increment = 0;
					}
					

				}
				else
				{
					//���������е�·��״̬, ��������
				}
	
			}
			else
			{
				//fuck
			}		


			

			
			//����������޷�, ��ֹ�ٶȹ���
			track_output_limit(track_pitch_offset_max, &pitch_offset);
			
			break; 
		}	
		case mode_step_9:
		{
			MODE=7;
			locate_single_loop_pid_control(g_position_circle_adjust.x, g_position_circle_adjust.y);
			set_height_cascade_control_2(set_height_control_expect_height, set_height_control_expect_height/5, set_height_control_expect_height/6);
			
			//LED3��3s, ���뷵�ز�
//			if(wait_for_n_ms(&system_time_tick, 300, &x_times_call_wait_for_n_ms_func))
			{
				*current_mode_flow_steps = mode_step_10;
				
//				LED3(OFF);
//				
//				fly_direction = (enum _direction)(1 - fly_direction);
//				
//				track_pitch_offset_max = 1.7f;
			}

			break;
			
		}		
		
//		//����ѭ��
//		case mode_step_8:
//		{
//			set_height_cascade_control_2(set_height_control_expect_height, set_height_control_expect_height/5, set_height_control_expect_height/6);
//			
//			if(fly_direction == forward)
//			{
//				//���������¼���·��״̬
//				if(road_state_machine == circle_connect_line)
//				{
//					//��ͷ�����ǳ���η�, ����λ line_2.end, ������F405���Ѿ��������
//					locate_single_loop_pid_control(g_position_line_2_adjust.x, g_position_line_2_adjust.y);					
//				}
//				else if(road_state_machine == straight_line || road_state_machine == two_stripes || road_state_machine == one_stripes)
//				{						
//					locate_single_loop_pid_control(g_position_line_1_adjust.x, (g_position_line_1_adjust.y + g_position_line_2_adjust.y) / 2.0f);
//				}
//				else if(road_state_machine == line_connect_circle)
//				{
//					//�Ѿ������յ�
//					locate_single_loop_pid_control(g_position_circle_adjust.x, g_position_circle_adjust.y);
//					
//					if(g_position_circle_adjust.x <100 && g_position_circle_adjust.x > -100 
//					&& g_position_circle_adjust.y <100 && g_position_circle_adjust.y > -100)
//					{
//					//������һ�� 
//					*current_mode_flow_steps = mode_step_9;
//						locate_single_loop_pid_x.increment = 0;
////						locate_single_loop_pid_y.increment = 0;
//					}
//				}
//				else
//				{
//					//���������е�·��״̬, ��������
//				}
//				
//			}
//			else if(fly_direction == backward)
//			{
//							
//				//���������¼���·��״̬
//				if(road_state_machine == line_connect_circle)
//				{
//					if(g_position_circle_adjust.x <100 && g_position_circle_adjust.x > -100) 
//					{
//						locate_single_loop_pid_control(g_position_line_2_adjust.x, g_position_circle_adjust.y);
//						
//					}
//					//��ͷ�����ǳ���η�, ����λ line_2.end, ������F405���Ѿ��������
//					locate_single_loop_pid_control(g_position_line_2_adjust.x, g_position_line_2_adjust.y);
//				}
//				else if(road_state_machine == straight_line || road_state_machine == two_stripes || road_state_machine == one_stripes)
//				{					
//					locate_single_loop_pid_control(g_position_line_2_adjust.x, (g_position_line_1_adjust.y + g_position_line_2_adjust.y) / 2.0f);
//				}
//				else if(road_state_machine == circle_connect_line)
//				{
//					//�Ѿ������յ�
//					locate_single_loop_pid_control(g_position_circle_adjust.x, g_position_circle_adjust.y);
//					
//					//������һ�� 
////					if(g_position_circle_adjust.x <100 && g_position_circle_adjust.x > -100
////					&& g_position_circle_adjust.y <100 && g_position_circle_adjust.y > -100)
////					{
//					//������һ�� 
//					*current_mode_flow_steps = mode_step_9;
//						locate_single_loop_pid_x.increment = -1000;
////						locate_single_loop_pid_y.increment = 0;
////					}

//				}
//				else
//				{
//					//���������е�·��״̬, ��������
//				}
//	
//			}
//			else
//			{
//				//fuck
//			}		

//			//������һ���ܵ�ʱ��, ����
//			if(road_state_machine == two_stripes)
//			{
//				track_pitch_offset_max = 1.7f;
//			}
//			
///**********************************************************************************************************************/
//			//
//			//ʵ��������ܵ��ڽ�Բ��ʱ��ǳ���, ����Ч��������
//			//
//			
//			//����������޷�, ��ֹ�ٶȹ���
//			
//			track_output_limit(track_pitch_offset_max, &pitch_offset);
//			
//			break; 
//		}		
//		
		//���յ�Բ�Ϸ�, ������̬����Խ���
		case mode_step_10:
		{
			locate_single_loop_pid_control(g_position_circle_adjust.x, g_position_circle_adjust.y);
			set_height_cascade_control_2(set_height_control_expect_height, set_height_control_expect_height/5, set_height_control_expect_height/6);
			
			//���յ�Բ�ϴﵽ��̬
			if(stady_state_judge(g_position_circle_adjust, set_point_expect_position, 10, 80))
			{
				*current_mode_flow_steps = mode_step_11;
			}

			break;
		}		
		
		//����
		case mode_step_11:
		{
			MODE=10;
			*current_mode_flow_steps = mode_step_12;
			
			set_height_height_grab.flag = 0;	
			
			set_height_control_expect_height = -60;   //
			
			Suddenly_flag = 1;
			
			break;
		}		
		
		case mode_step_12:
		{
			MODE=11;
			set_height_cascade_control_2(set_height_control_expect_height, 
										 (g_height-set_height_control_expect_height) * 0.1, 
										 (g_height-set_height_control_expect_height) * 0.1);


			
			if(g_height > 30)
			{
				locate_single_loop_pid_control(g_position_circle_adjust.x, g_position_circle_adjust.y);
				//�߶Ȳ���, ��������
			}
			else
			{
				//���䡢������������
				pitch_offset = 0;
				roll_offset = 0;
				locate_single_loop_pid_x.increment = 0;
				locate_single_loop_pid_y.increment = 0;	
				
				//ֱ�Ӽ�����, ���ٽ���
				set_height_throttle_grab.current_throttle -= 10;
				
				if(g_height < 20 && Suddenly_flag == 1)
				{
					set_height_throttle_grab.current_throttle -= 50;
					Suddenly_flag = 0;
				}
				
				if(set_height_throttle_grab.current_throttle <= 100)
				{
					set_height_throttle_grab.current_throttle = 100;
				}

			}

			if(g_height < 10)
			{
				*current_mode_flow_steps = mode_step_13;		
				
				ctrl.height.core.pid_out = 0;
				ctrl.height.shell.pid_out = 0;
				ctrl.height.shell.increment = 0;
				ctrl.height.core.increment = 0;
			
				ARMED = 0;
			}
			break;
		}
		
		case mode_step_13:
		{
			MODE=12;
			*current_mode_flow_steps = mode_step_none;
		
			mode_machine = mode_none;			
			
			break;
		}
		
		default:
		{
			
			break;
		}
	}
	

}


void FIND_LCUK_control(mode_flow_step_type *current_mode_flow_steps)
{	
	u8 Suddenly_flag = 1;
  
	float track_pitch_offset_max = 2.5f;
	
	if(g_height == 300) g_height =100;
	
	switch(*current_mode_flow_steps)
	{
		case mode_step_none:
		{
			MODE=0;
			*current_mode_flow_steps = mode_step_1;
			
			//��ƫ���ǵ�����ֵ�������˲���yawֵ, �����Ͳ��ᵼ���ϵ�����ᱻ������ת����ɵ���ʧ��
			yaw_offset = angle.yaw;
			
			set_height_height_grab.flag = 0;	
			set_height_throttle_grab.flag = 0;
			set_height_throttle_grab.current_throttle = 0 ;
			fly_direction = forward;
			magnet_ON;
		
			break;
		}
		
		case mode_step_1:
		{
			MODE=1;
			//�ȴ�8s
			if(wait_for_n_ms(&system_time_tick, 1000, &x_times_call_wait_for_n_ms_func))
			{
				*current_mode_flow_steps = mode_step_2;
				increment_set_to_zero();
			}
			
			break;
		}
		
		case mode_step_2:
		{
			MODE=2;
//			set_height_control_expect_height = delivery_param_to_control.expect_height_start_circle;
			set_height_control_expect_height =90;
			set_height_cascade_control_2(set_height_control_expect_height, set_height_control_expect_height/5, set_height_control_expect_height/6);
			
	
			if(g_height > 30)
			{
				locate_single_loop_pid_control(g_position_circle_adjust.x, g_position_circle_adjust.y);
				
				if(stady_state_judge(g_position_circle_adjust, set_point_expect_position, 3, 80))
				{
					*current_mode_flow_steps = mode_step_3;
					
					//ȡyaw����ֵ
//					yaw_offset_tmp = get_yaw_expect(g_position_circle_adjust.x, g_position_circle_adjust.y, 
//													g_position_line_1_adjust.x, g_position_line_1_adjust.y);
				}
				
			}
			else
			{
				roll_offset = 0;
				pitch_offset = 0;
			}
			
			break;
		}
		
		case mode_step_3:
		{
			MODE=3;
//			if(yaw_expect_wave(yaw_offset_tmp, &yaw_offset))
			{
				*current_mode_flow_steps = mode_step_4;
			}
			
			set_height_cascade_control_2(set_height_control_expect_height, set_height_control_expect_height/5, set_height_control_expect_height/6);
			
			if(g_height > 30)
			{
				locate_single_loop_pid_control(g_position_circle_adjust.x, g_position_circle_adjust.y);
			}
			else
			{
				roll_offset = 0;
				pitch_offset = 0;
			}
			
			break;
		}
		
		case mode_step_4:
		{
			MODE=4;
			set_height_cascade_control_2(set_height_control_expect_height, set_height_control_expect_height/5, set_height_control_expect_height/6);
			
			if(g_height > 30)
			{
				locate_single_loop_pid_control(g_position_circle_adjust.x, g_position_circle_adjust.y);
			}
			else
			{
				roll_offset = 0;
				pitch_offset = 0;
			}
			
			//����ѭ����
//			if(g_height > set_height_control_expect_height - 5 && g_height < set_height_control_expect_height + 5)
//			{
//				*current_mode_flow_steps = mode_step_5;
//			}
			if(stady_state_judge(g_position_circle_adjust, set_point_expect_position, 7, 80))//���Ⱥ󷢳�
			{
					*current_mode_flow_steps = mode_step_5;
			}
				
			
			break;
		}
		
		//ѭ��, ֻ�������ĵ㲻һ��
		case mode_step_5:
		{
			MODE=5;
			set_height_cascade_control_2(set_height_control_expect_height, set_height_control_expect_height/5, set_height_control_expect_height/6);
			
			if(fly_direction == forward)
			{
				//���������¼���·��״̬
				if(road_state_machine == circle_connect_line)
				{
					//��ͷ�����ǳ���η�, ����λ line_2.end, ������F405���Ѿ��������
					locate_single_loop_pid_control(g_position_line_2_adjust.x, g_position_circle_adjust.y);		
					pitch_offset=-2.7;
//					if(roll_offset>1.2)
//						roll_offset=1.2;
//					if(roll_offset<-1.2)
//						roll_offset=-1.2;
				}
				else if(road_state_machine == straight_line )
				{
				
					locate_single_loop_pid_control(g_position_line_1_adjust.x, (g_position_line_1_adjust.y + g_position_line_2_adjust.y) / 2.0f);
					pitch_offset=-2.7;
//					if(roll_offset>1.2)
//						roll_offset=1.2;
//					if(roll_offset<-1.2)
//						roll_offset=-1.2;
				}
				else if(road_state_machine == one_circle)
				{
					locate_single_loop_pid_control(g_position_circle_adjust.x, g_position_circle_adjust.y);				
					pitch_offset=-2.7;
//					if(roll_offset>1.2)
//						roll_offset=1.2;
//					if(roll_offset<-1.2)
//						roll_offset=-1.2;
				}
				else if(road_state_machine == line_connect_circle)
				{

					
						//�Ѿ������յ�
					locate_single_loop_pid_control(g_position_circle_adjust.x, g_position_circle_adjust.y);
				
//						if(g_position_circle_adjust.x <100 && g_position_circle_adjust.x > -100 
//						&& g_position_circle_adjust.y <100 && g_position_circle_adjust.y > -100)
//						{
//					//������һ�� 
								{
									*current_mode_flow_steps = mode_step_6;
									locate_single_loop_pid_x.increment = 1000;
								}
//								locate_single_loop_pid_y.increment = 0;
//						}
					

				}
	
				
			}
			else if(fly_direction == backward)
			{
				//���������¼���·��״̬
				if(road_state_machine == line_connect_circle)
				{
					//��ͷ�����ǳ���η�, ����λ line_2.end, ������F405���Ѿ��������
					locate_single_loop_pid_control(g_position_line_2_adjust.x, g_position_line_2_adjust.y);
				}
				else if(road_state_machine == straight_line || road_state_machine == two_stripes || road_state_machine == one_stripes)
				{				
					locate_single_loop_pid_control(g_position_line_2_adjust.x, (g_position_line_1_adjust.y + g_position_line_2_adjust.y) / 2.0f);
				}
				else if(road_state_machine == circle_connect_line)
				{
					//�Ѿ������յ�
					locate_single_loop_pid_control(g_position_circle_adjust.x, g_position_circle_adjust.y);
					
					
					if(g_position_circle_adjust.x <100 && g_position_circle_adjust.x > -100 
						&& g_position_circle_adjust.y <100 && g_position_circle_adjust.y > -100)
					{
					//������һ�� 
						*current_mode_flow_steps = mode_step_6;
						locate_single_loop_pid_x.increment = 0;
//						locate_single_loop_pid_y.increment = 0;
					}
					

				}
				else
				{
					//���������е�·��״̬, ��������
				}
	
			}
			else
			{
				//fuck
			}		

			//������һ���ܵ�ʱ��, ����
			if(road_state_machine == one_stripes)
			{
				track_pitch_offset_max = 1.8f;
			}
			
/**********************************************************************************************************************/
			//
			//ʵ��������ܵ��ڽ�Բ��ʱ��ǳ���, ����Ч��������
			//
			
			//����������޷�, ��ֹ�ٶȹ���
			track_output_limit(track_pitch_offset_max, &pitch_offset);
			
			break; 
		}		
		
		case mode_step_6:
		{
//			if((road_state_machine == line_connect_circle_connect_line))
//			{
//				
//				*current_mode_flow_steps = mode_step_5;
//			}
		
			MODE=6;
			locate_single_loop_pid_control(g_position_circle_adjust.x, g_position_circle_adjust.y);
			set_height_cascade_control_2(set_height_control_expect_height, set_height_control_expect_height/5, set_height_control_expect_height/6);
			magnet_OFF;//������
			//���յ�Բ�ϴﵽ��̬, ����LED3, ����ȴ���
			if(stady_state_judge(g_position_circle_adjust, set_point_expect_position, 20, 100))
			{
				*current_mode_flow_steps = mode_step_7;
				
				magnet_OFF;
//				set_height_throttle_grab.current_throttle -= 20;
			}
			
				
			

			break;
			
		}		
		
		case mode_step_7:
		{
			MODE=7;
			locate_single_loop_pid_control(g_position_circle_adjust.x, g_position_circle_adjust.y);
			set_height_cascade_control_2(set_height_control_expect_height, set_height_control_expect_height/5, set_height_control_expect_height/6);
			
			//LED3��3s, ���뷵�ز�
//			if(wait_for_n_ms(&system_time_tick, 300, &x_times_call_wait_for_n_ms_func))
			{
				*current_mode_flow_steps = mode_step_9;
				
				LED3(OFF);
				
				fly_direction = (enum _direction)(1 - fly_direction);
				
				track_pitch_offset_max = 2.3f;
			}

			break;
			
		}		
		
//		//����ѭ��
//		case mode_step_8:
//		{
//			set_height_cascade_control_2(set_height_control_expect_height, set_height_control_expect_height/5, set_height_control_expect_height/6);
//			
//			if(fly_direction == forward)
//			{
//				//���������¼���·��״̬
//				if(road_state_machine == circle_connect_line)
//				{
//					//��ͷ�����ǳ���η�, ����λ line_2.end, ������F405���Ѿ��������
//					locate_single_loop_pid_control(g_position_line_2_adjust.x, g_position_line_2_adjust.y);					
//				}
//				else if(road_state_machine == straight_line || road_state_machine == two_stripes || road_state_machine == one_stripes)
//				{						
//					locate_single_loop_pid_control(g_position_line_1_adjust.x, (g_position_line_1_adjust.y + g_position_line_2_adjust.y) / 2.0f);
//				}
//				else if(road_state_machine == line_connect_circle)
//				{
//					//�Ѿ������յ�
//					locate_single_loop_pid_control(g_position_circle_adjust.x, g_position_circle_adjust.y);
//					
//					if(g_position_circle_adjust.x <100 && g_position_circle_adjust.x > -100 
//					&& g_position_circle_adjust.y <100 && g_position_circle_adjust.y > -100)
//					{
//					//������һ�� 
//					*current_mode_flow_steps = mode_step_9;
//						locate_single_loop_pid_x.increment = 0;
////						locate_single_loop_pid_y.increment = 0;
//					}
//				}
//				else
//				{
//					//���������е�·��״̬, ��������
//				}
//				
//			}
//			else if(fly_direction == backward)
//			{
//							
//				//���������¼���·��״̬
//				if(road_state_machine == line_connect_circle)
//				{
//					if(g_position_circle_adjust.x <100 && g_position_circle_adjust.x > -100) 
//					{
//						locate_single_loop_pid_control(g_position_line_2_adjust.x, g_position_circle_adjust.y);
//						
//					}
//					//��ͷ�����ǳ���η�, ����λ line_2.end, ������F405���Ѿ��������
//					locate_single_loop_pid_control(g_position_line_2_adjust.x, g_position_line_2_adjust.y);
//				}
//				else if(road_state_machine == straight_line || road_state_machine == two_stripes || road_state_machine == one_stripes)
//				{					
//					locate_single_loop_pid_control(g_position_line_2_adjust.x, (g_position_line_1_adjust.y + g_position_line_2_adjust.y) / 2.0f);
//				}
//				else if(road_state_machine == circle_connect_line)
//				{
//					//�Ѿ������յ�
//					locate_single_loop_pid_control(g_position_circle_adjust.x, g_position_circle_adjust.y);
//					
//					//������һ�� 
////					if(g_position_circle_adjust.x <100 && g_position_circle_adjust.x > -100
////					&& g_position_circle_adjust.y <100 && g_position_circle_adjust.y > -100)
////					{
//					//������һ�� 
//					*current_mode_flow_steps = mode_step_9;
//						locate_single_loop_pid_x.increment = -1000;
////						locate_single_loop_pid_y.increment = 0;
////					}

//				}
//				else
//				{
//					//���������е�·��״̬, ��������
//				}
//	
//			}
//			else
//			{
//				//fuck
//			}		

//			//������һ���ܵ�ʱ��, ����
//			if(road_state_machine == two_stripes)
//			{
//				track_pitch_offset_max = 1.7f;
//			}
//			
///**********************************************************************************************************************/
//			//
//			//ʵ��������ܵ��ڽ�Բ��ʱ��ǳ���, ����Ч��������
//			//
//			
//			//����������޷�, ��ֹ�ٶȹ���
//			
//			track_output_limit(track_pitch_offset_max, &pitch_offset);
//			
//			break; 
//		}		
//		
		//���յ�Բ�Ϸ�, ������̬����Խ���
		case mode_step_9:
		{
			locate_single_loop_pid_control(g_position_circle_adjust.x, g_position_circle_adjust.y);
			set_height_cascade_control_2(set_height_control_expect_height, set_height_control_expect_height/5, set_height_control_expect_height/6);
			
			//���յ�Բ�ϴﵽ��̬
			if(stady_state_judge(g_position_circle_adjust, set_point_expect_position, 10, 80))
			{
				*current_mode_flow_steps = mode_step_10;
			}

			break;
		}		
		
		//����
		case mode_step_10:
		{
			MODE=10;
			*current_mode_flow_steps = mode_step_11;
			
			set_height_height_grab.flag = 0;	
			
			set_height_control_expect_height = -60;   //
			
			Suddenly_flag = 1;
			
			break;
		}		
		
		case mode_step_11:
		{
			MODE=11;
			set_height_cascade_control_2(set_height_control_expect_height, 
										 (g_height-set_height_control_expect_height) * 0.1, 
										 (g_height-set_height_control_expect_height) * 0.1);


			
			if(g_height > 30)
			{
				locate_single_loop_pid_control(g_position_circle_adjust.x, g_position_circle_adjust.y);
				//�߶Ȳ���, ��������
			}
			else
			{
				//���䡢������������
				pitch_offset = 0;
				roll_offset = 0;
				locate_single_loop_pid_x.increment = 0;
				locate_single_loop_pid_y.increment = 0;	
				
				//ֱ�Ӽ�����, ���ٽ���
				set_height_throttle_grab.current_throttle -= 10;
				
				if(g_height < 20 && Suddenly_flag == 1)
				{
					set_height_throttle_grab.current_throttle -= 50;
					Suddenly_flag = 0;
				}
				
				if(set_height_throttle_grab.current_throttle <= 100)
				{
					set_height_throttle_grab.current_throttle = 100;
				}

			}

			if(g_height < 10)
			{
				*current_mode_flow_steps = mode_step_12;		
				magnet_ON;     //�ؼ���
				ctrl.height.core.pid_out = 0;
				ctrl.height.shell.pid_out = 0;
				ctrl.height.shell.increment = 0;
				ctrl.height.core.increment = 0;
			
//				ARMED = 0;
			}
			break;
		}
		
		case mode_step_12:
		{
			MODE=12;
			*current_mode_flow_steps = mode_step_none;
			
//			ARMED=1;
			mode_machine = B_to_A;			
			
			break;
		}
		
		default:
		{
			
			break;
		}
	}
}


//

void B_to_A_control(mode_flow_step_type *current_mode_flow_steps)
{	
	u8 Suddenly_flag = 1;
  
	float track_pitch_offset_max = 2.5f;
	
	if(g_height == 300) g_height =110;
	
	switch(*current_mode_flow_steps)
	{
		case mode_step_none:
		{
			MODE=0;
			*current_mode_flow_steps = mode_step_1;
			
			//��ƫ���ǵ�����ֵ�������˲���yawֵ, �����Ͳ��ᵼ���ϵ�����ᱻ������ת����ɵ���ʧ��
			yaw_offset = angle.yaw;
			
			set_height_height_grab.flag = 0;	
			set_height_throttle_grab.flag = 0;
			set_height_throttle_grab.current_throttle = 0 ;
			fly_direction = backward;
			magnet_ON;
		
			break;
		}
		
		case mode_step_1:
		{
			MODE=1;
			//�ȴ�8s
			if(wait_for_n_ms(&system_time_tick, 300, &x_times_call_wait_for_n_ms_func))
			{
				*current_mode_flow_steps = mode_step_2;
				increment_set_to_zero();
			}
			
			break;
		}
		
		case mode_step_2:
		{
			MODE=2;
//			set_height_control_expect_height = delivery_param_to_control.expect_height_start_circle;
			set_height_control_expect_height =90;
			set_height_cascade_control_2(set_height_control_expect_height, set_height_control_expect_height/5, set_height_control_expect_height/6);
			
	
			if(g_height > 30)
			{
				locate_single_loop_pid_control(g_position_circle_adjust.x, g_position_circle_adjust.y);
				
				if(stady_state_judge(g_position_circle_adjust, set_point_expect_position, 3, 80))
				{
					*current_mode_flow_steps = mode_step_3;
					
					//ȡyaw����ֵ
//					yaw_offset_tmp = get_yaw_expect(g_position_circle_adjust.x, g_position_circle_adjust.y, 
//													g_position_line_1_adjust.x, g_position_line_1_adjust.y);
				}
				
			}
			else
			{
				roll_offset = 0;
				pitch_offset = 0;
			}
			
			break;
		}
		
		case mode_step_3:
		{
			MODE=3;
//			if(yaw_expect_wave(yaw_offset_tmp, &yaw_offset))
			{
				*current_mode_flow_steps = mode_step_4;
			}
			
			set_height_cascade_control_2(set_height_control_expect_height, set_height_control_expect_height/5, set_height_control_expect_height/6);
			
			if(g_height > 30)
			{
				locate_single_loop_pid_control(g_position_circle_adjust.x, g_position_circle_adjust.y);
			}
			else
			{
				roll_offset = 0;
				pitch_offset = 0;
			}
			
			break;
		}
		
		case mode_step_4:
		{
			MODE=4;
			set_height_cascade_control_2(set_height_control_expect_height, set_height_control_expect_height/5, set_height_control_expect_height/6);
			
			if(g_height > 30)
			{
				locate_single_loop_pid_control(g_position_circle_adjust.x, g_position_circle_adjust.y);
			}
			else
			{
				roll_offset = 0;
				pitch_offset = 0;
			}
			
			//����ѭ����
//			if(g_height > set_height_control_expect_height - 5 && g_height < set_height_control_expect_height + 5)
//			{
//				*current_mode_flow_steps = mode_step_5;
//			}
			if(stady_state_judge(g_position_circle_adjust, set_point_expect_position, 7, 80))//���Ⱥ󷢳�
			{
					*current_mode_flow_steps = mode_step_5;
			}
				
			
			break;
		}
		
		//����ѭ��
		case mode_step_5:
		{
			MODE=5;
			set_height_cascade_control_2(set_height_control_expect_height, set_height_control_expect_height/5, set_height_control_expect_height/6);
			
			if(fly_direction == forward)
			{
				//���������¼���·��״̬
				if(road_state_machine == circle_connect_line)
				{
					//��ͷ�����ǳ���η�, ����λ line_2.end, ������F405���Ѿ��������
					locate_single_loop_pid_control(g_position_line_2_adjust.x, g_position_line_2_adjust.y);
						pitch_offset=1;
					if(roll_offset>1)
						roll_offset=1;
					if(roll_offset<-1)
						roll_offset=-1;
				}
				else if(road_state_machine == straight_line || road_state_machine == two_stripes || road_state_machine == one_stripes)
				{						
					locate_single_loop_pid_control(g_position_line_1_adjust.x, (g_position_line_1_adjust.y + g_position_line_2_adjust.y) / 2.0f);
						pitch_offset=1;
					if(roll_offset>1)
						roll_offset=1;
					if(roll_offset<-1)
						roll_offset=-1;
				}
				else if(road_state_machine == line_connect_circle)
				{
					//�Ѿ������յ�
					locate_single_loop_pid_control(g_position_circle_adjust.x, g_position_circle_adjust.y);
					
					if(g_position_circle_adjust.x <100 && g_position_circle_adjust.x > -100 
					&& g_position_circle_adjust.y <100 && g_position_circle_adjust.y > -100)
					{
					//������һ�� 
					*current_mode_flow_steps = mode_step_9;
						locate_single_loop_pid_x.increment = 0;
//						locate_single_loop_pid_y.increment = 0;
					}
				}
				else
				{
					//���������е�·��״̬, ��������
				}
				
			}
			else if(fly_direction == backward)
			{
							
				//���������¼���·��״̬
				if(road_state_machine == line_connect_circle)
				{
					if(g_position_circle_adjust.x <150 && g_position_circle_adjust.x > -150) 
					{
						locate_single_loop_pid_control(g_position_line_2_adjust.x, g_position_circle_adjust.y);
							pitch_offset=2.0;
//					if(roll_offset>1.2)
//						roll_offset=1.2;
//					if(roll_offset<-1.2)
//						roll_offset=-1.2;
					}
					//��ͷ�����ǳ���η�, ����λ line_2.end, ������F405���Ѿ��������
					locate_single_loop_pid_control(g_position_line_2_adjust.x, g_position_circle_adjust.y);
						pitch_offset=2.0;
//					if(roll_offset>1.2)
//						roll_offset=1.2;
//					if(roll_offset<-1.2)
//						roll_offset=-1.2;
				}
				else if(road_state_machine == line_connect_circle_connect_line || road_state_machine == straight_line )
				{					
					locate_single_loop_pid_control(g_position_line_2_adjust.x, (g_position_line_1_adjust.y + g_position_line_2_adjust.y) / 2.0f);
						pitch_offset=2.0;
//					if(roll_offset>1.2)
//						roll_offset=1.2;
//					if(roll_offset<-1.2)
//						roll_offset=-1.2;
				}
				else if(road_state_machine == one_circle)
				{
					locate_single_loop_pid_control(g_position_circle_adjust.x, g_position_circle_adjust.y);				
					pitch_offset=2.0;
//					if(roll_offset>1.2)
//						roll_offset=1.2;
//					if(roll_offset<-1.2)
//						roll_offset=-1.2;
				}
				else if(road_state_machine == circle_connect_line)
				{
				
					{
					//�Ѿ������յ�
					locate_single_loop_pid_control(g_position_circle_adjust.x, g_position_circle_adjust.y);
					
					//������һ�� 
//					if(g_position_circle_adjust.x <100 && g_position_circle_adjust.x > -100
//					&& g_position_circle_adjust.y <100 && g_position_circle_adjust.y > -100)
//					{
					//������һ�� 
					*current_mode_flow_steps = mode_step_6;
						locate_single_loop_pid_x.increment = -1000;
//						locate_single_loop_pid_y.increment = 0;
//					}
					}
				}
				else
				{
					//���������е�·��״̬, ��������
				}
	
			}
			else
			{
				//fuck
			}		

			//������һ���ܵ�ʱ��, ����
			if(road_state_machine == two_stripes)
			{
				track_pitch_offset_max = 1.8f;
			}
			
/**********************************************************************************************************************/
			//
			//ʵ��������ܵ��ڽ�Բ��ʱ��ǳ���, ����Ч��������
			//
			
			//����������޷�, ��ֹ�ٶȹ���
			
			track_output_limit(track_pitch_offset_max, &pitch_offset);
			
			break; 
		}		
		
		//���յ�Բ�Ϸ�, ������̬����Խ���
		case mode_step_6:
		{
			if(road_state_machine == line_connect_circle_connect_line)
			{	
					*current_mode_flow_steps = mode_step_8;
			}
			
			locate_single_loop_pid_control(g_position_circle_adjust.x, g_position_circle_adjust.y);
			set_height_cascade_control_2(set_height_control_expect_height, set_height_control_expect_height/5, set_height_control_expect_height/6);
			
			//���յ�Բ�ϴﵽ��̬
			if(stady_state_judge(g_position_circle_adjust, set_point_expect_position, 10, 80))
			{
				*current_mode_flow_steps = mode_step_7;
			}

			break;
		}
		
		//����
		case mode_step_7:
		{
			MODE=10;
			*current_mode_flow_steps = mode_step_8;
			
			set_height_height_grab.flag = 0;	
			
			set_height_control_expect_height = -60;
			
			Suddenly_flag = 1;
			
			break;
		}		
		
		case mode_step_8:
		{
			MODE=11;
			set_height_cascade_control_2(set_height_control_expect_height, 
										 (g_height-set_height_control_expect_height) * 0.1, 
										 (g_height-set_height_control_expect_height) * 0.1);


			
			if(g_height > 30)
			{
				locate_single_loop_pid_control(g_position_circle_adjust.x, g_position_circle_adjust.y);
				//�߶Ȳ���, ��������
			}
			else
			{
				//���䡢������������
				pitch_offset = 0;
				roll_offset = 0;
				locate_single_loop_pid_x.increment = 0;
				locate_single_loop_pid_y.increment = 0;	
				
				//ֱ�Ӽ�����, ���ٽ���
				set_height_throttle_grab.current_throttle -= 10;
				
				if(g_height < 20 && Suddenly_flag == 1)
				{
					set_height_throttle_grab.current_throttle -= 50;
					Suddenly_flag = 0;
				}
				
				if(set_height_throttle_grab.current_throttle <= 100)
				{
					set_height_throttle_grab.current_throttle = 100;
				}

			}

			if(g_height < 10)
			{
				*current_mode_flow_steps = mode_step_9;		
				
				ctrl.height.core.pid_out = 0;
				ctrl.height.shell.pid_out = 0;
				ctrl.height.shell.increment = 0;
				ctrl.height.core.increment = 0;
			
				ARMED = 0;
			}
			break;
		}
		
		case mode_step_9:
		{
			MODE=12;
			*current_mode_flow_steps = mode_step_none;
		
			mode_machine = mode_none;			
			
			break;
		}
		
		default:
		{
			
			break;
		}
	}
}






void set_height_turning_control(mode_flow_step_type *current_mode_flow_steps)//���߿��ƺ���
{
	switch(*current_mode_flow_steps)
	{
		case mode_step_none:
		{
			MODE=0;
			*current_mode_flow_steps = mode_step_1;
			
			set_height_height_grab.flag = 0;	
			set_height_throttle_grab.flag = 0;
		
			break;
		}
		case mode_step_1:
		{
			
			
			//�ȴ�8s
			if(wait_for_n_ms(&system_time_tick, 1000, &x_times_call_wait_for_n_ms_func))
			{
				*current_mode_flow_steps = mode_step_2;
				increment_set_to_zero();
			}
			
			break;
		}
		case mode_step_2:    								//ͣ����һ������һ������λ��ָ����䣩
		{
			MODE=2;
			set_height_control_expect_height = 90;
			set_height_cascade_control_2(set_height_control_expect_height, 
																		set_height_control_expect_height/5, 
																		set_height_control_expect_height/6);
			
			
			if(g_height >(set_height_control_expect_height-10)&&wait_for_n_ms(&system_time_tick, 800, &x_times_call_wait_for_n_ms_func))
			{
				*current_mode_flow_steps = mode_step_3;
			}
			
			break;
		}
		
		case mode_step_3:      							//����
		{
			set_height_cascade_control_2(set_height_control_expect_height, 70, 50);

			set_height_height_grab.flag = 0;	
			
			set_height_control_expect_height = -60;
			
			*current_mode_flow_steps = mode_step_4;
		}
		
		case mode_step_4:
		{
			set_height_cascade_control_2(set_height_control_expect_height, (g_height-set_height_control_expect_height)*0.3, (g_height-set_height_control_expect_height) * 0.15);
		
			if(g_height < 30)
			{
					set_height_throttle_grab.current_throttle -= 4;
			}

			if(g_height < 10)
			{
				*current_mode_flow_steps = mode_step_5;		
				
				ctrl.height.core.pid_out = 0;
				ctrl.height.shell.pid_out = 0;
				ctrl.height.shell.increment = 0;
				ctrl.height.core.increment = 0;
				
				ARMED = 0;
			}
			break;
		}
		
		case mode_step_5:
		{
			*current_mode_flow_steps = mode_step_none;
		
			mode_machine = mode_none;			
		}
		
		default:
		{
			
			break;
		}
	}
}


void set_point_turning_control(mode_flow_step_type *current_mode_flow_steps)
{
	switch(*current_mode_flow_steps)
	{
		case mode_step_none:
		{
			*current_mode_flow_steps = mode_step_1;
			
			set_height_height_grab.flag = 0;	
			set_height_throttle_grab.flag = 0;
		
			break;
		}
		case mode_step_1:
		{
		  
			//�ȴ�8s
			if(wait_for_n_ms(&system_time_tick, 1000, &x_times_call_wait_for_n_ms_func))
			{
				*current_mode_flow_steps = mode_step_2;
				increment_set_to_zero();
			}
			
			break;
		}
		
		case mode_step_2:    //����
		{
//			set_height_control_expect_height = 110;
//			set_height_cascade_control_2(set_height_control_expect_height, 25, 15);

			set_height_cascade_control_2(	set_height_control_expect_height, 
																		set_height_control_expect_height/5, 
																		set_height_control_expect_height/6);
			
			if(g_height > 35)      //�߶ȴ���25��ʼ����
			{
				locate_single_loop_pid_control(g_position_circle_adjust.x, g_position_circle_adjust.y);//����PID
			}
			else
			{
				roll_offset = 0;
				pitch_offset = 0;
			}
			
			if(g_height >(set_height_control_expect_height-10)&&wait_for_n_ms(&system_time_tick, 1900, &x_times_call_wait_for_n_ms_func))
			{
				*current_mode_flow_steps = mode_step_3;
			}
//			if(g_height >(set_height_control_expect_height-10)&&fabs(g_position_circle_adjust.x)<60&&fabs(g_position_circle_adjust.y)<60)
//			{
//				
//				if(yaw_expect_wave(90, &yaw_offset))
//				{
//					*current_mode_flow_steps = mode_step_3;
//				}
//			}
			break;
		}
		
		

		
		
		
		
		case mode_step_3:
		{
			*current_mode_flow_steps = mode_step_4;
		
			set_height_height_grab.flag = 0;	
			
			set_height_control_expect_height = -60;
		
			//����PID
			
		}
		
		case mode_step_4:
		{
			set_height_cascade_control_2(set_height_control_expect_height, 
										 (g_height-set_height_control_expect_height) * 0.2, 
										 (g_height-set_height_control_expect_height) * 0.1);
		
			
			if(g_height > 30)
			{
				locate_single_loop_pid_control(g_position_circle_adjust.x, g_position_circle_adjust.y);
				//�߶Ȳ���, ��������
			}
			else
			{
				//���䡢������������
				pitch_offset = 0;
				roll_offset = 0;
				locate_single_loop_pid_x.increment = 0;
				locate_single_loop_pid_y.increment = 0;	
				
				//ֱ�Ӽ�����, ���ٽ���
				set_height_throttle_grab.current_throttle -= 4;
				
				if(set_height_throttle_grab.current_throttle  <= 10)
				{
					set_height_throttle_grab.current_throttle = 0;
				}
			}

			if(g_height < 10)
			{
				*current_mode_flow_steps = mode_step_5;		
				
				ctrl.height.core.pid_out = 0;
				ctrl.height.shell.pid_out = 0;
				ctrl.height.shell.increment = 0;
				ctrl.height.core.increment = 0;
			
				ARMED = 0;
			}
			break;
		}
		
		case mode_step_5:
		{
			*current_mode_flow_steps = mode_step_none;
		
			mode_machine = mode_none;			
		}
		
		default:
		{
			
			break;
		}
	}
}
//typedef enum _mode_flow_step
//{
//	mode_step_none,
//	mode_step_1,
//	mode_step_2,
//	mode_step_3,
//	mode_step_4,
//	mode_step_5,
//	mode_step_6,
//	mode_step_7,
//	mode_step_8,
//	mode_step_9,
//	mode_step_10,
//	mode_step_11,
//	mode_step_12,
//	mode_step_13,
//	mode_step_14,
//	mode_step_15,
//	mode_step_16,
//}mode_flow_step_type;

//�ȴ�+���+��ת+����...
mode_flow_step_type mode_1_flow_steps;

//ǿ���Զ�����
mode_flow_step_type mode_2_flow_steps;

//���߲���
mode_flow_step_type mode_3_flow_steps;

//�������...
mode_flow_step_type mode_4_flow_steps;

//ѭ������...
mode_flow_step_type mode_5_flow_steps;

//A_to_B_fire_B_to_A
mode_flow_step_type mode_A_to_B_fire_B_to_A_flow_steps;

//�ɾ���
mode_flow_step_type mode_fly_on_rectangle_flow_steps;

//A_to_B
mode_flow_step_type mode_A_to_B_steps;

//LUCK
mode_flow_step_type mode_FIND_LCUK_steps;

//B_to_A
mode_flow_step_type mode_B_to_A_steps;

//A_C_B
mode_flow_step_type mode_A_C_B_steps;

//����ģʽ...
mode_flow_step_type mode_6_flow_steps;

//typedef enum _mode_machine
//{
//	mode_none,	
//	gesture_turning,
//	force_to_auto_land,
//	wait_setheight_spin_land,
//	wait_setheight_setpoint_spin_track_setpoint_land,
//	set_height_turning,
//	set_point_turning,
//	
//	A_to_B_fire_B_to_A,		//7
//	
//	fly_on_rectangle,
//	
//	A_to_B,
//	
//	FIND_LCUK,
//	
//	B_to_A,
//	
//	set_height_,
//	
//}mode_machine_type;

void business_mode_handle(mode_machine_type *mode_machine)//��ĿҪ��ģʽ״̬��
{
	if(!ARMED)
	{
		*mode_machine = mode_none;
//		locate_single_loop_pid_control(g_position_circle_adjust.x, g_position_circle_adjust.y);
	}
	
	switch(*mode_machine)
	{
		case mode_none:
		{

			break;
		}
		

		case set_height_:
		{
			set_height_turning_control(&mode_3_flow_steps);  //���붨�߿���
		
			break;
		}			
		
		case set_point_turning:      //���붨�����
		{
			set_point_turning_control(&mode_4_flow_steps);
		
			break;
		}

		case A_to_B:     							//AB����
		{
			A_to_B_control(&mode_A_to_B_steps);
			
			break;
		}

		
		case A_to_B_fire_B_to_A:  		//ABAԲ-��-Բ
		{
			A_to_B_fire_B_to_A_control(&mode_A_to_B_fire_B_to_A_flow_steps);
			
			break;
		}

		case fly_on_rectangle:   		//����-����
		{
			fly_on_rectangle_control(&mode_fly_on_rectangle_flow_steps);
			
			break;
		}
		
		case FIND_LCUK:  //A-B(�������)-B
		{
			FIND_LCUK_control(&mode_FIND_LCUK_steps);
			
			break;
		}
		
		//���� FIND_LCUK
		case B_to_A:
		{
			B_to_A_control(&mode_B_to_A_steps);
			
			break;
		}
		
		case A_C_B:     //15��У��ACB
		{
			A_C_B_control(&mode_A_C_B_steps);
			
			break;
		}

		default: 
		{
			break;
		}
	}
	
	//������ģʽ�µ����̲������
	mode_flow_step_set_to_zero(mode_machine);
	
	//������ģʽ�µĿ��ƻ��֡��������
	integral_output_set_to_zero(mode_machine);
	
	//testing...
	test_report_data_s16_1 = mode_FIND_LCUK_steps;
	
}

//
//@brief :	�����ϴη���״̬, ��·״̬ ��  ����x_times_call_wait_for_n_ms_func
//
//
//
void update_last_mode_state(const enum _mode_machine current_mode, enum _mode_machine *last_mode,
							const enum _road_state_machine current_road_state, enum _road_state_machine *last_road_state,
							u16 *x_times_call_wait_for_n_ms_func)
{
	//����ǰ����ģʽ���ϴη���ģʽ��ͬʱ������x_times_call_wait_for_n_ms_func
	if(current_mode != *last_mode)
	{
		(*x_times_call_wait_for_n_ms_func)++;
		
	}
	
	
	//���µ�״̬��������wait��ʱ����ʱ��������Ǹ�ը��
	
	//�����ϴη���״̬
	*last_mode = current_mode;
	
	//�����ϴε�·״̬
	*last_road_state = current_road_state;
}



void mode_flow_step_set_to_zero(const mode_machine_type *mode_machine)
{
	switch(*mode_machine)
	{
		case mode_none:
		{
			mode_1_flow_steps = mode_step_none;
			mode_2_flow_steps = mode_step_none;
			mode_3_flow_steps = mode_step_none;
			mode_4_flow_steps = mode_step_none;
			mode_5_flow_steps = mode_step_none;
			
			mode_A_to_B_fire_B_to_A_flow_steps = mode_step_none;
			mode_fly_on_rectangle_flow_steps = mode_step_none;
			mode_A_to_B_steps = mode_step_none;
			
			mode_FIND_LCUK_steps = mode_step_none;
			mode_B_to_A_steps = mode_step_none;
			
			mode_A_C_B_steps = mode_step_none;
			break;
		}
		
		default: 
		{
			break;
		}
	}
}


void increment_set_to_zero(void)
{
	ctrl.pitch.shell.increment = 0;
	ctrl.roll.shell.increment = 0;
	ctrl.yaw.shell.increment = 0;

	ctrl.pitch.core.increment = 0;
	ctrl.roll.core.increment = 0;
	ctrl.yaw.core.increment = 0;

	
	ctrl.height.shell.increment = 0;
	ctrl.height.core.increment = 0;
	
	pitch_offset = 0;
	roll_offset = 0;
	
	ctrl.locate_x.shell.increment = 0;
	ctrl.locate_y.shell.increment = 0;

	ctrl.locate_x.core.increment = 0;
	ctrl.locate_y.core.increment = 0;
	
	locate_single_loop_pid_x.increment = 0;
	locate_single_loop_pid_y.increment = 0;

}


void integral_output_set_to_zero(const mode_machine_type *mode_machine)
{
	switch(*mode_machine)
	{
		case mode_none:
		{
			ctrl.pitch.shell.increment = 0;
			ctrl.roll.shell.increment = 0;
			ctrl.yaw.shell.increment = 0;

			ctrl.pitch.core.increment = 0;
			ctrl.roll.core.increment = 0;
			ctrl.yaw.core.increment = 0;

			
			ctrl.height.shell.increment = 0;
			ctrl.height.core.increment = 0;
			
			pitch_offset = 0;
			roll_offset = 0;
			
			ctrl.locate_x.shell.increment = 0;
			ctrl.locate_y.shell.increment = 0;

			ctrl.locate_x.core.increment = 0;
			ctrl.locate_y.core.increment = 0;
			
			break;
		}
		
		
		default: 
		{
			break;
		}

	}
		
}



