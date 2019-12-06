#ifndef RTOS_I2S_H_
#define RTOS_I2S_H_

#include "FreeRTOS.h"
#include "fsl_sai.h"
#include "stdbool.h"

/* Data structure for sai module configuration */
typedef struct
{
	sai_bit_clock_t clock;
	sai_serial_data_t data;
	sai_fifo_t fifo;
	sai_frame_sync_t frame;
	sai_transceiver_t transceiver;
} I2S_ConfigStr;

void i2s_clock_gating();
void i2s_tx_init();
void i2s_rx_init();


#endif
