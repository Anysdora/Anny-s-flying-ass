#include "include.h" 
#include "camera.h"

//������й����еĳ���
enum _direction fly_direction;


//λ������
struct _position_s16 g_position_circle_receive;          //struct _position_s16
struct _position_float g_position_circle_adjust;				 //{
																											   //		s16 x;
struct _position_s16 g_position_line_1_receive;					 //		s16 y;
struct _position_float g_position_line_1_adjust;	  	   //};

struct _position_s16 g_position_line_2_receive;
struct _position_float g_position_line_2_adjust;


//����ͷ���͹��������ݰ�
//u8 USART_ov7620_rxbuf[9];				//2 + 2 + 1 + 1 + 1 + 1 + 1
u8 USART_ov7620_rxbuf[1206];					//(3 + 600) * 2

u16 camera_RX_BUF_point = 0;


//ͼ�񻺳���
u8 camera_image_buf[603];

enum _new_imgae_flag new_image_flag;


//������ͷģ�鷢�ͻ�ȡͼ��ָ���λ������ָ��
void get_image_or_position_data(enum _get_data_from_camera cmd)
{	
	u16 height_tmp = (u16)g_height;

	if(cmd == get_image)
	{
		if(height_tmp == 201 || height_tmp == 202)
		{
			return ;
		}
		//ֻ������
		USART1->DR = (height_tmp & 0xfe) + 1;
		while((USART1->SR&0X40)==0);
		
	}
	else if(cmd == get_position_data)
	{		
		if(height_tmp == 201 || height_tmp == 202)
		{
			return ;
		}
		
		//ֻ��ż��
		USART1->DR = height_tmp & 0xfe;
		while((USART1->SR&0X40)==0);
	}
	else
	{
		//hello miss white
	}
}


//�����ںϽǶȺ͸߶Ƚ���У��
static void position_data_adjust(struct _position_float *position_circle_dst, const struct _position_s16 *position_circle_src,
								 struct _position_float *position_line_1_dst, const struct _position_s16 *position_line_1_src,
								 struct _position_float *position_line_2_dst, const struct _position_s16 *position_line_2_src)
{
	//?
	static struct _position_float position_circle_dst_tmp;
	struct _position_s16 position_circle_src_tmp = *position_circle_src;

	//??1
	static struct _position_float position_line_1_dst_tmp;
	struct _position_s16 position_line_1_src_tmp = *position_line_1_src;
	
	//??2
	static struct _position_float position_line_2_dst_tmp;
	struct _position_s16 position_line_2_src_tmp = *position_line_2_src;
	
	static float pit[5] = {0};
	static float roll[5] = {0};

	static struct _position_s16 position_circle_last;
	static struct _position_s16 position_line_1_last;
	static struct _position_s16 position_line_2_last;

	u8 i;

	for(i = 4; i > 0; i--)
	{
		pit[i] = pit[i-1];
		roll[i] = roll[i-1];	
	}
	pit[0] = angle.pitch;
	roll[0] = angle.roll;

	if(position_circle_src_tmp.x != position_circle_last.x || position_circle_src_tmp.y != position_circle_last.y)
	{
		position_circle_last.x = position_circle_src_tmp.x ;
		position_circle_last.y = position_circle_src_tmp.y ;
		
		position_circle_dst_tmp.x = position_circle_src_tmp.x  + 4 * ( 0.4 * pit[2] + 0.6 * pit[3]);
		position_circle_dst_tmp.y = position_circle_src_tmp.y  + 4 * ( 0.4 * roll[2] + 0.6 * roll[3]);
	}
		
	position_circle_dst->x = 0.5 * position_circle_dst->x + 0.5 * position_circle_dst_tmp.x;
	position_circle_dst->y = 0.5 * position_circle_dst->y + 0.5 * position_circle_dst_tmp.y;
	
	
	if(position_line_1_last.y != position_line_1_src_tmp.y)
	{

		position_line_1_last.y = position_line_1_src_tmp.y;

		position_line_1_dst_tmp.x = position_line_1_src_tmp.x;	//  + 4 * ( 0.4 * pit[2] + 0.6 * pit[3]);
		position_line_1_dst_tmp.y = position_line_1_src_tmp.y  + 4 * ( 0.4 * roll[2] + 0.6 * roll[3]);
	}

	position_line_1_dst->x = position_line_1_dst_tmp.x;
	position_line_1_dst->y = position_line_1_dst_tmp.y;

	if(position_line_2_last.y != position_line_2_src_tmp.y)
	{
		position_line_2_last.y = position_line_2_src_tmp.y;

		position_line_2_dst_tmp.x = position_line_2_src_tmp.x;	//  + 4 * ( 0.4 * pit[2] + 0.6 * pit[3]);
		position_line_2_dst_tmp.y = position_line_2_src_tmp.y  + 4 * ( 0.4 * roll[2] + 0.6 * roll[3]);
	}

	position_line_2_dst->x = position_line_2_dst_tmp.x;
	position_line_2_dst->y = position_line_2_dst_tmp.y;
}
//�޷���ͻ���˲�
struct _position_s16 camera_limit_filter(struct _position_s16 position)
{
	static struct _position_s16 last_position = {0, 0};
	struct _position_s16 tmp = position;
	
	//�����޷�
	if(position.x > 240.0f)
	{
		tmp.x = 240.0f;
	}		
	if(position.x < -240.0f)
	{
		tmp.x = -240.0f;
	}		
	
	if(position.y > 320.0f)
	{
		tmp.y = 320.0f;
	}	
	if(position.y < -320.0f)
	{
		tmp.y = -320.0f;
	}		
	
//	//���ݱ仯����ֵ������100
//	if(last_position.x - position.x > 60.0f || last_position.x - position.x < -60.0f ||
//	   last_position.y - position.y > 60.0f || last_position.y - position.y < -60.0f)
//	{
//		return last_position;
//	}
//	last_position = tmp;
	
	return tmp;
}


void position_data_decode(u8 *data_array, u16 *ov7620_rxbuf_cursor, struct _position_float *position_circle, struct _position_float *position_line_1, struct _position_float *position_line_2)
{
	u16 sum = 0;
	//��У������
	for(int i = 0; i < 8; i++)
	{
		sum += data_array[i];
	}
	
	u8 sum_verify = 0xff & sum;
	
	//���������⣬ �ط��� ������һ�ε�����
	if(sum_verify != data_array[8])
	{	
		get_image_or_position_data(get_position_data);
		
		//�α�����
		*ov7620_rxbuf_cursor = 0;				

		return;
	}
	
	//�ϳ�λ������......
	
	//·��,״̬��
	road_state_machine = (road_state_machine_type)((data_array[0] >> 3) & 0x0f);
	
	//���������Բ�ĵ�λ��
	g_position_circle_receive.y = ( data_array[0] & 0x07) << 8;
	g_position_circle_receive.y += data_array[1] & 0xff;
	
	g_position_circle_receive.y <<= 2;
	g_position_circle_receive.y *= (data_array[0] >> 7) > 0 ? -1 : 1;
	
	g_position_circle_receive.x = ( data_array[2] & 0x07) << 8;
	g_position_circle_receive.x += data_array[3] & 0xff;
	
	g_position_circle_receive.x <<= 2;
	g_position_circle_receive.x *= (data_array[2] >> 7) > 0 ? -1 : 1;
	
	//���������ֱ��(���˵�)��λ��
	g_position_line_1_receive.x = (data_array[4] & 0x7f) * ((data_array[4] >> 7) > 0 ? -1 : 1) * 4;
	g_position_line_1_receive.y = (data_array[5] & 0x7f) * ((data_array[5] >> 7) > 0 ? -1 : 1) * 4;
	
	
	
	g_position_line_2_receive.x = (data_array[6] & 0x7f) * ((data_array[6] >> 7) > 0 ? -1 : 1) * 4;
	g_position_line_2_receive.y = (data_array[7] & 0x7f) * ((data_array[7] >> 7) > 0 ? -1 : 1) * 4;
	
	//�α�����
	*ov7620_rxbuf_cursor = 0;		


//	USART_printf(USART2, "%d  %d  %d  %d  %d  %d  %d  \n", (u8)road_state_machine, g_position_circle_receive.x, g_position_circle_receive.y, 
//					g_position_line_1_receive.x, g_position_line_1_receive.y, g_position_line_2_receive.x, g_position_line_2_receive.y);


	//�������5���Ҳ���Բ��˵���Ѿ�ͼ����ȷʵû��Բ��Ӧ�����ط�
	static u8 cannot_find_circle_count = 0;
	
	static struct _position_s16 position_last;
	
	//����Ҳ������ݣ���������һ�ε�����
	if(g_position_circle_receive.x == 128 && g_position_circle_receive.y == 168)
	{
		g_position_circle_receive = position_last;

		//ֱ�ӷ��أ������нǶȡ��߶�У��
		//g_position_circle_receive��g_position_adjust��������һ�ε�����
		
		if(cannot_find_circle_count == 5)
		{
			//do something
			cannot_find_circle_count = 0;
		}
		
//��Ҫ��return, ��֪����, ���return��ֻ��ֱ��û��Բ��ʱ��, ��֪���ж���鷳��, �˷���1���ϵ�ʱ��
//		return;
	}
	

	
	
	position_last= g_position_circle_receive;

	//��δ��⵽Բ�������жϣ�����
	cannot_find_circle_count = 0;

	
	g_position_circle_receive = camera_limit_filter(g_position_circle_receive);
	
	
	//����У��
	position_data_adjust(position_circle, &g_position_circle_receive, 
						 position_line_1, &g_position_line_1_receive, 
						 position_line_2, &g_position_line_2_receive);


}


void camera_data_image_decode(u8 *ov7620_rxbuf, u16 *ov7620_rxbuf_cursor)
{
	u16 j = 0 , i = 0;
	u8 *data_buf;

	data_buf = ov7620_rxbuf;
	
	if(display_what == param)
	{
		for(j = 0; j < 600; j++)
		{
			if(	*(data_buf + j) == 0x13 
				&& *(data_buf + j + 1) == 0x14 
				&& *(data_buf + j + 2) == 0X45 )//????	
			{
				data_buf = data_buf + j;	
				break;
			}		
		}
			*data_buf = 0; 
			data_buf = data_buf + 3;	

			position_data_decode(data_buf, ov7620_rxbuf_cursor, &g_position_circle_adjust, &g_position_line_1_adjust, &g_position_line_2_adjust);
	}
	
	else if(display_what == image)
	{
			for(j = 0; j < 600; j++)
			{
				if(	*(data_buf + j) == 0x13 
					&& *(data_buf + j + 1) == 0x14 
					&& *(data_buf + j + 2) == 0X41 )//????	
				{
					data_buf = data_buf + j;	
					break;
				}		
			}
			
			if(*(data_buf+2) == 0X41)								//image
			{	 
				if(new_image_flag == coming)
				{		
					
					for( i = 0; i < 600; i++)
					{
						camera_image_buf[i] = *(data_buf + 3 + i);
						
						delay_us(95);		
					}					
					
					memset(USART_ov7620_rxbuf, 0, sizeof(USART_ov7620_rxbuf));

					new_image_flag = came;
					
					*ov7620_rxbuf_cursor = 0;
				}
			}
		}

}




//��ֱ�߶˵������Բ�ĵĽǶ�
float get_angle_of_line_relative_to_circle(float circle_center_x, float circle_center_y, float line_endpoint_x, float line_endpoint_y)
{
	float angle;
	float relative_x = -line_endpoint_x - (-circle_center_x);
	float relative_y = -line_endpoint_y - (-circle_center_y);

	angle = atan2(relative_y, relative_x);

	return angle * 57.296f;
}

struct _expect_yaw_grab expect_yaw_grab = {0, 0};

float get_yaw_expect(float circle_center_x, float circle_center_y, float line_endpoint_x, float line_endpoint_y)
{
	float angle_of_line_relative_to_circle;
	angle_of_line_relative_to_circle = get_angle_of_line_relative_to_circle(circle_center_x, circle_center_y, line_endpoint_x, line_endpoint_y);
		
	if(angle_of_line_relative_to_circle < 90 && angle_of_line_relative_to_circle > -90)
	{
		fly_direction = backward;
		
		return angle_of_line_relative_to_circle;
	}
	else if(angle_of_line_relative_to_circle >= 90 && angle_of_line_relative_to_circle < 180)
	{
		fly_direction = forward;

		return angle_of_line_relative_to_circle - 180;
	}
	else if(angle_of_line_relative_to_circle < -90 && angle_of_line_relative_to_circle > -180)
	{
		fly_direction = forward;
		
		return angle_of_line_relative_to_circle + 180;
	}
	else
	{
		return 0;
	}
}

u8 yaw_expect_wave(float yaw_expect_temp, float *yaw_off)
{
	const float steps_p = 2.5f;
	const float steps_n = -2.5f;
	
	static float ret = 0.0f;
	
	
	
	if(yaw_expect_temp >= 0)
	{
		if(ret < yaw_expect_temp)
		{
			ret += steps_p;
			
		  *yaw_off += steps_p;
			
			if(*yaw_off>180)
			{
				*yaw_off=(*yaw_off-360);
			}
			
			return 0;
		}
		else
		{
			*yaw_off += steps_p;
			
			ret = 0;
			
			return 1;
		}
	}
	else   //yaw_expect_temp<0
	{
		if(ret > yaw_expect_temp)
		{
			ret += steps_n;
			
			*yaw_off += steps_n;
			if(*yaw_off<-180)
			{
				*yaw_off=(*yaw_off+360);
			}
			
			return 0;
		}
		else
		{
			*yaw_off += yaw_expect_temp - (ret - steps_n);
			
			ret = 0;
			
			return 1;
		}
	}
}

