#include <stdio.h>
#include <stdlib.h>

#include "sensor.h"
#include "filter.h"
#include "peaks.h"
#include "config.h"

int main(int argc, char *argv[]) {
  int raw_value, filtered_value;
  unsigned long time = 0;

  // initialize peak detection
  initialize();

  while (get_next_data(&raw_value)) {
    // filter
    filtered_value = filter(raw_value);

    // peak detection
    process_data(time, filtered_value);

    // advance time
    time++;
  }

  return EXIT_SUCCESS;
}
