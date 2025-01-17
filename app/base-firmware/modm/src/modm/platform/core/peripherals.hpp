/*
 * Copyright (c) 2019, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once

namespace modm::platform
{

/// @ingroup modm_platform_core
enum class
Peripheral
{
	BitBang,
	Adc1,
	Adc2,
	Adc3,
	Bdma,
	Comp1,
	Comp2,
	Cordic,
	Crc,
	Dac1,
	Dcmi,
	Debug,
	Dfsdm1,
	Dma1,
	Dma2,
	Dma2d,
	Dts,
	Eth,
	Fdcan1,
	Fdcan2,
	Fdcan3,
	Flash,
	Fmac,
	Fmc,
	Hdmicec,
	I2c1,
	I2c2,
	I2c3,
	I2c4,
	I2c5,
	I2s,
	I2s1,
	I2s2,
	I2s3,
	I2s6,
	Iwdg1,
	Lptim1,
	Lptim2,
	Lptim3,
	Lptim4,
	Lptim5,
	Lpuart1,
	Ltdc,
	Mdios,
	Mdma,
	Octospi1,
	Octospi2,
	Octospim,
	Opamp1,
	Opamp2,
	Pssi,
	Pwr,
	Ramecc,
	Rcc,
	Rng,
	Rtc,
	Sai1,
	Sai4,
	Sdmmc1,
	Sdmmc2,
	Spdifrx,
	Spdifrx1,
	Spi1,
	Spi2,
	Spi3,
	Spi4,
	Spi5,
	Spi6,
	Swpmi1,
	Sys,
	Tim1,
	Tim12,
	Tim13,
	Tim14,
	Tim15,
	Tim16,
	Tim17,
	Tim2,
	Tim23,
	Tim24,
	Tim3,
	Tim4,
	Tim5,
	Tim6,
	Tim7,
	Tim8,
	Uart4,
	Uart5,
	Uart7,
	Uart8,
	Uart9,
	Usart1,
	Usart10,
	Usart2,
	Usart3,
	Usart6,
	Usbotghs,
	Usbotghsulpi,
	Vrefbuf,
	Wwdg1,
	Syscfg = Sys,
};

}