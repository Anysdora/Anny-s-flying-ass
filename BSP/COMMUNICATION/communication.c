#include "communication.h"
#include "include.h"

/********************************************************************************************************************/
//���Թ���������λ����������

//����λ����ȡ��������
struct _pid g_trans_data;

//���ʹ��󱨸�
enum _error_report error_report;

//���Ͳ�������
float test_report_data_float;
float test_report_data;
s16 test_report_data_s16;
s16 test_report_data_s16_1;
s16 test_report_data_s16_2;
/********************************************************************************************************************/
extern int16_t MODE;

void UART2_ReportIMU(void)
{
//	�ɿ�����λ��������̬����rol*100, pit*100, yaw*100, CSB*1000, Bar*100
//	pit*100 ��ֵ�ķ�Χ��-180*100 ~ 180*100
//	rol*100, yaw*100 ��ֵ�ķ�Χ��-32767 ~ 32768
//	CSB*1000 ��ֵ�ķ�Χ��-32767 ~ 32768 
//	Bar*100 ��ֵ�ķ�Χ��-2^31 ~ 2^31-1	
//	void Data_Send_Status(float Att_Angle_rol , float Att_Angle_pit,float Att_Angle_yaw,
//						   float High_CSB, float High_Bar);
	
	Data_Send_Status( angle.roll, angle.pitch,roll_offset, pitch_offset, g_height);
	
//	USART_printf( USART3, "%d   %d   %d\n", (int)(angle.roll*100), (int)(angle.pitch*100), (int)(angle.yaw*100));
//	//�ɿ�����λ�����ʹ���������
//	void Data_Send_Senser(int16_t Acc_X, int16_t Acc_Y,int16_t Acc_Z,
//						  int16_t Gyr_X, int16_t Gyr_Y,int16_t Gyr_Z,
//						  int16_t Mag_X, int16_t Mag_Y,int16_t Mag_Z);
	//MODEroad_state_machine
		//MODE*10����ĳ��״̬���еĲ���
	Data_Send_Senser(MODE * 10, 			g_position_circle_adjust.x,		g_position_circle_adjust.y,
					 set_height_control_expect_height,		sensor.gyro.angle.y,	sensor.gyro.angle.z,
					 g_position_line_1_adjust.y,		test_report_data_s16_1 * 10,			test_report_data );


//	//�ɿ�����λ�����͵������ Moto
//	//PWM��Χ��0~1000
//	Data_Send_MotoPWM(Moto_duty[0] , Moto_duty[1] ,
//						   Moto_duty[2] , Moto_duty[3] );
	
//	//�ɿ�����λ������ң������
//	void Data_Send_RCData(int16_t Rc_Throttle, int16_t Rc_Yaw,
//						  int16_t Rc_Roll, int16_t Rc_Pitch);


//	//��������λ�������Զ�������ݰ�1 
//	void Data_Send_DIY1(s16 _temp1,s16 _temp2,s16 _temp3,s32 _temp4);
//	Data_Send_DIY1(Moto_duty[0] , Moto_duty[1] ,
//					 Moto_duty[2] , Moto_duty[3]);

//	//��������λ�������Զ�������ݰ�2 
//	void Data_Send_DIY2(u8 dz,s16 _temp1,s16 _temp2,s16 _temp3,s32 _temp4);
}


//��������ͷ����õ�����λ��
void USART1_IRQHandler(void)                	//����1�жϷ������
{	
	if(USART1->SR & (1<<5))
	{		
//		static u8 i = 0;i++;if(i & 1){LED2(ON);}else{LED2(OFF);}if(i == 100){i = 0;}
	
		USART_ov7620_rxbuf[camera_RX_BUF_point++] = USART1->DR;
		
//		USART2->DR = USART1->DR;
				
		if(camera_RX_BUF_point == 1206)
		{
			camera_RX_BUF_point = 0;				
		}

	}

	USART1->SR=~0x0525;
}


//���ճ���������õ�����߶�
void USART3_IRQHandler(void)                	//����3�жϷ������
{		
	if(USART3->SR & (1<<5))
	{				
		if(g_HL_flag == low)
		{
			g_height_L = USART3->DR;	

			g_KS103_retrans_flag = 1;
		}
		
		if((g_HL_flag == high) && (g_KS103_retrans_flag == 0))
		{
			g_height_H = USART3->DR;	
			
			g_HL_flag = low;
		}
	}
	USART3->SR=~0x0525;
}

//��λ�����͹��������ݰ�
u8 USART_RX_BUF[50] = {0};	//9*2+4+1 == 23

u8 USART_RX_BUF_point = 0;

void USART2_IRQHandler(void)                	//����2�жϷ������
{	 	
	if(USART2->SR & (1<<5))
	{
		USART_RX_BUF[USART_RX_BUF_point++] = USART2->DR;
		
		if(USART_RX_BUF_point == 46)
		{
			USART_RX_BUF_point = 0;				
		}
	}

	USART2->SR=~0x0525;
}

void NRF_key_handle(u8 keyvalue)
{
	switch(keyvalue)
	{
		case 3: 	g_MatrixKey.arm_step_1 = !g_MatrixKey.arm_step_1;	break;
		case 6: 	g_MatrixKey.arm_step_2 = !g_MatrixKey.arm_step_2;	break;
		case 'B': 	g_MatrixKey.arm_step_3 = !g_MatrixKey.arm_step_3;	break;
		case 'A': 	g_MatrixKey.disarm     = !g_MatrixKey.disarm;		break;
		case 0:		break;
		//�ϵ����ٶȼơ������Ǻ�ŷ����У��
		case 1:		g_MatrixKey.calibration_flag.acc_gyro = !g_MatrixKey.calibration_flag.acc_gyro;
					g_MatrixKey.calibration_flag.euler_yaw = !g_MatrixKey.calibration_flag.euler_yaw;
					break;
		case 2:		break;
		case 4: 	break;
		case 5:		break;
		case 7:		break;
		case 8:		break;
		case 9:		break;
		case 'C':	break;
		case 'D':	break;
		case '*':	break;
		case '#':	break;
		default:	break;
	}
}

//NRF24L01 ���ջ�����
u8 NRF24L01_rx_buf[32];

//NFR24L01 IRQ�����ж�
void EXTI3_IRQHandler(void)
{
	if((EXTI->PR & EXTI_Line3) != (uint32_t)RESET)			//�����Ӧ�����ߴ��������Ƿ���
	{
		SPI1_SetSpeed(SPI_BaudRatePrescaler_8); //spi�ٶ�Ϊ9Mhz��24L01�����SPIʱ��Ϊ10Mhz��   
		u8 sta = NRF24L01_Read_Reg(STATUS);  					//��ȡ״̬�Ĵ�����ֵ    	 
		NRF24L01_Write_Reg(WRITE_REG_NRF+STATUS,sta); //���TX_DS��MAX_RT�жϱ�־

		if(sta&RX_OK)//���յ�����
		{			
			NRF24L01_Read_Buf(RD_RX_PLOAD,NRF24L01_rx_buf,RX_PLOAD_WIDTH);//��ȡ����
			NRF24L01_Write_Reg(FLUSH_RX,0xff);//���RX FIFO�Ĵ��� 
		}	   
	}
	
	EXTI->PR = EXTI_Line3;
}
