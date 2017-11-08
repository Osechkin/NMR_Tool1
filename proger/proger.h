/*
 * proger.h
 *
 *  Created on: 17.12.2014
 *      Author: aivanov
 *
 *      version number see in proger_defines.h
 */

#ifndef PROGER_H_
#define PROGER_H_

#include "proger_defines.h"

void proger_stop ();
void proger_start ();

unsigned int proger_rd_reg_32 (unsigned int adr);
unsigned int proger_wr_reg_32 (unsigned int adr, unsigned int data);

unsigned int proger_read_time_counter ();
unsigned int proger_restart_time_counter ();

int proger_rd_acquisition_info (unsigned char *array, unsigned int byte_count);

int proger_rd_echo_count (void);
int proger_rd_gamma_count (void);
int proger_rd_adc_points_count (void);
double proger_rd_adc_frq_hz (void);
int proger_rd_pwr_pg (void);
int proger_rd_proger_type (void);
int proger_rd_proger_version (void);
int proger_rd_proger_sub_version (void);
int proger_rd_group_index(void);			 // функция чтения номера группы последовательностей ("группового индекса"), применяемых в течение одного цикла измерений
unsigned int proger_read_gpio ();
unsigned int proger_rd_ch_number(void);      // функция чтения номера текущего канала данных, активированных командой COM_SET_FRQ
unsigned int proger_rd_device_serial(void);  // функция чтения серийного номера прибора
unsigned int proger_rd_connect_speed(void);  // функция чтения битовой скорости обмена данными с прибором

unsigned int proger_rd_status(void);         // функция чтения статуса программатора (запущен (1) или остановлен (0) в бите 0, закончил (1) или не закончил (0) последовательность по команде COM_STOP  в бите 1)
unsigned int proger_is_started(void);		 // возвращает 1 если программатор был запущен функцией proger_start, возвращает 0 если остановлен функцией stop
unsigned int proger_is_seq_done(void);       // возвращает 1 если программатор дошел до команды COM_STOP и запущен функцией proger_start, возвращает 0 во всех других случаях

int proger_rd_conf_mem ( unsigned char *data );

int proger_wr_pulseprog (unsigned char *array, unsigned int byte_count);

int proger_reset_comm_fifo_counter (void);
int proger_wr_comm_arr_to_fifo (volatile unsigned char *comm);
int proger_wr_comm_to_fifo (unsigned char comm, unsigned char arg1, unsigned char arg2, unsigned char arg3);

int proger_rd_pulseprog (unsigned char *array, unsigned int byte_count);
int proger_compare_pulseprog (unsigned char *array, unsigned int byte_count);

int proger_mem_init ();
int proger_mem_clear ();

//int proger_test_mem_pulseprog ();
//int main_proger_wr_pulseprog_test_GPIO3 ();
//int main_proger_wr_pulseprog_test_GPIO_48f_ADC_Diel();
int main_proger_wr_pulseprog_test_GPIO_48f_ADC_FID ();
int main_proger_wr_pulseprog_test_GPIO_49q_ADC_old_style_windows (
															double       resonance_frq_ofs_hz,
															unsigned int rf_pulse_duration,
															unsigned int nmr_aq_time,
															unsigned int noise_aq_time,
															unsigned int dead_time,
															double       adc_frq_ofs_hz,
															unsigned int damping_pulse_duration_after_rf_pulse,
															unsigned int delay_before_rf_pulses
);

int main_proger_wr_pulseprog_test_FID (
										double       resonance_frq_ofs_hz,
										unsigned int rf_pulse_duration,
										unsigned int nmr_aq_time,
										unsigned int noise_aq_time,
										unsigned int dead_time,
										double       adc_frq_ofs_hz,
										unsigned int damping_pulse_duration_after_rf_pulse,
										unsigned int delay_before_rf_pulses
										);



#endif /* PROGER_H_ */
