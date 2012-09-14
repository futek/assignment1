#include "peaks.h"

#include "config.h"
#include "util.h"

#define DATA_BUFSIZE 3
#define PEAKS_BUFSIZE 8
#define RR_BUFSIZE 8

unsigned long time;

int data[DATA_BUFSIZE];
int peaks[PEAKS_BUFSIZE];
int recent_rr[RR_BUFSIZE], recent_rr_ok[RR_BUFSIZE];

// TODO: initial estimates?
int npkf, spkf;
int threshold1, threshold2;
int rr_average1, rr_average2;
int rr_low, rr_high, rr_miss;

int detect_peak(int value) {
  int peak_value, peak_time, rr, i, peak2, rpeak_detected = 0;

  shift(data, DATA_BUFSIZE);
  data[0] = value;

  // find peak
  if (data[2] < data[1] && data[1] > data[0]) {
    peak_value = data[1];
    peak_time = time - 1;

    if (peak_value > threshold1) {
      // store peak
      shift(peaks, PEAKS_BUFSIZE);
      peaks[0] = peak_value;

      // calculate rr
      rr = peak_time - last_rpeak_time;

      if (rr_low < rr && rr < rr_high) {
        // store peak as rpeak
        last_rpeak_value = peak_value;
        last_rpeak_time = peak_time;
        rpeak_detected = 1;

        // update estimates
        spkf = peak_value / 8 + spkf * 7 / 8;

        shift(recent_rr, RR_BUFSIZE);
        recent_rr[0] = rr;

        shift(recent_rr_ok, RR_BUFSIZE);
        recent_rr_ok[0] = rr;

        rr_average1 = 0;
        rr_average2 = 0;

        for (i = 0; i < RR_BUFSIZE; i++) {
          rr_average1 += recent_rr[i] / RR_BUFSIZE;
          rr_average2 += recent_rr_ok[i] / RR_BUFSIZE;
        }

        rr_low = rr_average2 * 92 / 100;
        rr_high = rr_average2 * 116 / 100;
        rr_miss = rr_average2 * 166 / 100;

        threshold1 = npkf + (spkf - npkf) / 4;
        threshold2 = threshold1 / 2;

        // update heartrate
        heartrate = SAMPLE_RATE * 60 / rr_average2;

        rr_misses = 0;
      } else {
        if (rr > rr_miss) {
          // search backwards through peaks
          for (i = 1; i < PEAKS_BUFSIZE; i++) {
            if (peak2 > threshold2) {
              // store peak as rpeak
              last_rpeak_value = peak_value;
              last_rpeak_time = peak_time - i;
              rpeak_detected = 1;

              // update estimates
              spkf = peak_value / 8 + spkf * 7 / 8;

              shift(recent_rr, RR_BUFSIZE);
              recent_rr[0] = rr;

              rr_average1 = 0;

              for (i = 0; i < RR_BUFSIZE; i++) {
                rr_average1 += recent_rr[i] / RR_BUFSIZE;
              }

              rr_low = rr_average2 * 92 / 100;
              rr_high = rr_average2 * 116 / 100;
              rr_miss = rr_average2 * 166 / 100;

              threshold1 = npkf + (spkf - npkf) / 4;
              threshold2 = threshold1 / 2;

              break;
            }
          }
        }

        rr_misses++;
      }
    } else {
      // update estimates
      npkf = peak_value / 8 + npkf * 7 / 8;
      threshold1 = npkf + (spkf - npkf) / 4;
      threshold2 = threshold1 / 2;
    }
  }

  time++;

  return rpeak_detected;
}
