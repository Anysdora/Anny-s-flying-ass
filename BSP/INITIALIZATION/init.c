#include "include.h"
#include "init.h"


void IAC_Init(void)
{
	delay_init(72);		//��ʱ������ʼ�� ok
	Nvic_Init();		//�жϳ�ʼ�� ok
	LED_Init();			//led��ʼ��  ok
	OLED_Init();		//oled��ʼ�� ok
	//Draw_LibLogo();   //��LOGO ok
	ADC1_Init();		//AD��ʼ��   ok
	I2C_INIT();			//IIC��ʼ��  ok   PB6 PB7
	KEY_Init();             //   ok


	
	Moto_Init();		  //�����ʼ��     ok
	usart1_config(); 	//����1��ʼ�� --- ����ͷ 115200 ok    PA9 PA10
	usart2_config();	//����2��ʼ�� --- ��λ�� 115200 ok    PD5 PD6                               
	usart3_config(); 	//����3��ʼ�� --- ������ 9600   ok    PD8 PD9
	ultrasonic_init();	//�ó��������봦�����ڶ�ʱ����ʼ��֮ǰ������δ������ɲ��ܷ���̽��ָ��
	TIM3_Init(10000); 	//��ʱ����ʼ�� 10MS
	NRF24L01_Init();
	NRF24L01_Mode(1);	//����2401Ϊ����ģʽ	
	
	delay_ms(500);
	
	mpu_dmp_init();		//dmp��ʼ�� 100hz

}


