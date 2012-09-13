#include "util.h"

void shift(int buffer[], int size) {
  int i;

  for (i = size - 1; i > 0; i--) {
    buffer[i] = buffer[i - 1];
  }
}
