/* MIT License - Copyright (c) 2019-2022 Francis Van Roie
   For full license information read the LICENSE file in the project folder */

#ifndef HASP_EVDEV_TOUCH_DRIVER_H
#define HASP_EVDEV_TOUCH_DRIVER_H

#if defined(POSIX) && defined(USE_FBDEV)
#include "touch_driver.h" // base class
#include "dev/device.h"   // for haspTft
#include "drv/tft/tft_driver.h"

#include <indev/evdev.h>

#include "../../hasp/hasp.h" // for hasp_sleep_state
extern uint8_t hasp_sleep_state;

namespace dev {

class TouchEvdev : public BaseTouch {

  public:
    void init(int w, int h)
    {
        LOG_TRACE(TAG_GUI, F("Init evdev: %s"), EVDEV_NAME);
        evdev_init();
    }

    IRAM_ATTR bool read(lv_indev_drv_t* indev_driver, lv_indev_data_t* data)
    {
        return evdev_read(indev_driver, data);
    }
};

} // namespace dev

using dev::TouchEvdev;
dev::TouchEvdev haspTouch;

#endif // defined(POSIX) && defined(USE_FBDEV)

#endif // HASP_EVDEV_TOUCH_DRIVER_H
