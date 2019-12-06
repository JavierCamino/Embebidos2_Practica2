#include "wm8731.h"


static struct
{
	uint8_t slave_address;
	rtos_i2c_config_t config;
} wm8731_handle = {0};

static sai_bit_clock_t sai_tx_clock;
static sai_serial_data_t sai_tx_data;
static sai_fifo_t sai_tx_fifo;
static sai_frame_sync_t sai_tx_frame;
static sai_transceiver_t sai_tx_transceiver;

static sai_bit_clock_t sai_rx_clock;
static sai_serial_data_t sai_rx_data;
static sai_fifo_t sai_rx_fifo;
static sai_frame_sync_t sai_rx_frame;
static sai_transceiver_t sai_rx_transceiver;

//Callbacks for  sai TX and RX
 void (*i2s_tx_callback)(void) = 0;
 void (*i2s_rx_callback)(void) = 0;

//This pointer will have the direction of a buffer for TX and RX
static uint32_t  buffer_sai = 0;

static void wm8731_write_register (uint8_t reg, uint16_t data);

static void i2s_config (void);

// IRQ handlers
void I2S0_Tx_IRQHandler(void)
{
	I2S0->TDR[0] = buffer_sai;
	NVIC_ClearPendingIRQ(I2S0_Tx_IRQn);
}
/* ISR Handler*/
void I2S0_Rx_IRQHandler(void)
{
	buffer_sai = I2S0->RDR[0];
	NVIC_ClearPendingIRQ(I2S0_Rx_IRQn);
}






void wm8731_init(rtos_i2c_config_t config, uint8_t slave_address)
{
	wm8731_handle.slave_address = slave_address;
	wm8731_handle.config = config;

	rtos_i2c_init(config);

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
	wm8731_write_register(WM8731_REG_ANALOG_PATH, WM8731_ANALOG_AUDIO_LINE);
	/* Digital paths */
	wm8731_write_register(WM8731_REG_DIGITAL_PATH, WM8731_DIGITAL_AUDIO);
	/* Power down control */
	wm8731_write_register(WM8731_REG_PDOWN_CTRL, WM8731_POWER_MODE);
	/* Digital interface */
	wm8731_write_register(WM8731_REG_DIGITAL_IF, WM8731_DA_INTERFACE);

	/* Sampling control */
	wm8731_write_register(WM8731_REG_SAMPLING_CTRL, WM8731_SAMPLING);

	wm8731_write_register(WM8731_REG_ACTIVE_CTRL,WM8731_ACTIVATE);

	i2s_config();
}

static void wm8731_write_register (uint8_t reg, uint16_t data)
{
	uint8_t address;
	uint8_t buffer;

	/* delay */
	vTaskDelay(pdMS_TO_TICKS(10));

	address = reg << 1;
	address = address | (Hi(data) & 1);

	buffer = Lo(data);

	rtos_i2c_transfer(
		wm8731_handle.config.i2c_number,
		&buffer,
		1,
		wm8731_handle.slave_address,
		address,
		1
	);
}

static void i2s_config (void)
{
	//Enabling Clock gating
	CLOCK_EnableClock(kCLOCK_PortC);
	CLOCK_EnableClock(kCLOCK_Sai0);

	PORTC->PCR[1] = PORT_PCR_MUX(6); //I2S0 TxD0 alt6 PTC1
	PORTC->PCR[5] = PORT_PCR_MUX(4); //I2S0 RxD0 alt4 PTC5
	PORTC->PCR[9] = PORT_PCR_MUX(4); //I2S0 Rx_BCLK alt4 PTC9
	PORTC->PCR[7] = PORT_PCR_MUX(4); //I2S0 Rx_FS alt4 PTC7

	sai_config_t sai_tx_config =
	{
			.protocol = kSAI_BusPCMA,
			.syncMode = kSAI_ModeSync,
			.mclkOutputEnable = false,
			.mclkSource = kSAI_MclkSourceSysclk,
			.bclkSource = kSAI_BclkSourceBusclk,
			.masterSlave = kSAI_Slave,
	};

	sai_frame_sync_t sai_tx_frame =
	{
			.frameSyncEarly = true,
			.frameSyncPolarity = kSAI_PolarityActiveHigh,
			.frameSyncWidth = 1,
	};

	sai_bit_clock_t sai_tx_clock =
	{
			.bclkSrcSwap = true,
			.bclkInputDelay = false,
			.bclkPolarity = kSAI_PolarityActiveLow,
			.bclkSource = kSAI_BclkSourceBusclk,
	};

	sai_serial_data_t sai_tx_data =
	{
			.dataOrder = kSAI_DataMSB,
			.dataWord0Length = 32,
			.dataWordNLength = 32,
			.dataWordLength =32,
			.dataFirstBitShifted = 32,
			.dataWordNum = 1,
			.dataMaskedWord = 0,
	};

	sai_fifo_t sai_tx_fifo = { .fifoWatermark = 1 };

	sai_transceiver_t sai_tx_transiever =
	{
			.serialData = sai_tx_data,
			.frameSync = sai_tx_frame,
			.bitClock = sai_tx_clock,
			.fifo = sai_tx_fifo,
			.masterSlave = kSAI_Slave,
			.syncMode = kSAI_ModeSync,
			.startChannel = 1,
			.channelMask = 1,
			.endChannel = 1,
			.channelNums = 1,
	};

	SAI_TxSetConfig(I2S0, &sai_tx_transiever);
	SAI_TxSetBitClockPolarity(I2S0, sai_tx_clock.bclkPolarity);




	sai_config_t sai_rx_config =
	{
			.protocol = kSAI_BusPCMA,
			.syncMode = kSAI_ModeAsync,
			.mclkOutputEnable = false,
			.mclkSource = kSAI_MclkSourceSysclk,
			.bclkSource = kSAI_BclkSourceBusclk,
			.masterSlave = kSAI_Slave,
	};

	sai_bit_clock_t sai_rx_clock =
	{
			.bclkSrcSwap = false,
			.bclkInputDelay = false,
			.bclkPolarity = kSAI_PolarityActiveLow,
			.bclkSource = kSAI_BclkSourceBusclk,
	};

	sai_serial_data_t sai_rx_data =
	{
			.dataOrder = kSAI_DataMSB,
			.dataWord0Length = 32,
			.dataWordNLength = 32,
			.dataWordLength =32,
			.dataFirstBitShifted = 32,
			.dataWordNum = 1,
			.dataMaskedWord = 0,
	};

	sai_frame_sync_t sai_rx_frame =
	{
			.frameSyncEarly = true,
			.frameSyncPolarity = kSAI_PolarityActiveHigh,
			.frameSyncWidth = 1,
	};

	sai_fifo_t sai_rx_fifo = { .fifoWatermark = 1 };

	sai_transceiver_t sai_rx_transiever =
	{
			.serialData = sai_rx_data,
			.frameSync = sai_rx_frame,
			.bitClock = sai_rx_clock,
			.fifo = sai_rx_fifo,
			.masterSlave = kSAI_Slave,
			.syncMode = kSAI_ModeAsync,
			.startChannel = 1,
			.channelMask = 1,
			.endChannel = 1,
			.channelNums = 1,
	};


	SAI_RxSetConfig(I2S0, &sai_rx_transiever);
	SAI_RxSetBitClockPolarity(I2S0, sai_rx_clock.bclkPolarity);




	//Enabling interrupts for TX and RX
	I2S0->RCSR |= I2S_RCSR_FRIE_MASK;
	I2S0->TCSR |= I2S_TCSR_FRIE_MASK;

	NVIC_SetPriority(I2S0_Rx_IRQn, 28);
	NVIC_SetPriority(I2S0_Tx_IRQn ,29);

	NVIC_EnableIRQ(I2S0_Rx_IRQn);
	NVIC_EnableIRQ(I2S0_Tx_IRQn);

	// Transmit FIFO start
	I2S0->TCSR |= (I2S_TCSR_FR_MASK);
	I2S0->TCSR |= (I2S_TCSR_TE_MASK);
	//Recive FIFO start
	I2S0->RCSR |= (I2S_RCSR_FR_MASK);
	I2S0->RCSR |= (I2S_RCSR_RE_MASK);

}
