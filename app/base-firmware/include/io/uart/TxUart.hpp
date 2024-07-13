//
// Created by clemens on 7/12/24.
//

#ifndef TXUART_HPP
#define TXUART_HPP

#include <tx_api.h>

#include <modm/platform/uart/uart.hpp>
#include <util/TxQueue.hpp>

namespace modm::platform {
template <typename Hal, size_t RxBufferSize = 512>
class TxUart : public UartBase, public Uart {
 private:
  static const uint8_t *volatile buffer;
  static volatile size_t txbuf_len;
  static TX_EVENT_FLAGS_GROUP events;
  static TxQueue<RxBufferSize> rxQueue;

  static constexpr ULONG EVENT_TX_EMPTY = 1;

 public:
  template <class... Signals>
  static void connect(
      Gpio::InputType InputTypeRx = Gpio::InputType::PullUp,
      Gpio::OutputType OutputTypeTx = Gpio::OutputType::PushPull) {
    using Connector = GpioConnector<Hal::UartPeripheral, Signals...>;
    using Tx = typename Connector::template GetSignal<Gpio::Signal::Tx>;
    using Rx = typename Connector::template GetSignal<Gpio::Signal::Rx>;
    static_assert(
        ((Connector::template IsValid<Tx> and
          Connector::template IsValid<Rx>)and sizeof...(Signals) == 2) or
            ((Connector::template IsValid<Tx> or
              Connector::template IsValid<Rx>)and sizeof...(Signals) == 1),
        "BufferedUart::connect() requires one Tx and/or one Rx signal!");

    Tx::setOutput(true);
    Tx::setOutput(OutputTypeTx);
    Rx::setInput(InputTypeRx);
    Connector::connect();
  }

  /// @warning Remember to set word length correctly when using the parity bit!
  template <class SystemClock, baudrate_t baudrate,
            percent_t tolerance = pct(1)>
  static inline void initialize(
      Hal::Parity parity = Hal::Parity::Disabled,
      Hal::WordLength length = Hal::WordLength::Bit8) {
    tx_event_flags_create(&events, "uart events");
    tx_event_flags_set(&events, EVENT_TX_EMPTY, TX_OR);
    rxQueue.Initialize();
    Hal::InterruptCallback = isr;
    Hal::template initialize<SystemClock, baudrate, tolerance>(parity, length);
    Hal::enableInterruptVector(true, 12);
    Hal::setTransmitterEnable(true);
    Hal::setReceiverEnable(true);
    Hal::enableOperation();
    Hal::enableInterrupt(Hal::Interrupt::RxNotEmpty);
  }

  static void writeBlocking(uint8_t data) { writeBlocking(&data, 1); }

  static void writeBlocking(const uint8_t *data, std::size_t length) {
    if (data == nullptr || length == 0) return;

    tx_event_flags_set(&events, ~EVENT_TX_EMPTY, TX_AND);

    buffer = data;
    txbuf_len = length;

    // transmission finished, disable TxEmpty interrupt
    Hal::enableInterrupt(Hal::Interrupt::TxEmpty);

    // Wait for Tx to finish
    ULONG actual = 0;
    tx_event_flags_get(&events, EVENT_TX_EMPTY, TX_OR, &actual,
                       TX_WAIT_FOREVER);
  }

  static void flushWriteBuffer() {
    // Wait for Tx to finish
    ULONG actual = 0;
    tx_event_flags_get(&events, EVENT_TX_EMPTY, TX_OR, &actual,
                       TX_WAIT_FOREVER);
  }

  static bool write(uint8_t data) {
    // Check, if buffer is empty
    ULONG actual = 0;
    tx_event_flags_get(&events, EVENT_TX_EMPTY, TX_OR, &actual, TX_NO_WAIT);

    if (actual & EVENT_TX_EMPTY) {
      // TX was empty, we can blocking write the byte
      writeBlocking(&data, 1);
      return true;
    }

    return false;
  }

  static size_t available() { return rxQueue.length(); }

  static bool isWriteFinished() {
    // Check, if buffer is empty
    ULONG actual = 0;
    tx_event_flags_get(&events, EVENT_TX_EMPTY, TX_OR, &actual, TX_NO_WAIT);
    return (actual & EVENT_TX_EMPTY) != 0;
  }

  static bool read(uint8_t &data) { return rxQueue.pop(&data, TX_NO_WAIT); }

  static std::size_t read(uint8_t *buffer, std::size_t amount) {
    return readBlocking(buffer, amount, TX_NO_WAIT);
  }

  static std::size_t readBlocking(uint8_t *buffer, std::size_t amount,
                                  ULONG wait_option = TX_WAIT_FOREVER) {
    size_t read_count = 0;
    for (read_count = 0; read_count < amount; read_count++) {
      if (!rxQueue.pop(buffer++, wait_option)) {
        // Cannot read further, quit
        return read_count;
      }
    }
    return read_count;
  }

  static std::size_t readBlockingUntil(uint8_t *buffer, std::size_t amount,
                                       uint8_t delim,
                                       ULONG wait_option = TX_WAIT_FOREVER) {
    size_t read_count = 0;
    for (read_count = 0; read_count < amount; read_count++) {
      if (!rxQueue.pop(buffer, wait_option)) {
        // Cannot read further, quit
        return read_count;
      }
      if (*buffer == delim) {
        // Got delim, return (+1 because read_count++ wasn't executed yet)
        return read_count + 1;
      }

      buffer++;
    }
    return read_count;
  }

  static std::size_t receiveBufferSize() {
    return Hal::isReceiveRegisterNotEmpty() ? 1 : 0;
  }

  static std::size_t discardReceiveBuffer() { return 0; }

  static bool hasError() {
    return Hal::getInterruptFlags().any(Hal::InterruptFlag::ParityError |
#ifdef USART_ISR_NE
                                        Hal::InterruptFlag::NoiseError |
#endif
                                        Hal::InterruptFlag::OverrunError |
                                        Hal::InterruptFlag::FramingError);
  }

  static void clearError() {
    return Hal::acknowledgeInterruptFlags(Hal::InterruptFlag::ParityError |
#ifdef USART_ISR_NE
                                          Hal::InterruptFlag::NoiseError |
#endif
                                          Hal::InterruptFlag::OverrunError |
                                          Hal::InterruptFlag::FramingError);
  }

 private:
  static bool isr(bool b) {
    if (Hal::getInterruptFlags().any(Hal::InterruptFlag::TxEmpty)) {
      Hal::acknowledgeInterruptFlags(Hal::InterruptFlag::TxEmpty);
      if (txbuf_len > 0) {
        Hal::write(*(buffer++));
        txbuf_len--;
      } else {
        buffer = nullptr;
        // transmission finished, disable TxEmpty interrupt
        Hal::disableInterrupt(Hal::Interrupt::TxEmpty);
        // Notify transmission done
        tx_event_flags_set(&events, EVENT_TX_EMPTY, TX_OR);
      }
    }

    if (Hal::getInterruptFlags().any(Hal::InterruptFlag::RxNotEmpty)) {
      uint8_t tmp;
      Hal::read(tmp);
      rxQueue.push(tmp, TX_NO_WAIT);
    }

    if (hasError()) {
      clearError();
    }

    return true;
  }
};

template <typename Hal, size_t RxBufferSize>
const uint8_t *volatile TxUart<Hal, RxBufferSize>::buffer = nullptr;
template <typename Hal, size_t RxBufferSize>
volatile size_t TxUart<Hal, RxBufferSize>::txbuf_len = 0;
template <typename Hal, size_t RxBufferSize>
TX_EVENT_FLAGS_GROUP TxUart<Hal, RxBufferSize>::events{};
template <typename Hal, size_t RxBufferSize>
TxQueue<RxBufferSize> TxUart<Hal, RxBufferSize>::rxQueue{};

}  // namespace modm::platform

#endif  // TXUART_HPP
