/*
 * Copyright (c) 2016-2017, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "board.hpp"

#include <modm/architecture/interface/assert.hpp>
#include <modm/architecture/interface/delay.hpp>

modm::I2cEeprom<Board::id::I2C, 1> Board::id::CoreEEPROM{0b10100110};

modm_extern_c void modm_abandon(const modm::AssertionInfo &info) {
  __disable_irq();
  Board::Leds::setOutput();
  for (int times = 10; times >= 0; times--) {
    Board::Leds::write(1);
    modm::delay_ms(20);
    Board::Leds::write(0);
    modm::delay_ms(180);
  }
  // Do not flush here otherwise you may deadlock due to waiting on the UART
  // interrupt which may never be executed when abandoning in a higher
  // priority Interrupt!!!
  // MODM_LOG_ERROR << modm::flush;
}

extern "C" void TxSysTick_Handler();
extern "C" void Modm_SysTick_Handler();
extern "C" void SysTick_Handler() {
  TxSysTick_Handler();
  Modm_SysTick_Handler();
}
