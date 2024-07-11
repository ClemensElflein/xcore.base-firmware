//
// Created by clemens on 7/11/24.
//

#ifndef TXI2CMASTER_HPP
#define TXI2CMASTER_HPP

#include <tx_api.h>

#include <modm/architecture/interface/i2c.hpp>
#include <modm/architecture/interface/i2c_master.hpp>
#include <modm/architecture/interface/i2c_transaction.hpp>

namespace modm {
template <typename I2cMaster>
class TxI2cMaster : I2cMaster {
  /**
   * Set up the I2C module for master operation.
   *
   * @param	rate
   *		`Standard` or `Fast`, `High` datarate is not supported
   */
  template <class SystemClock, baudrate_t baudrate = kBd(100),
            percent_t tolerance = pct(5), uint32_t timeout_ms = 1000>
  static void initialize(uint8_t isrPriority = 10u) {
    tx_semaphore_create(&sema, "i2c sema", 0);
    I2cMaster::template initialize<SystemClock, baudrate, tolerance>(
        isrPriority);
  }

  static inline bool start(I2cTransaction *transaction,
                           I2c::ConfigurationHandler handler = nullptr) {
    const bool success = I2cMaster::start(transaction, handler);
    // Check, if the start was a success, if not we return here instead of
    // blocking the caller will try again
    if (!success) {
      return false;
    }
    // Wait for tx completion
  }

 private:
  static TX_SEMAPHORE sema{};
};
}  // namespace modm

#endif  // TXI2CMASTER_HPP
