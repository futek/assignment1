#include "util.h"

void shift_int(int buffer[], int size) {
  int i;

  for (i = size - 1; i > 0; i--) {
    buffer[i] = buffer[i - 1];
  }
}

void shift_peak(struct peak buffer[], int size) {
  int i;

  for (i = size - 1; i > 0; i--) {
    buffer[i] = buffer[i - 1];
  }
}