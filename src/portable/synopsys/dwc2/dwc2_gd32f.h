/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2021, Ha Thach (tinyusb.org)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * This file is part of the TinyUSB stack.
 */

#ifndef _DWC2_GD32F_H_
#define _DWC2_GD32F_H_

#ifdef __cplusplus
extern "C" {
#endif

// EP_MAX       : Max number of bi-directional endpoints including EP0
// EP_FIFO_SIZE : Size of dedicated USB SRAM
#if CFG_TUSB_MCU == OPT_MCU_GD32F4
#include "gd32f4xx.h"
#define EP_MAX_FS 4
#define EP_FIFO_SIZE_FS 1280U
#define EP_MAX_HS 6
#define EP_FIFO_SIZE_HS 4096U

#else
#error "Unsupported MCUs"
#endif

// On GD32 we associate Port0 to OTG_FS, and Port1 to OTG_HS
#if TUD_OPT_RHPORT == 0
#define DWC2_REG_BASE USBFS_BASE
#define DWC2_EP_MAX EP_MAX_FS
#define DWC2_EP_FIFO_SIZE EP_FIFO_SIZE_FS
#define RHPORT_IRQn USBFS_IRQn

#else
#define DWC2_REG_BASE USBHS_BASE
#define DWC2_EP_MAX EP_MAX_HS
#define DWC2_EP_FIFO_SIZE EP_FIFO_SIZE_HS
#define RHPORT_IRQn USBHS_IRQn

#endif

extern uint32_t SystemCoreClock;

TU_ATTR_ALWAYS_INLINE
static inline void dwc2_dcd_int_enable(uint8_t rhport) {
  switch (rhport) {
    case 0:
      NVIC_EnableIRQ(USBFS_IRQn);
    break;

    case 1:
      NVIC_EnableIRQ(USBHS_IRQn);
    break;
  }
}

TU_ATTR_ALWAYS_INLINE
static inline void dwc2_dcd_int_disable(uint8_t rhport) {
  switch (rhport) {
    case 0:
      NVIC_DisableIRQ(USBFS_IRQn);
    break;

    case 1:
      NVIC_DisableIRQ(USBHS_IRQn);
    break;
  }
}

TU_ATTR_ALWAYS_INLINE
static inline void dwc2_remote_wakeup_delay(void) {
  // try to delay for 1 ms
  uint32_t count = SystemCoreClock / 1000;
  while (count--)
    __NOP();
}

// MCU specific PHY init, called BEFORE core reset
static inline void dwc2_phy_init(dwc2_regs_t *dwc2, uint8_t hs_phy_type) {

  if (hs_phy_type == HS_PHY_TYPE_NONE) {
    // Enable on-chip FS PHY
    dwc2->stm32_gccfg |= STM32_GCCFG_PWRDWN;
    dwc2->gusbcfg |= GUSBCFG_PHYSEL;
    // TODO: Put under a define?
    // Disable VBUS sensing
    dwc2->stm32_gccfg |= STM32_GCCFG_VBDEN;

  } else {
    // Disable FS PHY
    dwc2->stm32_gccfg &= ~STM32_GCCFG_PWRDWN;
    dwc2->gusbcfg &= ~GUSBCFG_PHYSEL;
  }
}

// MCU specific PHY update, it is called AFTER init() and core reset
static inline void dwc2_phy_update(dwc2_regs_t *dwc2, uint8_t hs_phy_type) {
  // used to set turnaround time for fullspeed, nothing to do in highspeed mode
  // if (hs_phy_type == HS_PHY_TYPE_NONE) {
  //   // Turnaround timeout depends on the AHB clock dictated by STM32
  //   Reference
  //   // Manual
  //   uint32_t turnaround;

  //   if (SystemCoreClock >= 32000000u)
  //     turnaround = 0x6u;
  //   else if (SystemCoreClock >= 27500000u)
  //     turnaround = 0x7u;
  //   else if (SystemCoreClock >= 24000000u)
  //     turnaround = 0x8u;
  //   else if (SystemCoreClock >= 21800000u)
  //     turnaround = 0x9u;
  //   else if (SystemCoreClock >= 200000000u)
  //     turnaround = 0xAu;
  //   else if (SystemCoreClock >= 18500000u)
  //     turnaround = 0xBu;
  //   else if (SystemCoreClock >= 17200000u)
  //     turnaround = 0xCu;
  //   else if (SystemCoreClock >= 16000000u)
  //     turnaround = 0xDu;
  //   else if (SystemCoreClock >= 15000000u)
  //     turnaround = 0xEu;
  //   else
  //     turnaround = 0xFu;

  //   dwc2->gusbcfg =
  //       (dwc2->gusbcfg & ~GUSBCFG_TRDT_Msk) | (turnaround <<
  //       GUSBCFG_TRDT_Pos);
  // }
}

#ifdef __cplusplus
}
#endif

#endif /* _DWC2_GD32F_H_ */
