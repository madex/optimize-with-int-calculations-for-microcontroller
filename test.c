#ifdef  __IAR_SYSTEMS_ICC__
#include <ioavr.h>
#else
#include <avr/io.h>
#endif

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
	PrintString(" = ");
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
	TIME_FUNC(int32_mult);		
	return 0;	
}
