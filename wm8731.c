#include "wm8731.h"



void wm8731_init()
{
	/* Non blocking delay to allow the system to settle down */
	vTaskDelay(pdMS_TO_TICKS(100));



	/* Reset module */
	wm8731_write_register(WM8731_REG_RESET, WM8731_RESET);
	/* Left line in settings */
	wm8731_write_register(WM8731_REG_LLINE_IN, WM8731_LINE_IN_LEFT);
	/* Rigth line in settings */
	wm8731_write_register(WM8731_REG_RLINE_IN, WM8731_LINE_IN_RIGHT);
	/* Left headphone out settings */
	wm8731_write_register(WM8731_REG_LHPHONE_OUT, WM8731_HP_LEFT);
	/* Right headphone out settings */
	wm8731_write_register(WM8731_REG_RHPHONE_OUT, WM8731_HP_RIGHT);
	/* Analog paths */
	wm8731_write_register(WM8731_REG_ANALOG_PATH, WM8731_ANALOG_AUDIO_BYPASS);
	/* Digital paths */
	wm8731_write_register(WM8731_REG_DIGITAL_PATH, WM8731_DIGITAL_AUDIO);
	/* Power down control */
	wm8731_write_register(WM8731_REG_PDOWN_CTRL, WM8731_POWER_MODE);
	/* Digital interface */
	wm8731_write_register(WM8731_REG_DIGITAL_IF, WM8731_DA_INTERFACE);
	/* Sampling control */
	wm8731_write_register(WM8731_REG_SAMPLING_CTRL, WM8731_SAMPLING);
	wm8731_write_register(WM8731_REG_ACTIVE_CTRL, WM8731_ACTIVATE);



	/* End of function */
}
void wm8731_write_register(uint8_t reg, uint16_t data)
{
	/* Prepare transmission */
	uint8_t address = (reg << 1) | (Hi(data) & 1);;
	uint8_t buffer  = Lo(data);

	/* Non-blocking delay to grant that I2C module is ready */
	vTaskDelay(pdMS_TO_TICKS(10));

	/* Transmit through I2C */
	rtos_i2c_transfer(rtos_i2c_0, &buffer, 1, WM8731_DEVICE_ADDRESS, address, 1);
}

