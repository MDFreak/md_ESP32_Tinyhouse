#ifndef _PRJ_CONF_TOUCHTEST_1_H_
  #define _PRJ_CONF_TOUCHTEST_1_H_

  #include <Arduino.h>
  #include <md_defines.h>
  #include <prj_touchtest_1.h>

  // ******************************************
    // --- test features --------------------------------
      #define TEST_SOCKET_SERVER    OFF
      #define TEST_RGBLED_PWM       OFF
      #define USE_WEBCTRL_RGB       OFF
      #define USE_WEBCTRL_FAN       OFF
      #define USE_POTICTRL_RGB      OFF
      #define USE_POTICTRL_FAN      OFF
      #define USE_SWCTRL_RGB        OFF
      #define USE_SWCTRL_FAN        OFF
      #define USE_SWCTRL_1812       OFF
  // ******************************************
  // ******************************************
  // --- specification first to define device config
    // --- system
      // --- error status bits
        #define ERRBIT_TOUCH      0x00000001     // touchscreen
        #define ERRBIT_SERVER     0x00000002     // webserver
        #define ERRBIT_WIFI       0x00000004     // WIFI connection
        #define ERRBIT_NTPTIME    0x00000008     // NTP timeserver connection
      // --- generic
        #define SCAN_I2C          ON // 128
        #define TEST_NUM_CONVERT  ON
        #define CHECK_I2C_DEVICES
        //#define UTC_SEASONTIME UTC_WINTERTIME
        #define UTC_SEASONTIME UTC_SUMMERTIME
        #if (USE_TASKING > 0)
            #define USE_SONGTASK
            #define TASK_SLICE_T  5000ul   // task beat [us]
          #endif // USE_TASKING
        #if (USE_LED_BLINK_OUT > 0)
            #define BLINKTIME_MS  1200ul
            #define BLINKPWM_FREQ 400
            #define BLINKPWM_RES  8
          #endif

    // --- I2C interface
      // --- address configuration
        #if (USE_OLED_I2C > OFF)
            #define OLED1_I2C_ADDR  I2C_OLED_3C
            #define OLED1_I2C       I2C1
            #if (USE_OLED_I2C > 1)
                #define OLED2_I2C_ADDR  I2C_OLED_3C
                #define OLED2_I2C       I2C2
              #endif
          #endif

        #if (USE_FRAM_I2C > OFF)
            #define FRAM1_I2C_ADDR      I2C_FRAM_50
            #define FRAM1_SIZE          0x8000
            #define FRAM1_I2C           I2C1
            #if (USE_FRAM_I2C > 1 )
                #define FRAM2_I2C_ADDR  I2C_FRAM_50
                #define FRAM1_SIZE      0x8000
                #define FRAM2_I2C       I2C2
              #endif
          #endif
        #if (USE_BME280_I2C > OFF )
            #define BME280_I2C         I2C1
          #endif

        #if (USE_ADC1115_I2C > OFF)
            #ifndef USE_INPUT_CYCLE
                #define USE_INPUT_CYCLE
              #endif
            #define ADC1115_1_CHANS      4
            #define ADC1115_1_I2C        I2C1
            #define ADC1115_1_ADDR       I2C_ADS1115_48
            #if (USE_ADC1115_I2C > 1)
                #define ADC1115_2_CHANs  4
                #define ADC1115_1_I2C    I2C1
                #define ADC1115_1_ADDR   I2C_ADS1115_49
              #endif
          #endif

    // --- user interface
      // --- display output
        #define DISP_CYCLE_MS       1000ul   // Intervallzeit [us]
        // output status line
        #define STAT_DELTIME_MS     5000u    // default time to clear status
        #define STAT_NEWTIME_MS     1000u    // default time to clear status

    // --- user output
      // --- display
        #if (USE_DISP > 0)
            #define USE_STATUS
            #if (USE_OLED_I2C > OFF)
                          // select OLED - device & GEO   GEO_RAWMODE?
                          // MC_UO_OLED_066_AZ   GEO_64_48    OLED_DRV_1306
                          // MC_UO_OLED_091_AZ   GEO_128_32   OLED_DRV_1306
                          // MC_UO_OLED_096_AZ   GEO_128_64   OLED_DRV_1306
                          // MC_UO_OLED_130_AZ   GEO_128_64   OLED_DRV_1106
                #define OLED1_I2C_TYP MC_UO_OLED_130_AZ  // OLED1 on I2C1
                #define OLED1_STATUS     ON
                #define OLED1_I2C        I2C1
                #if !(OLED1_I2C_TYP ^ MC_UO_OLED_066_AZ)
                    #define OLED1_MAXCOLS  OLED_066_MAXCOLS
                    #define OLED1_MAXROWS  OLED_066_MAXROWS
                    #define OLED1_GEO      GEO_64_48
                    #define OLED1_DRV      OLED_DRV_1306
                  #endif
                #if !(OLED1_I2C_TYP ^ MC_UO_OLED_091_AZ)
                    #define OLED1_MAXCOLS  OLED_091_MAXCOLS
                    #define OLED1_MAXROWS  OLED_091_MAXROWS
                    #define OLED1_GEO      GEO_128_32
                    #define OLED1_DRV      OLED_DRV_1306
                  #endif
                #if !(OLED1_I2C_TYP ^ MC_UO_OLED_096_AZ)
                    #define OLED1_MAXCOLS  OLED_096_MAXCOLS
                    #define OLED1_MAXROWS  OLED_096_MAXROWS
                    #define OLED1_GEO      GEO_128_64
                    #define OLED1_DRV      OLED_DRV_1306
                  #endif
                #if !(OLED1_I2C_TYP ^ MC_UO_OLED_130_AZ)
                    #define OLED1_MAXCOLS         OLED_130_MAXCOLS
                    #define OLED1_MAXROWS         OLED_130_MAXROWS
                    #define OLED1_GEO             GEO_128_64
                    #define OLED1_DRV             OLED_DRV_1106
                  #endif

                #if ( USE_OLED_I2C > 1 )
                    #define OLED2_I2C_TYP         MC_UO_OLED_130_AZ  // OLED1 on I2C1
                    #define USE_STATUS2           ON
                    #define OLED1_I2C        I2C1
                    #if !(OLED2_I2C_TYP ^ MC_UO_OLED_066_AZ)
                        #define OLED2_MAXCOLS OLED_066_MAXCOLS
                        #define OLED2_MAXROWS OLED_066_MAXROWS
                        #define OLED2_GEO     GEO_64_48
                        #define OLED2_DRV     OLED_DRV_1306
                      #endif
                    #if !(OLED2_I2C_TYP ^ MC_UO_OLED_091_AZ)
                        #define OLED2_MAXCOLS OLED_091_MAXCOLS
                        #define OLED2_MAXROWS OLED_091_MAXROWS
                        #define OLED2_GEO     GEO_128_32
                        #define OLED2_DRV     OLED_DRV_1306
                      #endif
                    #if !(OLED2_I2C_TYP ^ MC_UO_OLED_096_AZ)
                        #define OLED2_MAXCOLS  OLED_096_MAXCOLS
                        #define OLED2_MAXROWS  OLED_096_MAXROWS
                        #define OLED2_GEO      GEO_128_64
                        #define OLED2_DRV      OLED_DRV_1306
                      #endif
                    #if !(OLED2_I2C_TYP ^ MC_UO_OLED_130_AZ)
                        #define OLED2_MAXCOLS  OLED_130_MAXCOLS
                        #define OLED2_MAXROWS  OLED_130_MAXROWS
                        #define OLED2_GEO      GEO_128_64
                        #define OLED2_DRV      OLED_DRV_1106
                      #endif
                  #else
                    #define OLED2_I2C       OFF  //
                  #endif

              #endif
            #if ( USE_TOUCHSCREEN > 0 )
                //
              #endif // USE_TOUCHSCREEN

            #if ( USE_TFT > 0 )
                #if !(DISP_TFT ^ MC_UO_TOUCHXPT2046_AZ)
                    #define TOUCH_ROTATION    3      // 0:USB oben, 1:USB links, 2:USB unten, 3:USB rechts
                    #define DATE_DISP_COL  0
                    #define DATE_DISP_LINE 9      // line on display for date / time
                    // text display area
                    #define DISP_X         0
                    #define DISP_Y         0
                    #define DISP_W         240
                    #define DISP_H         300
                    #define DISP_BCOL      0x000F // TFT_NAVY
                    #define DISP_ANZ_SP    20
                    #define DISP_ANZ_ZE    12
                    #define DISP_Hoe_ZE    25
                    #define DISP_TX_FCOL   0xB7E0 // TFT_GREENYELLOW
                    #define DISP_TX_BCOL   DISP_BCOL
                    // status line for messages
                    #define USE_STAT_TOUCH 1
                    #define STATUS_XCENT   120 // Centred on this
                    #define STATUS_YCENT   315
                    #define STATUS_XLI     0
                    #define STATUS_XRE     239
                    #define STATUS_YOB     DISP_H // 300
                    #define STATUS_YUN     319
                    #define STATUS_FCOL    0xF81F // TFT_MAGENTA
                    #define STATUS_BCOL    0x0000 // TFT_BLACK
                  #endif
                //
                #if !(DISP_TFT ^ MC_UO_TFT1602_GPIO_RO)
                    #define DATE_DISP_COL   0
                    #define DATE_DISP_LINE  0    // line on display for date / time
                    #define LCD_ROWS        2
                    #define LCD_CULS        2
                  #endif
              #endif
          #endif // DISP

      // --- acoustic output
        #if (USE_AOUT > OFF)
            #if !(BUZZER1 ^ AOUT_PAS_BUZZ_3V5V)
                #define PLAY_MUSIC
                #define MUSIC_BASE_OCTA 5        // base oktave for musik
                //#define PLAY_START_MUSIC
                #define PLAY_START_DINGDONG
              #endif
          #endif // USE_BUZZER_PWM

      // WS2812 LEDs
        #if (USE_WS2812_MATRIX_OUT > OFF)
            #ifndef USE_OUTPUT_CYCLE
                #define USE_OUTPUT_CYCLE
              #endif
            #define COLCHAR_2812   6
            #define COLBMP_2812    8
            #define ROWBMP_2812    8
            #define UPD_2812_M1_MS 70
            //#define COL24_2812_M1  0xFF0000u   // 0xB924u   // color r-g-b (5-6-5)
            //#define COL24_2812_BM1 0x00FF00u   // color r-g-b (5-6-5)
            #define COL24_2812_M1 0xFF0080u   // color r-g-b (5-6-5) = 255,0,128
            #define COL24_2812_BM1  0xF0B63Cu   // color r-g-b (5-6-5) = 240, 182, 56
            #define BRI_2812_M1    255
            #define BRI_2812_BM1   255

            #define ROW1_2812_M1   NEO_MATRIX_TOP
            #define COL1_2812_M1   NEO_MATRIX_LEFT
            #define DIR_2812_M1    NEO_MATRIX_COLUMNS
            #define ORI_2812_M1    NEO_MATRIX_ZIGZAG

            #define TYPE_2812_M1   WS2812B
            #define COLORD_2812_M1 NEO_GRB
            #define ROWPIX_2812_M1 8
            #define LEDS_2812_M1   COLPIX_2812_M1 * ROWPIX_2812_M1

            #define ROW1_2812_T1   NEO_MATRIX_TOP
            #define COL1_2812_T1   NEO_MATRIX_LEFT
            #define DIR_2812_T1    NEO_MATRIX_COLUMNS
            #define ORI_2812_T1    NEO_MATRIX_ZIGZAG

            #define COLPIX_2812_T1 8
            #define ROWPIX_2812_T1 8
            #define COLTIL_2812_M1 16  // needs correct value
            #define ROWTIL_2812_M1 1   // 0 = OFF
            #define ANZ_TILES_M1   COLTIL_2812_M1 * ROWPIX_2812_T1
            #define COLPIX_2812_M1 COLTIL_2812_M1 * COLPIX_2812_T1
            #define OFFBEG_2812_M1 1 //+ COLPIX_2812_T1
            #define OFFEND_2812_M1 0 //+ COLPIX_2812_T1
            #if (USE_WS2812_MATRIX_OUT > 1)
                #ifndef USE_OUTPUT_CYCLE
                    #define USE_OUTPUT_CYCLE
                  #endif
                #define UPD_2812_M2_MS 8
                #define LEDS_2812_M2   512
                #define BRIGHT_2812_M2 5
                #define TYPE_2812_M2   WS2812B
                #define COLORD_2812_M2 NEO_GRB
                #define COLPIX_2812_M2 128
                #define ROWPIX_2812_M2 8
                #define COLPIX_2812_T2 8
                #define ROWPIX_2812_T2 8
                #define COLTIL_2812_M2 4
                #define ROWTIL_2812_M2 1
              #endif
          #endif

        #if (USE_WS2812_LINE_OUT > OFF)
            #ifndef USE_OUTPUT_CYCLE
                #define USE_OUTPUT_CYCLE
              #endif
            #define UPD_2812_L1_MS 10
            #define COL24_2812_L1  0x6300F1u   // color r-g-b (5-6-5)
            #define BRI_2812_L1    5u
            #define TYPE_2812_L1   WS2812B
            #define COLORD_2812_L1 NEO_GRB
            #define COLPIX_2812_L1 30
            #define ROWPIX_2812_L1 1
            #define COLTIL_2812_L1 4
            #define ROWTIL_2812_L1 1
              //#define COLPIX_2812_T1 8
              //#define ROWPIX_2812_T1 8
                  //#define UPD_2812_L1_MS 1
                  //#define LEDS_2812_L1   300
                  //#define BRIGHT_2812_L1 5
                  //#define TYPE_2812_L1   WS2812B
                  //#define COLORD_2812_L1 NEO_GRB
            #if (USE_WS2812_LINE_OUT > 1)
                #define UPD_2812_L2_MS 10
                #define LEDS_2812_L2   30
                #define BRIGHT_2812_L2 12
                #define TYPE_2812_L2   WS2812B
                #define COLORD_2812_L2 GRB
                #if (USE_WS2812_LINE_OUT > 2)
                    #define LEDS_2812_L3   30
                    #define BRIGHT_2812_L3 12
                    #define TYPE_2812_L3   WS2812
                    #define COLORD_2812_L3 GRB
                    #if (USE_WS2812_LINE_OUT > 3)
                        #define LEDS_2812_L4   30
                        #define BRIGHT_2812_L4 12
                        #define TYPE_2812_L4   WS2812
                        #define COLORD_2812_L4 GRB
                      #endif
                  #endif
              #endif
          #endif

      // --- PWM
        #if (USE_RGBLED_PWM > OFF)
            #ifndef USE_OUTPUT_CYCLE
                #define USE_OUTPUT_CYCLE
              #endif
            #define PWM_LEDS_FREQ  4000u
            #define PWM_LEDS_RES   8
            #define BRI_RGBLED_1   15
            #define COL24_RGBLED_1 0xBE2727u   // bright 10 + red 10 + green 10 + blue 10
          #endif
        #if (USE_FAN_PWM > OFF)
            #ifndef USE_OUTPUT_CYCLE
                #define USE_OUTPUT_CYCLE
              #endif
            #define PWM_FAN_FREQ      4500u
            #define PWM_FAN_RES       8
          #endif

        #ifdef USE_OUTPUT_CYCLE
            #define OUTPUT_CYCLE_MS  500u
          #endif

    // --- user input
      // --- keypads
        #if defined(KEYS)
            #ifndef USE_INPUT_CYCLE
                #define USE_INPUT_CYCLE
              #endif
            #if !(KEYS ^ MC_UI_Keypad_ANA0_RO)
                #define USE_KEYPADSHIELD
                #define KEYS_ADC        34   // ADC Pin GPIO02
                #define ADC_STD_RES     12   // default resolution 12 Bit
                #define ADC_RES         12   // active resolution
                #define KP_NUM_KEYS     5
                #define KP_KEY_VAL_1    200  // max ADC value of button 0
                #define KP_KEY_VAL_2    750
                #define KP_KEY_VAL_3    1470
                #define KP_KEY_VAL_4    2330
                #define KP_KEY_VAL_5    3200
              #endif // keypad

            #if !(KEYS ^ MC_UI_TOUCHXPT2046_AZ)
                // Keypad start position, key sizes and spacing
                #define KEY_X          40 // Centre of key
                #define KEY_Y          287
                #define KEY_W          62 // Width and height
                #define KEY_H          26
                #define KEY_SPACING_X  18 // X and Y gap
                #define KEY_SPACING_Y  20
                #define KEY_TEXTSIZE   1   // Font size multiplier
                #define KEY_NUM_LEN    3 // Anzahl der Tasten
              #endif // touchpad

          #endif

      // --- counter
        #if (USE_CNT_INP > OFF)
            #define PCNT_H_LIM_VAL      0   // not used
            #define PCNT_L_LIM_VAL      0   // not used
            #define PNCT_AUTO_SWDN      5000000ul  // > 5 sec period
            #define PNCT_AUTO_SWUP      50000ul    // < 50 msec period
            // counter 1
            #define PCNT1_INP_FILT      10  // glitch filter (clock 80 MHz)
            #define PCNT1_UFLOW         3000000ul  // timedelay due to 0 Hz [us]
            #define PCNT1_INP_SIG_IO    PIN_CNT_FAN_1       // Pulse Input GPIO
            #define PCNT1_INP_CTRL_IO   PIN_CNT_FAN_1       // Control GPIO HIGH=count up, LOW=count down
            //#define PCNT1_THRESH1_VAL   5
            #define PCNT1_THRESH0_VAL   3
            #define PCNT1_EVT_0         PCNT_EVT_THRES_0
            //#define PCNT1_EVT_1         PCNT_EVT_THRES_1
            #define PCNT1_UNIDX          0
            #define PCNT1_CHAN          0
            #define PCNT1_ID            0
            #if (USE_CNT_INP > 1)
                // counter 2
                #define PCNT2_INP_FILT      10  // glitch filter (clock 80 MHz)
                #define PCNT2_UFLOW         3000000ul  // timedelay due to 0 Hz [us]
                #define PCNT2_INP_SIG_IO    PIN_CNT_FAN_2   // Pulse Input GPIO
                #define PCNT2_INP_CTRL_IO   PIN_CNT_FAN_2       // Control GPIO HIGH=count up, LOW=count down
                //#define PCNT2_THRESH1_VAL   2
                #define PCNT2_THRESH0_VAL   400
                #define PCNT2_EVT_0         PCNT_EVT_THRES_0
                #define PCNT2_UNIDX          1
                #define PCNT2_CHAN          0
                #define PCNT2_ID            1
              #endif
            #ifndef USE_INPUT_CYCLE
                #define USE_INPUT_CYCLE
              #endif
          #endif
      // --- dutycycle (pwm) input
        #if (USE_PWM_INP > OFF)
            //
          #endif
      // --- internal adc input
        #if (USE_ADC1 > OFF)
            #ifndef USE_INPUT_CYCLE
                #define USE_INPUT_CYCLE
              #endif
          #endif
      // --- internal digital input
        #if (USE_DIG_INP > OFF)
            #ifndef USE_INPUT_CYCLE
                #define USE_INPUT_CYCLE
              #endif
            #if (USE_CTRL_SW_INP > OFF)
                #define INP_SW_CTRL  0
                #define POL_SW_CTRL  HIGH // LOW, HIGH
                #define MOD_SW_CTRL  INPUT_PULLUP // INPUT, INPUT_PULLDOWN
              #endif
            #if (USE_GEN_SW_INP > OFF)
                #define INP_REED_1   0
                #define POL_REED_1   LOW // LOW, HIGH
                #define MOD_REED_1   INPUT_PULLUP // INPUT, INPUT_PULLDOWN
              #endif
          #endif

    // --- memories
      // --- FRAM
        #if (USE_FRAM_I2C > OFF)
            #define SIZE_FRAM     0x8000
          #endif
    // --- sensors
      #if (USE_DS18B20_1W_IO > OFF)
          #define DS_T_PRECISION   9
          #define DS18B20_ANZ      1
          #ifndef USE_INPUT_CYCLE
              #define USE_INPUT_CYCLE
            #endif
        #endif

      #if (USE_BME280_I2C > OFF)
          #define BME280_I2C      I2C1
          #define BME280T_FILT    0
          #define BME280T_Drop    0
          #define BME280P_FILT    0
          #define BME280P_Drop    0
          #define BME280H_FILT    0
          #define BME280H_Drop    0
          #ifndef USE_INPUT_CYCLE
              #define USE_INPUT_CYCLE
            #endif
        #endif

      /* analog channels
        ADC channels
          ***_ADC_RES 12                    --> resolution 12 bit (def)
          ***_ADC_ATT  ADC_ATTEN_DB_0   -->  range 0 -  800 mV
          ***_ADC_ATT  ADC_ATTEN_DB_2_5 -->  range 0 - 1100 mV
          ***_ADC_ATT  ADC_ATTEN_DB_6   -->  range 0 - 1350 mV
          ***_ADC_ATT  ADC_ATTEN_DB_11  -->  range 0 - 2600 mV (def)

        ADS1115 channels
          ***_1115_ATT GAIN_TWOTHIRDS --> range +/-6144mV - 187.5    uV/bit
          ***_1115_ATT GAIN_ONE       --> range +/-4096mV - 125      uV/bit
          ***_1115_ATT GAIN_TWO (def) --> range +/-2048mV -  62.5    uV/bit
          ***_1115_ATT GAIN_FOUR      --> range +/-1024mV -  31.25   uV/bit
          ***_1115_ATT GAIN_EIGHT     --> range +/- 512mV -  15.625  uV/bit
          ***_1115_ATT GAIN_SIXTEEN   --> range +/- 256mV -   7.8125 uV/bit

        scaling parameters and calculation
        *pValue = (  (*pValue + (double) *_SCAL_OFFRAW)
                   * (double)*_SCAL_GAIN
                   + (double) *_SCAL_OFFRREAL
                  )
        */
      #if (USE_MQ135_GAS_ANA > OFF)
          #define MQ135_GAS_ADC    ON
          #define MQ135_GAS_1115   OFF
          #define MQ135_FILT       15       // floating  measure filtering
          #define MQ135_EM_WIN     100      // window for traffic light
          #define MQ135_SCAL_MIN   0
          #define MQ135_SCAL_MAX   100
            //#define MQ135_ThresFilt 25       // threshold measure filtering
            //#define MQ135_EM_MID    2350    // green < (MID-(WIN/2) < yellow < (MID+(WIN/2) < red
          #ifndef USE_INPUT_CYCLE
              #define USE_INPUT_CYCLE
            #endif
        #endif
      #if (USE_MQ3_ALK_ANA > OFF)
          #define MQ3_FILT              11       // floating  measure filtering
          #define MQ3_DROP              2
          #define MQ3_EM_WIN            100      // window for traffic light
          #define MQ3_SCAL              OFF
          #define MQ3_ALK_ADC           OFF
          #define MQ3_ALK_1115          ON
          #if (MQ3_ALK_1115 > OFF)
              #define MQ3_1115_UNIDX      0
              #define MQ3_1115_CHAN     3
              #define MQ3_1115_ATT      GAIN_TWOTHIRDS
              #define MQ3_OFFRAW        0
              #define MQ3_GAIN          0
              #define MQ3_OFFREAL       0
            #endif
          #ifndef USE_INPUT_CYCLE
              #define USE_INPUT_CYCLE
            #endif
        #endif

      #if (USE_TYPE_K_SPI > OFF)
          #define TYPEK_FILT      11       // floating  measure filtering
          #define TYPEK_DROP_PEEK 2        // drop biggest / lowest
          #define TYPEK1_OFFSET   0.       // offset unit °C
          #define TYPEK1_GAIN     1.       // result = (measure * gain) + offset
          #define TYPEK2_OFFSET   0.       // offset unit °C
          #define TYPEK2_GAIN     1.       // result = (measure * gain) + offset
          #ifndef USE_INPUT_CYCLE
              #define USE_INPUT_CYCLE
            #endif
        #endif

      #if (USE_PHOTO_SENS_ANA > OFF)
          #define PHOTO1_FILT               7
          #define PHOTO1_DROP               0
          #define PHOTO1_ADC                ON
            #if (PHOTO1_ADC > OFF)
                #define PHOTO1_ADC_ATT      ADC_ATTEN_DB_11
                #define PHOTO1_SCAL_OFFRAW  0
                #define PHOTO1_SCAL_GAIN    1
                #define PHOTO1_SCAL_OFFREAL 0
              #endif
          #define PHOTO1_1115               OFF
            #if (PHOTO1_1115 > OFF)
                #define PHOTO1_1115_UNIDX     0
                #define PHOTO1_1115_CHAN    0
                #define PHOTO1_1115_ATT     GAIN_ONE
                #define PHOTO1_SCAL_OFFRAW  0
                #define PHOTO1_SCAL_GAIN    1
                #define PHOTO1_SCAL_OFFREAL 0
              #endif
          #if (USE_PHOTO_SENS_ANA > 1)
              #define PHOTO2_FILT          7
              #define PHOTO2_DROP          0
              #define PHOTO2_SCAL          OFF
              #define PHOTO2_SCAL_MIN      0
              #define PHOTO2_SCAL_MAX      100
              #define PHOTO2_ADC           ON
                #if (PHOTO2_ADC > OFF)
                    #define PHOTO2_ADC_ATT   ADC_ATTEN_DB_11
                  #endif
              #define PHOTO2_1115          OFF
                #if (PHOTO2_1115 > OFF)
                    #define PHOTO2_1115_ATT  GAIN_ONE
                    #define PHOTO2_1115_UNIDX  0
                    #define PHOTO2_1115_CHAN 0
                  #endif
            #endif
          #ifndef USE_INPUT_CYCLE
              #define USE_INPUT_CYCLE
            #endif
        #endif

      #if (USE_VCC_ANA > OFF)
          #define VCC_FILT              5
          #define VCC_DROP              1
          #define VCC_ADC               OFF
            #if (VCC_ADC > OFF)
                #define VCC_ADC_ATT       ADC_ATTEN_DB_11
                #define VCC_SCAL_OFFRAW   0
                #define VCC_SCAL_GAIN     1
                #define VCC_SCAL_OFFREAL  0
              #endif
          #define VCC_1115              ON
            #if (VCC_1115 > OFF)
                #define VCC_1115_UNIDX      0
                #define VCC_1115_CHAN     2
                #define VCC_1115_ATT      GAIN_TWOTHIRDS
                #define VCC_OFFRAW        0
                #define VCC_GAIN          1
                #define VCC_OFFREAL       0
              #endif
          #ifndef USE_INPUT_CYCLE
              #define USE_INPUT_CYCLE
            #endif
        #endif
      #if (USE_POTI_ANA > OFF)
          #define POTI1_FILT              9
          #define POTI1_DROP              1
          #define POTI1_ADC               OFF
            #if (POTI1_ADC > OFF)
                #define POTI1_ADC_ATT     ADC_ATTEN_DB_11
                #define VCC_SCAL_OFFRAW   0
                #define VCC_SCAL_GAIN     1
                #define VCC_SCAL_OFFREAL  0
              #endif
          #define POTI1_1115              ON
            #if (POTI1_1115 > OFF)
                #define POTI1_1115_UNIDX    0
                #define POTI1_1115_CHAN   0
                #define POTI1_1115_ATT    GAIN_TWO
                #define POTI1_OFFRAW      0
                #define POTI1_GAIN        1
                #define POTI1_OFFREAL     0
              #endif
          #if (USE_POTI_ANA > 1)
              #define POTI2_FILT            7
              #define POTI2_DROP            0
              #define POTI2_SCAL            OFF
              #define POTI2_SCAL_MIN        0
              #define POTI2_SCAL_MAX        100
              #define POTI2_ADC             ON
              #if (POTI2_ADC > OFF)
                  #define POTI2_ADC_ATT   ADC_ATTEN_DB_11
                #endif
              #define POTI2_1115            OFF
                #if (POTI2_1115 > OFF)
                    #define POTI2_1115_UNIDX  0
                    #define POTI2_1115_CHAN 0
                    #define POTI2_1115_ATT  GAIN_TWOTHIRDS
                  #endif
            #endif
          #ifndef USE_INPUT_CYCLE
              #define USE_INPUT_CYCLE
            #endif
        #endif
      #if (USE_ACS712_ANA > OFF)
          /*  ACS712 hall effect current sensor +/- 5A/20A/30 A
              output: VCC/2 (2,5V) + measured value
              sensitivity: type  5A -> 186mV/A ->  1570 - 3430 mV
              sensitivity: type 20A -> 100mV/A ->   500 - 4500 mV
              sensitivity: type 30A ->  66mV/A ->   520 - 4480 mV
            */
          #define I712_1_FILT             9
          #define I712_1_DROP             2
          #define I712_1_IMAX             5000 // mA
          #define I712_1_ADC              OFF // not recommended, low resolution
            #if (I712_1_ADC > OFF)
                #define I712_1_ADC_ATT   ADC_ATTEN_DB_11
              #endif
          #define I712_1_1115             ON
          #if (I712_1_1115 > OFF)
              #define I712_1_1115_UNIDX     0
              #define I712_1_1115_CHAN    2
              #if   (I712_1_IMAX ==  5000)
                  #define I712_1_1115_ATT       GAIN_ONE
                  #define I712_1_SCAL_OFFRAW    0
                  #define I712_1_SCAL_GAIN      1
                  #define I712_1_SCAL_OFFREAL   0
                #endif
              #if (I712_1_IMAX == 20000)
                  #define I712_1_1115_ATT      GAIN_TWOTHIRDS
                  #define I712_1_SCAL_OFFRAW    0
                  #define I712_1_SCAL_GAIN      1
                  #define I712_1_SCAL_OFFRAW    0
                #endif
              #if (I712_1_IMAX == 30000)
                  #define I712_1_1115_ATT      GAIN_TWOTHIRDS
                  #define I712_1_SCAL_OFFRAW    0
                  #define I712_1_SCAL_GAIN      1
                  #define I712_1_SCAL_OFFRAW    0
                #endif
            #endif
        #endif
      //#define ANZ_ANASENS  USE_DS18B20_1W_IO + USE_BME280_I2C * 3 + USE_MQ135_GAS_ADC + USE_TYPE_K_SPI
      #ifdef USE_INPUT_CYCLE
          #define INPUT_CYCLE_MS  500u
        #endif

    // --- network
      // --- WIFI
        #if (USE_WIFI > OFF)
            #define WIFI_ANZ_LOGIN  7
            #define WIFI_IS_DUTY    ON
            #define WIFI_SSID0      "MAMD-HomeG" // WLAN Moosgrabenstrasse 26
            #define WIFI_SSID0_PW   "ElaNanniRalf3"
            #define WIFI_SSID1      "HS-HomeG" // WLAN Am Jungberg 9
            #define WIFI_SSID1_PW   "ElaNanniRalf3"
            #define WIFI_SSID2      "WL-Fairnetz" //Weltladen
            #define WIFI_SSID2_PW   "WL&Fair2Live#"
            #define WIFI_SSID3      "machquadrat" //machQuadrat
            #define WIFI_SSID3_PW   "IamSecure"
            #define WIFI_SSID4      "MD_KingKong" //Hotspot Martin
            #define WIFI_SSID4_PW   "ElaNanniRalf3"
            #define WIFI_SSID5      "CDWiFi"        //OEBB Raijet
            #define WIFI_SSID5_PW   ""
            #define WIFI_SSID6      "xWIFI@DB"        //DB ICE
            #define WIFI_SSID6_PW   ""
            #define WIFI_SSID7      ""        //?
            #define WIFI_SSID7_PW   ""
            #define WIFI_CONN_DELAY 500000ul // Scan-Abstand [us]
            #define WIFI_CONN_REP   5        // Anzahle der Connect-Schleifen
            #define WIFI_CONN_CYCLE 4000ul   // Intervallzeit fuer Recoonect [us]
            #define NTPSERVER_CYCLE 1000ul   // Intervallzeit [us]

            #define WIFI_ANZ_LOCIP  WIFI_ANZ_LOGIN
            #if !(BOARD ^ MC_ESP32_Node)
                #define WIFI_FIXIP0     0x1800000Aul // 10.0.0.24   lowest first
            #elif !(BOARD ^ MC_ESP32_D1_MINI)
                  #define WIFI_FIXIP0   0x1800000Aul // 10.0.0.24
            #elif !(BOARD ^ MC_ESP32_D1_R32)
                #define WIFI_FIXIP0     0x1800000Aul // 10.0.0.24   lowest first
              #endif
            #define WIFI_GATEWAY0   0x8B00000Aul // 10.0.0.139 // Moosgraben
            #define WIFI_FIXIP1     0x1800000Aul // 10.0.0.24
            #ifdef USE_LOCAL_IP
                #define WIFI_GATEWAY1   0x8B00000Aul // 10.0.0.139      // Jungberg
                #define WIFI_GATEWAY2   0x8a00000Aul // 10.0.0.138      // Weltladen
                #define WIFI_FIXIP2     0x1600000Aul // 10.0.0.22
                #define WIFI_GATEWAY3   0x0100000Aul // 10.0.0.1        // machquadrat
                #define WIFI_FIXIP3     0x1600000Aul // 10.0.0.22
                #define WIFI_GATEWAY4   0x012BA8C0ul // 192.168.43.154  // hotspot KingKong
                #define WIFI_FIXIP4     0x162BA8C0ul // 192.168.43.22
                #define WIFI_GATEWAY5   0x0926A8C0ul // 192.168.32.1    // OEBB Railjet
                #define WIFI_FIXIP5     0x1620A8C0ul // 192.168.32.22
                #define WIFI_GATEWAY6   0x01AE12ACul // 172.18.0.1    // DB ICE
                #define WIFI_FIXIP6     0x16AE12ACul // 172.18.174.22
                #define WIFI_GATEWAY7   0x0100000Aul // 10.0.0.10       // ?
                #define WIFI_FIXIP7     0x1600000Aul // 10.0.0.22  // ?
                #define WIFI_GATEWAY8   0x0100000Aul // 10.0.0.1        // ?
                #define WIFI_FIXIP8     0x1600000Aul // 10.0.0.22  // ?
                #define WIFI_SUBNET     0x0000FFFFul // 255.255.255.0
              #endif
          #endif

      // --- webserver
        #if (USE_WEBSERVER > OFF)
            #define WEBSERVER_CYCLE 1000ul  // Intervallzeit [us]
          #endif

  // ----------------------------------------------------------------
  // --- board management
  // ----------------------------------------------------------------
    #if !(BOARD ^ MC_ESP32_Node)
      // --- system
        #define SER_BAUDRATE ESP_SER_BAUD
        // --- network
        // --- user output
          // --- LEDs
          // --- display
          // --- acustic output
        // --- user input
        // --- sensors
        // --- memories
      // --- pins, connections
        // --- system
          #if (USE_LED_BLINK_OUT > OFF)
              #define PIN_BOARD_LED 2
            #endif
          // --- I2C board connection
            #define PIN_I2C1_SDA      21
            #define PIN_I2C1_SCL      22
            #if ( USE_I2C > 1 )
                #define PIN_I2C2_SDA  25
                #define PIN_I2C2_SCL  26
              #endif
          // --- SPI board connection
            #define PIN_SPI_MOSI      23
            #define PIN_SPI_MISO      19
            #define PIN_SPI_SCL       18

        // --- display
          #if (USE_OLED_I2C > OFF)
              #define OLED1_I2C_SCL   PIN_I2C1_SCL
              #define OLED1_I2C_SDA   PIN_I2C1_SDA
              #if (USE_OLED_I2C > 1)
                  #define OLED2_I2C_SCL   PIN_I2C1_SCL
                  #define OLED2_I2C_SDA   PIN_I2C1_SDA
                #endif
            #endif

        // --- user input
          #if (USE_CTRL_POTI > OFF)
              #define PIN_INP_POTI_1 35   // ADC 1-5
              #define ADC_INP_POTI_1 NU   // ADC 1-5
            #endif

          #if (USE_CTRL_SW_INP > OFF)
              #define PIN_INP_SW_1  32   // INPUT_PULLUP
            #endif

          #if (USE_DIG_INP > OFF)
              #if (USE_GEN_SW_INP > OFF)
                  #define PIN_INP_REED_1 15
                #endif
            #endif
          #if (USE_GEN_CNT_INP > OFF)
              #define PIN_CNT_GEN_S0 4
              #define PIN_CNT_GEN_C0 PCNT_PIN_NOT_USED
              #define PIN_CNT_GEN_S1 17
              #define PIN_CNT_GEN_C1 PCNT_PIN_NOT_USED
            #endif
          #if (USE_PWM_INP > OFF)
                #define PIN_PWM_INP_1 PIN_CNT_GEN_2
            #endif
        // --- counter
          #if (USE_CNT_INP > OFF)
              #define PCNT1_INP_SIG_IO    PIN_CNT_FAN_1       // Pulse Input GPIO
              #define PCNT1_INP_CTRL_IO   PIN_CNT_FAN_1       // Control GPIO HIGH=count up, LOW=count down
              #if (USE_CNT_INP > 1)
                  // counter 2
                  #define PCNT2_INP_SIG_IO    PIN_CNT_FAN_2   // Pulse Input GPIO
                  #define PCNT2_INP_CTRL_IO   PIN_CNT_FAN_2       // Control GPIO HIGH=count up, LOW=count down
                #endif
              #ifndef USE_INPUT_CYCLE
                  #define USE_INPUT_CYCLE
                #endif
            #endif
        // --- user output
          #if (USE_TRAFFIC_LED_OUT > OFF)
              #define PIN_TL_RED    26   // RGB red
              #define PIN_TL_YELLOW 25   // RGB green
              #define PIN_TL_GREEN  33   // RGB blue
            #endif

          #if (USE_RGBLED_PWM > OFF)
              #define PIN_RGB_RED   33 //26   // RGB red
              #define PIN_RGB_GREEN 26   // RGB blue
              #define PIN_RGB_BLUE  14 //33   // RGB blue
            #endif

          #if (USE_FAN_PWM > OFF)
              #define PIN_PWM_FAN_1 0
              #define PIN_PWM_FAN_2 4
            #endif

          #if (USE_OUT_FREQ_PWM > OFF)
              #define PIN_FREQ_1    26
            #endif

          #if (USE_WS2812_MATRIX_OUT > OFF)
              #define PIN_WS2812_M1      16
              #if (USE_WS2812_PWR_IN_SW > OFF)
                  #define PIN_WS2812_PWR_IN_SW 36
                #endif
              #if (USE_WS2812_MATRIX_OUT > 1)
                  #define PIN_WS2812_M2  17
                #endif
                //#define PIN_WS2812_M3  x
                //#define PIN_WS2812_M4  x
            #endif

          #if (USE_WS2812_LINE_OUT > OFF)
              #if (USE_WS2812_PWR_IN_SW > OFF)
                  #define PIN_WS2812_PWR_IN_SW 36
                #endif
              #define PIN_WS2812_L1      17
                //#define PIN_WS2812_L2      17
                //#define PIN_WS2812_L3  x
                //#define PIN_WS2812_L4  x
            #endif

          #if (USE_TFT > OFF)
              #if !(DISP_TFT ^ MC_UO_TFT1602_GPIO_RO)
                  #define LCD_BL      5    // D10/SS  ARDUINO
                  #define LCD_EN      13   // D9
                  #define LCD_RS      12   // D8
                  #define LCD_D7      14   // D7
                  #define LCD_D6      27   // D6
                  #define LCD_D5      16   // D5
                  #define LCD_D4      17   // D4
                #endif
              #if !(DISP_TFT ^ MC_UO_TOUCHXPT2046_AZ)
                  #define TFT_CS      5
                  #define TFT_DC      4
                  #define TFT_RST     22
                  #define TFT_LED     15
                  #define TOUCH_CS    14
                  #define TOUCH_IRQ   27
                  #define LED_ON      0    // activ low
                #endif
              #if !(DISP_TFT ^ MC_UO_TFT1602_I2C_XA)
                  #define ME
                #endif
            #endif

          #if (USE_BUZZER_PWM > OFF)
              #define PIN_BUZZ      21
            #endif
        // --- sensors
          #if (USE_DS18B20_1W_IO > OFF)
              #define DS_ONEWIRE_PIN 27
            #endif
          #if (USE_TYPE_K_SPI > OFF)
              #define TYPEK_DATA_PIN PIN_SPI_MISO
              #define TYPEK_CLK_PIN  PIN_SPI_CLK
              #define TYPEK1_CS_PIN  16
              #define TYPEK2_CS_PIN  17
            #endif
          #if (USE_MQ135_GAS_ANA > OFF)
              #define PIN_MQ135     35
              #define ADC_MQ135     5   // ADC 1-3
            #endif
          #if (USE_MQ3_ALK_ANA > OFF)
              #define PIN_MQ135     35
              #define ADC_MQ135     5   // ADC 1-3
            #endif

          #if (USE_PHOTO_SENS_ANA > OFF)
              #define PIN_PHOTO1_SENS     39
              #define ADC_PHOTO1_SENS     3
            #endif
        // --- memory
          #if (USE_FRAM_I2C > OFF)
              #define FRAM1_I2C_SCL       PIN_I2C1_SCL
              #define FRAM1_I2C_SDA       PIN_I2C1_SDA
              #if (USE_FRAM_I2C > 1 )
                  #define FRAM2_I2C_SCL   I2C1_SCL
                  #define FRAM2_I2C_SDA   I2C1_SDA
                #endif
            #endif

          #if (USE_SD_SPI > OFF)
              #define SD_SPI              VSPI
              #define SD_MISO             PIN_SPI_MISO
              #define SD_MOSI             PIN_SPI_MOSI
              #define SD_SCL              PIN_SPI_SCL
              #define SD_CS               5
            #endif

        // --- PWM channels   0..15
          #if (USE_PWM_OUT > OFF)
              #if (USE_AOUT > OFF)
                  #if (USE_BUZZER_PWM > OFF)
                      #define PWM_BUZZ  0
                    #endif
                #endif

              #if (USE_TRAFFIC_LED_OUT > OFF)
                  #define PWM_TL_GREEN  1
                  #define PWM_TL_YELLOW 2
                  #define PWM_TL_RED    3
                #endif

              #if (USE_RGBLED_PWM > OFF)
                  #define PWM_RGB_RED   1
                  #define PWM_RGB_GREEN 2
                  #define PWM_RGB_BLUE  3
                #endif

              #if (USE_FAN_PWM > OFF)
                  #define PWM_FAN_1     4
                  #if (USE_FAN_PWM > 1)
                      #define PWM_FAN_2     5
                    #endif
                #endif

              #if (USE_OUT_FREQ_PWM > OFF)
                  #define PWM_FREQ_1    6
                #endif
            #endif

        // --- counter channels  0..7
          #if (USE_CNT_INP > OFF)
              #define USE_CNT_UNIDX       PCNT_UNIDX_0
              #if (USE_GEN_CNT_INP > OFF)
                  #define CNT_UNIDX_GEN0  PCNT_UNIDX_0
                  #define CNT_CH_GEN0    (PCNT_CHANNEL_0)
                  #undef  USE_CNT_UNIDX
                  #define USE_CNT_UNIDX   (CNT_UNIDX_GEN0 + 1)
                #endif
              #if (USE_GEN_CNT_INP > 1)
                  #define CNT_UNIDX_GEN1  PCNT_UNIDX_1
                  #define CNT_CH_GEN1    (PCNT_CHANNEL_0)
                  #undef  USE_CNT_UNIDX
                  #define USE_CNT_UNIDX   (CNT_UNIDX_GEN1 + 1)
                #endif
              #if (USE_GEN_CNT_INP > 2)
                  #define CNT_UNIDX_GEN2  PCNT_UNIDX_2
                  #define CNT_CH_GEN2    (PCNT_CHANNEL_0)
                  #undef  USE_CNT_UNIDX
                  #define USE_CNT_UNIDX   (CNT_UNIDX_GEN2 + 1)
                #endif
              #if (USE_GEN_CNT_INP > 3)
                  #define CNT_UNIDX_GEN3  PCNT_UNIDX_3
                  #define CNT_CH_GEN3    (PCNT_CHANNEL_0)
                  #undef  USE_CNT_UNIDX
                  #define USE_CNT_UNIDX   (CNT_UNIDX_GEN3 + 1)
                #endif
            #endif


      #endif

    // ******************************************
#endif // _PRJ_CONF_TOUCHTEST_1_H_
