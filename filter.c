#include "filter.h"

static long low_pass(long value) {
  static int n = 12;
  static long x[26], y1, y2;
  long y0;

  x[n] = x[n + 13] = value;
  y0 = 2 * y1 - y2 + (x[n] - 2 * x[n + 6] + x[n + 12]) / 32;
  y2 = y1;
  y1 = y0;

  if (--n < 0) n = 12;

  return y0;
}

static long high_pass(long value) {
  static int n = 32;
  static long x[66], y1;
  long y0;

  x[n] = x[n + 33] = value;
  y0 = y1 - x[n] / 32 + x[n + 16] - x[n + 17] + x[n + 32] / 32;
  y1 = y0;

  if (--n < 0) n = 32;

  return y0;
}

static long derivative(long value) {
  static int n = 4;
  static long x[10];
  long y0;

  x[n] = x[n + 5] = value;
  y0 = (2 * x[n] + x[n + 1] - x[n + 3] - 2 * x[n + 4]) / 8;

  if (--n < 0) n = 4;

  return y0;
}

static long square(long value) {
  return value * value;
}

static long moving_window_integration(long value) {
  static int n = 30;
  static long x[62], sum;

  x[n] = x[n + 31] = value;
  sum -= x[n + 30];
  sum += x[n];

  if (--n < 0) n = 30;

  return sum / 30;
}

int filter(int value) {
  return moving_window_integration(square(derivative(high_pass(low_pass(value)))));
}
