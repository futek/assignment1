#ifndef PEAKS_H
#define PEAKS_H

struct peak {
  int value;
  unsigned long time;
};

void process_data(long time, int x0);
void initialize();

#endif
