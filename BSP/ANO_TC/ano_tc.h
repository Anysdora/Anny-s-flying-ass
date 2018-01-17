#ifndef __ANO_TC_H
#define __ANO_TC_H

#include "usart.h"
#include "delay.h"

#define BYTE3(_temp) 	(u8)(_temp>>24)
#define BYTE2(_temp) 	(u8)(_temp>>16)
#define BYTE1(_temp) 	(u8)(_temp>>8)
#define BYTE0(_temp) 	(u8)_temp

typedef struct _pid		_pid;

//�������ݣ���λרҵ
extern _pid scapegoat_1, scapegoat_2, scapegoat_3;


//�ɿ�����λ��������̬����rol, pit, yaw, CSB, PRS
//	pit*100 ��ֵ�ķ�Χ��-180*100 ~ 180*100
//	rol*100, yaw*100 ��ֵ�ķ�Χ��-32767 ~ 32768
//	CSB*1000 ��ֵ�ķ�Χ��-32767 ~ 32768 
//	PRS*100 ��ֵ�ķ�Χ��-2^31 ~ 2^31-1	
void Data_Send_Status(float Att_Angle_rol,float Att_Angle_pit,float Att_Angle_yaw,
					   float ALT_CSB, float ALT_PRS);

//�ɿ�����λ�����ʹ���������
void Data_Send_Senser(int16_t Acc_X, int16_t Acc_Y,int16_t Acc_Z,
					  int16_t Gyr_X, int16_t Gyr_Y,int16_t Gyr_Z,
					  int16_t Mag_X, int16_t Mag_Y,int16_t Mag_Z);

//�ɿ�����λ�����͵������ Moto
//PWM��Χ��0~1000
void Data_Send_MotoPWM(int16_t Moto_PWM_1, int16_t Moto_PWM_2,
					   int16_t Moto_PWM_3, int16_t Moto_PWM_4);

//�ɿ�����λ������ң������
void Data_Send_RCData(int16_t Rc_Throttle, int16_t Rc_Yaw,
					  int16_t Rc_Roll, int16_t Rc_Pitch);

//��������λ�������Զ�������ݰ�1 
void Data_Send_DIY1(s16 _temp1,s16 _temp2,s16 _temp3,s32 _temp4);

//��������λ�������Զ�������ݰ�2 
void Data_Send_DIY2(u8 dz,s16 _temp1,s16 _temp2,s16 _temp3,s32 _temp4);

void Data_Send_Check(u16 check);

//������λ�����͹��������ݰ� pid1
void Data_Receive_Pid1(struct _pid *PID_ROL,struct _pid *PID_PIT,struct _pid *PID_YAW);

//������λ�����͹��������ݰ� pid2
void Data_Receive_Pid2(struct _pid *PID_ROL,struct _pid *PID_PIT,struct _pid *PID_YAW);

//������λ�����͹��������ݰ� ��������(��λ���϶�Ӧ���Ǵ�������У��)
void Data_Receive_lock(u8 *armed);

#endif
