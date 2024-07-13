//
// Created by clemens on 7/13/24.
//

#ifndef TXQUEUE_HPP
#define TXQUEUE_HPP
#include <tx_api.h>

#include <cstdint>

template <size_t QueueSize>
class TxQueue {
 private:
  // This flag acts as a mutex. Get AND_CLEAR it to prevent others from
  // modifying the buffer
  static constexpr ULONG FLAG_QUEUE_UNLOCKED = 1;
  static constexpr ULONG FLAG_QUEUE_NOT_FULL = 2;
  static constexpr ULONG FLAG_QUEUE_NOT_EMPTY = 4;
  // Event flags to lock and also notify
  static TX_EVENT_FLAGS_GROUP queue_flags_;
  // Queue data buffer.
  static uint8_t buffer_[QueueSize];

  // The front_ points to the first item on the queue, the item_count_ member
  // keeps track of the valid items in the queue. We can easily insert into the
  // queue by pushing to the back (front_idx_ + item_count_) % QueueSize. We can
  // easily read from the queue by reading the front_ pointer, if item_count_ >
  // 0.
  static size_t front_idx_;
  static size_t item_count_;

 public:
  static bool Initialize() {
    if (tx_event_flags_create(&queue_flags_, "queue evts") != TX_SUCCESS)
      return false;

    if (tx_event_flags_set(&queue_flags_,
                           FLAG_QUEUE_UNLOCKED | FLAG_QUEUE_NOT_FULL,
                           TX_OR) != TX_SUCCESS)
      return false;
    return true;
  }

  size_t length() {
    // Lock the buffer
    ULONG flags = 0;
    if (tx_event_flags_get(&queue_flags_, FLAG_QUEUE_UNLOCKED, TX_AND_CLEAR,
                           &flags, TX_WAIT_FOREVER) != TX_SUCCESS) {
      // Timeout
      return 0;
    }
    size_t result = item_count_;
    // Lock
    tx_event_flags_set(&queue_flags_, FLAG_QUEUE_UNLOCKED, TX_OR);
    return result;
  }

  bool push(uint8_t data, ULONG wait_option = TX_WAIT_FOREVER) {
    // Lock the buffer, but only if not full
    ULONG flags = 0;
    if (tx_event_flags_get(&queue_flags_,
                           FLAG_QUEUE_UNLOCKED | FLAG_QUEUE_NOT_FULL,
                           TX_AND_CLEAR, &flags, wait_option) != TX_SUCCESS) {
      // Timeout
      return false;
    }

    // We locked the buffer and can be sure it's not full, so we can safely push
    // Calcuate the back index
    size_t back_idx = (front_idx_ + item_count_) % QueueSize;
    buffer_[back_idx] = data;
    item_count_++;

    flags = FLAG_QUEUE_UNLOCKED | FLAG_QUEUE_NOT_EMPTY;
    if (item_count_ != QueueSize) {
      flags |= FLAG_QUEUE_NOT_FULL;
    }
    tx_event_flags_set(&queue_flags_, flags, TX_OR);
    return true;
  }

  bool pop(uint8_t *data, ULONG wait_option = TX_WAIT_FOREVER) {
    // Lock the buffer, but only if not empty
    ULONG flags = 0;
    if (tx_event_flags_get(&queue_flags_,
                           FLAG_QUEUE_UNLOCKED | FLAG_QUEUE_NOT_EMPTY,
                           TX_AND_CLEAR, &flags, wait_option) != TX_SUCCESS) {
      // Timeout
      return false;
    }

    // We locked the buffer and can be sure it's not empty, so we can safely pop
    *data = buffer_[front_idx_];
    // Move index to next
    front_idx_ = (front_idx_ + 1) % QueueSize;
    // Decrease item_count_, so that the back points to the same item still.
    item_count_--;

    flags = FLAG_QUEUE_UNLOCKED | FLAG_QUEUE_NOT_FULL;
    if (item_count_ != 0) {
      flags |= FLAG_QUEUE_NOT_EMPTY;
    }
    tx_event_flags_set(&queue_flags_, flags, TX_OR);
    return true;
  }
};
template <size_t QueueSize>
TX_EVENT_FLAGS_GROUP TxQueue<QueueSize>::queue_flags_;

template <size_t QueueSize>
uint8_t TxQueue<QueueSize>::buffer_[QueueSize] = {0};

template <size_t QueueSize>
size_t TxQueue<QueueSize>::front_idx_ = 0;

template <size_t QueueSize>
size_t TxQueue<QueueSize>::item_count_ = 0;
#endif  // TXQUEUE_HPP
