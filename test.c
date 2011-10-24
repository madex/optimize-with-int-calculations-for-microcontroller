#ifdef  __IAR_SYSTEMS_ICC__
#include <ioavr.h>
#define PROGMEM __flash
#define PW(a) (a)
#else
#include <avr/io.h>
#include <stdlib.h>
#include <math.h>
#include <avr/pgmspace.h>
#define PW(a) pgm_read_word(&(a))
#endif

int16_t PROGMEM sin_table[66] = {

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
static uint32_t isqrt(uint32_t val) {
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

void float_convert_to_float() {
	c = d;
}

void float_convert_to_int() {
	d = c;
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

void float_sqrt() {
	c = sqrt(a);
}

void float_sin() {
	c = sin(a);	
}

unsigned short i_a = 23444, i_b = 23, i_c;

static inline unsigned short div10(unsigned short in) { 
    return (((unsigned long) in * 52429L)) >> 19;
}

void int16_div10() {
	i_c = i_a / 10;
}

void int16_div10_fast() {
	i_c = div10(i_a);
}

void int16_div16() {
	i_c = i_a / 16;
}

void int16_div() {
    i_c = i_a / i_b;
} 

void int16_mult_const() {
    i_c = i_a * 234;
} 

void int16_mult() {
    i_c = i_a * i_b;
} 

unsigned long l_a = 23444, l_b = 23, l_c;

void int32_div10() {
	l_c = l_a / 10;
}

void int32_div16() {
	l_c = l_a / 16;
}

void int32_div() {
    l_c = l_a / l_b;
} 

void int32_mult_const() {
    l_c = l_a * 234;
} 

void int32_mult() {
    l_c = l_a * l_b;
}

void int32_sqrt() {
	l_c = isqrt(23442342);
}

void int32_sqrt_asm() {
	l_c = isqrt32(23442342);
}

void int16_sqrt_asm() {
	l_c = isqrt16(23442);
}

void int16_sqrt() {
	l_c = isqrt_c16(23442);
}

void int16_sin() {
	l_c = Sine(0x7FFF/6);
}

#ifdef  __IAR_SYSTEMS_ICC__
/* This port correponds to the "-W 0x20,-" command line option. */
#define special_output_port (*((__io *) SIM_WIO))
/* This port correponds to the "-R 0x22,-" command line option. */
#define special_input_port  (*((__io *) SIM_RIO))
#else
/* This port correponds to the "-W 0x20,-" command line option. */
#define special_output_port (*((volatile char *) SIM_WIO))
/* This port correponds to the "-R 0x22,-" command line option. */
#define special_input_port  (*((volatile char *) SIM_RIO))
#endif

static PrintString(const char* str) {
	const char *c;
	for(c = str; *c; c++)
	  special_output_port = *c;
}

static void PrintSignedShortFormated(signed short value) {
    unsigned char sBuf0[7];
    unsigned char *sBuf = &sBuf0[6];
    unsigned char  minus;
    unsigned short valueAlt;   // Alte Wert für schneller % 10 Berechnung
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
      valueCalc = div10(valueCalc); 
      // schneller als  *--sBuf = '0' + (valueAlt % 10)
      *--sBuf = '0' + valueAlt - (valueCalc * 10); 
    } 
    *--sBuf = minus;                  // Minuszeichen vor die höchstwertigste Stelle setzen
    while (sBuf > sBuf0)              // mit Leerzeichen auffüllen (für Formatierung)  
      *--sBuf = ' ';
    PrintString((const char*) sBuf0); // String ausgeben
}


void print_int16(void) {
	PrintSignedShortFormated(-32421);
}

void print_int16_itoa(void) {
	unsigned char sbuf[7];
	itoa(-32421, sbuf, 10);
	PrintString(sbuf);
}

uint16_t buffer[16][16];

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
  	uint8_t x = 10*10-1;
  	uint16_t *ptr = (uint16_t *) buffer; 
   	do {
   		*ptr++ = 0;
   	} while (--x);
}


/*
static void PrintSignedShort(signed short value) {
    unsigned char sBuf0[7];
    unsigned char *sBuf = &sBuf0[6];
    unsigned char x = 6, minus;
    unsigned short valueAlt;   // Alte Wert für schneller % 10 Berechnung
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
		*--sBuf = '-';                // Minuszeichen vor die höchstwertigste Stelle setzen
    PrintString((const char*) sBuf0); // String ausgeben
}
*/

#define TIME_FUNC(func) print_execute_time(func, "" # func) 

unsigned short print_execute_time(test_func_t function, char name[]) {
	// Timer anhalten und initialisieren
	TCCR1A = 0;
	TCCR1B = 0;
	TCNT1  = 0;
	TIFR   = 0;
	TCCR1B = 1; // prescaler 1 Timer starten
	function();
	TCCR1B = 0; // Timer stoppen
	// print
	PrintSignedShortFormated(TCNT1);
	PrintString(" : ");
	PrintString(name);
	if (TIFR & (1 << 2))
		PrintString(" (Timeroverflow!)");	
	PrintString("\n");
	return TCNT1;
}

int main() {
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
	TIME_FUNC(int16_div10_fast);
	TIME_FUNC(int16_div16);
	TIME_FUNC(int16_div);
	TIME_FUNC(int16_mult_const);
	TIME_FUNC(int16_mult);	
	TIME_FUNC(int32_div10);
	TIME_FUNC(int32_div16);
	TIME_FUNC(int32_div);
	TIME_FUNC(int32_mult_const);
	TIME_FUNC(float_sqrt);
	TIME_FUNC(int16_sqrt);
	TIME_FUNC(int16_sqrt_asm);	
	TIME_FUNC(int32_sqrt);
	TIME_FUNC(int32_sqrt_asm);
	TIME_FUNC(float_sin);
	TIME_FUNC(int16_sin);
	TIME_FUNC(test_loop1);
	TIME_FUNC(test_loop2);
	TIME_FUNC(test_loop3);
	TIME_FUNC(test_loop4);
	
	TIME_FUNC(print_int16);
	TIME_FUNC(print_int16_itoa);
	PrintString(" - sqrt(23442.0)  = ");
	PrintSignedShortFormated(sqrt(23442.0));	
	PrintString("\n - isqrt16(23442) = ");
	PrintSignedShortFormated(isqrt_c16(23442));
	PrintString("\n");	
	return 0;	
}
