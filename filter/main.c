// main.c
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIMULATED_DATA_PTS 3220
#define NUM_CHANNELS 64
#define INPUT_SAMPLE_LEN 1
#define FILTER_LEN 10
#define INPUT_BUFFER_LEN FILTER_LEN - 1 + INPUT_SAMPLE_LEN
#define INFILE "unfiltered.csv"
#define OUTPUT_PATH "C:\\Users\\Jeremy.SV\\Documents\\octave-projects\\"
#define OUTFILE "filtered.csv"

float sampleBuffer[NUM_CHANNELS][INPUT_BUFFER_LEN];

// simulated in/out streams
float unfiltered[SIMULATED_DATA_PTS];
float filtered[SIMULATED_DATA_PTS];

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

float LowPassFilter(int chan, float tempin)
{
	float acc = 0;	// accumulator
	int k;			// coeff idx

	// circular buffer index
	static unsigned idx[NUM_CHANNELS];
	int n = idx[chan] % INPUT_BUFFER_LEN;

	// write new sample to the index of the oldest sample
	sampleBuffer[chan - 1][n] = tempin;

	// apply filter to the input sample:
	// y[n] = sum_{k=0}..{N-1}(h(k) * x(n-k))
	for (k = 0; k < FILTER_LEN; k++)
	{
		acc += (float)coeffs[k] * sampleBuffer[chan - 1][(n + INPUT_BUFFER_LEN - k) % INPUT_BUFFER_LEN];
	}

	// move the index for next function call and return sum
	idx[chan]++;
	return acc;
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
	int i, len;
	FILE *infp, *outfp;
	char tstr[24];
	//const char * outfilename = OUTPUT_PATH OUTFILE;

	// open input file
	printf("Opening %s for reading\n", INFILE);
	infp = fopen(INFILE, "r");
	if (NULL == infp)
		return -1;

	// open output file
	printf("Opening %s for writing\n", OUTFILE);
	//outfp = fopen(outfilename, "w");
	outfp = fopen(OUTFILE, "w");
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
	printf("Initializing input signal buffer\n");
	len = sizeof sampleBuffer[0] / sizeof(float);
	signalInit(0, sampleBuffer[0], len);

	// apply filter (using single channel buffer)
	printf("Applying filter to simulated signal stream\n");
	printf("and writing filtered data to output file\n");
	for (i = 0; i < SIMULATED_DATA_PTS; i++)
	{
		filtered[i] = LowPassFilter(1, unfiltered[i]);
		fprintf(outfp, "%.3f\n", filtered[i]);
	}

	printf("Closing %s\n", INFILE);
	fclose(infp);
	printf("Closing %s\n", OUTFILE);
	fclose(outfp);

	return 0;
}
