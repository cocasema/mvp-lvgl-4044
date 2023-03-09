#include "lvgl.h"

#include <chrono>
#include <cstdio>
#include <thread>

//-----------------------------------------------------------------------------

uint32_t now_ms() {
  return std::chrono::duration_cast<std::chrono::milliseconds>(
             std::chrono::system_clock::now().time_since_epoch())
      .count();
}

extern "C" uint32_t millis() { return now_ms(); }

struct Timer {
  uint32_t interval;
  uint32_t last_time;

  Timer(uint32_t interval) : interval(interval), last_time(millis()) {}

  bool execute() {
    uint32_t now = millis();
    if (now - last_time >= interval) {
      last_time = now;
      return true;
    }
    return false;
  }
};

//-----------------------------------------------------------------------------

#ifndef CASE
#define CASE 0
#endif

#if CASE == 0
enum { DISP_BUFFER_SIZE = 240 * 240 };
lv_color_t disp_buffer[DISP_BUFFER_SIZE];
#elif CASE == 1 || CASE == 2
enum { DISP_BUFFER_SIZE = 240 * 60 };
lv_color_t disp_buffer[DISP_BUFFER_SIZE];
#endif

lv_disp_draw_buf_t draw_buf{};

lv_disp_drv_t disp_drv{};

lv_disp_drv_t *flush_disp_drv = nullptr;

void display_flush(lv_disp_drv_t *disp, lv_area_t const *, lv_color_t *) {
  printf("display_flush {\n");
#if CASE == 2
  lv_disp_flush_ready(disp);
#else
  // do nothing
  flush_disp_drv = disp;
#endif
  printf("} display_flush\n");
}

void init() {
  lv_init();

  lv_disp_draw_buf_init(&draw_buf, disp_buffer, nullptr, DISP_BUFFER_SIZE);

  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res = 240;
  disp_drv.ver_res = 240;
  disp_drv.draw_buf = &draw_buf;
  disp_drv.flush_cb = display_flush;
  lv_disp_drv_register(&disp_drv);
}

Timer gui_update_timer(500);

void update() {
  printf("\n--- %u ---\n", millis());
  printf("update {\n");

  if (flush_disp_drv) {
    printf("lv_disp_flush_ready {\n");
    lv_disp_flush_ready(flush_disp_drv);
    flush_disp_drv = nullptr;
    printf("} lv_disp_flush_ready\n");
  }

  if (gui_update_timer.execute()) {
    printf("lv_task_handler {\n");
    lv_task_handler();
    printf("} lv_task_handler\n");
  }

  printf("} update\n");
}

int main() {
  init();
  while (true) {
    update();
    std::this_thread::sleep_for(std::chrono::milliseconds(250));
  }
  return 0;
}