#include <8051.h>

/* this register control the blue LED */
#define GPIO_LED_CTRL (*(__xdata volatile unsigned char*)0x204c)

extern void bitbang_putc(char c);

static void delay(void);
static void serial_putc(char tx_data);
static char serial_getc(void);
static void serial_puts(char *str);
static unsigned serial_get_word(void);
static unsigned char get_iram(unsigned char pos);
static void set_iram(unsigned char pos, unsigned char val);

void main(void)
{
    char c;
    unsigned addr;
    unsigned char val;

    /* Disable interrupt */
    EA = 0;

    /* Setup serial port */
    TMOD = 0x20;		/* Timer 1, 8-bit auto reload mode */
    PCON = 0x80;
    TH1 = 0xf3;		/* Load value for 9600 baud rate */
    SCON = 0x50;		/* Mode 1, reception enable */
    TR1 = 1;		/* Start timer 1 */

    delay();
    serial_puts("Puppet Alive! :)");

    while (1) {
        c = serial_getc();
        switch(c) {
        case 'P': /* ping */
            serial_putc('A');
            break;
        case 'c': /* read code */
            addr = serial_get_word();
            val = *(__code unsigned char *)addr;
            serial_putc(val);
            break;
        case 'x': /* read xdata */
            addr = serial_get_word();
            val = *(__xdata volatile unsigned char *)addr;
            serial_putc(val);
            break;
        case 'i': /* read iram */
            addr = serial_getc();
            val = get_iram(addr);
            serial_putc(val);
            break;
        case 'w': /* write xdata */
            addr = serial_get_word();
            val = serial_getc();
            *(__xdata volatile unsigned char *)addr = val;
            serial_putc('A');
            break;
        case 'e': /* write iram */
            addr = serial_getc();
            val = serial_getc();
            set_iram(addr, val);
            serial_putc('A');
            break;
        default:
            continue;
        }
    }
}

static unsigned char get_iram(unsigned char pos)
{
    (void) pos;
    __asm__ (
        "mov r0, dpl\n"
        "mov A, @r0\n"
        "mov dpl, A\n"
        "ret\n"
    );
    return 0; //avoid compiler error
}

static void set_iram(unsigned char pos, unsigned char val)
{
    (void) pos;
    (void) val;
    __asm__ (
        "mov r0, dpl\n"
        "mov A, _set_iram_PARM_2\n"
        "mov @r0, A\n"
    );
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

static unsigned serial_get_word(void)
{
    unsigned a;
    a = serial_getc();
    a |= serial_getc() << 8;
    return a;
}
