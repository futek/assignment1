#include "filter.h"

int low_pass(int value) {
  static int n = 12, x[26], y1, y2;
  int y0;

  x[n] = x[n + 13] = value;
  y0 = 2 * y1 - y2 + (x[n] - 2 * x[n + 6] + x[n + 12]) / 32;
  y2 = y1;
  y1 = y0;

  if (--n < 0) n = 12;

  return y0;
}

int high_pass(int value) {
  static int n = 32, x[66], y1;
  int y0;

  x[n] = x[n + 33] = value;
  y0 = y1 - x[n] / 32 + x[n + 16] - x[n + 17] + x[n + 32] / 32;
  y1 = y0;

  if (--n < 0) n = 32;

  return y0;
}

int derivative(int value) {
  static int n = 4, x[10];
  int y0;

  x[n] = x[n + 5] = value;
  y0 = (2 * x[n] + x[n + 1] - x[n + 3] - 2 * x[n + 4]) / 8;

  if (--n < 0) n = 4;

  return y0;
}

int square(int value) {
  return value * value;
}

int moving_window_integration(int value) {
  static int n = 29, x[60];
  int i, y0 = 0;

  x[n] = x[n + 30] = value;
  for (i = 0; i < 30; i++) y0 += x[n + i] / 30;

  if (--n < 0) n = 29;

  return y0;
}

int filter(int value) {
  return moving_window_integration(square(derivative(high_pass(low_pass(value)))));
}
