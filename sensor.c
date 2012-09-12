#include <stdio.h>
#include <stdlib.h>

#include "sensor.h"

int getNextData() {
  int rc, x;

  rc = fscanf(stdin, "%i", &x);
  if (rc <= 0) {
    exit(EXIT_FAILURE);
  }

  return x;
}
