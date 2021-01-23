#include <8051.h>

/* this register control the blue LED */
#define GPIO_LED_CTRL (*(__xdata volatile unsigned char*)0x204c)

extern void bitbang_putc(char c);

static void delay(void);
static void serial_putc(char tx_data);
static char serial_getc(void);
static void serial_puts(char *str);

void main(void)
{
    char c;

    /* Disable interrupt */
    EA = 0;

    /* Setup serial port */
    TMOD = 0x20;		/* Timer 1, 8-bit auto reload mode */
    PCON = 0x80;
    TH1 = 0xf3;		/* Load value for 9600 baud rate */
    SCON = 0x50;		/* Mode 1, reception enable */
    TR1 = 1;		/* Start timer 1 */

    delay();
    serial_puts("Hello World!");

    while (1) {
        c = serial_getc();
        serial_putc(c);
    }
}

static void delay(void)
{
    unsigned i;
    for (i = 0; i < 0xffff; i++) {
        __asm__ ("nop; nop; nop");
    }
}

static char serial_getc(void)
{
    char c;
    while (RI==0) continue;
    c = SBUF;
    RI = 0;			/* Clear RI flag */
    return c;
}

/* No idea where the TX line routed to on this capture card. Let just bitbang for now */
#if 0
static void serial_putc(char c)
{
    SBUF = c;
    while (TI==0);
    TI = 0;
}
#else
static void serial_putc(char c)
{
    bitbang_putc(c);
}
#endif

static void serial_puts(char *str)
{
    while(*str)
        serial_putc(*(str++));
    serial_putc('\n');
}
