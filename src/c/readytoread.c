// Copyright [2015] Pebble Technology

#include <pebble.h>

static Window *s_window;
static Layer *s_separator_layer;
static TextLayer *s_battery_layer;
static TextLayer *s_day_layer;
static TextLayer *s_monthday_layer;
static TextLayer *s_time_layer;

static char s_battery_buffer[6];   // "100%"
static char s_day_buffer[4];       // "SAT"
static char s_monthday_buffer[8];  // "JUN 07"
static char s_time_buffer[6];      // "23:59"

static const char *s_day_names[] = {
  "SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"
};

static const char *s_month_names[] = {
  "JAN", "FEB", "MAR", "APR", "MAY", "JUN",
  "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"
};

static void prv_separator_update(Layer *layer, GContext *ctx) {
  graphics_context_set_fill_color(ctx, GColorDarkGray);
  graphics_fill_rect(ctx, layer_get_bounds(layer), 0, GCornerNone);
}

static void prv_update_battery(BatteryChargeState state) {
  if (state.is_charging) {
    snprintf(s_battery_buffer, sizeof(s_battery_buffer), "+%d%%", state.charge_percent);
  } else {
    snprintf(s_battery_buffer, sizeof(s_battery_buffer), "%d%%", state.charge_percent);
  }

  GColor color;
  if (!state.is_charging && state.charge_percent <= 20) {
    color = PBL_IF_COLOR_ELSE(GColorRed, GColorWhite);
  } else {
    color = PBL_IF_COLOR_ELSE(GColorLightGray, GColorWhite);
  }
  text_layer_set_text_color(s_battery_layer, color);
  text_layer_set_text(s_battery_layer, s_battery_buffer);
}

static void prv_battery_handler(BatteryChargeState state) {
  prv_update_battery(state);
}

static void prv_update_display(struct tm *t) {
  snprintf(s_day_buffer, sizeof(s_day_buffer), "%s", s_day_names[t->tm_wday]);
  snprintf(s_monthday_buffer, sizeof(s_monthday_buffer), "%s %02d",
           s_month_names[t->tm_mon], t->tm_mday);
  strftime(s_time_buffer, sizeof(s_time_buffer), "%H:%M", t);

  GColor day_color;
  if (t->tm_wday == 0) {
    day_color = PBL_IF_COLOR_ELSE(GColorRed, GColorWhite);   // Sunday
  } else if (t->tm_wday == 6) {
    day_color = PBL_IF_COLOR_ELSE(GColorBlue, GColorWhite);  // Saturday
  } else {
    day_color = GColorWhite;
  }

  text_layer_set_text_color(s_day_layer, day_color);
  text_layer_set_text(s_day_layer, s_day_buffer);
  text_layer_set_text(s_monthday_layer, s_monthday_buffer);
  text_layer_set_text(s_time_layer, s_time_buffer);
}

static void prv_tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  prv_update_display(tick_time);
}

static void prv_window_load(Window *window) {
  Layer *root = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(root);
  int16_t w = bounds.size.w;
  int16_t mid = bounds.size.h / 2;

  // Top half layout (each row = 42px, total = 84px):
  //   Row 1: battery % (left) | day-of-week (right)
  //   Row 2: MMM DD (full width)
  int16_t row_h = 42;
  int16_t top_pad = (mid - row_h * 2) / 2;
  if (top_pad < 0) top_pad = 0;

  int16_t battery_w = 52;  // enough for "+100%"

  // Battery: top-left, vertically centered in row 1
  s_battery_layer = text_layer_create(
      GRect(w / 4 - battery_w / 2, top_pad + (row_h - 28) / 2, battery_w, 28));
  text_layer_set_text_alignment(s_battery_layer, GTextAlignmentCenter);
  text_layer_set_background_color(s_battery_layer, GColorClear);
  text_layer_set_text_color(s_battery_layer, PBL_IF_COLOR_ELSE(GColorLightGray, GColorWhite));  // overridden by prv_update_battery
  text_layer_set_font(s_battery_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  layer_add_child(root, text_layer_get_layer(s_battery_layer));

  // Day of week: right side of row 1
  s_day_layer = text_layer_create(GRect(battery_w, top_pad, w - battery_w, row_h));
  text_layer_set_text_alignment(s_day_layer, GTextAlignmentCenter);
  text_layer_set_background_color(s_day_layer, GColorClear);
  text_layer_set_text_color(s_day_layer, GColorWhite);
  text_layer_set_font(s_day_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  layer_add_child(root, text_layer_get_layer(s_day_layer));

  // MMM DD: full width, row 2
  s_monthday_layer = text_layer_create(GRect(0, top_pad + row_h, w, row_h));
  text_layer_set_text_alignment(s_monthday_layer, GTextAlignmentCenter);
  text_layer_set_background_color(s_monthday_layer, GColorClear);
  text_layer_set_text_color(s_monthday_layer, GColorWhite);
  text_layer_set_font(s_monthday_layer, fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK));
  layer_add_child(root, text_layer_get_layer(s_monthday_layer));

  // Horizontal separator at midpoint
  s_separator_layer = layer_create(GRect(8, mid - 1, w - 16, 2));
  layer_set_update_proc(s_separator_layer, prv_separator_update);
  layer_add_child(root, s_separator_layer);

  // Bottom half: 24-hour time "HH:MM"
  int16_t time_h = 54;
  int16_t time_y = mid + (mid - time_h) / 4;
  s_time_layer = text_layer_create(GRect(0, time_y, w, time_h));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorWhite);
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_ROBOTO_BOLD_SUBSET_49));
  layer_add_child(root, text_layer_get_layer(s_time_layer));

  time_t now = time(NULL);
  prv_update_display(localtime(&now));  // NOLINT(runtime/threadsafe_fn)
  prv_update_battery(battery_state_service_peek());
}

static void prv_window_unload(Window *window) {
  text_layer_destroy(s_battery_layer);
  text_layer_destroy(s_day_layer);
  text_layer_destroy(s_monthday_layer);
  text_layer_destroy(s_time_layer);
  layer_destroy(s_separator_layer);
}

static void prv_init(void) {
  s_window = window_create();
  window_set_window_handlers(s_window, (WindowHandlers){
    .load = prv_window_load,
    .unload = prv_window_unload
  });
  window_set_background_color(s_window, GColorBlack);
  window_stack_push(s_window, true);
  tick_timer_service_subscribe(MINUTE_UNIT, prv_tick_handler);
  battery_state_service_subscribe(prv_battery_handler);
}

static void prv_deinit(void) {
  tick_timer_service_unsubscribe();
  battery_state_service_unsubscribe();
  window_destroy(s_window);
}

int main(void) {
  prv_init();
  app_event_loop();
  prv_deinit();
}
