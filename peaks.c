#include "peaks.h"

#include "config.h"
#include "util.h"

#define PEAKS_BUFSIZE 8
#define RR_BUFSIZE 8

static int calculate_rr_average1(int rr) {
  static int n = RR_BUFSIZE - 1, x[RR_BUFSIZE * 2], sum;

  x[n] = x[n + RR_BUFSIZE] = rr;
  sum -= x[n + RR_BUFSIZE - 1];
  sum += x[n];

  if (--n < 0) n = RR_BUFSIZE - 1;

  return sum / RR_BUFSIZE;
}

static int calculate_rr_average2(int rr_ok) {
  static int n = RR_BUFSIZE - 1, x[RR_BUFSIZE * 2], sum;

  x[n] = x[n + RR_BUFSIZE] = rr_ok;
  sum -= x[n + RR_BUFSIZE - 1];
  sum += x[n];

  if (--n < 0) n = RR_BUFSIZE - 1;

  return sum / RR_BUFSIZE;
}

int detect_peak(int x0) {
  static int x1, x2;
  static unsigned long time;
  static struct peak peaks[PEAKS_BUFSIZE];
  static int npkf, spkf;
  static int threshold1, threshold2;
  static int rr_low = HEART_RATE * 92 / 100, rr_high = HEART_RATE * 116 / 100, rr_miss = HEART_RATE * 166 / 100;

  struct peak peak, peak2;
  int rr_average1, rr_average2;
  int i, rr, rpeak_detected = 0;

  // find peak
  if (x2 < x1 && x1 > x0) {
    peak.value = x1;
    peak.time = time - 1;

    if (peak.value > threshold1) {
      // store peak
      shift_peak(peaks, PEAKS_BUFSIZE);
      peaks[0] = peak;

      // calculate rr
      rr = peak.time - last_rpeak.time;

      if (rr_low < rr && rr < rr_high) {
        // store peak as rpeak
        last_rpeak.value = peak.value;
        last_rpeak.time = peak.time;
        rpeak_detected = 1;

        // update estimates
        spkf = peak.value / 8 + spkf * 7 / 8;

        rr_average1 = calculate_rr_average1(rr);
        rr_average2 = calculate_rr_average2(rr);

        rr_low = rr_average2 * 92 / 100;
        rr_high = rr_average2 * 116 / 100;
        rr_miss = rr_average2 * 166 / 100;

        threshold1 = npkf + (spkf - npkf) / 4;
        threshold2 = threshold1 / 2;

        // update heartrate
        heartrate = SAMPLE_RATE * 60 / rr;

        // reset successive rr misses counter
        rr_misses = 0;
      } else {
        if (rr > rr_miss) {
          // search backwards through peaks
          for (i = 1; i < PEAKS_BUFSIZE; i++) {
            peak2 = peaks[i];

            if (peak2.value > threshold2) {
              // store peak2 as rpeak
              last_rpeak.value = peak2.value;
              last_rpeak.time = peak2.time;
              rpeak_detected = 1;

              // update estimates
              spkf = peak.value / 8 + spkf * 7 / 8;

              rr_average1 = calculate_rr_average1(rr);

              rr_low = rr_average2 * 92 / 100;
              rr_high = rr_average2 * 116 / 100;
              rr_miss = rr_average2 * 166 / 100;

              threshold1 = npkf + (spkf - npkf) / 4;
              threshold2 = threshold1 / 2;

              // update heartrate
              heartrate = SAMPLE_RATE * 60 / rr;

              break;
            }
          }
        }

        // increment successive rr misses counter
        rr_misses++;
      }
    } else {
      // update estimates
      npkf = peak.value / 8 + npkf * 7 / 8;
      threshold1 = npkf + (spkf - npkf) / 4;
      threshold2 = threshold1 / 2;
    }
  }

  x2 = x1;
  x1 = x0;

  time++;

  return rpeak_detected;
}
