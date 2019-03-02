#include <pebble.h>
#include <time.h>

static Window *window;
static TextLayer *text_layer;

uint32_t short_vibrate_segments[] = { 200, 500, 80, 100, 80 };
VibePattern short_vibrate_pat = {
  .durations = short_vibrate_segments,
  .num_segments = 5
};
void short_vibrate() {
  vibes_enqueue_custom_pattern(short_vibrate_pat);
}

uint32_t long_vibrate_segments[] = { 200, 500, 200, 200, 100, 200, 200};
VibePattern long_vibrate_pat = {
  .durations = long_vibrate_segments,
  .num_segments = 7
};
void long_vibrate() {
  vibes_enqueue_custom_pattern(long_vibrate_pat);
}

char time_string[10];
uint32_t segments[20];
static void vibrate_number(int d, int bits) {
  int i;

  for(i = 0; i < bits; i++) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "On %d, %d => %d", i, d, (d >>i) & 1);
    if((d >> (bits - i - 1)) & 1)
      segments[2*i] = 70;
    else
      segments[2*i] = 20;
    segments[2 * i + 1] = 250;
  }
  VibePattern pat = {
    .durations = segments,
    .num_segments = bits * 2 - 1
  };
  vibes_enqueue_custom_pattern(pat);
}

static void buzzTime() {
  time_t t;
  time(&t);
  struct tm *now = localtime(&t);
  snprintf(time_string, 20, "%02d:%02d:%02d", now->tm_hour, now->tm_min, now->tm_sec);
  text_layer_set_text(text_layer, time_string);
  vibrate_number(now->tm_min, 6);
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(text_layer, "Select");
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(text_layer, "Up");
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(text_layer, "Down");
}

static void exit_click_handler(ClickRecognizerRef recognizer, void *context) {
  window_stack_pop_all(false);
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_multi_click_subscribe(BUTTON_ID_SELECT, 2, 2, 0, true, exit_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
  window_single_click_subscribe(BUTTON_ID_BACK, buzzTime);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  text_layer = text_layer_create((GRect) { .origin = { 0, 20 }, .size = { bounds.size.w, 80 } });
  text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28));
  text_layer_set_text(text_layer, "Press a button");
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(text_layer));
}

static void window_unload(Window *window) {
  text_layer_destroy(text_layer);
}

static void init(void) {
  window = window_create();
  window_set_click_config_provider(window, click_config_provider);
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(window, animated);
}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();

  bool enabled = !persist_read_bool(0);
  persist_write_bool(0, enabled);

  if(enabled) {
    long_vibrate();
    AppWorkerResult result = app_worker_launch();
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Starting worker: %d", result);
  } else {
    short_vibrate();
    AppWorkerResult result = app_worker_kill();
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Stopping worker: %d", result);
  }

  deinit();
}
