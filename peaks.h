#ifndef PEAKS_H
#define PEAKS_H

struct peak {
  int value;
  unsigned long time;
};

struct peak last_rpeak;
int heartrate, rr_misses;

int detect_peak(int x0);

#endif
