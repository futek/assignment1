#include "peaks.h"

#include "config.h"

#define PEAKS_BUFSIZE 20
#define RR_BUFSIZE 8

static int peaks_size;
static struct peak peaks[PEAKS_BUFSIZE];

static struct peak last_rpeak;

static int npkf, spkf;
static int threshold1, threshold2;
static int rr_low = HEART_RATE_EST * 92 / 100, rr_high = HEART_RATE_EST * 116 / 100, rr_miss = HEART_RATE_EST * 166 / 100;

// TODO: make use of these
static int rr_misses;
static int heartrate;

static int calculate_rr_average1(int rr) {
  static int n = RR_BUFSIZE, x[(RR_BUFSIZE + 1) * 2], sum;

  x[n] = x[n + RR_BUFSIZE + 1] = rr;
  sum -= x[n + RR_BUFSIZE];
  sum += x[n];

  if (--n < 0) n = RR_BUFSIZE;

  return sum / RR_BUFSIZE;
}

static int calculate_rr_average2(int rr_ok) {
  static int n = RR_BUFSIZE, x[(RR_BUFSIZE + 1) * 2], sum;

  x[n] = x[n + RR_BUFSIZE + 1] = rr_ok;
  sum -= x[n + RR_BUFSIZE];
  sum += x[n];

  if (--n < 0) n = RR_BUFSIZE;

  return sum / RR_BUFSIZE;
}

static void store_peak(struct peak peak) {
  int i;

  if (peaks_size < PEAKS_BUFSIZE) {
    peaks[peaks_size++] = peak;
  } else {
    for (i = PEAKS_BUFSIZE - 1; i > 0; i--) {
      peaks[i] = peaks[i - 1];
    }
  }
}

static void store_rpeak(struct peak rpeak) {
  last_rpeak = rpeak;
  // TODO: output this
}

static void process_peak(struct peak peak) {
  int i, j, rr, rr_average1, rr_average2;
  struct peak peak2;

  if (peak.value > threshold1) {
    // calculate rr interval
    rr = peak.time - last_rpeak.time;

    if (rr_low < rr && rr < rr_high) {
      store_rpeak(peak);

      // update estimates
      spkf = peak.value / 8 + spkf * 7 / 8;
      threshold1 = npkf + (spkf - npkf) / 4;
      threshold2 = threshold1 / 2;

      calculate_rr_average1(rr);
      rr_average2 = calculate_rr_average2(rr);

      rr_low = rr_average2 * 92 / 100;
      rr_high = rr_average2 * 116 / 100;
      rr_miss = rr_average2 * 166 / 100;

      // clear peaks
      peaks_size = 0;

      // reset rr misses counter
      rr_misses = 0;

      // update heartrate
      heartrate = SAMPLE_RATE * 60 / rr;
    } else {
      // increment rr misses counter
      rr_misses++;

      if (rr > rr_miss) {
        // search back through peaks
        for (i = peaks_size - 2; i >= 0; i++) {
          peak2 = peaks[i];

          if (peak2.value > threshold2) {
            // recalculate rr interval
            rr = peak2.time - last_rpeak.time;

            store_rpeak(peak2);

            spkf = peak2.value / 4 + spkf * 3 / 4;
            threshold1 = npkf + (spkf - npkf) / 4;
            threshold2 = threshold1 / 2;

            rr_average1 = calculate_rr_average1(rr);

            rr_low = rr_average1 * 92 / 100;
            rr_high = rr_average1 * 116 / 100;
            rr_miss = rr_average1 * 166 / 100;

            // clear out peaks before found rpeak and the rpeak itself
            peaks_size = peaks_size - 1 - i;
            for (j = 0; j < peaks_size; j++) {
              peaks[j] = peaks[i + 1 + j];
            }

            // update heartrate
            heartrate = SAMPLE_RATE * 60 / rr;

            // reprocess the peak that initiated the searchback
            process_peak(peak);

            break;
          }
        }
      }
    }
  } else {
    // update estimates
    npkf = peak.value / 8 + npkf * 7 / 8;
    threshold1 = npkf + (spkf - npkf) / 4;
    threshold2 = threshold1 / 2;
  }
}

void process_data(long time, int x0) {
  static int x1, x2;

  struct peak peak;

  // find local peak
  if (x2 < x1 && x1 > x0) {
    peak.value = x1;
    peak.time = time - 1;

    store_peak(peak);
    process_peak(peak);
  }

  x2 = x1;
  x1 = x0;
}

void initialize() {
  int i;

  // initialize rr averages
  for (i = 0; i < RR_BUFSIZE; i++) {
    calculate_rr_average1(RR_EST);
    calculate_rr_average2(RR_EST);
  }
}
