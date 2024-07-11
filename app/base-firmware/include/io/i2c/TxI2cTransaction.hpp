//
// Created by clemens on 7/11/24.
//

#ifndef TXI2CDEVICE_HPP
#define TXI2CDEVICE_HPP

#include <tx_api.h>

#include <cstdint>
#include <modm/architecture/interface/i2c_transaction.hpp>

namespace modm {

/**
 * Wraps the I2cTransaction and provides a blockingWait() call.
 * Also block on the isBusy call, so if you run the tx with RF_BLOCKING_CALL()
 * it will block using semaphore instead of busy loop.
 */
template <class Transaction>
requires std::derived_from<Transaction, I2cTransaction>
class TxI2cTransaction : public Transaction {
 public:
  explicit TxI2cTransaction(uint8_t address) : Transaction(address) {
    tx_semaphore_create(&sema, "i2c_sema", 0);
  }

  virtual ~TxI2cTransaction() { tx_semaphore_delete(&sema); }

  void detaching(I2cTransaction::DetachCause cause) override {
    Transaction::detaching(cause);
    tx_semaphore_put(&sema);
  }

  bool isBusy() {
    blockingWait();
    return Transaction::isBusy();
  }

  void blockingWait() { tx_semaphore_get(&sema, TX_WAIT_FOREVER); }

 private:
  TX_SEMAPHORE sema{};
};

}  // namespace modm

#endif  // TXI2CDEVICE_HPP
