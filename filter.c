#include "filter.h"

#include "util.h"

#define RAW_BUFSIZE 13
#define LOW_BUFSIZE 33
#define HIGH_BUFSIZE 5
#define DER_BUFSIZE 1
#define SQR_BUFSIZE 30
#define MWI_BUFSIZE 1

int x_raw[RAW_BUFSIZE];
int x_low[LOW_BUFSIZE];
int x_high[HIGH_BUFSIZE];
int x_der[DER_BUFSIZE];
int x_sqr[SQR_BUFSIZE];
int x_mwi[MWI_BUFSIZE];

int filter(int value) {
  int i;

  // raw data
  shift_int(x_raw, RAW_BUFSIZE);
  x_raw[0] = value;

  // low-pass
  shift_int(x_low, LOW_BUFSIZE);
  x_low[0] = 2 * x_low[1] - x_low[2] + (x_raw[0] - 2 * x_raw[6] + x_raw[12]) / 32;

  // high-pass
  shift_int(x_high, HIGH_BUFSIZE);
  x_high[0] = x_high[1] - x_low[0] / 32 + x_low[16] - x_low[17] + x_low[32] / 32;

  // derivative
  shift_int(x_der, DER_BUFSIZE);
  x_der[0] = (2 * x_high[0] + x_high[1] - x_high[3] - 2 * x_high[4]) / 8;

  // square
  shift_int(x_sqr, SQR_BUFSIZE);
  x_sqr[0] = x_der[0] * x_der[0];

  // moving window integration
  shift_int(x_mwi, MWI_BUFSIZE);
  x_mwi[0] = 0;
  for (i = 0; i < 30; i++) x_mwi[0] += x_sqr[i] / 30;

  return x_mwi[0];
}
