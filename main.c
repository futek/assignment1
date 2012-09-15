#include <stdio.h>
#include <stdlib.h>

#include "sensor.h"
#include "filter.h"
#include "peaks.h"

int main(int argc, char *argv[]) {
  int raw_value, filtered_value;

  while (1) {
    raw_value = getNextData();

    // filter
    filtered_value = filter(raw_value);

    // peak detection
    if (detect_peak(filtered_value)) {
      printf("rpeak time:  %ld\n", last_rpeak.time);
      printf("rpeak value: %i\n", last_rpeak.value);
      printf("heart rate:  %i\n", heartrate);

      if (last_rpeak.value < 2000) {
        printf("warning: weak heartbeat intensity!\n");
      }

      if (rr_misses >= 5) {
        printf("warning: irregular heartbeat rythm!\n");
      }

      printf("\n");
    }
  }

  return EXIT_SUCCESS;
}
