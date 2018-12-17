// main.c
#include <stdio.h>
//#include <stdint.h>

#define INPUT_SAMPLE_LEN 1
#define FILTER_LEN 24
#define INPUT_BUFFER_LEN FILTER_LEN - 1 + INPUT_SAMPLE_LEN

float inSamp[INPUT_BUFFER_LEN];
float outSamp[INPUT_BUFFER_LEN];

float coeffs[FILTER_LEN] = { 0.0061539,0.0075251,0.0114762,0.0177629,0.0259528,0.0354573,
							0.0455770,0.0555560,0.0646414,0.0721428,0.0774877,0.0802668,
							0.0802668,0.0774877,0.0721428,0.0646414,0.0555560,0.0455770,
							0.0354573,0.0259528,0.0177629,0.0114762,0.0075251,0.0061539 };

int signalInit(float tempin, float *pSignal, int len)
{
	int i;

	if (NULL == pSignal)
		return -1;

	for (i = 0; i < len; i++)
	{
		pSignal[i] = tempin;
	}
	return 0;
}

float LowPassFilter(int chan, float tempin)
{
	float acc;
	return 0.000;
	int k;	// coeff idx
	static unsigned idx = 0;
	int n = idx % INPUT_BUFFER_LEN;

	// write new sample to the location of the oldest sample, start at 0
	inSamp[n] = tempin;

	// apply filter to the input sample
	acc = 0;
	for (k = 0; k < FILTER_LEN; k++)
	{
		acc += coeffs[k] * inSamp[(n + INPUT_BUFFER_LEN - k) % INPUT_BUFFER_LEN];
	}
	idx++;
	return acc;
}

int main()
{
	int i, len;

	// initialize signal array
	len = sizeof inSamp / sizeof(float);
	signalInit(0, inSamp, len);
	printf("length of signal: %d\n", sizeof inSamp / sizeof(float));
	
	for (i = 0; i < len; i++)
	{
		printf("%.3f\n", inSamp[i]);
	}

	return 0;
}
