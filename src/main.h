#ifndef _MAIN_H_
  #define _MAIN_H_

  // --- includes
    #include <Arduino.h>
    #include <unity.h>
    #include <Wire.h>
    #include <string.h>
    #include <stdio.h>                                                        // Biblioteca STDIO
    #include <iostream>
    #include <md_time.hpp>
    #include <md_defines.h>
    #include <md_util.h>
    #include <md_filter.hpp>
    #include <project.h>
      #if (PROJECT == ESP32_TINYHOUSE)
          #include <prj_config.h>
          #include <Adafruit_GFX.h> //Grafik Bibliothek
          #include <Adafruit_ILI9341.h> // Display Treiber
          #include <XPT2046_Touchscreen.h> //Touchscreen Treiber
          #include <md_TouchEvent.h> //Auswertung von Touchscreen Ereignissen
          #if   (PROJ_BRANCH == BRANCH_VAL_1)
              #include <prj_conf_tinyval_1.h>
          #elif (PROJ_BRANCH == BRANCH_VAL_2)
              #include <prj_conf_tinyval_2.h>
          #elif (PROJ_BRANCH == BRANCH_BLUETTI)
              #include <prj_conf_tinybluetti.h>
          #elif (PROJ_BRANCH == BRANCH_VAL_1)
              #include <prj_conf_tinytouch.h>
            #endif
          //#include <prj_conf_esp32_test2.h>
        #endif
      #if (PROJECT == ESP32_TEST2)
          #include <Adafruit_GFX.h> //Grafik Bibliothek
          #include <Adafruit_ILI9341.h> // Display Treiber
          #include <XPT2046_Touchscreen.h> //Touchscreen Treiber
          #include <md_TouchEvent.h> //Auswertung von Touchscreen Ereignissen
          #include <prj_conf_esp32_test2.h>
        #endif
      #if (PROJECT == MEASFREQ_1)
          #include <prj_measfreq_1.h>
        #endif
      #if (PROJECT == LIGHTSHOW_1)
          #include <prj_lightshow_1.h>
        #endif
      #if (PROJECT == GEN_ESP32_NODE)
          #include <prj_conf_gen_esp32_node.h>
        #endif
      #if (PROJECT == GEN_XIAO_ESP32C3)
          #include <prj_conf_gen_xiao_esp32c3.h>
        #endif
      #if (PROJECT == GEN_ESP32_D1_MINI)
          #include <prj_gen_esp32_d1_mini.h>
        #endif

    #include <prj_config.h>
        //#include <driver\gpio.h>
        //#include <driver\adc.h>
        //#include "freertos/task.h"
        //#include "freertos/queue.h"
        //#include "driver/ledc.h"
        //#include "driver/mcpwm.h"
    #include "driver/pcnt.h"
        //#include "esp_attr.h"
        //#include "esp_log.h"
  // --- system components
    #if (DEV_VSPI > OFF) || (DEV_HSPI > OFF)
        #include "spi.h"
      #endif //
  // --- memory
    #if (USE_FLASH_MEM > OFF)
        #include <FS.h>
        //#include <SPIFFS.h>
        #include <md_spiffs.h>
      #endif

    #if (USE_FRAM_I2C > OFF)
        #include <md_FRAM.h>
      #endif

    #if (USE_SD_SPI > OFF)
        //#include "sdmmc_cmd.h"
        #include <FS.h>
        #include <sd.h>
        #include <spi.h>
      #endif
  // --- user inputs
    #if (USE_TOUCHSCREEN > OFF)
        #include <prj_menu_touchtest.h>
        #include <md_touch.h>

      #endif
    #if (USE_MD_ATSMARTHOME > OFF)
        #include <AT_Database.h>
        #include <AT_MessageBuffer.h>
        #include "md_TouchEvent.h"
        #include <AT_Display.h>
        #include <AT_Webserver.h>
      #endif
    #ifdef ANTIK
      //original -> still activ

        //Unterschiedliche Pin-Belegung fuer ESP32 und D1Mini
          /*
            #ifdef ESP32
                #define TFT_CS   5
                #define TFT_DC   4
                #define TFT_RST  22
                #define TFT_LED  15
                #define TOUCH_CS 14
                #define LED_ON   0
              #endif
            */
        // --- functions
          /* Funktion zeigt in der Kopfleiste den aktuellen Score und den Level an
             Abhaengig vom Score wird der Level hinaufgesetzt und das Intervall verringert
            */
          void displayScore();

          /*  Funktion um ein Tetris-Teil zu drehen. Der Parameter ist die Nummer des Teils
             das gedreht werden soll. R�ckgabewert ist der Index des gedrehten Teils
            */
          uint8_t rotate(uint8_t pc);

          //Funktion testet ob eine Zeile voll belegt ist
          boolean rowComplete(int8_t rpg);

          /* Funkzion prueft ob es zwischen der Zeile rpc des Tetris-Teils pc und
             der Zeile rpg des Spielfelds ab der Position cpg Kolklisionen gibt.
             Wenn eine Kollision auftritt oder die letzte Zeile des Spielfelds
             erreicht wurde wird falsch zurueckgegeben
            */
          boolean checkRow(uint8_t pc, int8_t rpc, int8_t cpg, int8_t rpg);

          /* Funktion prueft ob das Tetris Teil pc am Spielfeld an der Position
             Zeile rpg Spalte cpg (linke untere Ecke des Teils) Kollisionen auftreten
            */
          boolean checkPiece(uint8_t pc, int8_t cpg, int8_t rpg);

          /* Funktion zeigt einen Block des Spielfeldes in Zeile y Spalte x mit der Farbe color an
             color ist die Farbe im 565 Format fuer das Display */
          void showBlock(uint8_t x, uint8_t y, uint16_t color);

          // Funktion faellt einen Block des Spielfeldes in Zeile y Spalte x mit der Hintergrundfarbe
          void hideBlock(uint8_t x, uint8_t y);

          /* Funktion zeigt das Tetris-Teil pc in Zeile rpg, Spalte cpg (Linke untere Ecke) an
             Die Farbe wird der Definition des Tetris-Teils entnommen
            */
          void showPiece(uint8_t pc, uint8_t cpg, uint8_t rpg);

          /* Funktion faellt die belegten Bloecke des Tetris-Teil pc in Zeile rpg,
             Spalte cpg (Linke untere Ecke) an mit Hintergrundfarbe
           */
          void hidePiece(uint8_t pc, int8_t cpg, int8_t rpg);

          /*funktion faellt die Zeile row des Spielfelds mit Hintergrundfarbe und
            loescht alle Eintraege fuer diese Zeile im Spielfeld-Speicher
            */
          void deleteRow(int8_t row);

          /* Funktion kopiert die Zeile srcrow in die Zeile dstrow
             Die Anzeige der Zielzeile wird vorher geloescht. Beim
             kopieren wird die Quellzeile in der Zielzeile angezeigt
            */
          void copyRow(int8_t srcrow, int8_t dstrow);

          /* Funktion zeigt alle Bloecke des Spielfeldes mit der Farbe ALLON an.
             Nach einer Pause von 500 ms wird das Sielfeld komplett geloescht
            */
          void clearBoard();

          /* Funktion uebertraegt das Tetris-Teil pc in den Spielfeldspeicher in der Zeile
             rpg an der Spalte cpg (linke untere Ecke)
            */
          void putPiece(uint8_t pc, int8_t cpg, int8_t rpg);

          /* Ein neues Tetristeil wird am oberen Rand des Spielfeldes eingefuegt.
             Welches Teil und in welcher Spalte wird als Zufallszahl ermittelt
             Hat das neue Teil keinen Platz am Spielfeld, so ist das Spiel zu Ende
            */
          boolean newPiece();

          /* Funktion ermittelt komplett gef�llte Zeilen am Spielfeld und entfernt diese
             Darueberliegende Zeilen werden nach unten verschoben
           */
          void removeComplete();

          /* Funktion beginnt ein neues Spiel. Der score wird auf 0 gesetzt, das Spielfeld
             geloescht und mit einem neuen Tetris Teil gestartet
           */
          void newGame();

          /* Callbackfunktion fuer Touchscreen Ereignis Klick
             Diese Funktion wird immer dann aufgerufen, wenn der Bildschirm
             kurz beruehrt wird. p gibt die Position des Beruehrungspunktes an
           */
          void onClick(TS_Point p);
      #endif
    #if (USE_KEYPADSHIELD > OFF)
        #include "md_keypadshield.h"
      #endif // USE_KEYPADSHIELD

    #if (USE_CTRL_POTI > OFF)
        // nothing to do
      #endif

    #if (USE_DIG_INP > OFF)
        //#include <driver\gpio.h>
      #endif

    #if (USE_CNT_INP > OFF)
        #include <freertos/queue.h>
        #include <driver\pcnt.h>
        #include <esp_attr.h>
      #endif

    #if (USE_PWM_INP > OFF)
        #include <driver\mcpwm.h>
        #include <esp_attr.h>
      #endif

    #if (DEV_ADC_INT > OFF)
        #include <driver\adc.h>
      #endif
    #if (DEV_ADC_ADS1115 > OFF)
        #include <md_ADS1115.hpp>
        #ifndef IRAM_ATTR
            #define IRAM_ATTR
          #endif
      #endif
  // --- user outputs
    // --- PWM
      /** ### Configure the project ------------------------

        - The example uses fixed PWM frequency of 5 kHz, duty cycle in 50%,
          and output GPIO pin.
          To change them, adjust `LEDC_FREQUENCY`, `LEDC_DUTY`,
          `LEDC_OUTPUT_IO` macros at the top of ledc_basic_example_main.c.

        - Depending on the selected `LEDC_FREQUENCY`,
          you will need to change the `LEDC_DUTY_RES`.

        - To dynamicaly set the duty and frequency,
          you can use the following functions:
          - To set the frequency to 2.5 kHZ i.e:
            ```c
            ledc_set_freq(LEDC_MODE, LEDC_TIMER, 2500);
            ```
          - Now the duty to 100% i.e:
            ```c
            ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, 8191);
            ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
            ```
        - To change the duty cycle you need to calculate
          the duty range according to the duty resolution.
          - If duty resolution is 13 bits:
            Duty range: `0 to (2 ** 13) - 1 = 8191` where 0 is 0% and 8191 is 100%.
       **/
    #if (DEV_PWM_OUTPUT > OFF)
        #include <driver\ledc.h>
        #include <md_leds.h>
        typedef struct
          {
            uint16_t red;
            uint16_t green;
            uint16_t blue;
          } outRGBVal_t;
      #endif
    #if (USE_BUZZER_PWM > OFF)
        #include "md_buzzer.h"
      #endif // USE_BUZZER_PWM

    #if (USE_OLED_I2C > OFF)
        #if !(OLED1_DRV ^ OLED_DRV_1106)
            #include "md_oled_SH1106.h"
        #else
            #include <md_oled.h>
          #endif
        #if (USE_OLED_I2C > 1)
            #if !(OLED2_DRV ^ OLED_DRV_1106)
                #include "md_oled_SH1106.h"
            #else
                #include <md_oled.h>
              #endif
          #endif
      #endif

    #if (USE_WS2812_MATRIX_OUT > OFF)
        #include <md_leds.h>
      #endif

    #if (USE_WS2812_LINE_OUT > OFF)
        #include <Adafruit_NeoPixel.h>
      #endif

    #if (USE_TFT > OFF)
        #if !(USE_TFT ^ MC_UO_TFT1602_GPIO_RO)
            #include "md_lcd.h"
          #endif
        #if !(DISP_TFT ^ MC_UO_TOUCHXPT2046_AZ)
            //#include <md_touch.h>
          #endif
      #endif
    #if (USE_CNT_INP > OFF)
        static void initGenPCNT();
        void getCNTIn();
      #endif
  // --- user inputs
    #if (USE_CNT_INP > OFF)
      #endif
  // --- network
    #if (USE_WIFI > OFF)
        #include <AsyncTCP.h>
        #include <ESPAsyncWebServer.h>
        #include <md_webserver.h>
        #include <ip_list.hpp>
        #if (USE_MQTT > OFF)
            #include <Network/Clients/MQTT.hpp>
          #endif
      #endif
  // --- sensors
    #if ( USE_DS18B20_1W_IO > OFF )
        #include <OneWire.h>
        #include <DallasTemperature.h>
      #endif

    #if ( USE_BME280_I2C > OFF )
        #include <Adafruit_Sensor.h>
        #include <Adafruit_BME280.h>
      #endif
    #if ( USE_BME680_I2C > OFF )
        #include <Adafruit_Sensor.h>
        #include <Adafruit_BME680.h>
      #endif
    #if (USE_CCS811_I2C > OFF)
        #include <Adafruit_CCS811.h>
      #endif
    #if (USE_INA3221_I2C > OFF)
        #include <SDL_Arduino_INA3221.h>
      #endif
    #if ( USE_TYPE_K_SPI > OFF )
        #include <md_31855_ktype.h>
      #endif

  // ---------------------------------------
  // --- prototypes
    // ------ system -------------------------
      // --- heap ------------------------
        void heapFree(const char* text);
    // ------ user interface -----------------
      // --- user output
        // --- display
          void clearDisp();
          void dispStatus(String msg, int16_t mstout = 10); // time to stay persistent, neg = instantly
          void dispStatus(const char* msg, int16_t mstout = 10);
          void dispText(char* msg, uint8_t col, uint8_t row, uint8_t len);
          void dispText(String msg, uint8_t col, uint8_t row, uint8_t len);
          void startOLED();
          void startTFT_TOUCH();
        // --- LED output
          #if (USE_RGBLED_PWM > OFF)
              void initRGBLED();
            #endif
        // --- passive buzzer
          #ifdef PLAY_MUSIC
              void playSong(int8_t songIdx);
              void playSong();
            #endif

        // --- traffic Light of gas sensor
          #if (USE_MQ135_GAS_ANA > OFF)
              int16_t showTrafficLight(int16_t inval, int16_t inthres);
            #endif
          #if (USE_MQ3_ALK_ANA > OFF)
              int16_t showTrafficLight(int16_t inval, int16_t inthres);
            #endif

        // WS2812 LED
          #if (USE_WS2812_LINE_OUT > OFF)
              void initWS2812Line();
              void FillLEDsFromPaletteColors(uint8_t lineNo, uint8_t colorIndex);
              void ChangePalettePeriodically(uint8_t lineNo);
              #ifdef XXXX
                  void SetupTotallyRandomPalette();
                  void SetupBlackAndWhiteStripedPalette();
                  void SetupPurpleAndGreenPalette();
                #endif
            #endif

          #if (USE_WS2812_MATRIX_OUT > OFF)
              void initWS2812Matrix();
            #endif

      // --- user input
        // --- keypad
          #if defined(KEYS)
              void startKeys();
              uint8_t getKey();
            #endif
        // --- digital input
          #if (USE_DIG_INP > OFF)
              void getDIGIn();
            #endif
          #if (USE_CTRL_POTI > OFF)
              void getADCIn();
            #endif
        // --- counter input
          #if (USE_CNT_INP > OFF)
              static void initPCNT();
              void getCNTIn();
            #endif
      // --- sensors
        // --- BME280
          #if (USE_BME280_I2C > OFF)
              static void initBME280();
            #endif
        // --- BME680
          #if (USE_BME680_I2C > OFF)
              static void initBME680();
            #endif
        // --- ADS1115
          #if (USE_ADC1115_I2C > OFF)
              static void initADS1115();
              static void startADS1115();
            #endif
        // --- CCS811
          #if (USE_CCS811_I2C > OFF)
              void initCCS811();
            #endif
          #if (USE_INA3221_I2C > OFF)
              void initINA3221();
            #endif
        // --- DS18B20
          #if (USE_DS18B20_1W_IO > OFF)
              String getDS18D20Str();
            #endif
        // --- MQ135 gas sensor
          #if (USE_MQ135_GAS_ANA > OFF)
              int16_t getGasValue();
              int16_t getGasThres();
            #endif

        // --- T-element type K
        // --- photo sensor
          #if (USE_PHOTO_SENS_ANA > OFF)
              void initPhoto();
            #endif
        // poti measure
          #if (USE_POTI_ANA > OFF)
              void initPoti();
            #endif
        // vcc measure
          #if (USE_VCC50_ANA > OFF)
              void initVCC50();
            #endif
          #if (USE_VCC50_ANA > OFF)
              void initVCC33();
            #endif
        // ACS712 current measurement
          #if (USE_ACS712_ANA > OFF)
              void initACS712();
            #endif
    // ----- memory ---------------------------
          #if (USE_FLASH_MEM > OFF)
              void startFlash();
              void testFlash();
            #endif
    // --- ATSmartHome ----
      #if (USE_MD_ATSMARTHOME > OFF)
          void initSmartHome();
          void onClick(TS_Point p);
          void onLong(TS_Point p);
          void onSwipe(uint8_t direction);
          void handleRoot();
          void handleNotFound();
          //this function is called if system setup has changed
          void systemChanged();
          // callback function. It is called on successfully received and parsed rc signal
          void rfCallback(const String &protocol, const String &message, int status,
                          size_t repeats, const String &deviceID);
        #endif
    // ------ network -------------------------
      // --- WIFI
        #if (USE_WIFI > OFF)
            uint8_t startWIFI(bool startup);
            #if (USE_NTP_SERVER > OFF)
                void    initNTPTime();
              #endif
          #endif

      // --- webserver
        #if (USE_WEBSERVER > OFF)
            //void handlingIncomingData(AsyncWebSocketClient *client, void *arg, uint8_t *data, size_t len);
            //void onEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type,
            //             void * arg, uint8_t *data, size_t len);
            //void configWebsite();
            void startWebServer();
            void readWebMessage();
            void sendMessage();
          #endif
      // --- MQTT
        #if (USE_MQTT > OFF)
            void startMQTT();
            void connectMQTT();
            void soutMQTTerr(String text, int8_t errMQTT);
            void readMQTTmsg();
          #endif
    // ---------
#endif // _MAIN_H_
