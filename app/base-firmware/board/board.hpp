/*
 * Copyright (c) 2021, Christopher Durand
 * Copyright (c) 2021, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once

#include <io/uart/TxUart.hpp>
#include <modm/platform.hpp>

using namespace modm::platform;

#define MODM_BOARD_HAS_LOGGER

namespace Board {

using namespace modm::literals;

/// STM32H723ZG running at 550MHz from PLL clock generated from 25 MHz HSE
struct SystemClock {
  // Max 550MHz
  static constexpr uint32_t SysClk = 550_MHz;
  static constexpr uint32_t Pll1Q = SysClk / 2;
  // Max 550MHz
  static constexpr uint32_t Hclk = SysClk / 1;  // D1CPRE
  static constexpr uint32_t Frequency = Hclk;
  // Max 275MHz
  static constexpr uint32_t Ahb = Hclk / 2;  // HPRE
  static constexpr uint32_t Ahb1 = Ahb;
  static constexpr uint32_t Ahb2 = Ahb;
  static constexpr uint32_t Ahb3 = Ahb;
  static constexpr uint32_t Ahb4 = Ahb;
  // Max 137.5MHz
  static constexpr uint32_t Apb1 = Ahb / 2;  // D2PPRE1
  static constexpr uint32_t Apb2 = Ahb / 2;  // D2PPRE2
  static constexpr uint32_t Apb3 = Ahb / 2;  // D1PPRE
  static constexpr uint32_t Apb4 = Ahb / 2;  // D3PPRE

  static constexpr uint32_t Adc1 = Ahb1;
  static constexpr uint32_t Adc2 = Ahb1;
  static constexpr uint32_t Adc3 = Ahb4;

  static constexpr uint32_t Dac1 = Apb1;

  static constexpr uint32_t Spi1 = Pll1Q;
  static constexpr uint32_t Spi2 = Pll1Q;
  static constexpr uint32_t Spi3 = Pll1Q;
  static constexpr uint32_t Spi4 = Apb2;
  static constexpr uint32_t Spi5 = Apb2;
  static constexpr uint32_t Spi6 = Apb4;

  static constexpr uint32_t Usart1 = Apb2;
  static constexpr uint32_t Usart2 = Apb1;
  static constexpr uint32_t Usart3 = Apb1;
  static constexpr uint32_t Uart4 = Apb1;
  static constexpr uint32_t Uart5 = Apb1;
  static constexpr uint32_t Usart6 = Apb2;
  static constexpr uint32_t Uart7 = Apb1;
  static constexpr uint32_t Uart8 = Apb1;
  static constexpr uint32_t Uart9 = Apb2;
  static constexpr uint32_t Usart10 = Apb2;

  static constexpr uint32_t LpUart1 = Apb4;

  static constexpr uint32_t Can1 = Apb1;
  static constexpr uint32_t Can2 = Apb1;

  static constexpr uint32_t I2c1 = Apb1;
  static constexpr uint32_t I2c2 = Apb1;
  static constexpr uint32_t I2c3 = Apb1;
  static constexpr uint32_t I2c4 = Apb4;
  static constexpr uint32_t I2c5 = Apb1;

  static constexpr uint32_t Apb1Timer = Apb1 * 2;
  static constexpr uint32_t Apb2Timer = Apb2 * 2;
  static constexpr uint32_t Timer1 = Apb2Timer;
  static constexpr uint32_t Timer2 = Apb1Timer;
  static constexpr uint32_t Timer3 = Apb1Timer;
  static constexpr uint32_t Timer4 = Apb1Timer;
  static constexpr uint32_t Timer5 = Apb1Timer;
  static constexpr uint32_t Timer6 = Apb1Timer;
  static constexpr uint32_t Timer7 = Apb1Timer;
  static constexpr uint32_t Timer8 = Apb2Timer;
  static constexpr uint32_t Timer12 = Apb1Timer;
  static constexpr uint32_t Timer13 = Apb1Timer;
  static constexpr uint32_t Timer14 = Apb1Timer;
  static constexpr uint32_t Timer15 = Apb2Timer;
  static constexpr uint32_t Timer16 = Apb2Timer;
  static constexpr uint32_t Timer17 = Apb2Timer;
  static constexpr uint32_t Timer23 = Apb1Timer;
  static constexpr uint32_t Timer24 = Apb1Timer;

  static constexpr uint32_t Iwdg = Rcc::LsiFrequency;

  static bool inline enable() {
    // Switch core supply voltage to maximum level
    // Required for running at 550 MHz
    Rcc::setVoltageScaling(Rcc::VoltageScaling::Scale0);

    Rcc::enableExternalCrystal();  // 25 MHz
    const Rcc::PllFactors pllFactors1{
        .range = Rcc::PllInputRange::MHz8_16,
        .pllM = 2,
        .pllN = 44,
        .pllP = 1,
        .pllQ = 2,
        .pllR = 2,
    };
    Rcc::enablePll1(Rcc::PllSource::Hse, pllFactors1);

    Rcc::setFlashLatency<Ahb>();

    // max. 275MHz
    Rcc::setAhbPrescaler(Rcc::AhbPrescaler::Div2);
    // max. 137.5MHz on Apb clocks
    Rcc::setApb1Prescaler(Rcc::Apb1Prescaler::Div2);
    Rcc::setApb2Prescaler(Rcc::Apb2Prescaler::Div2);
    Rcc::setApb3Prescaler(Rcc::Apb3Prescaler::Div2);
    Rcc::setApb4Prescaler(Rcc::Apb4Prescaler::Div2);

    // update clock frequencies
    Rcc::updateCoreFrequency<Frequency>();
    // switch system clock to pll
    Rcc::enableSystemClock(Rcc::SystemClockSource::Pll1P);

    return true;
  }
};

using Button = GpioInputC13;

using LedRed = GpioOutputF11;
using LedGreen = GpioOutputF12;
using LedBlue = GpioOutputF13;
using Leds = SoftwareGpioPort<LedRed, LedGreen, LedBlue>;

namespace eth {
using RefClkPin = GpioA1;
using MdioPin = GpioA2;
using CrsPin = GpioA7;
using Txd0Pin = GpioB12;
using MdcPin = GpioC1;
using Rxd0Pin = GpioC4;
using Rxd1Pin = GpioC5;
using TxcPin = GpioG11;
using Txd1Pin = GpioG14;
}  // namespace eth

namespace id {
using SclPin = GpioD12;
using SdaPin = GpioD13;
using I2C = I2cMaster4;
}  // namespace id

namespace debug {
using TxPin = GpioE3;
using RxPin = GpioE2;
using Uart = TxUart<UsartHal10>;
}  // namespace debug

using LoggerDevice =
    modm::IODeviceWrapper<debug::Uart, modm::IOBuffer::BlockIfFull>;

inline void initialize() {
  // Need to enable->disable because ethernet driver calls DCache functions
  // which hard fault, if we don't do this on boot.
  SCB_EnableDCache();
  SCB_DisableDCache();
  SystemClock::enable();

  id::I2C::connect<id::SdaPin::Sda, id::SclPin::Scl>();
  id::I2C::initialize<SystemClock, 100_kHz>();

  LedGreen::setOutput(modm::Gpio::Low);
  LedBlue::setOutput(modm::Gpio::Low);
  LedRed::setOutput(modm::Gpio::Low);

  debug::Uart::connect<debug::TxPin::Tx, debug::RxPin::Rx>();
  debug::Uart::initialize<SystemClock, 1000000>();

  // Manually set AF of eth pins, since we don't use MODM HAL for those
  // (it's not done yet for Ethernet)
  eth::RefClkPin::setAlternateFunction(0x0B);
  eth::MdioPin::setAlternateFunction(0x0B);
  eth::CrsPin::setAlternateFunction(0x0B);
  eth::Txd0Pin::setAlternateFunction(0x0B);
  eth::MdcPin::setAlternateFunction(0x0B);
  eth::Rxd0Pin::setAlternateFunction(0x0B);
  eth::Rxd1Pin::setAlternateFunction(0x0B);
  eth::TxcPin::setAlternateFunction(0x0B);
  eth::Txd1Pin::setAlternateFunction(0x0B);

  Button::setInput();
}

}  // namespace Board
