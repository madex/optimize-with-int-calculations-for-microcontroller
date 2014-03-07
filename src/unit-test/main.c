#include "tests.h"
#include <stdint.h>
#include <math.h>

// Auf einigen Sytemen war M_PI definiert aber auf anderen wieder nicht, deshalb:
#ifdef M_PI
#	undef M_PI
#endif
#define M_PI 3.1415926535897932384626433832795

#define NUM_ROWS 16
#define NUM_COLS 16

#define PLASMA_X     (1.0/(NUM_COLS/(2.0 * M_PI)))
#define PLASMA_X_INT (0x8000 / (NUM_COLS/2))

const int16_t sin_table[66] = {
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

int16_t Sine(int32_t phase)
{
        int16_t s0;
        uint16_t tmp_phase, tmp_phase_hi;

        tmp_phase = phase & 0x7fff;

        if (tmp_phase & 0x4000)
                tmp_phase = 0x8000 - tmp_phase;

        tmp_phase_hi = tmp_phase >> 8; // 0...64

        s0 = sin_table[tmp_phase_hi];

        s0 += ((int16_t)((((int32_t)(sin_table[tmp_phase_hi+1] - s0))*(tmp_phase&0xff))>>8));

        if (phase & 0x8000) {
                s0 = -s0;
        }

        return s0;
}

int16_t Cosi(int32_t phase)
{
    return Sine(phase + 0x4000);
}

/* by Jim Ulery  http://www.azillionmonkeys.com/qed/ulerysqroot.pdf  */
uint32_t isqrt(uint32_t val) {
    uint32_t temp, g = 0, b = 0x8000, bshft = 15;
    do {
        if (val >= (temp = (((g << 1) + b)<<bshft--))) {
            g += b;
            val -= temp;
        }
    } while (b >>= 1);
    return g;
}

static double dist_d(double x1, double y1, double x2, double y2)
{
	return sqrt(((x1 - x2) * (x1 - x2)) + ((y1 - y2) * (y1 - y2)));
}

static uint32_t dist_i(int32_t x1, int32_t y1, int32_t x2, int32_t y2)
{
	x1 -= x2;
	y1 -= y2;
	return isqrt(((long long)x1 * x1) + ((long long)y1 * y1));
}

/* Formel für die Plasma Animation 
   sin(x * PLASMA_X + t/10.0)) +
            sin( dist(x, y, NUM_COLS * sin(t) + NUM_COLS, NUM_ROWS * cos(t) + NUM_ROWS) *
                  PLASMA_X) 
                  + 2) * (NUMPLANE - 1) /
            2;
 */
 
START_TESTS()

START_TEST("Vergleiche int dist mit float dist.")
	int i, j, k, l, max = 0, help;
	ASSERT_EQUALS((int16_t) dist_d(1, 1, 1, 1),   dist_i(1, 1, 1, 1));
	ASSERT_EQUALS((int16_t) dist_d(1, 1, 4, 1),   dist_i(1, 1, 4, 1));
	ASSERT_EQUALS((int16_t) dist_d(1, 2, 10, 12), dist_i(1, 2, 10, 12));
	// Vorsicht sehr viele Testfälle ca. 640000. Laufen aber alle durch
	/*for (i = 0; i < 16; i++) {
		for (j = 0; j < 16; j++) {
			for (k = 0; k <= 32; k++) {  // 0 <= sin(x) * 16 + 16 <= 32 
				for (l = 0; l <= 32; l++) {
					ASSERT_EQUALS((int16_t) (256.0*dist_d(i, j, k, l)), dist_i(i*256, j*256, k*256, l*256));
				}
			}
		}
	}*/
END_TEST()
   
START_TEST("Vergleiche PLASMA_X")
	ASSERT_EQUALS((PLASMA_X * 0x8000 / M_PI), PLASMA_X_INT) 	
END_TEST()

// wegen 0.1 Schritten /10
START_TEST("Vergleiche sin(x * PLASMA_X + t/10.0)) mit integer implementierung")
	int i, x = 15;	  // TIP: Immer durch 2er Potenzen teilen
	ASSERT_EQUALS(0x7FFF*sin(0),        Sine(0));
	ASSERT_EQUALS(0x7FFF*sin(M_PI/2),   Sine(0x4000));
    ASSERT_EQUALS(0x7FFF*sin(M_PI),     Sine(0x8000));
    ASSERT_EQUALS(0x7FFF*sin(3*M_PI/2), Sine(0xC000));
	for (i = 0; i < 1000; i++) {
		ASSERT_EQUALS_IN_RANGE(0x7FFF*sin(x * PLASMA_X + i/10.0), // max(x) = 15 
		                       //Sine((15L*PLASMA_X_INT*128 + 133509L*i + 64)/128),  // ca. 20 mal genauer aber aufwendiger
		                       Sine(x*PLASMA_X_INT + 1043L*i),
							   120);  // +- (120 / 0x7FFF)  Genauigkeit	
	}
END_TEST()

#define TD      2
#define D(num)  ((1024*num*TD)/(0x7fff)) 

START_TEST("dist(x, y, NUM_COLS * sin(t) + NUM_COLS, NUM_ROWS * cos(t) + NUM_ROWS)")
  	int i, x = 0, y = 15;
  	printf("D(16) = %d\n", D(16));
	for (i = 0; i < 1000; i++) {
		ASSERT_EQUALS_IN_RANGE((int32_t) (16.0*1024.0*sin(i/10.0)), Sine(1043*i)/2, 60);	
		ASSERT_EQUALS_IN_RANGE((int32_t) (16.0*1024.0*cos(i/10.0)), Cosi(1043*i)/2, 60);
		ASSERT_EQUALS_IN_RANGE((int32_t) (1024.0*dist_d(x, y, NUM_COLS*sin(i/10.0) + NUM_COLS, 
		                                                NUM_ROWS*cos(i/10.0) + NUM_ROWS)),                                              
		                       dist_i(x*1024, y*1024, 
		                              (D(NUM_COLS)*Sine(1043*i))/TD + NUM_COLS*1024, 
		                              (D(NUM_ROWS)*Cosi(1043*i))/TD + NUM_ROWS*1024),
							   60); 
	}	////  bei 2048 gibts overflow Probleme
END_TEST()

START_TEST("sin(dist(..) * PLASMA_X) +2")
	ASSERT_EQUALS_IN_RANGE(0x7FFF*sin(dist_d(2, 3, 34, 55) * PLASMA_X),
	      Sine((dist_i(2*1024, 3*1024, 34*1024, 55*1024) * PLASMA_X_INT)/1024), 5);
 	
END_TEST() 

/* 
sin(dist_i(x, y, NUM_COLS * cos(t/10.0), NUM_ROWS * sin(t/10.0)) - t) + 1)
			* (NUMPLANE - 1)
 */
 
/* 
 *   0x8000/PI = 1043    0x8000/PI = 0x7FFF*NUM_COLS*c => c = 0x8000/(PI*0x7FFF*NUM_COLS) 
 *   c = 
 * 
 */ 
#define T2      16
#define B       (unsigned long) (0x8000/(M_PI*T2)
#define T       (1L << 11) 
#define C(num)  (unsigned long) ((T*0x8000*num)/(M_PI*0x7FFF*10)) 
 
START_TEST("dist(..) * PLASMA_X) - t") 
	int x = 3, y = 15, t;
	ASSERT_EQUALS_IN_RANGE( (C(10)*0x7FFF)/T, 0x8000/M_PI, 40);
	for (t = 0; t < 750; t++) {
		ASSERT_EQUALS_IN_RANGE((0x8000/M_PI)*(dist_d(x, y, NUM_COLS * cos(t/10.0), 
		                                                   NUM_ROWS * sin(t/10.0)) - t),
		                       10*dist_i(1043*x, 1043*y, (C(NUM_COLS)*Cosi(1043L*t))/T,
		                                                 (C(NUM_ROWS)*Sine(1043L*t))/T) -
		                       10430L*t,
		                       430); 
	}
END_TEST()

END_TESTS()
