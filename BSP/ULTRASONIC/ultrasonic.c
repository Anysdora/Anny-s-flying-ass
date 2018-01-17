#include "ultrasonic.h"

//����߶�
float g_height = 0;
float g_height_original;

//����2�������Գ�����̽�⵽�ĸ߶ȵĸߵ��ֽ�
u8 g_height_H = 0;
u8 g_height_L = 0;

//��ǰ�ߵ��ֽڱ�־
enum _flag g_HL_flag;

//�ط�ָ���־
u8 g_KS103_retrans_flag = 1;			//1:��ʾ���ط�̽��ָ��

//ͨ������3����������̽��ָ��,ͬʱ����ߵ��ֽ�,����ߵ��ֽڱ�־
//0xe8+0x02+0xb4
//0XB4 : 0-5m��Χ�����¶Ȳ���������mm,̽���ʱԼ87ms.
void send_sonar_cmd(void)
{
	while((USART3->SR&0X40)==0);
	USART3->DR = 0XE8;
	delay_us(80);
	while((USART3->SR&0X40)==0);
	
	USART3->DR = 0X02;
	delay_us(80);
	while((USART3->SR&0X40)==0);
	
	USART3->DR = 0XB4;
	delay_us(80);
	while((USART3->SR&0X40)==0);
		
	g_HL_flag = high;
}

//�߶������˲����� 
//�޷� ��ͻ�䣬 �����˲�
#define data_N 5
float sonar_slide_filter(float height)
{
	static float yuesefu[10] = {0};
	static char cursor = 0;
	float ans = 0;
	static float ans_old = 0;
	
	//�����޷�
	if((height > 350) || (height < 0))
	{
		return ans_old;
	}
	
	yuesefu[cursor] = height;
	
	cursor++;
	if(data_N == cursor)
	{
		cursor = 0;
	}
	
	for(int i = 0; i < data_N; i++)
	{
		ans += yuesefu[i];
	}
	
	ans /= data_N;
	
	//�������ͻ������
	if(ans > 350 || ans < 0)
	{
		ans = ans_old;
	}
	
	ans_old = ans;
	
	return ans;
}

//���ͳ���������ָ��
//��ʱ1s
//��ָ�����һ���ϵ�ʱ������
void ultrasonic_init(void)
{
	while((USART3->SR&0X40)==0);
	USART3->DR = 0XE8;
	delay_us(80);
	while((USART3->SR&0X40)==0);
	
	USART3->DR = 0X02;
	delay_us(80);
	while((USART3->SR&0X40)==0);
	
	USART3->DR = 0X75;						//��߼�����
	delay_us(80);
	while((USART3->SR&0X40)==0);
	
	delay_ms(1500);
}


#define data_N_2 5
float sonar_accel_slide_limit_filter(float height)
{
	static float loop[data_N_2] = {0};
	static u8 cursor = 0;
	
	float ans = 0;
	static float last_ans = 0;
	
	static float last_height = 0;
	static float last_single_height;
	
	//�����޷�
	if((height > 250) || (height < 0))
	{
		return last_ans;
	}
	
	//�½�����ֵ������40cm(���������˶�)
	if(last_height - height > 40)
	{
		return last_ans;
	}
	//��������ֵ������40cm(���ٶȼƵ�����Ϊ+-2g)
	//���ڷɻ�ͻȻ��裬������������ʱ����ֵ���ܳ���40cm
	if(height - last_height > 40)
	{
		//��Ȼ����
		if(height - last_height > 60)
		{
			return 300;
		}
		return last_ans;
	}
	
	if( height > 60 && height - last_single_height > 4) 
	{
		height = height * 0.05 + 0.95 * last_single_height; 
	}
	last_single_height = height; 
	
	loop[cursor] = height;
	
	cursor = (cursor + 1) % data_N_2;
	
	for(int i = 0; i < data_N_2; i++)
	{
		ans += loop[i];
	}
	ans /= data_N_2;
	
	last_ans = ans;
	last_height = ans;
	
	return ans;
}


void height_composite(float *height)//height=&g_height
{
	//�ϳɸ߶�����	��λ:cm		
	 g_height_original = *height = ((g_height_H << 8) | g_height_L) / 10.0 ;
	*height = sonar_accel_slide_limit_filter(g_height);
}

