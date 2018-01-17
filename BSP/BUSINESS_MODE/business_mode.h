#ifndef _BUSINESS_MODE_H_
#define _BUSINESS_MODE_H_

#include "include.h"
#include "state_machine.h"



typedef enum _mode_flow_step
{
	mode_step_none,
	mode_step_1,
	mode_step_2,
	mode_step_3,
	mode_step_4,
	mode_step_5,
	mode_step_6,
	mode_step_7,
	mode_step_8,
	mode_step_9,
	mode_step_10,
	mode_step_11,
	mode_step_12,
	mode_step_13,
	mode_step_14,
	mode_step_15,
	mode_step_16,
	mode_step_17,
	mode_step_18,
	mode_step_19,
}mode_flow_step_type;



//@brief :	ϵͳ��n�ε���wait_for_n_ms����, 
//			�ñ��� ����+����+����(��Ҫ������Ҫ˵����) 
//			�� ��"update_last_mode_state()����" ���Լ� ��"ÿ�εȴ�ʱ�䵽���" ����
//
extern u16 x_times_call_wait_for_n_ms_func;



//
//@brief :	�����ϴη���״̬�͵�·״̬
//
//
//

void update_last_mode_state(const enum _mode_machine current_mode, enum _mode_machine *last_mode,
							const enum _road_state_machine current_road_state, enum _road_state_machine *last_road_state,
							u16 *x_times_call_wait_for_n_ms_func);

							
//����ģʽ�ж�
void business_mode_handle(mode_machine_type *mode_machine);


void mode_flow_step_set_to_zero(const mode_machine_type *mode_machine);
							

void increment_set_to_zero(void);


void integral_output_set_to_zero(const mode_machine_type *mode_machine);
							


							
extern mode_flow_step_type mode_1_flow_steps;
extern mode_flow_step_type mode_2_flow_steps;
extern mode_flow_step_type mode_3_flow_steps;
extern mode_flow_step_type mode_4_flow_steps;
extern mode_flow_step_type mode_5_flow_steps;

extern mode_flow_step_type mode_A_to_B_fire_B_to_A_flow_steps;
extern mode_flow_step_type mode_fly_on_rectangle_flow_steps;
extern mode_flow_step_type mode_A_to_B_steps;
extern mode_flow_step_type mode_FIND_LCUK_steps;
extern mode_flow_step_type mode_B_to_A_steps;

extern mode_flow_step_type mode_A_C_B_steps;
#endif



