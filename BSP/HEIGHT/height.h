#ifndef	_HEIGHT_H_
#define _HEIGHT_H_

#include"include.h"

#define height_base 4.0f

enum _height_mode
{
	manually,						
	auto_takeoff,
	set_height,
	auto_land,
};

extern enum _height_mode height_mode;

extern struct _pid auto_land_pid;


//��������ֵ
extern float set_height_control_expect_height;



//΢�������з�ֹ��һ���������bug
extern float auto_land_last_height;

//�Զ�����
void auto_land_control(void);

//ң�ض���ͨ���ж� + ����ģʽ�ж�
void height_mode_judge(void);


struct _throttle_grab
{
	u8 flag;
	int current_throttle;
};
extern struct _throttle_grab auto_land_throttle_grab;
extern struct _throttle_grab set_height_throttle_grab;


//_height_grab.flag == 0˵����δץȡ�߶�
struct _height_grab
{
	u8 flag;
	float current_height;
};
extern struct _height_grab auto_land_height_grab;
extern struct _height_grab set_height_height_grab;



//�߶�ģʽ������
void height_mode_handle(void);

//�Զ�����PID����
void auto_land_pid_set(void);


//���ߺ���, ģ�黯���
void set_height_cascade_control_2(float expect_height, u16 high_speed_time, u16 low_speed_time);

#endif


