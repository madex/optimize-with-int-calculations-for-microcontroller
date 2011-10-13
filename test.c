#include <avr/io.h>

typedef void (*test_func_t)(void);


float a = 12.0, b = 12232.4, c = 0.0;
short d = 323;

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


#define TIME_FUNC(func) print_execute_time(func, "" # func) 

unsigned short print_execute_time(test_func_t function, char name[]) {
	// Timer anhalten und initialisieren
	TCCR1A = 0;
	TCCR1B = 0;
	TCNT1  = 0;
	TCCR1B = 1; // prescaler 1 Timer starten
	function();
	TCCR1B = 0; // Timer stoppen
	// print
	return TCNT1;
}

int main() {
	TIME_FUNC(float_convert_to_float);
	TIME_FUNC(float_convert_to_int);
	TIME_FUNC(float_add_const);
	TIME_FUNC(float_add);
	TIME_FUNC(float_mul_const);
	TIME_FUNC(float_mul);
	TIME_FUNC(float_div_const);
	TIME_FUNC(float_div);	
	return 0;	
}
