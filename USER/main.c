#include "include.h" 
int k=1;

extern int16_t MODE;

int main(void)
{
	IAC_Init();     	//���輰��������
	paramLoad();		//��������  ǰ�������͸���PID����
	//State_Display();	//OLED������ʾ
	EnTIM3();       	//����ʱ�� ����ģʽ��ʶ�� ��̬���� ������������
	
	magnet_ON;
	
//	magnet_OFF;//������
	while(1)
	{    
		//DMP-->��׼100HZ, �������ܶ�ʱ������������ִ�е�Ӱ��8
		//����ֲ����, δ����
		inv_mpu_read(&angle, &sensor);
					
		//ÿ20MS����λ������һ������
		//ÿ20MS�ϳ�һ������(�߶�)
		if(sentDateFlag)  		//����λ���������ݱ�־
		{
/**********************************************************************************************************/	
			sentDateFlag = 0;			
		
			
			//���ڷ�����̬
			UART2_ReportIMU();


			//�߶����ݺϳɣ��˲�
			height_composite(&g_height);

			//λ�����ݺϳɣ�У�����˲�
			camera_data_image_decode(USART_ov7620_rxbuf, &camera_RX_BUF_point);
/*
      P/1000   I/10000   D/100 ����̬pid����
      Data_Receive_Pid1(&locate_single_loop_pid_x, &locate_single_loop_pid_y, &scapegoat_1);		//kp/1000
  		Data_Receive_Pid1(&ctrl.roll.core, &ctrl.pitch.core, &ctrl.yaw.core);		//kp/1000  �ڻ�
		 	Data_Receive_Pid2(&ctrl.roll.shell, &ctrl.pitch.shell, &ctrl.yaw.shell);		//kp/1000  �⻷
			Data_Receive_Pid2(&ctrl.height.shell, &ctrl.height.core, &ctrl.yaw.shell);		//kp/1000  �⻷
	
			//�˺����쳣��Ҫ���ѿغ�����������ͨ���˺���������λ�����͹�����ָ��
			Data_Receive_lock(&ARMED);
		
*/	
								
				//������ͷģ�鷢�͸߶���Ϣ(����ȡλ�á�״̬�����ݵ�ָ��)				
				if(display_what == param)
				{
					get_image_or_position_data(get_position_data);
				}
/**********************************************************************************************************/					
				//������ģ�鷢�ͻ�ȡ�߶�ָ��
				if(1 == g_KS103_retrans_flag)
				{
					send_sonar_cmd();

					g_KS103_retrans_flag = 0;
				}	

				led_handle();	
				
				if(!ARMED || g_height < 10 )
				{
					if(k)
					{
//						OLED_P6x8Str(10,0,"pitch:");
//						OLED_P6x8Str(10,1,"roll:");
//						OLED_P6x8Str(10,2,"yaw:");
//						OLED_P6x8Str(10,5,"Heigh:");
				
						Dis_Float(0, 10, angle.pitch,2);			
						Dis_Float(1, 10, angle.roll, 2);			
						Dis_Float(2, 10, angle.yaw,  2);						
						Dis_Float(4, 10, g_height,   2);
//						Dis_Float(6, 10, MODE,   2);
						Dis_Float(6, 10, road_state_machine,   2);
						Dis_Float(2, 65, g_position_circle_receive.y, 1);
						Dis_Float(3, 65, g_position_circle_receive.x, 1);
//						
//						Dis_Float(4, 65, (g_position_line_1_adjust.y+g_position_line_2_adjust.y)/2.0f, 1);
//						Dis_Float(5, 65, g_position_line_1_adjust.y, 1);
//						Dis_Float(6, 65, g_position_line_2_adjust.y, 1);
//						
						Dis_Float(5, 65, g_position_line_1_adjust.y, 1);
						Dis_Float(6, 65, g_position_line_2_adjust.y, 1);
						
						Dis_Float(7, 65, g_position_line_1_receive.x, 1);

//						Dis_Float(2, 65, g_position_circle_adjust.x, 1);
//						Dis_Float(3, 65, g_position_circle_adjust.y, 1);
						
						
//						locate_single_loop_pid_control(g_position_circle_adjust.x, g_position_circle_adjust.y);//����PID
//						Dis_Float(5, 65, pitch_offset, 1);
//						Dis_Float(6, 65, roll_offset, 1);
						
						
					}
					
					
					hci();                          //�ص㿴��hci����Ϊ�˻���������
				}
				
			}
	}
}

