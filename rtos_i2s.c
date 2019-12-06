#include "rtos_i2s.h"

static I2S_ConfigStr Rx = {0};
static I2S_ConfigStr Tx = {0};

void i2s_clock_gating()
{
	/* Clock gating */
	SIM->SCGC6 |= SIM_SCGC6_I2S_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;

	/* Initialize port C */
	PORTC->PCR[1U] = PORT_PCR_MUX(6U);    // I2S D0
	PORTC->PCR[5U] = PORT_PCR_MUX(4U);    // I2S DI
	PORTC->PCR[9U] = PORT_PCR_MUX(4U);    // I2S CK
	PORTC->PCR[7U] = PORT_PCR_MUX(4U);    // I2S WS
}

void i2s_tx_init()
{
	/* clock*/
	Tx.clock.bclkInputDelay = false;
	Tx.clock.bclkPolarity   = kSAI_PolarityActiveLow;
	Tx.clock.bclkSource     = kSAI_BclkSourceBusclk;
	Tx.clock.bclkSrcSwap    = true;

	/* data */
	Tx.data.dataFirstBitShifted = 32;
	Tx.data.dataOrder           = kSAI_DataMSB;
	Tx.data.dataWord0Length     = 32;
	Tx.data.dataWordLength      = 32;
	Tx.data.dataWordNLength     = 32;

	/* fifo */
	Tx.fifo.fifoWatermark = 1;

	/* frame */
	Tx.frame.frameSyncEarly    = true;
	Tx.frame.frameSyncPolarity = kSAI_PolarityActiveHigh;
	Tx.frame.frameSyncWidth    = 1;

	/* transceiver */
	Tx.transceiver.bitClock     = Tx.clock;
	Tx.transceiver.channelMask  = 1;
	Tx.transceiver.channelNums  = 1;
	Tx.transceiver.endChannel   = 1;
	Tx.transceiver.fifo         = Tx.fifo;
	Tx.transceiver.frameSync    = Tx.frame;
	Tx.transceiver.masterSlave  = kSAI_Slave;
	Tx.transceiver.serialData   = Tx.data;
	Tx.transceiver.startChannel = 1;
	Tx.transceiver.syncMode     = kSAI_ModeSync;


	/* Tx config */
	SAI_TxSetConfig(I2S0, &Tx.transceiver);
	SAI_TxSetBitClockPolarity(I2S0, kSAI_PolarityActiveLow);
	SAI_TxEnable(I2S0, true);

	/* Enable module interrupt */
	I2S0->TCSR |= I2S_TCSR_FRIE_MASK;



	/* End of function */
}

void i2s_rx_init()
{
	/* clock*/
	Rx.clock.bclkInputDelay = false;
	Rx.clock.bclkPolarity   = kSAI_PolarityActiveLow;
	Rx.clock.bclkSource     = kSAI_BclkSourceBusclk;
	Rx.clock.bclkSrcSwap    = true;

	/* data */
	Rx.data.dataFirstBitShifted = 32;
	Rx.data.dataOrder           = kSAI_DataMSB;
	Rx.data.dataWord0Length     = 32;
	Rx.data.dataWordLength      = 32;
	Rx.data.dataWordNLength     = 32;

	/* fifo */
	Rx.fifo.fifoWatermark = 1;

	/* frame */
	Rx.frame.frameSyncEarly    = true;
	Rx.frame.frameSyncPolarity = kSAI_PolarityActiveHigh;
	Rx.frame.frameSyncWidth    = 1;

	/* transceiver */
	Rx.transceiver.bitClock     = Rx.clock;
	Rx.transceiver.channelMask  = 1;
	Rx.transceiver.channelNums  = 1;
	Rx.transceiver.endChannel   = 1;
	Rx.transceiver.fifo         = Rx.fifo;
	Rx.transceiver.frameSync    = Rx.frame;
	Rx.transceiver.masterSlave  = kSAI_Slave;
	Rx.transceiver.serialData   = Rx.data;
	Rx.transceiver.startChannel = 1;
	Rx.transceiver.syncMode     = kSAI_ModeAsync;



	/* Rx config */
	SAI_RxSetConfig(I2S0, &Rx.transceiver);
	SAI_RxSetBitClockPolarity(I2S0, kSAI_PolarityActiveLow);
	SAI_RxEnable(I2S0, true);

	/* Enable module interrupt */
	I2S0->RCSR |= I2S_RCSR_FRIE_MASK;



	/* End of function */
}
