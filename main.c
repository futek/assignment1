#include <stdio.h>
#include <stdlib.h>

#include "sensor.h"
#include "filter.h"

int main(int argc, char *argv[]) {
  int before, after;

  while (1) {
    before = getNextData();
    after = filter(before);

    printf("%i\t->\t%i\n", before, after);
  }

  return EXIT_SUCCESS;
}
