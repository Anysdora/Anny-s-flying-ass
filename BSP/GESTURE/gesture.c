#include "gesture.h"
#include "include.h"

u8 ARMED = 0;

vs16 Moto_duty[4];//typedef __IO int16_t  vs16;

struct _ctrl ctrl;
/********************************************************/
/*void X_Control(float rol, float pit, float yaw)		*/
/*���룺rol   �����                               		*/
/*      pit   ������                                 	*/
/*		yaw   �����                                 	*/
/*�����                                             	*/
/********************************************************/

//float steady_state_roll_expect = 0.0f;
//float steady_state_pitch_expect = 0.0f;
//float steady_state_yaw_expect = 0.0f;

void X_CONTROL(float rol, float pit, float yaw)
{		
	float roll_error, pitch_error, yaw_error;
	static float roll_error_last, pitch_error_last, yaw_error_last;
	
	if(ctrl.ctrlRate >= 2) 													
	{
		//*****************�⻷PID**************************//
		
		//*********************************************����**********************************************//
		
		
		//pitch_offset �������
		pitch_error = pitch_offset - pit + steady_state_pitch_expect;	//����offset-��ǰpitch+�Ƕ�����
	

		ctrl.pitch.shell.increment += pitch_error;   			//pitch�⻷����ۻ�
		//�����޷�
		if(ctrl.pitch.shell.increment > ctrl.pitch.shell.increment_max)
				ctrl.pitch.shell.increment = ctrl.pitch.shell.increment_max;
		else if(ctrl.pitch.shell.increment < -ctrl.pitch.shell.increment_max)
				ctrl.pitch.shell.increment = -ctrl.pitch.shell.increment_max;
		
		ctrl.pitch.shell.pid_out = ctrl.pitch.shell.kp * pitch_error      //pid�㷨��ʽ
								 + ctrl.pitch.shell.ki * ctrl.pitch.shell.increment 
								 + ctrl.pitch.shell.kd * (pitch_error - pitch_error_last);
	
		pitch_error_last = pitch_error; 
		
		//*********************************************���**********************************************//
		
				//roll_offset �������
				roll_error = roll_offset - rol + steady_state_roll_expect;


		ctrl.roll.shell.increment += roll_error;  
		//�����޷�
		if(ctrl.roll.shell.increment > ctrl.roll.shell.increment_max)
				ctrl.roll.shell.increment = ctrl.roll.shell.increment_max;
		else if(ctrl.roll.shell.increment < -ctrl.roll.shell.increment_max)
				ctrl.roll.shell.increment = -ctrl.roll.shell.increment_max;

		ctrl.roll.shell.pid_out  = ctrl.roll.shell.kp * roll_error 
								 + ctrl.roll.shell.ki * ctrl.roll.shell.increment 
								 + ctrl.roll.shell.kd * (roll_error - roll_error_last);
	
		roll_error_last = roll_error;  

		//*********************************************ƫ��**********************************************//
		yaw_error = yaw_offset - yaw;			// + (Rc_Data.YAW - Rc_Data.yaw_offset)/5
		
		//�� angle.yaw ��180�㸽������������
		if(yaw_error > 180)
		{
			yaw_error = yaw_error - 360;
		}
		if(yaw_error < -180)
		{
			yaw_error = 360 + yaw_error;
		}
		

		ctrl.yaw.shell.increment += yaw_error;  		
		//�����޷�
		if(ctrl.yaw.shell.increment > ctrl.yaw.shell.increment_max)
				ctrl.yaw.shell.increment = ctrl.yaw.shell.increment_max;
		else if(ctrl.yaw.shell.increment < -ctrl.yaw.shell.increment_max)
				ctrl.yaw.shell.increment = -ctrl.yaw.shell.increment_max;

		ctrl.yaw.shell.pid_out = ctrl.yaw.shell.kp * yaw_error
							   + ctrl.yaw.shell.ki * ctrl.yaw.shell.increment
							   + ctrl.yaw.shell.kd * (yaw_error - yaw_error_last);		
		
		yaw_error_last = yaw_error;
//ע�⣺������ʹ��yaw_errorֵ�Ĳyawֵ�Ĳ��sensor.gyro.radian.z��ֵ��������ģ�
//yaw_errorֵ�Ĳyawֵ�Ĳ�������ֵ�ı�ʱ�����,yawֵ�Ĳ��sensor.gyro.radian.z��ֵ����ȣ���ΪƵ�ʲ�ͬ
		
		ctrl.ctrlRate = 0;//���� Ҳ����˵�����ڻ���ִ�����Σ��⻷ִ��һ��
	}
	
	ctrl.ctrlRate ++;
	
	//*******************************************�ڻ�(���ٶȻ�)PID*********************************//	

	//********************************************���**********************************************//
	float roll_rate_error, pitch_rate_error, yaw_rate_error;
	
	//΢������, �ڻ�������ֵһֱ�ڸı�, ��ʹ��΢�����лᵼ���ڻ�������Ҳ���
	static float roll_rate_last, pitch_rate_last, yaw_rate_last;
	
	roll_rate_error = ctrl.roll.shell.pid_out - sensor.gyro.angle.y;  //sensor.gyro.angle.yΪ�����ǵó��Ľ��ٶ�
	ctrl.roll.core.kp_out = ctrl.roll.core.kp * roll_rate_error;
	
	ctrl.roll.core.increment += roll_rate_error;   	
	if(ctrl.roll.core.increment > ctrl.roll.core.increment_max)
			ctrl.roll.core.increment = ctrl.roll.core.increment_max;
	else if(ctrl.roll.core.increment < -ctrl.roll.core.increment_max)
			ctrl.roll.core.increment = -ctrl.roll.core.increment_max;
	ctrl.roll.core.ki_out = ctrl.roll.core.ki * ctrl.roll.core.increment; 
	
	ctrl.roll.core.kd_out = ctrl.roll.core.kd * (roll_rate_last - sensor.gyro.angle.y);
	roll_rate_last = sensor.gyro.angle.y;

	ctrl.roll.core.pid_out = ctrl.roll.core.kp_out + ctrl.roll.core.ki_out + ctrl.roll.core.kd_out;//��Ϊ�����roll�����pid

	
	//*********************************************����**********************************************//
	pitch_rate_error = ctrl.pitch.shell.pid_out - sensor.gyro.angle.x;
	
	ctrl.pitch.core.kp_out = ctrl.pitch.core.kp * pitch_rate_error;

	ctrl.pitch.core.increment += pitch_rate_error;   		
	if(ctrl.pitch.core.increment > ctrl.pitch.core.increment_max)
			ctrl.pitch.core.increment = ctrl.pitch.core.increment_max;
	else if(ctrl.pitch.core.increment < -ctrl.pitch.core.increment_max)
			ctrl.pitch.core.increment = -ctrl.pitch.core.increment_max;
	ctrl.pitch.core.ki_out = ctrl.pitch.core.ki * ctrl.pitch.core.increment; 
	
	ctrl.pitch.core.kd_out = ctrl.pitch.core.kd * (pitch_rate_last - sensor.gyro.angle.x);	
	pitch_rate_last = sensor.gyro.angle.x;

	ctrl.pitch.core.pid_out = ctrl.pitch.core.kp_out + ctrl.pitch.core.ki_out + ctrl.pitch.core.kd_out;

	//*********************************************ƫ��**********************************************//
	yaw_rate_error = ctrl.yaw.shell.pid_out - sensor.gyro.angle.z;
	
	ctrl.yaw.core.kp_out = ctrl.yaw.core.kp * yaw_rate_error;
	
	ctrl.yaw.core.increment += yaw_rate_error;   		
	if(ctrl.yaw.core.increment > ctrl.yaw.core.increment_max)
			ctrl.yaw.core.increment = ctrl.yaw.core.increment_max;
	else if(ctrl.yaw.core.increment < -ctrl.yaw.core.increment_max)
			ctrl.yaw.core.increment = -ctrl.yaw.core.increment_max;
	ctrl.yaw.core.ki_out = ctrl.yaw.core.ki * ctrl.yaw.core.increment; 

	ctrl.yaw.core.kd_out = ctrl.yaw.core.kd * (yaw_rate_last - sensor.gyro.angle.z);
	yaw_rate_last = sensor.gyro.angle.z;
	
	ctrl.yaw.core.pid_out =  ctrl.yaw.core.kp_out + ctrl.yaw.core.ki_out + ctrl.yaw.core.kd_out;
}


