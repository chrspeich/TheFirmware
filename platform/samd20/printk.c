//
// Copyright (c) 2013, Christian Speich
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

#include <platform/printk.h>
#include <platform/clock.h>

#include "samd20.h"
#include <stdbool.h>
#include <string.h>
#include <platform/pinmux.h>

#define EDBG_CDC_MODULE              SERCOM3
#define EDBG_CDC_SERCOM_MUX_SETTING  USART_RX_3_TX_2_XCK_3
#define EDBG_CDC_SERCOM_PINMUX_PAD0  PINMUX_UNUSED
#define EDBG_CDC_SERCOM_PINMUX_PAD1  PINMUX_UNUSED
#define EDBG_CDC_SERCOM_PINMUX_PAD2  PINMUX_PA24C_SERCOM3_PAD2
#define EDBG_CDC_SERCOM_PINMUX_PAD3  PINMUX_PA25C_SERCOM3_PAD3

SercomUsart* hw = &SERCOM3->USART;

#define SHIFT 32

struct system_gclk_chan_config {
	/** Generic Clock Generator source channel. */
	gclock_generator_t source_generator;
	/** If \c true the clock configuration will be locked until the device is
	 *  reset. */
	bool write_lock;
};

static inline void system_gclk_chan_get_config_defaults(
		struct system_gclk_chan_config *const config)
{
	/* Sanity check arguments */

	/* Default configuration values */
	config->source_generator = GCLOCK_GENERATOR_0;
	config->write_lock       = false;
}

void system_gclk_chan_disable(
		const uint8_t channel)
{
	/* Select the requested generator channel */
	*((uint8_t*)&GCLK->CLKCTRL.reg) = channel;

	/* Switch to known-working source so that the channel can be disabled */
	uint32_t prev_gen_id = GCLK->CLKCTRL.bit.GEN;
	GCLK->CLKCTRL.bit.GEN = 0;

	/* Disable the generic clock */
	GCLK->CLKCTRL.reg &= ~GCLK_CLKCTRL_CLKEN;
	while (GCLK->CLKCTRL.reg & GCLK_CLKCTRL_CLKEN) {
		/* Wait for clock to become disabled */
	}

	/* Restore previous configured clock generator */
	GCLK->CLKCTRL.bit.GEN = prev_gen_id;
}

void system_gclk_chan_enable(
		const uint8_t channel)
{
	/* Select the requested generator channel */
	*((uint8_t*)&GCLK->CLKCTRL.reg) = channel;

	/* Enable the generic clock */
	GCLK->CLKCTRL.reg |= GCLK_CLKCTRL_CLKEN;
}

void system_gclk_chan_set_config(
		const uint8_t channel,
		struct system_gclk_chan_config *const config)
{
	/* Sanity check arguments */

	/* Cache the new config to reduce sync requirements */
	uint32_t new_clkctrl_config = (channel << GCLK_CLKCTRL_ID_Pos);

	/* Select the desired generic clock generator */
	new_clkctrl_config |= config->source_generator << GCLK_CLKCTRL_GEN_Pos;

	/* Enable write lock if requested to prevent further modification */
	if (config->write_lock) {
		new_clkctrl_config |= GCLK_CLKCTRL_WRTLOCK;
	}

	/* Disable generic clock channel */
	system_gclk_chan_disable(channel);

	/* Write the new configuration */
	GCLK->CLKCTRL.reg = new_clkctrl_config;
}

int _sercom_get_async_baud_val(
		const uint32_t baudrate,
		const uint32_t peripheral_clock,
		uint16_t *const baudval)
{
	/* Temporary variables  */
	uint64_t ratio = 0;
	uint64_t scale = 0;
	uint64_t baud_calculated = 0;

	/* Check if the baudrate is outside of valid range */
	if ((baudrate * 16) >= peripheral_clock) {
		/* Return with error code */
		return 1;
	}

	/* Calculate the BAUD value */
	ratio = ((16 * (uint64_t)baudrate) << SHIFT) / peripheral_clock;
	scale = ((uint64_t)1 << SHIFT) - ratio;
	baud_calculated = (65536 * scale) >> SHIFT;

	*baudval = baud_calculated;

	return 0;
}

struct _sercom_conf {
	/* Status of gclk generator initialization. */
	bool generator_is_set;
	/* Sercom gclk generator used. */
	gclock_generator_t generator_source;
};

static struct _sercom_conf _sercom_config;
#  define SERCOM_GCLK_ID SERCOM0_GCLK_ID_SLOW

int sercom_set_gclk_generator(
		const gclock_generator_t generator_source,
		const bool force_change)
{
	/* Check if valid option. */
	if (!_sercom_config.generator_is_set || force_change) {
		/* Create and fill a GCLK configuration structure for the new config. */
		struct system_gclk_chan_config gclk_chan_conf;
		system_gclk_chan_get_config_defaults(&gclk_chan_conf);
		gclk_chan_conf.source_generator = generator_source;
		system_gclk_chan_set_config(SERCOM_GCLK_ID, &gclk_chan_conf);
		system_gclk_chan_enable(SERCOM_GCLK_ID);

		/* Save config. */
		_sercom_config.generator_source = generator_source;
		_sercom_config.generator_is_set = true;

		return 0;
	} else if (generator_source == _sercom_config.generator_source) {
		/* Return status OK if same config. */
		return 0;
	}

	/* Return invalid config to already initialized GCLK. */
	return 01;
}

uint32_t system_gclk_chan_get_hz(
		const uint8_t channel);

uint32_t system_gclk_chan_get_hz(
		const uint8_t channel)
{
	uint8_t gen_id;

	/* Select the requested generic clock channel */
	*((uint8_t*)&GCLK->CLKCTRL.reg) = channel;
	gen_id = GCLK->CLKCTRL.bit.GEN;

	/* Return the clock speed of the associated GCLK generator */
	return gclock_get_generator(gen_id);
}

void printk_init(uint32_t baud)
{
	uint32_t pm_index     = 3 + PM_APBCMASK_SERCOM0_Pos;
	uint32_t gclk_index   = 3 + SERCOM0_GCLK_ID_CORE;

	// REG_PAC0_WPCLR |= PAC_WPCLR_WP(1);

	PM->APBCMASK.reg |= 1 << pm_index;

	pinmux_set_mux(PINMUX_PA24C_SERCOM3_PAD2);
	pinmux_set_mux(PINMUX_PA25C_SERCOM3_PAD3);

	struct system_gclk_chan_config gclk_chan_conf;
	system_gclk_chan_get_config_defaults(&gclk_chan_conf);
	gclk_chan_conf.source_generator = GCLOCK_GENERATOR_0;
	system_gclk_chan_set_config(gclk_index, &gclk_chan_conf);
	system_gclk_chan_enable(gclk_index);
	sercom_set_gclk_generator(GCLOCK_GENERATOR_0, false);

	/* USART mode with external or internal clock must be selected first by writing 0x0
	or 0x1 to the Operating Mode bit group in the Control A register (CTRLA.MODE) */

	hw->CTRLA.bit.MODE = 0x1; // Internal

	/* Communication mode (asynchronous or synchronous) must be selected by writing to
	the Communication Mode bit in the Control A register (CTRLA.CMODE) */

 	// is default no setting needed
	// USART_TRANSFER_ASYNCHRONOUSLY = 0

	/* SERCOM pad to use for the receiver must be selected by writing to the Receive
	Data Pinout bit group in the Control A register (CTRLA.RXPO) */
	hw->CTRLA.bit.RXPO = 0x3;

	/* SERCOM pads to use for the transmitter and external clock must be selected by
	writing to the Transmit Data Pinout bit in the Control A register (CTRLA.TXPO) */
	hw->CTRLA.bit.TXPO = 0x1;

	/* Character size must be selected by writing to the Character Size bit group
	in the Control B register (CTRLB.CHSIZE) */

	hw->CTRLB.reg |= SERCOM_USART_CTRLB_CHSIZE(0);
// USART_CHARACTER_SIZE_8BIT = SERCOM_USART_CTRLB_CHSIZE(0)

	/* MSB- or LSB-first data transmission must be selected by writing to the Data
	Order bit in the Control A register (CTRLA.DORD) */

	hw->CTRLA.reg |= SERCOM_USART_CTRLA_DORD;
// USART_DATAORDER_LSB = SERCOM_USART_CTRLA_DORD

	/* When parity mode is to be used, even or odd parity must be selected by writing
	to the Parity Mode bit in the Control B register (CTRLB.PMODE) and enabled by
	writing 0x1 to the Frame Format bit group in the Control A register (CTRLA.FORM) */

	hw->CTRLA.reg |= SERCOM_USART_CTRLA_FORM(0);
// USART_PARITY_NONE = 0xFF

	/* Number of stop bits must be selected by writing to the Stop Bit Mode bit in
	 the Control B register (CTRLB.SBMODE) */

	// Default
// USART_STOPBITS_1 = 0

	/* When using an internal clock, the Baud register (BAUD) must be written to
	generate the desired baud rate */

	uint16_t bval = 0;
	_sercom_get_async_baud_val(baud, system_gclk_chan_get_hz(gclk_index), &bval);
	hw->BAUD.reg = bval;

// 9600

	/* The transmitter and receiver can be enabled by writing ones to the Receiver
	Enable and Transmitter Enable bits in the Control B register (CTRLB.RXEN and CTRLB.TXEN) */
	hw->CTRLB.bit.RXEN = 1;
	hw->CTRLB.bit.TXEN = 1;
	hw->CTRLA.bit.ENABLE = 1;
}

void sync()
{
	while (hw->STATUS.reg & SERCOM_USART_STATUS_SYNCBUSY) {
		/* Wait until the synchronization is complete */
	}
}

int getchar(char* c)
{
	while (!(hw->INTFLAG.reg & SERCOM_USART_INTFLAG_RXC))
		;

	sync();

	/* Read out the status code and mask away all but the 3 LSBs*/
	int error_code = (uint8_t)(hw->STATUS.reg & SERCOM_USART_STATUS_MASK);

	/* Check if an error has occurred during the receiving */
	if (error_code) {
		/* Check which error occurred */
		if (error_code & SERCOM_USART_STATUS_FERR) {
			/* Clear flag by writing a 1 to it and
			 * return with an error code */
			hw->STATUS.reg = SERCOM_USART_STATUS_FERR;

			return -1;
		} else if (error_code & SERCOM_USART_STATUS_BUFOVF) {
			/* Clear flag by writing a 1 to it and
			 * return with an error code */
			hw->STATUS.reg = SERCOM_USART_STATUS_BUFOVF;

			return -1;
		} else if (error_code & SERCOM_USART_STATUS_PERR) {
			/* Clear flag by writing a 1 to it and
			 * return with an error code */
			hw->STATUS.reg = SERCOM_USART_STATUS_PERR;

			return -1;
		}
	}

	*c = hw->DATA.reg;

	return 0;
}

void putchar(char c)
{
	sync();

	hw->DATA.reg = c;

	while (!(hw->INTFLAG.reg & SERCOM_USART_INTFLAG_TXC)) {
		/* Wait until data is sent */
	}
}

static int read_op(file_t f, void* buf, size_t nbytes)
{
	size_t n;

	for (n = 0; n < nbytes; n++, buf++) {
		if (getchar(buf) < 0) {
			break;
		}
	}

	return n;
}

static int write_op(file_t f, const void* buf, size_t nbytes)
{
	for (size_t n = 0; n < nbytes; n++, buf++)
		putchar(*(char*)buf);

	return nbytes;
}

static const struct file_operations ops = {
	.read = read_op,
	.write = write_op,
};

static struct file _debug_serial = {
	.ops = &ops,
};

file_t debug_serial = &_debug_serial;

void printk(const char* str)
{
	write(debug_serial, str, strlen(str));
}
