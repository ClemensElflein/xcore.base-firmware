#include <tx_api.h>

#include <modm/architecture/interface/delay.hpp>

#include "board/board.hpp"

TX_THREAD blink_thread;
uint8_t blink_thread_stack[2048];

void blink_thread_entry(uint32_t arg) {
  auto now = modm::chrono::milli_clock::now();
  while (1) {
    Board::Leds::toggle();
    tx_thread_sleep(100);
  }
}

void tx_application_define(void *) {
  nx_ethernet_init();

  tx_thread_create(&blink_thread, "Blink", blink_thread_entry, 0,
                   blink_thread_stack, sizeof(blink_thread_stack), 1, 1,
                   TX_NO_TIME_SLICE, TX_AUTO_START);
}

int main() {
  Board::initialize();

  // Start ThreadX, so that advanced initializers can use ThreadX services
  tx_kernel_enter();
  // we should never get here.
  while (1) {
    modm::delay_ms(1000);
    Board::Leds::toggle();
  }
}
