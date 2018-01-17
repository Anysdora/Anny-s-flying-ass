#ifndef	_POSITION_H_
#define _POSITION_H_

#include "include.h"


//��Ϊ����ͷ�����������������, ����ͼ�񴰿�Ҳ����������, ����Ϊ��ʹ��������Ķ�׼Բ, �ı�����ֵ
extern struct _position_float set_point_expect_position;



extern float pitch_offset;
extern float roll_offset;
extern float yaw_offset;
extern float yaw_offset_tmp;

enum _position_mode
{
	libertine,
	locate,
	track,
};

extern enum _position_mode position_mode;


//ң��ˮƽ��λͨ���жϼȸ߶�ģʽ�ж�
void position_mode_judge(void);

//��ˮƽ��λ��ص�������--���ӡ���λ��ѭ��
void position_mode_handle(void);


//���㣺����·pid
extern struct _pid locate_single_loop_pid_x;
extern struct _pid locate_single_loop_pid_y;

void locate_single_loop_pid_control(float coordinate_x, float coordinate_y);
//void locate_cascade_control(float coordinate_x, float coordinate_y);

//ѭ�����������޷�
void track_output_limit(float max, float *pitch);


//������̬�ж�
//
//@brief :	����n���ж����������Բ�ĵ�����current_position�Ƿ�����̬��Χ��
//
//@param :	1.���������Բ�ĵ�����
//			2.�������������
//		 	3.�жϵĴ���
//		 	4.��̬����
//
//@retval:	1--�Ѵﵽ��̬
//			0--��û�ﵽ��̬
//
//@note	 :
//
u8 stady_state_judge(struct _position_float current_position, struct _position_float expect_position, u16 n_times, u16 steady_threshold);


#endif

