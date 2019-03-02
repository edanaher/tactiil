#include <pebble_worker.h>
#include "vibrate-time.h"

static AppTimer *s_timer = NULL;

static void delayed_tap_handler(void *ignored) {
  s_timer = NULL;
  buzz_time();
}

static void accel_tap_handler(AccelAxisType axis, int32_t direction) {
  static int enabled = 1;
  if(enabled)
    s_timer = app_timer_register(500, delayed_tap_handler, NULL);
}

int main() {
  APP_LOG(APP_LOG_LEVEL_INFO, "Starting the worker\n");
  accel_tap_service_subscribe(accel_tap_handler);
  worker_event_loop();
}
