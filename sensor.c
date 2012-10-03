#include <stdio.h>
#include <stdlib.h>

#include "sensor.h"

int get_next_data(int *data) {
  int rc;

  rc = scanf("%i", data);

  return (rc == 1);
}
