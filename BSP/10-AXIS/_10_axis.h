#ifndef __10_AXIS_H_
#define __10_AXIS_H_

#include "include.h"

#define RtA 	57.29578f		//  180/3.1415  	������ת��Ϊ�Ƕ���	
#define AtR    	0.017453f		//  1/RtA			RtA����		
#define Gyro_G  0.061035f		//  1/32768/2000	����������Ϊ +��2000			

struct _axis
{
	s16 x;
	s16 y;
	s16 z;
};

struct _trans
{
	struct _axis origin;		//ԭʼֵ
	struct _axis radian;		//����ֵ 
	struct _axis angle;		//�Ƕ�ֵ
};

struct _sensor
{   
	struct _trans accel;
	struct _trans gyro;
};

extern struct _sensor sensor;	


struct _angle
{
	float pitch;
	float roll;
	float yaw;
};

extern struct _angle angle;


void angle_calculation(signed long *quat, struct _angle *angle);

void sensor_gyro_calculation(signed short *gyro, struct _trans *sensor_gyro);

void sensor_accel_calculation(signed short *accel, struct _trans *sensor_accel);


#endif

