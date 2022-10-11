/* MIT License - Copyright (c) 2019-2022 openHASP Developers
   For full license information read the LICENSE file in the project folder */

#if defined(POSIX) && defined(USE_FBDEV)

#include "hasplib.h"
#include "lvgl.h"

#include "drv/tft/tft_driver.h"
#include <lv_drivers/display/fbdev.h>

#include "dev/device.h"
#include "hasp_debug.h"

#ifdef HASP_CUSTOMIZE_BOOTLOGO
#include "custom/bootlogo.h" // Sketch tab header for xbm images
#else
#include "custom/bootlogo_template.h" // Sketch tab header for xbm images
#endif

namespace dev {

int32_t TftFbdev::width()
{
    return _width;
}
int32_t TftFbdev::height()
{
    return _height;
}

void TftFbdev::init(int32_t w, int h)
{
    _width  = w;
    _height = h;

    /*Linux frame buffer device init*/
    fbdev_init();
}
void TftFbdev::show_info()
{
    splashscreen();

    LOG_VERBOSE(TAG_TFT, F("Driver     : fbdev"));
    LOG_VERBOSE(TAG_TFT, F("fbdev      : %s"), FBDEV_PATH);
}

void TftFbdev::splashscreen()
{

}

void TftFbdev::set_rotation(uint8_t rotation)
{
}

void TftFbdev::set_invert(bool invert)
{
}

void TftFbdev::flush_pixels(lv_disp_drv_t* disp, const lv_area_t* area, lv_color_t* color_p)
{
    fbdev_flush(disp, area, color_p);
}

bool TftFbdev::is_driver_pin(uint8_t pin)
{
    return false;
}

const char* TftFbdev::get_tft_model()
{
    return "fbdev";
}

} // namespace dev

dev::TftFbdev haspTft;

#endif // WINDOWS || POSIX
