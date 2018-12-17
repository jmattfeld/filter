// main.c
#include <stdio.h>

#define FILTER_LEN 24
#define INPUT_BUFFER_LEN 100

float signal[INPUT_BUFFER_LEN];

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
	return 0.000;
}

int main()
{
	int i, len;

	// initialize signal array
	len = sizeof signal / sizeof(float);
	signalInit(15.000, signal, len);
	printf("length of signal: %d\n", sizeof signal / sizeof(float));
	
	for (i = 0; i < len; i++)
	{
		printf("%.3f\n", signal[i]);
	}

	return 0;
}
