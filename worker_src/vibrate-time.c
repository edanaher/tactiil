#include <pebble.h>
#include <time.h>

uint32_t short_vibrate_segments[] = { 30 };
VibePattern short_vibrate_pat = {
  .durations = short_vibrate_segments,
  .num_segments = 1
};
void short_vibrate() {
  vibes_enqueue_custom_pattern(short_vibrate_pat);
}

char time_string[10];
uint32_t segments[20];
static void vibrate_number(int d, int bits) {
  int i;

  segments[0] = 110;
  segments[1] = 300;
  for(i = 0; i < bits; i++) {
    //APP_LOG(APP_LOG_LEVEL_DEBUG, "On %d, %d => %d", i, d, (d >>i) & 1);
    if((d >> (bits - i - 1)) & 1) {
      segments[2*i + 2] = 110;
      segments[2 * i + 3] = 170;
    } else {
      segments[2*i + 2] = 40;
      segments[2 * i + 3] = 240;
    }
  }
  VibePattern pat = {
    .durations = segments,
    .num_segments = bits * 2 + 1
  };
  vibes_enqueue_custom_pattern(pat);
}

void buzz_time() {
  time_t t;
  time(&t);
  struct tm *now = localtime(&t);
  vibrate_number(now->tm_min, 6);
}
