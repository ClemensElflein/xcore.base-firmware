#include <modm/architecture/interface/delay.hpp>

#include "board/board.hpp"
int main() {
  Board::initialize();

  while (1) {
    modm::delay_ms(1000);
    Board::Leds::toggle();
  }
}
