#include "types.h"
#include "platform.h"

#define UART_REG(reg) ((volatile uint8_t *) (UART0 + reg))
#define RHR 0	//	Receive Holding Register(read mode)
#define THR 0	//	Transmit Holding Register(write mode)
#define DLL 0	//	LSB Of Divisor Latch(write mode)
#define IER 1	//	Interrupt Enable Register(write mode)
#define DLM 1	//	MSB of Divisor Latch(write mode)
#define FCR 2	//	FIFO Control Register(write mode)
#define ISR 2	//	Interrupt Status Register(read mode)
#define LCR 3 	//	Line Control Register(read mode)
#define MCR 4	//	Modem Control Register
#define LSR 5	//	Line Status Register
#define MSR 6	//	Modem Status Regsister
#define SPR 7 	//	ScratchPad Register


#define LSR_RX_READY (1 << 0)
#define LSR_TX_IDLE	 (1 << 5)

#define uart_read_reg(reg) (*(UART_REG(reg)))
#define uart_write_reg(reg, v)(*(UART_REG(reg)) = (v))

void uart_init(){
	
	/*disable interrupt*/
	uart_write_reg(IER, 0x00);
	
	uint8_t lcr = uart_read_reg(LCR);
	uart_write_reg(LCR, lcr | (1 << 7));
	uart_write_reg(DLL, 0x03);
	uart_write_reg(DLM, 0x00);

	lcr = 0;
	uart_write_reg(LCR, lcr | (3 << 0));
}

int uart_putc(char ch){
	while((uart_read_reg(LSR) & LSR_TX_IDLE) == 0);
	return uart_write_reg(THR, ch);
}

void uart_puts(char *s){
	while(*s){
		uart_putc(*s);
		s++;
	}
}
