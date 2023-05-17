#ifndef _PRJ_TOUCHTEST_1_H_
  #define _PRJ_TOUCHTEST_1_H_

  #include <Arduino.h>
  #include <md_defines.h>
  #include <project.h>

  // ******************************************
    // --- debugging
      #define DEBUG_MODE      CFG_DEBUG_STARTUP
        //#define DEBUG_MODE      CFG_DEBUG_NONE
        //#define DEBUG_MODE      CFG_DEBUG_ACTIONS
        //#define DEBUG_MODE      CFG_DEBUG_DETAILS

    // --- SW config
      #define USE_TASKING           ON
      #define USE_LED_BLINK_OUT     ON
    // --- system components and devices
      #define DEV_I2C1              ON  // ON
      #define DEV_I2C2              OFF
      #define DEV_VSPI              ON
      #define DEV_HSPI              OFF
      #define DEV_PWM_OUTPUT        ON
      #define DEV_ADC_INT           1
      #define DEV_ADC_ADS1115       1
      #define DEV_ADC_ADS1115_1     4
      #define DEV_ADC_ADS1115_2     OFF
    // --- user output components
      #define USE_WS2812_PWR_IN_SW  OFF                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         // some time matrix and line not allowed
      // --- displays
        #define USE_DISP            1
          // OLEDs
            #define USE_OLED_I2C    1
          // TFTs
            #define USE_DISP_TFT    OFF
              // MC_UO_TFT1602_GPIO_RO, MC_UO_TOUCHXPT2046_AZ_UNO, MC_UO_TXPT2046_AZ_SPI
              // MC_UO_TFT1602_I2C_XA,  MC_UO_Keypad_ANA0_RO
      // --- acustic output
        #define USE_AOUT              OFF
          // buzzer
            #define BUZZER1           OFF
            // AOUT_PAS_BUZZ_3V5V
      // --- PWM output
        #define USE_FAN_PWM           1   // 2
        #define USE_OUT_FREQ_PWM      OFF // 1
      // --- LED output
        #define USE_TRAFFIC_LED_OUT   OFF
        #define USE_RGBLED_PWM        1
        #define USE_WS2812_MATRIX_OUT OFF // [0, 1..4]
        #define USE_WS2812_LINE_OUT   OFF // [0, 1..4]
    // --- user input components
      // --- touchscreen ---
        #define USE_TOUCHSCREEN     OFF
        // MC_UI_TXPT2046_AZ_SPI, MC_UI_TXPT2046_AZ_UNO
      // --- keypads ---
        #define USE_KEYPADSHIELD    OFF
        // MC_UI_Keypad_ANA0_RO
      #define USE_GEN_SW_INP        1   // 1
      #define USE_GEN_CNT_INP       OFF // 1
      #define USE_GEN_PWM_INP       OFF // 2
    // --- sensors
      #define USE_ESPHALL           OFF
      #define USE_DS18B20_1W_IO     OFF // [0, 1, ....] limited by 1W connections
      #define USE_BME280_I2C        1   // [0, 1, ....] limited by I2C channels/addr
      #define USE_TYPE_K_SPI        OFF // [0, 1, ....] limited by Pins
      #define USE_MQ135_GAS_ANA     OFF // [0, 1, ....] limited by analog inputs
      #define USE_MQ3_ALK_ANA       1   // ADC or ADS1115
      #define USE_PHOTO_SENS_ANA    1   // ADC or ADS1115
      #define USE_ACS712_ANA        1   // only ADS1115
      #define USE_POTI_ANA          1   // ADC or ADS1115
      #define USE_VCC_ANA           1   // ADC or ADS1115
      #define USE_ADC1115_I2C       1   // ADC 4 channels 16Bit
    // --- network  components
      #define USE_WIFI              ON
      #define USE_NTP_SERVER        ON
      #define USE_LOCAL_IP          ON
      #define USE_WEBSERVER         ON
    // --- memory components
      #define USE_FLASH_MEM         ON
      #define USE_FRAM_I2C          1   // 1   // [0, 1, ...] limited by I2C channel/addr
      #define USE_SD_SPI            ON
    // --- test components
      #define USE_CTRL_POTI         OFF   // [0, 1, ....] limited by analog inputs
      #define USE_CTRL_SW_INP       OFF // 1   // [0, 1, ....] limited by digital pins
    // usage of peripherals
      #define USE_PWM_OUT           3 * USE_RGBLED_PWM + USE_FAN_PWM + USE_OUT_FREQ_PWM + USE_BUZZER_PWM // max 16
      #define USE_CNT_INP           USE_GEN_CNT_INP     // max 2 * 8 independent
      #define USE_PWM_INP           USE_GEN_PWM_INP
      // #define USE_ADC1              USE_KEYPADSHIELD_ADC + USE_MQ135_GAS_ADC + USE_CTRL_POTI_ADC + USE_PHOTO_SENS
      // #define USE_ADC2              OFF // not to use
      #define USE_DIG_INP           USE_GEN_SW_INP + USE_CTRL_SW_INP + USE_WS2812_PWR_IN_SW    //
      #define USE_DIG_OUT           USE_WS2812_LINE_OUT + USE_LED_BLINK_OUT //
      #define USE_DIG_IO            USE_DS18B20_1W_IO     //
      #define USED_IOPINS           USE_DIG_INP + USE_DIG_OUT + USE_DIG_IO + (2 * USE_I2C) + USED_SPI_PINS + USE_PWM_OUT + USE_CNT_INP + USE_ADC1

    // to be reorganised
        #if (USE_KEYPADSHIELD > OFF)
            #define USE_TFT1602_GPIO_RO_V5  // used by KEYPADSHIELD
            #define KEYS_Keypad_ANA0_RO_V5        // used by KEYPADSHIELD
            #define KEYS            ?
          #endif // USE_KEYPADSHIELD

      #define USE_TOUCHSCREEN     (3 * USE_TRAFFIC_LED_OUT) +USE_TOUCHSCREEN_SPI + USE_TOUCHSCREEN_OUT
        #if (USE_TOUCHSCREEN > OFF)
            #define TOUCHSCREEN1     TOUCHXPT2046_AZ_3V3
            #define TOUCHKEYS1       KEYS_TOUCHXPT2046_AZ_3V3
          #endif // USE_TOUCHSCREEN

#endif // _PRJ_TOUCHTEST_1_H_