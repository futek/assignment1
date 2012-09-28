#include <stdio.h>
#include <stdlib.h>

#include "sensor.h"

int get_next_data(int *data) {
  int rc;

  rc = fscanf(stdin, "%i", data);

  return (rc == 1);
}
