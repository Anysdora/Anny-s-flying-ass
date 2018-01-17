#ifndef __ULTRASONIC_H_
#define __ULTRASONIC_H_

#include "include.h"

//����߶�
extern float g_height;


extern u8 g_height_H;
extern u8 g_height_L;

//̽��ָ���ط���־
extern u8 g_KS103_retrans_flag;

enum _flag
{
	high,
	low 
};

extern enum _flag g_HL_flag;

void send_sonar_cmd(void);

//�߶������˲�����  �����˲�
float sonar_slide_filter(float height);

//���ͳ���������ָ��
//��ʱ1s
//��ָ�����һ���ϵ�ʱ������
void ultrasonic_init(void);


//��main�ļ��кϳɸ߶�����
void height_composite(float *height);


#endif
