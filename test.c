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

void float_sqrt() {
	c = sqrt(a);
}

void float_sin() {
	c = sin(a);	
}

unsigned short i_a = 2344, i_b = 23, i_c;

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
    i_c = i_a * 23;
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
#ifndef  __IAR_SYSTEMS_ICC__     
	l_c = isqrt32(23442342);
#endif
}

void int16_sqrt_asm() {
#ifndef  __IAR_SYSTEMS_ICC__       
	i_c = isqrt16(23442);
#endif        
}

void int16_sqrt() {
	i_c = isqrt_c16(23442);
}

void int16_sin() {
	l_c = Sine(0x7FFF/6);
}

#define MAX_U_STELL 1000L
int16_t v_soll = 100, v_ist = 80, u_stell;

void int16_pi_regler() {
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
#ifndef  __IAR_SYSTEMS_ICC__    
	itoa(-32421, sbuf, 10);
#endif
	PrintString((const char*) sbuf);
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
  	uint8_t x = 10*10;
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

static void assert_(uint8_t compareTrue, uint16_t line) {
    if (!compareTrue) {
      PrintString("Assert Error in Line ");
      PrintSignedShortFormated(line);
      PrintString("\n");
    }
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
        //assert_(buffer[9][9] == 0, __LINE__);
    TIME_FUNC(int16_pi_regler);    
	TIME_FUNC(print_int16);
	TIME_FUNC(print_int16_itoa);
	//PrintString(" - sqrt(23442.0)  = ");
	//PrintSignedShortFormated(sqrt(23442.0));	
	//PrintString("\n - isqrt16(23442) = ");
	//PrintSignedShortFormated(isqrt_c16(23442));
	//PrintString("\n");	
	return 0;	
}
