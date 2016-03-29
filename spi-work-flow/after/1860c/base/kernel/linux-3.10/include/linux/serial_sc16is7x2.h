/*
include/linux/serial_sc16is7x2.h
*/

#ifndef LINUX_SPI_SC16IS752_H
#define LINUX_SPI_SC16IS752_H

/* SC16IS7x2 SPI UART */
#define PORT_SC16IS7X2	98

#define SC16IS7X2_NR_GPIOS 8

struct sc16is7x2_platform_data {
	unsigned int	uartclk;
	/* uart line number of the first channel */
	unsigned int	uart_base;
	/* number assigned to the first GPIO */
	unsigned int	gpio_base;
	char		*label;
	/* list of GPIO names (array length = SC16IS7X2_NR_GPIOS) */
	const char	*const *names;
//yasin_s
	unsigned int gpio_for_irq;
	unsigned int gpio_for_reset;
//yasin_e
};

#endif
