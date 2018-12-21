//=======================================================
// filename: main.c
// author: Jeremy Mattfeld
// email: jmattfeld@scanivalve.com
//
// LowPassFilter() function and supporting test functions
//=======================================================

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIMULATED_DATA_PTS 3220
#define NUM_CHANNELS 64
#define INPUT_SAMPLE_LEN 20
#define FILTER_LEN 10
#define INPUT_BUFFER_LEN (FILTER_LEN - 1 + INPUT_SAMPLE_LEN)

// I/O file definitions.  These may need to be modified according to your environment.
#define INPUT_PATH "C:\\Users\\Jeremy.SV\\Documents\\octave-projects\\outputs\\"
#define INFILE "unfiltered.csv"
#define OUTPUT_PATH "C:\\Users\\Jeremy.SV\\Documents\\octave-projects\\data\\"
#define OUTFILE "filtered.csv"

// global static memory here.  Not sure if we need a buffer for each Px channel or just one for each Tx sensor.
float sampleBuffer[NUM_CHANNELS][INPUT_BUFFER_LEN];

// simulated in/out streams
float unfiltered[SIMULATED_DATA_PTS];
float filtered[SIMULATED_DATA_PTS];

// filter definitions here.  Make sure to redefine the FILTER_LEN constant above where appropriate
// FILTER_LEN 10 / cutoff freq=0.010417 Hz
double coeffs[FILTER_LEN] = { 0.016154,0.037930,0.093105,0.155901,0.196910,
							  0.196910,0.155901,0.093105,0.037930,0.016154 };

// FLITER_LEN 16 / cutoff freq=0.015625 Hz
//double coeffs[FILTER_LEN] = { 0.0096199,0.0144736,0.0281796,0.0484536,
//							  0.0718235,0.0942400,0.1117946,0.1214152,
//							  0.1214152,0.1117946,0.0942400,0.0718235,
//							  0.0484536,0.0281796,0.0144736,0.0096199 };

// FILTER_LEN 24 / cutoff freq=0.015625 Hz
//double coeffs[FILTER_LEN] = { 0.0061539,0.0075251,0.0114762,0.0177629,0.0259528,0.0354573,
//							0.0455770,0.0555560,0.0646414,0.0721428,0.0774877,0.0802668,
//							0.0802668,0.0774877,0.0721428,0.0646414,0.0555560,0.0455770,
//							0.0354573,0.0259528,0.0177629,0.0114762,0.0075251,0.0061539 };

// use to initialize input buffer to current temp if desired
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

///===================================================================================
/// Description:
///		Applies a discrete-time convolution ov the filter
///		coefficients over the input
///
///	Parameters:
///		int chan		- the channel that we are filtering samples for
///		float *tempin	- address of the first sample to filter in the input array
///		float *tempin	- address of the output array to copy the filtered samples to
///		int num			- number of samples to filter this invocation
///
///	Returns:
///		void
///===================================================================================
void LowPassFilter(int chan, float *tempin, float *tempout, int num)
{
	float acc;		// accumulator
	int i, k;		// sample idx, coeff idx
	static unsigned idx[NUM_CHANNELS];
	int n = idx[chan - 1] % INPUT_BUFFER_LEN; // input buffer idx
	int nBytes = num * sizeof(float);
	int nWrap;

	//printf("idx=%d, n=%d for this call\n", idx[chan - 1], n);

	if (n + num < INPUT_BUFFER_LEN)
	{
		//printf("copying %d bytes for %d floats to sampleBuffer\n", nBytes, num);
		memcpy(&sampleBuffer[chan - 1][n], tempin, nBytes);
	}
	else // wrap the new input samples around to the front of the input buffer
	{
		nWrap = n + num - INPUT_BUFFER_LEN;
		//printf("copying %d bytes for %d floats to hi side of sampleBuffer\n", nBytes - nWrap * sizeof(float), num - nWrap);
		memcpy(&sampleBuffer[chan - 1][n], tempin, nBytes - nWrap * sizeof(float));
		//printf("copying %d bytes for %d floats to lo side of sampleBuffer\n", nWrap * sizeof(float), nWrap);
		memcpy(&sampleBuffer[chan - 1][0], tempin + num - nWrap, nWrap * sizeof(float));
	}

	// apply filter to each input sample for this call
	for (i = 0; i < num; i++)
	{
		//printf("**********************************\n");
		acc = 0;
		for (k = 0; k < FILTER_LEN; k++)
		{
			// y(n) = sum_{k=0 to N-1} (h(k)*x(n-k))
			//printf("y[%d] += h[%d] * x[%d] = %f\n", ((n + i) % INPUT_BUFFER_LEN), k, ((n + i + INPUT_BUFFER_LEN - k) % INPUT_BUFFER_LEN), acc);
			acc += (float)coeffs[k] * sampleBuffer[chan - 1][(n + i + INPUT_BUFFER_LEN - k) % INPUT_BUFFER_LEN];
		}
		tempout[i] = acc;
	}

	// move the index into the input buffer for next function call
	idx[chan - 1] += num;
}

int ReadLine(FILE *fp, char * tstr) 
{
	int c, i, bEof;

	i = 0;
	bEof = 0;
	while (1) {
		c = fgetc(fp);
		if (c == EOF) {
			// End of file encountered
			tstr[i] = 0;
			bEof = 1;
			break;
		}
		else if ((c == '\r') || (c == '\n')) {
			// End of line
			tstr[i] = 0;
			i = 0;
			break;
		}
		else {
			// Build the string, dont return
			tstr[i] = (char)c;
			i++;
		}
	}
	return(bEof);
}

int main()
{
	int i, j, len;
	FILE *infp, *outfp;
	char tstr[24];
	const char * inpfilename = INPUT_PATH INFILE;
	const char * outfilename = OUTPUT_PATH OUTFILE;
	float *pSample, *pOut;

	// open input file
	printf("Opening %s\n", INFILE);
	infp = fopen(inpfilename, "r");
	if (NULL == infp)
		return -1;

	// open output file
	printf("Opening %s\n", OUTFILE);
	outfp = fopen(outfilename, "w");
	if (NULL == outfp)
		return -1;

	// read unfiltered data into array
	printf("Reading unfiltered data into array\n");
	i = 0;
	while (1)
	{
		if (ReadLine(infp, tstr))
			break;
		unfiltered[i] = strtof(tstr, NULL);
		i++;
	}

	// initialize signal array
	len = sizeof sampleBuffer[0] / sizeof(float);
	signalInit(0, sampleBuffer[0], len);
	printf("sample length: %d\n", INPUT_SAMPLE_LEN);
	printf("filter length: %d\n", FILTER_LEN);
	printf("input buffer length: %d\n", INPUT_BUFFER_LEN);

	// pointer to beginning of simulated datastream
	pSample = &unfiltered[0];
	pOut = &filtered[0];
	// apply filter (using single channel buffer)
	for (i = 0; i < SIMULATED_DATA_PTS; i += INPUT_SAMPLE_LEN)
	{
		//printf("processing unfiltered[%d .. %d]\n", i, i + INPUT_SAMPLE_LEN);
		//printf("*****************************\n");
		LowPassFilter(1, pSample, pOut, INPUT_SAMPLE_LEN);
		// print the filtered results to .CSV file
		for (j = 0; j < INPUT_SAMPLE_LEN; j++)
		{
			fprintf(outfp, "%.3f\n", filtered[i + j]);
		}
		// advance the datastream pointers
		pSample += INPUT_SAMPLE_LEN;
		pOut += INPUT_SAMPLE_LEN;
	}

	fclose(infp);
	fclose(outfp);

	return 0;
}
