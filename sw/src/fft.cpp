//---------------------------------------------------------------------------


#pragma hdrstop

#include "fft.h"
#include <math.h>
#define SWAP(a,b) tempr=(a);(a)=(b);(b)=tempr
//---------------------------------------------------------------------------

#pragma package(smart_init)
//---------------------------------------------------------------------------

void four1(float data[], unsigned long nn, int isign)
// Replaces data[1..2*nn] by its discrete Fourier transform, if isign is input as 1; or replaces
// data[1..2*nn] by nn times its inverse discrete Fourier transform, if isign is input as −1.
// data is a complex array of length nn or, equivalently, a real array of length 2*nn. nn MUST
// be an integer power of 2 (this is not checked for!).
{
unsigned long n, mmax, m, j, istep, i;
double wtemp, wr, wpr, wpi, wi, theta;
float tempr, tempi;
n = nn << 1;
j = 1;
for (i = 1; i<n; i+=2)
	{
	if (j > i)
		{
		SWAP(data[j-1],data[i-1]);
		SWAP(data[j],data[i]);
		}
	m = nn;
	while (m >= 2 && j > m)
		{
		j -= m;
		m >>= 1;
		}
	j += m;
	}

mmax = 2;
while (n > mmax)
	{
	istep=mmax << 1;
	theta=isign*(6.28318530717959/mmax);
	wtemp=sin(0.5*theta);
	wpr = -2.0*wtemp*wtemp;
	wpi=sin(theta);
	wr=1.0;
	wi=0.0;
	for (m=1;m<mmax;m+=2)
		{
		for (i=m;i<=n;i+=istep)
			{
			j=i+mmax;
			tempr=wr*data[j-1]-wi*data[j];
			tempi=wr*data[j]+wi*data[j-1];
			data[j-1]=data[i-1]-tempr;
			data[j]=data[i]-tempi;
			data[i-1] += tempr;
			data[i] += tempi;
			}
		wr=(wtemp=wr)*wpr-wi*wpi+wr;
		wi=wi*wpr+wtemp*wpi+wi;
		}
	mmax=istep;
	}
}
//---------------------------------------------------------------------------
