#include"include.h"
#include"height.h"

//����ģʽ	Ĭ���ֿ�
enum _height_mode height_mode;
enum _height_mode height_mode_last;


//�����ڻ��˲�
#define KALMAN_Q        0.2
#define KALMAN_R        6.0000
/*           ����������������ٶȽ����˲�����           */
static double KalmanFilter_speed(const double ResrcData,double ProcessNiose_Q,double MeasureNoise_R)
{
	double R = MeasureNoise_R;
	double Q = ProcessNiose_Q;
	static double x_last;
	double x_mid = x_last;
	double x_now;
	static double p_last;
	double p_mid ;
	double p_now;
	double kg;        

	x_mid=x_last; 			//x_last=x(k-1|k-1),x_mid=x(k|k-1)  //x_last:��һ�ε�����Ԥ��ֵ��x_mid:���ڴ���һ������Ԥ��ֵ
	p_mid=p_last+Q; 		//p_mid=p(k|k-1),p_last=p(k-1|k-1),Q=����   //p_mid�����δ���һ������Э���p_last:�ϴε�Э����
	kg=p_mid/(p_mid+R); 	//kgΪkalman filter��RΪ����     	//kg����������   ResrcData�������Ĳ���ֵ
	x_now=x_mid+kg*(ResrcData-x_mid);//���Ƴ�������ֵ    		//���ε�����Ԥ��ֵ
		
	p_now=(1-kg)*p_mid;		//����ֵ��Ӧ��covariance        	//���ε�Э����
	p_last = p_now; 		//����covarianceֵ               	//����������ɵݹ�
	x_last = x_now; 		//����ϵͳ״ֵ̬
	return x_now;   		//�������Ź���ֵ          
 }

/**************************************************************************************************************/
//��  �ܣ���ɡ����� ����ֵ�趨���ߣ���Ϊ���������׶κͻ��������׶�
//��  ����step_high_time��  ��������ʱ��Ϊ����λ n * 40 ��ms��
//		��step_low_time :   ��������ʱ��Ϊ����λ n * 40 ��ms��
//		: zero_height   : 	����ֵ�ı�˲��߶�
//		: expect_height :	���������߶�
//˵  ��: �˺�����ͬһ�������߶���ֻ�ܵ���һ�Ρ�
/**************************************************************************************************************/
static float wave_expect_height(float expect_height, float zero_height, u16 step_high_time, u16 step_low_time)
{
	//��last_height == 0ʱ, ���expect_height��һ��Ϊ0, ������bug.
	static float last_height = -1314;

	static s16 high_time_count = 1;							//high_speed
	static s16 low_time_count = 1;							//low_speed
	
	float ret_ept_hgt = zero_height;

	if(last_height != expect_height)
	{		
		last_height = expect_height;
		
		high_time_count = 1;
		low_time_count = 1;
	}	
	
	float step1_distance = (expect_height - zero_height) * 0.75;
	float step2_distance = (expect_height - zero_height) * 0.25;
	float high_time_count_max = step_high_time;
	float low_time_count_max =  step_low_time;

	if(high_time_count <= high_time_count_max)
	{
		ret_ept_hgt += step1_distance / step_high_time * (high_time_count++);
	}
	else if(low_time_count <= low_time_count_max)
	{
		ret_ept_hgt += step1_distance + step2_distance / step_low_time * (low_time_count++);
	}
	else
	{
		ret_ept_hgt = expect_height;
	}

	return ret_ept_hgt;
}

/************************************************************************/
/*void auto_land_control(void)											*/
/*���Ʋ��ԣ�����ȫ΢�ֿ���	--  pid_out��ͨ�˲�							*/
/*���룺			                               						*/
/*�����                                             					*/
/************************************************************************/
#define auto_land_alfa 0.1
struct _pid auto_land_pid;

struct _height_grab auto_land_height_grab = {0, 0};

float auto_land_last_height;

void auto_land_control(void)
{
	float error;

	float auto_land_pid_out;
		
	/********************************************************************************
	//����һ��ץȡ����ʱ����static��������, ��ֹ��ʷ�������ݶԵ�ǰ���Ʋ���Ӱ��
	//auto_land_throttle_grab.flag������ץȡ���� 1
	if(auto_land_throttle_grab.flag == 0)
	{
		auto_land_pid.pid_out = 0;
	}
	********************************************************************************/	

	
	//����ץȡ	
	if(auto_land_throttle_grab.flag == 0)
	{
		auto_land_throttle_grab.flag = 1;
		
		//�ɶ��������Զ�����
//		auto_land_throttle_grab.current_throttle = set_height_throttle_grab.current_throttle + ctrl.height.core.pid_out;
	}
		
	//�߶�ץȡ
	if(auto_land_height_grab.flag == 0)	
	{	
		auto_land_height_grab.flag = 1;
	
		auto_land_height_grab.current_height = g_height;
	}

	//����ֵ���� ���������Զ�����֮ǰ
	float expect_height = wave_expect_height(-30 , auto_land_height_grab.current_height, 100, 100);
/*************************************************************************************************/
	test_report_data_s16 = expect_height;
/*************************************************************************************************/
	
	//�Զ�����ģʽ�£��߶ȵ���10cm�Զ�������
	if(g_height <= 10)
	{	
		ARMED = 0;
		
		return ;
	}
	
	//
	auto_land_pid_set();

	error = expect_height - g_height;
	
	auto_land_pid.kp_out = auto_land_pid.kp * error;
	
	auto_land_pid.increment += error;
	if(auto_land_pid.increment > auto_land_pid.increment_max)
	{
		auto_land_pid.increment  = auto_land_pid.increment_max;
	}
	else if(auto_land_pid.increment < -auto_land_pid.increment_max)
	{
		auto_land_pid.increment = -auto_land_pid.increment_max;
	}
	auto_land_pid.ki_out = auto_land_pid.ki * auto_land_pid.increment;
	
	auto_land_pid.kd_out = auto_land_pid.kd * (auto_land_last_height - g_height);
	
	auto_land_last_height = g_height;
	
	auto_land_pid_out = auto_land_pid.kp_out 
						  + auto_land_pid.ki_out
						  + auto_land_pid.kd_out;
	
	//һ�׵�ͨ�˲�
	auto_land_pid_out = auto_land_alfa * auto_land_pid_out  + (1 - auto_land_alfa) * auto_land_pid.pid_out;			  
						  
	auto_land_pid.pid_out = auto_land_pid_out;
}

//����������ֵ�趨
static float step_expect_height(float expect_height, float current_height)
{
	float return_expect_height;
		
	if(current_height < expect_height)
	{
		switch((int)(current_height / 20))
		{
			case 0:		return_expect_height = 20;	break;
			case 1:		return_expect_height = 40;	break;
			case 2:		return_expect_height = 60;	break;
			case 3:		return_expect_height = 80;	break;
			case 4:		return_expect_height = 100;	break;
			case 5:		return_expect_height = 120;	break;
			case 6:		return_expect_height = 140;	break;
			case 7:		return_expect_height = 160;	break;
			case 8:		return_expect_height = 180;	break;
			case 9:		return_expect_height = 200;	break;
			case 10:	return_expect_height = 220;	break;
			case 11:	return_expect_height = 240;	break;
			case 12:	return_expect_height = 260;	break;
			case 13:	return_expect_height = 280;	break;
			case 14:	return_expect_height = 300;	break;
			default:	return_expect_height = 0;	break;	
		}
	}
	else if(current_height > expect_height)
	{
		switch((int)(current_height / 20))
		{
			case 1:		return_expect_height = 20;	break;
			case 2:		return_expect_height = 40;	break;
			case 3:		return_expect_height = 60;	break;
			case 4:		return_expect_height = 80;	break;
			case 5:		return_expect_height = 100;	break;
			case 6:		return_expect_height = 120;	break;
			case 7:		return_expect_height = 140;	break;
			case 8:		return_expect_height = 160;	break;
			case 9:		return_expect_height = 180;	break;
			case 10:	return_expect_height = 200;	break;
			case 11:	return_expect_height = 220;	break;
			case 12:	return_expect_height = 240;	break;
			case 13:	return_expect_height = 260;	break;
			case 14:	return_expect_height = 280;	break;
			case 15:	return_expect_height = 300;	break;
			default:	return_expect_height = 0;	break;	
		}
	}
	else
	{
		return_expect_height = expect_height;
	}
	
	if((expect_height - return_expect_height > -19.99) && (expect_height - return_expect_height < 0))
	{
		return expect_height;
	}
	
	return return_expect_height;

}

static u8 set_height_start_control(void)   //���Ż�׼����
{
	static u8 flag = 0;

/********************************************************************************/
	//ֻץȡһ��
	if(set_height_throttle_grab.flag == 0)
	{
		set_height_throttle_grab.flag = 1;
		
		set_height_throttle_grab.current_throttle = 1200;    //��ǰ����1200
		flag = 1;                                            //flag��1
	}
/********************************************************************************/

	if(flag)
	{
		if(g_height < height_base+5)               //#define height_base 4.0f
		{
			
			if(set_height_throttle_grab.current_throttle <1250 && set_height_throttle_grab.current_throttle >1000)
			{
				set_height_throttle_grab.current_throttle++;
			}
			else 
			{
				set_height_throttle_grab.current_throttle += 5.7;	
			}
			return 0;        					           							//��׼�������
		}
		else
		{
			//��Ҷ��
			set_height_throttle_grab.current_throttle += 16;
			flag = 0;
		}
	}
	return 1;
}

//��������ֵ
float set_height_control_expect_height = 0;


//����������ֵ�趨
struct _height_grab set_height_height_grab = {0, 0};

static void set_height_cascade_control(float expect_height)
{
	static u8 control_rate;
	
	float error;
	static float shell_last_height;
	
	if(!set_height_start_control())    //���Ż�׼����δ���
	{
		return;
	}

	//����ֵ����
	expect_height = wave_expect_height(expect_height, set_height_height_grab.current_height, 100, 100);
/*************************************************************************************************/
	test_report_data_s16 = expect_height;
/*************************************************************************************************/
		
	if(control_rate == 2)
	{	
		error = expect_height - g_height;

		ctrl.height.shell.increment += error;
//		//�����޷�
		if(ctrl.height.shell.increment > ctrl.height.shell.increment_max)
			ctrl.height.shell.increment = ctrl.height.shell.increment_max;
		else if(ctrl.height.shell.increment < -ctrl.height.shell.increment_max)
			ctrl.height.shell.increment = -ctrl.height.shell.increment_max;
		
		ctrl.height.shell.pid_out = ctrl.height.shell.kp * error 
								  + ctrl.height.shell.ki * ctrl.height.shell.increment 
								  + ctrl.height.shell.kd * (shell_last_height - g_height);//΢������			//΢������
		shell_last_height = g_height;

		control_rate = 0;
	}
	control_rate++;
	
	float height_core_error;
	static float core_last_error;
	float vertical_speed;
	static float core_last_height;
	
	vertical_speed = (g_height - core_last_height) / 0.04;							// 	cm/s
	core_last_height = g_height;
	
	height_core_error = ctrl.height.shell.pid_out - vertical_speed;
	
	ctrl.height.core.kp_out = ctrl.height.core.kp * height_core_error;

	ctrl.height.core.increment += height_core_error;   		
	if(ctrl.height.core.increment > ctrl.height.core.increment_max)
			ctrl.height.core.increment = ctrl.height.core.increment_max;
	else if(ctrl.height.core.increment < -ctrl.height.core.increment_max)
			ctrl.height.core.increment = -ctrl.height.core.increment_max;
	ctrl.height.core.ki_out = ctrl.height.core.ki * ctrl.height.core.increment; 
	
	//΢������
	ctrl.height.core.kd_out = ctrl.height.core.kd * (height_core_error - core_last_error);
	
	core_last_error = height_core_error;
	
	ctrl.height.core.pid_out = ctrl.height.core.kp_out 
							 + ctrl.height.core.ki_out 
							 + ctrl.height.core.kd_out;	
}


void set_height_cascade_control_2(float expect_height, u16 high_speed_time, u16 low_speed_time)//����PID����
{
	float error; 
	static float shell_last_height;     //�⻷֮ǰ�߶�
	static u8 control_rate = 0 ;
	
	float height_core_error;              
	static float core_last_speed;
	float vertical_speed;               //��ֱ�ٶ�
	static float core_last_height;      //�ڻ�֮ǰ�߶�
	
	if(g_height == 300)
	{
		return;
	}
	if(fabs(core_last_height-g_height)>15)//�Ż�1.0
	{
		g_height=core_last_height;
	}
	//�����������ݲ�������

	
/********************************************************************************/
	//����һ��ץȡ����ʱ����static��������, ��ֹ��ʷ�������ݶԵ�ǰ���Ʋ���Ӱ��
	//set_height_throttle_grab.flag��set_height_start_control()���� 1
	if(set_height_throttle_grab.flag == 0)
	{
		shell_last_height = g_height;    
		core_last_speed = 1.1;
		core_last_height = g_height;
	}
/********************************************************************************/	
	
	if( !set_height_start_control() )    //���Ż�׼����δ���
	{
		return;
	}
	
	//�߶�ץȡ, Ϊ��������ֵ������׼��,
	//һ������ֻץһ��
	if(set_height_height_grab.flag == 0)
	{	
		set_height_height_grab.flag = 1;
	
		set_height_height_grab.current_height = g_height;
	}

	
	//����ֵ����
	expect_height = wave_expect_height(expect_height, set_height_height_grab.current_height, high_speed_time, low_speed_time);
/*************************************************************************************************/
	test_report_data_s16 = expect_height;
/*************************************************************************************************/
	
	//�⻷ -- �߶�
	control_rate++;
	if(control_rate == 2)
	{
			control_rate = 0;
		error = expect_height - g_height;

		ctrl.height.shell.increment += error;

	//		//�����޷�
		if(ctrl.height.shell.increment > ctrl.height.shell.increment_max)
			ctrl.height.shell.increment = ctrl.height.shell.increment_max;
		else if(ctrl.height.shell.increment < -ctrl.height.shell.increment_max)
			ctrl.height.shell.increment = -ctrl.height.shell.increment_max;

		ctrl.height.shell.pid_out = ctrl.height.shell.kp * error 
									+ ctrl.height.shell.ki * ctrl.height.shell.increment 
									+ ctrl.height.shell.kd * (shell_last_height - g_height);//΢������			//΢������
		shell_last_height = g_height;
	
	}

	//�ڻ� -- �ٶ�
	vertical_speed = (g_height - core_last_height) ;							// 	(*25cm/s)
	
//	vertical_speed = KalmanFilter_speed(vertical_speed, KALMAN_Q, KALMAN_R);
//	
	//ֻ������ֵ����ʹ�õ�ͨ�˲�
//	if(g_height - expect_height < 20 && g_height - expect_height > -20)
//	{
//		vertical_speed = 0.95 * core_last_speed  + 0.05 * vertical_speed;
//	}
	
	core_last_height = g_height;		
	
	height_core_error = ctrl.height.shell.pid_out - vertical_speed;
	
	ctrl.height.core.kp_out = ctrl.height.core.kp * height_core_error;

	ctrl.height.core.increment += height_core_error;   		
	if(ctrl.height.core.increment > ctrl.height.core.increment_max)
			ctrl.height.core.increment = ctrl.height.core.increment_max;
	else if(ctrl.height.core.increment < -ctrl.height.core.increment_max)
			ctrl.height.core.increment = -ctrl.height.core.increment_max;
	
	
	if(error > 5 || error < -5)
	{

		ctrl.height.core.increment += 2.5 * height_core_error;   		
	}
	else
	{
		ctrl.height.core.increment += height_core_error; 
	}
	
	ctrl.height.core.ki_out = ctrl.height.core.ki * ctrl.height.core.increment; 
	
	//΢������
	ctrl.height.core.kd_out = ctrl.height.core.kd * (core_last_speed - vertical_speed);
	core_last_speed = vertical_speed;
	
	ctrl.height.core.pid_out = ctrl.height.core.kp_out 
							 + ctrl.height.core.ki_out 
							 + ctrl.height.core.kd_out;	

}


//ң�ض���ͨ���жϼȸ߶�ģʽ�ж�
void height_mode_judge(void)
{
	if(Rc_Data_T8_C3 < 1100 && Rc_Data_T8_C3 > 1080)				//0
	{		
		mode_machine = mode_none;
	}
	else if(Rc_Data_T8_C3 < 1500 && Rc_Data_T8_C3 > 1480)			//1
	{	
		//�����Զ����̿���
		mode_machine = wait_setheight_spin_land;
	}
	else if(Rc_Data_T8_C3 < 1900 && Rc_Data_T8_C3 > 1880)			//2														//�Զ���� + ����
	{		
		mode_machine = force_to_auto_land;
	}
	else															//ң����ͨʱ�޴�ѡ��
	{	
		//do nothing
	}
	
	if(!ARMED)
	{
		mode_machine = mode_none;
	}
}

//�Զ�����PID����
void auto_land_pid_set(void)
{
	if(g_height > 100)
	{
		auto_land_pid.kd  = 4;
	}
	else if(g_height > 40)
	{
		auto_land_pid.kd = 2;
	}
	else
	{
//		auto_land_pid.kp = 0.8;
		auto_land_pid.kd  = 0;
	}
}

//height_variable_reset
//�붨���йر�����ʼ��
void height_variable_reset()
{
	if(height_mode != set_height)
	{
		set_height_throttle_grab.flag = 0;
		
		ctrl.height.core.pid_out = 0;
		ctrl.height.shell.pid_out = 0;
	}
	
	if(height_mode != auto_land)
	{
		auto_land_throttle_grab.flag = 0;
		auto_land_height_grab.flag = 0;
		
		auto_land_pid.pid_out = 0;
		auto_land_pid.increment = 0;
		
		auto_land_last_height = g_height;
	}	
}
/************************************************************************/
/*void height_mode_handle(void)											*/
/*���ԣ����������߶��йصĿ���										*/
/*�ؼ����⣺��ץȡ��ǰ���� �ڻ��֡���־����              				*/
/*���룺			                               						*/
/*�����                                             					*/
/************************************************************************/
struct _throttle_grab auto_land_throttle_grab = {0, 0};
struct _throttle_grab set_height_throttle_grab = {0, 0};

void height_mode_handle(void)
{
	height_variable_reset();
	
	if(height_mode == manually)
	{
		//�˺������ڸ����ڲ�static����
		float expect = wave_expect_height(99, 100, 10, 10);
	}
	else if(height_mode == auto_land)
	{
//		//����ץȡ	
//		if(auto_land_throttle_grab.flag == 0)
//		{
//			auto_land_throttle_grab.flag = 1;
//			
//			//�ɶ��������Զ�����
//			auto_land_throttle_grab.current_throttle = set_height_throttle_grab.current_throttle;
//		}
		
//		//�߶�ץȡ
//		if(auto_land_height_grab.flag == 0)	
//		{	
//			auto_land_height_grab.flag = 1;
//		
//			auto_land_height_grab.current_height = g_height;
//		}

		
		//�Զ��������
//		auto_land_control();
	}
	else if(height_mode == set_height)
	{
		//����ץȡ
//		if(set_height_throttle_grab.flag == 0)
//		{
//			set_height_throttle_grab.flag = 1;
//		}
		
		
		//���ߴ���pid
//		set_height_cascade_control_2(set_height_control_expect_height);
		
	}
	else
	{
		//do nothing
	}
	
//	height_mode_last = height_mode;
}

