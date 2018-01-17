#ifndef __IMU_H
#define	__IMU_H
#include "stm32f10x.h"
#include "include.h"

#define RtA 	57.324841		//  180/3.1415  �Ƕ��� ת��Ϊ������		
#define AtR    	0.0174533		//  1/RtA             RtA����		
#define Acc_G 	0.0011963		//  1/32768/4/9.8     ���ٶ�����Ϊ4G		
#define Gyro_G 	0.03051756	//  1/32768/1000      ����������Ϊ +��1000			
#define Gyro_Gr	0.0005326   //  1/32768/1000/57.3 

#define halfT 0.00125f                 // �������ڵ�һ��  ������ 2.5MS �ɼ�һ��  ���� halfT��1.25MS

extern float Kp;
extern float Ki;

struct _angle
{
	float pitch;
	float roll;
	float yaw;
};

extern struct _angle angle;

//�ϵ��ŷ����ֵ�ȶ����yawУ��
extern float yaw_offset;

void Prepare_Data(void);
void Get_Attitude(void);
void IMUupdate(float gx, float gy, float gz, float ax, float ay, float az);
//ŷ����yawУ��
void euleryaw_correction(void);

double KalmanFilter(const double ResrcData,double ProcessNiose_Q,double MeasureNoise_R);

float SIN(float y);
float FL_ABS(float x);

#endif













