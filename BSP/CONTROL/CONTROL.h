#ifndef _CONTROL_H_
#define _CONTROL_H_

#include "stm32f10x.h"
#include "_10_axis.h"
#include "include.h"


//����������
void MOTO_output_handle(void);
	

void Deblocking(void);

//NRF24L01 ��������
void arm_disarm(void);

//����ʧ��pitch, roll, yaw����һ���Ƕ�ʱ���Զ�����
//��ֹ���̫�󣬳��������¹�
void out_of_control_lock(const struct _angle *current_angle);

#endif
