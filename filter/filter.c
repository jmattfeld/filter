// filter.c
#include "filter.h"

void filterCoeffsGet(float *p)
{
	int i;

	for (i = 0; i < 8; i++)
	{
		*(p + i) = coeffs[i];
	}
}
