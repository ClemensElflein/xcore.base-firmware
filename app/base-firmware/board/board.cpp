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
#include <modm/debug/logger/logger.hpp>

Board::LoggerDevice loggerDevice;

// Set all four logger streams to use the UART
modm::log::Logger modm::log::debug(loggerDevice);
modm::log::Logger modm::log::info(loggerDevice);
modm::log::Logger modm::log::warning(loggerDevice);
modm::log::Logger modm::log::error(loggerDevice);

// Default all calls to printf to the UART
modm_extern_c void putchar_(char c) { loggerDevice.write(c); }

modm_extern_c void modm_abandon(const modm::AssertionInfo &info) {
  Board::Leds::setOutput();
  for (int times = 10; times >= 0; times--) {
    Board::Leds::write(1);
    modm::delay_ms(20);
    Board::Leds::write(0);
    modm::delay_ms(180);
  }
}

extern "C" void TxSysTick_Handler();
extern "C" void Modm_SysTick_Handler();
extern "C" void SysTick_Handler() {
  TxSysTick_Handler();
  Modm_SysTick_Handler();
}
