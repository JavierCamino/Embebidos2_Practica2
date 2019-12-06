#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MK64F12.h"
#include "fsl_debug_console.h"

#include "stdint.h"
#include "rtos_i2c.h"
#include "wm8731.h"

#include "FreeRTOS.h"
#include "task.h"
#include "projdefs.h"





/* Global variables */
rtos_i2c_config_t i2c_config;

/* Function definition */
void audio_config (void * arg);

/* Main function */
int main(void)
{
  	/* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
  	/* Init FSL debug console. */
    BOARD_InitDebugConsole();


    /* Configure the I2C channel used to set up the Codec */
    i2c_config.SCL_pin = 24;
	i2c_config.SDA_pin = 25;
	i2c_config.baudrate = 100000;
	i2c_config.i2c_number = rtos_i2c_0;
	i2c_config.pin_mux = kPORT_MuxAlt5;
	i2c_config.port = rtos_i2c_portE;


	xTaskCreate(audio_config, "audo config", 4*configMINIMAL_STACK_SIZE, NULL, configMAX_PRIORITIES-1, NULL);


	vTaskStartScheduler();



    for(;;);
    return 0 ;
}

/* Function declaration */
void audio_config (void * arg)
{
	wm8731_init(i2c_config, WM8731_DEVICE_ADDRESS);

	for(;;);
}
