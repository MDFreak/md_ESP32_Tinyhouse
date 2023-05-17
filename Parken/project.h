#ifndef _PROJECT_H_
  #define _PROJECT_H_

  #include <Arduino.h>
  #include <md_defines.h>

  // ******************************************
  // --- project configuration
    // --- projects
      #define TOUCHTEST_1        1
      #define MEASFREQ_1         2
      #define LIGHTSHOW_1        3
      #define GEN_ESP32_NODE     4
      #define GEN_ESP32_D1_MINI  5
      #define GEN_ESP32_D1_R32   6
      #define ESP32_TEST2        7
      #define BLUETTI_CONTROL    8
      #define ESP32_TINYHOUSE    9

      #define PROJECT ESP32_TINYHOUSE
        //#define PROJECT BLUETTI_CONTROL
        //#define PROJECT ESP32_TEST2
        //#define PROJECT TOUCHTEST_1
        //#define PROJECT MEASFREQ_1
        //#define PROJECT LIGHTSHOW_1
        //#define PROJECT GEN_ESP32_NODE
        //#define PROJECT GEN_XIAO_ESP32C3   !!!  nicht kompatibel
        //#define PROJECT GEN_ESP32_D1_MINI
        //#define PROJECT GEN_ESP32_D1_R32

      #if (PROJECT == ESP32_TINYHOUSE)
          #define PROJ_TITLE "ESP32_TINYHOUSE"
          #define BOARD      MC_ESP32_Node     // platform=espressiv32, env=env:esp32dev, az-delivery-devkit-v4
        #endif
      #if (PROJECT == ESP32_TEST2)
          #define PROJ_TITLE "ESP32_TEST2"
          #define BOARD      MC_ESP32_Node     // platform=espressiv32, env=env:esp32dev, az-delivery-devkit-v4
        #endif
      #if (PROJECT == TOUCHTEST_1)
          #define PROJ_TITLE "Test Toucscreen"
          #define BOARD   MC_ESP32_AZTOUCH     // platform=espressiv32, env=env:esp32dev, az-delivery-devkit-v4
        #endif
      #if (PROJECT == MEASFREQ_1)
          #define PROJ_TITLE "Measure Frequency "
          #define BOARD   MC_ESP32_Node     // platform=espressiv32, env=env:esp32dev, az-delivery-devkit-v4
        #endif
      #if (PROJECT == LIGHTSHOW_1)
          #define PROJ_TITLE "Light Show"
          #define BOARD   MC_ESP32_Node     // platform=espressiv32, env=env:esp32dev, az-delivery-devkit-v4
        #endif
      #if (PROJECT == GEN_ESP32_NODE)
          #define PROJ_TITLE "Generic ESP32-Node"
          #define BOARD   MC_ESP32_Node     // platform=espressiv32, env=env:esp32dev, az-delivery-devkit-v4
        #endif
      #if (PROJECT == GEN_XIAO_ESP32C3)
          #define PROJ_TITLE "Generic XIAO ESP32C3"
          #define BOARD   XIAO_ESP32C3     // platform=espressiv32, env=env:esp32dev, az-delivery-devkit-v4
        #endif
      #if (PROJECT == GEN_ESP32_D1_MINI)
          #define PROJ_TITLE "Generic ESP32-D1-Mini"
          #define BOARD   MC_ESP32_Node     // platform=espressiv32, env=env:esp32dev, az-delivery-devkit-v4
        #endif
      #if (PROJECT == GEN_ESP32_D1_R32)
          #define PROJ_TITLE "Generic ESP32-D1-R32"
          #define BOARD   MC_ESP32_D1_R32     // platform=espressiv32, env=env:esp32dev, az-delivery-devkit-v4
        #endif
#endif // _PRJ_CONFIG_H_