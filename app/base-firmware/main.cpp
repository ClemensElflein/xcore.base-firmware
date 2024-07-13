#include <tx_api.h>

#include <modm/architecture/interface/delay.hpp>
#include <modm/debug.hpp>

#include "board/board.hpp"
#include "board/ethernet/nx_ethernet.hpp"
#include "io/i2c/TxI2cTransaction.hpp"

TX_THREAD blink_thread;
uint8_t blink_thread_stack[2048];

void blink_thread_entry(uint32_t arg) {
  MODM_LOG_DEBUG << "Nx Initialized!" << modm::flush;
  uint8_t mac[6];
  int success = 0;

  modm::TxI2cTransaction<modm::I2cWriteReadTransaction> tx{0b1010011};
  u_int8_t address = 0xFA;
  tx.configureWriteRead(&address, 1, mac, sizeof(mac));
  Board::id::I2C::start(&tx);
  tx.blockingWait();

  auto now = modm::chrono::milli_clock::now();
  char buffer[100] = "";
  while (1) {
    for (size_t i = 0; i < sizeof(buffer); i++) {
      buffer[i] = 0;
    }
    Board::Leds::toggle();
    // tx_thread_sleep(100);
    // snprintf(buffer, sizeof(buffer), "Available: %u\r\n",
    // Board::Uart10::available());
    size_t read =
        Board::debug::Uart::readBlockingUntil((uint8_t *)buffer, 10, '\r', 100);
    Board::debug::Uart::writeBlocking((const uint8_t *)buffer, read);
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
  }
}
