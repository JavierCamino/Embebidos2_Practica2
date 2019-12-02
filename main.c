#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MK64F12.h"
#include "fsl_debug_console.h"
#include "stdint.h"

#include "audio.h"
#include "rtos_i2c.h"
#include "wm8731.h"

#include "FreeRTOS.h"
#include "task.h"
#include "projdefs.h"
#include "semphr.h"



/////////////////////////////////////////
/* Macros */
/////////////////////////////////////////
#define CREATE_TASK(a,b,c,d,e,f)	{												\
										if( pdPASS != xTaskCreate(a,b,c,d,e,f) )	\
										{											\
											PRINTF("%s creation failed.\r\n",#b);	\
											for(;;);								\
										}											\
									}
#define CREATE_SEMAPHORE(r)			{												\
										r = xSemaphoreCreateBinary();				\
										if(NULL == r)								\
										{											\
											PRINTF("%s creation failed.\r\n",#r);	\
											for(;;);								\
										}											\
									}
#define CREATE_MUTEX(r)				{												\
										r = xSemaphoreCreateMutex();				\
										if(NULL == r)								\
										{											\
											PRINTF("%s creation failed.\r\n",#r);	\
											for(;;);								\
										}											\
									}
#define CREATE_EVENTGROUP(r)		{												\
										r = xEventGroupCreate();					\
										if(NULL == r)								\
										{											\
											PRINTF("%s creation failed.\r\n",#r);	\
											for(;;);								\
										}											\
									}
#define CREATE_QUEUE(r,a,b)			{												\
										r = xQueueCreate(a,b);						\
										if(NULL == r)								\
										{											\
											PRINTF("%s creation failed.\r\n",#r);	\
											for(;;);								\
										}											\
									}



/* Functions */
static void audio_config (void * arg);



/* Tasks */
	/* CodecConfig */
const TaskFunction_t CodecConfig_task_code     = audio_config;
const char * const   CodecConfig_task_name     = "CodecConfig_task";
const uint16_t       CodecConfig_task_stack    = 4*configMINIMAL_STACK_SIZE;
void *               CodecConfig_task_params   = NULL;
const UBaseType_t    CodecConfig_task_priority = configMAX_PRIORITIES - 1U;
TaskHandle_t         CodecConfig_task_handle   = 0;






int main(void)
{
  	/* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
  	/* Init FSL debug console. */
    BOARD_InitDebugConsole();
    PRINTF("Hello World\n");




    /* I2C configuration */
    rtos_i2c_config_t i2c_config;
    i2c_config.SCL_pin = 24;
	i2c_config.SDA_pin = 25;
	i2c_config.baudrate = 100000;
	i2c_config.i2c_number = rtos_i2c_0;
	i2c_config.pin_mux = kPORT_MuxAlt5;
	i2c_config.port = rtos_i2c_portE;
	rtos_i2c_init(i2c_config);



	/* Create tasks */
	CREATE_TASK(CodecConfig_task_code, CodecConfig_task_name, CodecConfig_task_stack, CodecConfig_task_params, CodecConfig_task_priority, &CodecConfig_task_handle);


	/* Start scheduler */
	vTaskStartScheduler();






    /* Force the counter to be placed into memory. */
    volatile static int i = 0 ;
    /* Enter an infinite loop, just incrementing a counter. */
    while(1) {
        i++ ;
        /* 'Dummy' NOP to allow source level single stepping of
            tight while() loop */
        __asm volatile ("nop");
    }
    return 0;
}



static void audio_config (void * arg)
{
	/* Initialize audio codec */
	wm8731_init();

	/* Delay to ensure communication ended before suspending task */
	vTaskDelay(pdMS_TO_TICKS(100));

	/* Self suspend */
	vTaskSuspend(CodecConfig_task_handle);
}

