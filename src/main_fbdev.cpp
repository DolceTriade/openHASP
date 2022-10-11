/* MIT License - Copyright (c) 2022 OpenHASP Developers
   For full license information read the LICENSE file in the project folder */

#if defined(USE_FBDEV) && defined(POSIX)

#include <netdb.h>
#include <unistd.h>
#define cwd getcwd
#define cd chdir

#include <cstdlib>
#include <iostream>

#include "hasplib.h"

#include "display/monitor.h"

#include "hasp_debug.h"
#include "hasp_gui.h"

#include "dev/device.h"

bool isConnected;
bool isRunning = 1;

uint8_t mainLoopCounter        = 0;
unsigned long mainLastLoopTime = 0;

#ifdef HASP_USE_STAT_COUNTER
uint16_t statLoopCounter = 0; // measures the average looptime
#endif

extern uint16_t tft_width;
extern uint16_t tft_height;

void setup()
{
    // Load Settings

    // Init debug log
    // debug_init();

    // Initialize lvgl environment
    lv_init();
    lv_log_register_print_cb(debugLvglLogEvent);

    haspDevice.init();      // hardware setup
    haspDevice.show_info(); // debug info
    guiSetup();

    printf("%s %d\n", __FILE__, __LINE__);
    dispatchSetup(); // for hasp and oobe
    haspSetup();

#if HASP_USE_MQTT > 0
    printf("%s %d\n", __FILE__, __LINE__);
    mqttSetup(); // Hasp must be running
    mqttStart();
#endif

#if HASP_USE_GPIO > 0
    printf("%s %d\n", __FILE__, __LINE__);
    gpioSetup();
#endif

#if defined(HASP_USE_CUSTOM)
    custom_setup();
#endif

    mainLastLoopTime = millis(); // - 1000; // reset loop counter
    printf("%s %d\n", __FILE__, __LINE__);
    // delay(250);
}

void loop()
{
    haspLoop();
    mqttLoop();

    //    debugLoop(); // Console
    haspDevice.loop();
    guiLoop();

#if HASP_USE_GPIO > 0
    gpioLoop();
#endif

#if defined(HASP_USE_CUSTOM)
    custom_loop();
#endif

#ifdef HASP_USE_STAT_COUNTER
    statLoopCounter++; // measures the average looptime
#endif

    /* Timer Loop */
    if(millis() - mainLastLoopTime >= 1000) {
        /* Runs Every Second */
        haspEverySecond();     // sleep timer
        dispatchEverySecond(); // sleep timer

#if HASP_USE_ARDUINOOTA > 0
        otaEverySecond(); // progressbar
#endif

#if defined(HASP_USE_CUSTOM)
        custom_every_second();
#endif

        /* Runs Every 5 Seconds */
        if(mainLoopCounter == 0 || mainLoopCounter == 5) {

            haspDevice.loop_5s();
            gpioEvery5Seconds();

#if defined(HASP_USE_CUSTOM)
            custom_every_5seconds();
#endif
        }

        /* Reset loop counter every 10 seconds */
        ++mainLoopCounter %= 10;
        mainLastLoopTime += 1000;
    }
}

void usage(const char* progName)
{
    std::cout << progName << " [options]" << std::endl
              << std::endl
              << "Options:" << std::endl
              << "    -h | --help        Print this help" << std::endl
              << "    -n | --name        Plate hostname used in the mqtt topic"
              << std::endl
              //   << "    -b | --broker      Mqtt broker name or ip address" << std::endl
              //   << "    -P | --port        Mqtt broker port (default: 1883)" << std::endl
              //   << "    -u | --user        Mqtt username (optional)" << std::endl
              //   << "    -p | --pass        Mqtt password (optional)" << std::endl
              //   << "    -t | --topic       Base topic of the mqtt messages (default: hasp)" << std::endl
              //   << "    -g | --group       Group topic of on which to accept incoming messages (default: plates)"
              //   << std::endl
              //   << "    -f | --fullscreen  Open the application fullscreen" << std::endl
              //   << "    -v | --verbose     Verbosity level" << std::endl
              << std::endl;
    fflush(stdout);
}

int main(int argc, char* argv[])
{
    bool showhelp = false;
    int count;

    char buf[4096]; // never know how much is needed
    std::cout << "CWD: " << cwd(buf, sizeof(buf)) << std::endl;
    std::cout << "CWD changed to: " << cwd(buf, sizeof(buf)) << std::endl;

    // Change to preferences dir
    std::cout << "\nCommand-line arguments:\n";
    for(count = 0; count < argc; count++)
        std::cout << "  argv[" << count << "]   " << argv[count] << std::endl << std::flush;

    for(count = 0; count < argc; count++) {
        if(argv[count][0] == '-') {

            if(strncmp(argv[count], "--help", 6) == 0 || strncmp(argv[count], "-h", 2) == 0) {
                showhelp = true;
            }

            if(strncmp(argv[count], "--width", 7) == 0 || strncmp(argv[count], "-x", 2) == 0) {
                int w = atoi(argv[count + 1]);
                if(w > 0) tft_width = w;
            }

            if(strncmp(argv[count], "--height", 8) == 0 || strncmp(argv[count], "-y", 2) == 0) {
                int h = atoi(argv[count + 1]);
                if(h > 0) tft_height = h;
            }

            if(strncmp(argv[count], "--name", 6) == 0 || strncmp(argv[count], "-n", 2) == 0) {
                std::cout << "  argv[" << count << "]   " << argv[count] << std::endl << std::flush;
                fflush(stdout);
                if(count + 1 < argc) {
                    haspDevice.set_hostname(argv[count + 1]);
                } else {
                    showhelp = true;
                }
            }
        }
    }

    if(showhelp) {
        usage("openHASP");
        return 0;
    }

    debugPrintHaspHeader(stdout);
    LOG_INFO(TAG_MAIN, "resolution %d x %d", tft_width, tft_height);
    LOG_INFO(TAG_MAIN, "pre setup");

    setup();

    LOG_TRACE(TAG_MAIN, "loop started");
    while(1) {
        loop();
    }
    LOG_TRACE(TAG_MAIN, "main loop completed");

    return 0;
}

#endif
