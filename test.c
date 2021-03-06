#ifdef  __IAR_SYSTEMS_ICC__

#include <ioavr.h>
typedef unsigned char  uint8_t;
typedef   signed char   int8_t;
typedef unsigned short uint16_t;
typedef   signed short  int16_t;
typedef unsigned long  uint32_t;
typedef   signed long   int32_t;
#define PROGMEM __flash
#define PW(a) (a)
#include <math.h>

#elif defined  CODE_VISION

#include <io.h> 
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#define PW(a) (a)
#include <math.h>
#define const flash
#define PROGMEM 

#else
#include <stdarg.h>
#include <avr/io.h>
#include <stdlib.h>
#include <math.h>
#include <avr/pgmspace.h>
#define PW(a) pgm_read_word(&(a))

#endif

const int16_t PROGMEM sin_table[66] = {

      0,   804,  1608,  2410,  3212,  4011,  4808,  5602,
   6393,  7179,  7962,  8739,  9512, 10278, 11039, 11793,
  12539, 13279, 14010, 14732, 15446, 16151, 16846, 17530,
  18204, 18868, 19519, 20159, 20787, 21403, 22005, 22594,
  23170, 23731, 24279, 24811, 25329, 25832, 26319, 26790,
  27245, 27683, 28105, 28510, 28898, 29268, 29621, 29956,
  30273, 30571, 30852, 31113, 31356, 31580, 31785, 31971,
  32137, 32285, 32412, 32521, 32609, 32678, 32728, 32757,
  32767, 32757
};

static int16_t Sine(int32_t phase)
{
        int16_t s0;
        uint16_t tmp_phase, tmp_phase_hi;

        tmp_phase = phase & 0x7fff;

        if (tmp_phase & 0x4000)
                tmp_phase = 0x8000 - tmp_phase;

        tmp_phase_hi = tmp_phase >> 8; // 0...64

        s0 = PW(sin_table[tmp_phase_hi]);

        s0 += ((int16_t)((((int32_t)(PW(sin_table[tmp_phase_hi+1]) - s0))*
                                    (tmp_phase&0xff))>>8));

        if (phase & 0x8000) {
                s0 = -s0;
        }

        return s0;
}

static int16_t Cosi(int32_t phase)
{
    return Sine(phase + 0x4000);
}

/* by Jim Ulery  http://www.azillionmonkeys.com/qed/ulerysqroot.pdf  */
static uint32_t isqrt_c32(uint32_t val) {
    uint32_t temp, g = 0, b = 0x8000, bshft = 15;
    do {
        if (val >= (temp = (((g << 1) + b)<<bshft--))) {
            g += b;
            val -= temp;
        }
    } while (b >>= 1);
    return g;
}

static uint16_t isqrt_c16(uint16_t val) {
    uint16_t temp, g = 0, b = 0x80, bshft = 7;
    do {
        if (val >= (temp = (((g << 1) + b)<<bshft--))) {
            g += b;
            val -= temp;
        }
    } while (b >>= 1);
    return g;
}

#include "asmlib.h"

typedef void (*test_func_t)(void);

float a = 3454.345334, b = 12232.45345, c = 0.0;
short d = 23442;

void float_convert_to_float(void) {
	c = d;
}

void float_convert_to_int(void) {
	d = (short) c;
}

void float_add_const(void) {
	c = a + 4.0;
}

void float_add(void) {
	c = a + b;
}  

void float_mul_const(void) {
	c = a * 4.0;
} 

void float_mul(void) {
	c = a * b;
} 

void float_div_const(void) {
	c = a / 2.4;
} 

void float_div(void) {
	c = a / b;
}

void float_sqrt(void) {
	c = sqrt(a);
}

void float_sin(void) {
	c = sin(a);	
}

unsigned short i_a = 2344, i_b = 23, i_c;

static inline unsigned short div10(unsigned short in) { 
    return (((unsigned long) in * 52429L)) >> 19;
}

void int16_div10(void) {
	i_c = i_a / 10;
}

void int16_div10_fast(void) {
	i_c = div10(i_a);
}

void int16_div16(void) {
	i_c = i_a / 16;
}

void int16_div(void) {
    i_c = i_a / i_b;
} 

void int16_mult_const(void) {
    i_c = i_a * 23;
} 

void int16_mult(void) {
    i_c = i_a * i_b;
} 

unsigned long l_a = 23444, l_b = 23, l_c;

void int32_div10(void) {
	l_c = l_a / 10;
}

void int32_div16(void) {
	l_c = l_a / 16;
}

void int32_div(void) {
    l_c = l_a / l_b;
} 

void int32_mult_const(void) {
    l_c = l_a * 234;
} 

void int32_mult(void) {
    l_c = l_a * l_b;
}

void int32_sqrt(void) {
	l_c = isqrt_c32(23442342);
}

void int32_sqrt_asm(void) {
#ifndef  __IAR_SYSTEMS_ICC__ 
#ifndef  CODE_VISION    
	l_c = isqrt32(23442342);
#endif
#endif
}

void int16_sqrt_asm(void) {
#ifndef  __IAR_SYSTEMS_ICC__
#ifndef  CODE_VISION       
	i_c = isqrt16(23442);
#endif    
#endif    
}

void int16_sqrt(void) {
	i_c = isqrt_c16(23442);
}

void int16_sin(void) {
	l_c = Sine(0x7FFF/6);
}


void empty_func(void) {
}

#define MAX_U_STELL 1000L
int16_t v_soll = 100, v_ist = 80, u_stell;

void int16_pi_regler(void) {
	static int32_t u_stell_l = 0;
	static int16_t reg_abw, reg_abw_old;
        static uint8_t kp = 50, ki = 15;
        reg_abw = (v_soll - v_ist);
	// PI-Regeler Geschwindigkeitsalgorithmus 
	u_stell_l  += reg_abw*kp + ((reg_abw*ki + 16) >> 5) - reg_abw_old*kp;
	if (u_stell_l > (MAX_U_STELL << 5))
            u_stell_l = MAX_U_STELL << 5;
        else if (u_stell_l < -(MAX_U_STELL << 5))
            u_stell_l = -(MAX_U_STELL << 5); 
        u_stell     = u_stell_l >> 5;
	reg_abw_old = reg_abw; 
}

#ifdef  __IAR_SYSTEMS_ICC__
/* This port correponds to the "-W 0x20,-" command line option. */
#define special_output_port (*((__io *) SIM_WIO))
/* This port correponds to the "-R 0x22,-" command line option. */
#define special_input_port  (*((__io *) SIM_RIO))

__no_init uint8_t WIO @ 0x30;

#else
/* This port correponds to the "-W 0x20,-" command line option. */
#define special_output_port (*((volatile char *) SIM_WIO))
/* This port correponds to the "-R 0x22,-" command line option. */
#define special_input_port  (*((volatile char *) SIM_RIO))
#endif

static void PrintString(char* str) {
#ifndef  __IAR_SYSTEMS_ICC__
#ifndef  CODE_VISION
    const char *c;
	for(c = str; *c; c++)
	  special_output_port = *c;
#endif
#else
   const char *c;
   for(c = str; *c; c++)
	  WIO = *c; 
#endif
  
}

static inline xputc(char c) {
	special_output_port = c; 
}

static
void xvprintf (
	const char*	fmt,	/* Pointer to the format string */
	va_list arp			/* Pointer to arguments */
)
{
	unsigned int r, i, j, w, f;
	unsigned long v, vold;
	char s[16], c, d, *p;


	for (;;) {
		c = *fmt++;					/* Get a char */
		if (!c) break;				/* End of format? */
		if (c != '%') {				/* Pass through it if not a % sequense */
			xputc(c); continue;
		}
		f = 0;
		c = *fmt++;					/* Get first char of the sequense */
		if (c == '0') {				/* Flag: '0' padded */
			f = 1; c = *fmt++;
		} else {
			if (c == '-') {			/* Flag: left justified */
				f = 2; c = *fmt++;
			}
		}
		for (w = 0; c >= '0' && c <= '9'; c = *fmt++)	/* Minimum width */
			w = w * 10 + c - '0';
		if (c == 'l' || c == 'L') {	/* Prefix: Size is long int */
			f |= 4; c = *fmt++;
		}
		if (!c) break;				/* End of format? */
		d = c;
		if (d >= 'a') d -= 0x20;
		switch (d) {				/* Type is... */
		case 'S' :					/* String */
			p = va_arg(arp, char*);
			for (j = 0; p[j]; j++) ;
			while (!(f & 2) && j++ < w) xputc(' ');
			PrintString(p);//xputs(p);
			while (j++ < w) xputc(' ');
			continue;
		case 'C' :					/* Character */
			xputc((char)va_arg(arp, int)); continue;
		case 'B' :					/* Binary */
			r = 2; break;
		case 'O' :					/* Octal */
			r = 8; break;
		case 'D' :					/* Signed decimal */
		case 'U' :					/* Unsigned decimal */
			r = 10; break;
		case 'X' :					/* Hexdecimal */
			r = 16; break;
		default:					/* Unknown type (passthrough) */
			xputc(c); continue;
		}

		/* Get an argument and put it in numeral */
		v = (f & 4) ? va_arg(arp, long) : ((d == 'D') ? (long)va_arg(arp, int) : (long)va_arg(arp, unsigned int));
		if (d == 'D' && (v & 0x80000000)) {
			v = 0 - v;
			f |= 8;
		}
		i = 0;
		do {
			//v = (vold = v)/r; d = (char)(vold - v*r); // leider nicht schneller
			d = (char)(v % r); v /= r;
			if (d > 9) d += (c == 'x') ? 0x27 : 0x07;
			s[i++] = d + '0';
		} while (v && i < sizeof(s));
		if (f & 8) s[i++] = '-';
		j = i; d = (f & 1) ? '0' : ' ';
		while (!(f & 2) && j++ < w) xputc(d);
		do xputc(s[--i]); while(i);
		while (j++ < w) xputc(' ');
	}
}


void xprintf (			/* Put a formatted string to the default device */
	const char*	fmt,	/* Pointer to the format string */
	...					/* Optional arguments */
)
{
	va_list arp;
	va_start(arp, fmt);
	xvprintf(fmt, arp);
	va_end(arp);
}


static void PrintSignedShortFormated(signed short value) {
    unsigned char sBuf0[7];
    unsigned char *sBuf = &sBuf0[6];
    unsigned char  minus;
    unsigned short valueAlt;   // Alte Wert f�r schneller % 10 Berechnung
    unsigned short valueCalc;  // muss unsigned weil signed short kann kein 32768
    
    *sBuf = 0;       // String Terminierung
    if (value < 0) { // Vorzeichen behandeln
      minus = '-';
      valueCalc = -value;
    } else {
      minus = ' ';
      valueCalc = value;
    }
    if (!valueCalc) 
      *--sBuf = '0';
    else while (valueCalc) {
      valueAlt = valueCalc;
      //  schneller als valueCalc /= 10;
      //valueCalc = div10(valueCalc); 
      valueCalc /= 10;
      
      // schneller als  *--sBuf = '0' + (valueAlt % 10)
      *--sBuf = '0' + valueAlt - (valueCalc * 10); 
    } 
    *--sBuf = minus;                  // Minuszeichen vor die h�chstwertigste Stelle setzen
    while (sBuf > sBuf0)              // mit Leerzeichen auff�llen (f�r Formatierung)  
      *--sBuf = ' ';
    PrintString((char*) sBuf0); // String ausgeben
}


void print_int16(void) {
	PrintSignedShortFormated(-32421);
}

static char* myitoa(signed long val) {
	static char buf[13];
	char *sBuf = &buf[12];
	unsigned char sign = val < 0;
	unsigned long value, valueOld;
	*sBuf = 0;
	if (sign)
		value = (unsigned long) -val;
	else
		value = (unsigned long) val;
	do {
		valueOld = value;
		value /= 10;
		*--sBuf = '0' + valueOld - (value * 10); // schneller als % 10
	}  while (value);
	if (sign)
		*--sBuf = '-';
	return sBuf;
}

static char *myitoa2(int32_t zahl) {
	static uint32_t subtractors[] = {1000000000,100000000,10000000,1000000,
                                     100000,10000,1000,100,10,1};
	static char string[12];
	char *str = string, n;	
	uint32_t u, *sub = subtractors;  
    uint8_t i = 10;
	if (zahl < 0) {                 
		*str++ = '-';              
		u = (uint32_t) -zahl;
	} else
		u = (uint32_t) zahl;
	// f�hrende nullen entfernen 			
	while (i > 1 && u < *sub) {
		if (u >= *sub)
			break;
		i--;
		sub++;
	}
	while (i--) {
		n = '0';
		while (u >= *sub) {
			n++;
			u -= *sub;
		} 
		*str++ = n;
		sub++;
	}
	*str = 0;
	return string;
}

void print_myitoa(void) {
   PrintString(myitoa(-32421)); 

}

void print_myitoa2(void) {
   PrintString(myitoa2(-32421));

}

void print_int16_itoa(void) {
	char sbuf[7];
#ifndef  __IAR_SYSTEMS_ICC__
#ifndef  CODE_VISION  
	itoa(-32421, sbuf, 10);
#endif
#endif
	PrintString(sbuf);
}

uint16_t buffer[10][10];

void testData() {
  	uint8_t x, y;
  	for (x = 0; x < 10; x++) {
            for (y = 0; y < 10; y++) {
                buffer[x][y] = x + y*10;
            }
  	}	
}

void test_loop1() {
  	uint8_t x, y;
  	for (x = 0; x < 10; x++) {
            for (y = 0; y < 10; y++) {
                buffer[x][y] = 0;
            }
  	}	
}

void test_loop2() {
  	uint16_t *ptr = (uint16_t *) buffer; 
  	uint8_t x;
  	for (x = 0; x < 10*10; x++) {
     	    *ptr++ = 0;
  	}
}

void test_loop3() {
  	uint16_t *ptr = (uint16_t *) buffer; 
  	uint8_t x = 10*10;
  	while (x--) {
     	    *ptr++ = 0;
  	}
}

void test_loop4() {
  	uint8_t x = 10;
  	uint16_t *ptr = (uint16_t *) buffer; 
   	do {
   		*ptr++ = 0;
   		*ptr++ = 0;
   		*ptr++ = 0; 
   		*ptr++ = 0; 
   		*ptr++ = 0; 
   		*ptr++ = 0; 
   		*ptr++ = 0; 
   		*ptr++ = 0; 
   		*ptr++ = 0; 
   		*ptr++ = 0; 		
   	} while (--x);
}

void test_xprintf() {
	xprintf("Wert = %10d", -34234);
}

/*
static void PrintSignedShort(signed short value) {
    unsigned char sBuf0[7];
    unsigned char *sBuf = &sBuf0[6];
    unsigned char x = 6, minus;
    unsigned short valueAlt;   // Alte Wert f�r schneller % 10 Berechnung
    unsigned short valueCalc;  // muss unsigned weil signed short kann kein 32768
    
    *sBuf = 0;
    if (value < 0) { // Vorzeichen behandeln
      minus = 1;
      valueCalc = -value;
    } else {
      minus = 0;
      valueCalc = value;
    }
    do {
      valueAlt = valueCalc;
      valueCalc = div10(valueCalc); //  schneller als valueCalc /= 10;
      *--sBuf = '0' + valueAlt - (valueCalc * 10); // schneller als  *--sBuf = '0' + (valueAlt % 10)
    } while (valueCalc);
    if (minus)
		*--sBuf = '-';                // Minuszeichen vor die h�chstwertigste Stelle setzen
    PrintString((const char*) sBuf0); // String ausgeben
}
*/

// typedef void (*test_func_t)(void);
#define TIME_FUNC(func) print_execute_time(func, (char*) "" # func) 

unsigned short print_execute_time(test_func_t function, char* name) {
	// Timer anhalten und initialisieren
	TCCR1A = 0;
	TCCR1B = 0;
	TCNT1  = 0;
	TIFR   = 0;
	TCCR1B = 1; // prescaler 1 Timer starten
	function();
	TCCR1B = 0; // Timer stoppen
	PrintSignedShortFormated(TCNT1);
	PrintString(" : ");
	PrintString(name);
	if (TIFR & (1 << 2))
		PrintString(" (Timeroverflow!)");	
	PrintString("\n");
	return TCNT1;
}

static void assert_(uint8_t compareTrue, uint16_t line) {
    if (!compareTrue) {
      PrintString("Assert Error in Line ");
      PrintSignedShortFormated(line);
      PrintString("\n");
    }
}

extern void chg_registers();

void main() {
	PrintString("\nZeitmessung Rechenoperationen\n");
	PrintString("\n Takte   Funktionsname\n");
	TIME_FUNC(float_convert_to_float);
	TIME_FUNC(float_convert_to_int);
	TIME_FUNC(float_add_const);
	TIME_FUNC(float_add);
	TIME_FUNC(float_mul_const);
	TIME_FUNC(float_mul);
	TIME_FUNC(float_div_const);
	TIME_FUNC(float_div);
	TIME_FUNC(int16_div10);
    //assert_(i_c == 234, __LINE__);
	TIME_FUNC(int16_div10_fast);
    //assert_(i_c == 234, __LINE__);
	TIME_FUNC(int16_div16);
	TIME_FUNC(int16_div);
	TIME_FUNC(int16_mult_const);
    //assert_(i_c == 53912, __LINE__);
	TIME_FUNC(int16_mult);
    //assert_(i_c == 53912, __LINE__);	
	TIME_FUNC(int32_div10);
    //assert_(l_c == 2344, __LINE__);
	TIME_FUNC(int32_div16);
	TIME_FUNC(int32_div);
	TIME_FUNC(int32_mult_const);
	TIME_FUNC(float_sqrt);
	TIME_FUNC(int16_sqrt);
    //assert_(i_c == 153, __LINE__);
	TIME_FUNC(int16_sqrt_asm);
    //assert_(i_c == 153, __LINE__);	
	TIME_FUNC(int32_sqrt);
    //assert_(l_c == 4841, __LINE__);
	TIME_FUNC(int32_sqrt_asm);
    //assert_(l_c == 4841, __LINE__);
	TIME_FUNC(float_sin);
	TIME_FUNC(int16_sin);
    testData();
	TIME_FUNC(test_loop1);
    //assert_(buffer[9][9] == 0, __LINE__);
	testData();
    TIME_FUNC(test_loop2);
    //assert_(buffer[9][9] == 0, __LINE__);
	testData();
    TIME_FUNC(test_loop3);
    //assert_(buffer[9][9] == 0, __LINE__);
	testData();
    TIME_FUNC(test_loop4);
    assert_(buffer[9][9] == 0, __LINE__);
    TIME_FUNC(int16_pi_regler);    
	TIME_FUNC(empty_func);  
	TIME_FUNC(print_int16);
	TIME_FUNC(print_int16_itoa);
    TIME_FUNC(print_myitoa);  
    TIME_FUNC(print_myitoa2);

	TIME_FUNC(test_xprintf);
   // TIME_FUNC(chg_registers);
        
	//PrintString(" - sqrt(23442.0)  = ");
	//PrintSignedShortFormated(sqrt(23442.0));	
	//PrintString("\n - isqrt16(23442) = ");
	//PrintSignedShortFormated(isqrt_c16(23442));
	//PrintString("\n");	
	//return 0;	
}
