#include <Arduino.h>
#include <main.h>
#include <md_spiffs.h>
#include <prj_config.h>
#include <md_ADS1115.hpp>

// ----------------------------------------------------------------
// --- declarations
// ----------------------------------------------------------------
  // ------ system -----------------------
    static uint64_t anzUsCycles = 0ul;
    static uint64_t usLast      = 0ul;
    static uint64_t usTmp       = 0ul;
    static uint64_t usPerCycle  = 0ul;
    static uint32_t freeHeap    = 10000000;
    static int32_t  tmpval32;
    static int16_t  tmpval16;
      //static uint64_t anzMsCycles = 0;
	    //static uint64_t msLast = 0;
  	  //static uint64_t msPerCycle = 0;

	    //static uint32_t anzMsCycles = 0;
	    //static uint64_t 	    //static uint64_t msLast      = 0;msLast      = 0;
    static char     cmsg[MSG_MAXLEN+1] = "";
    static char     ctmp30[LOGINTXT_MAX_LEN];
    static String   tmpStr;
    static uint8_t  firstrun = true;
    static uint8_t  iret        = 0;
    #if ( DEV_I2C1 > OFF )
        TwoWire i2c1 = TwoWire(0);
      #endif
    #if ( DEV_I2C2 > OFF )
        TwoWire i2c2 = TwoWire(1);
      #endif
    #if ( DEV_VSPI > OFF )
        //SPIClass  SPI;
        SPIClass *pSPI = &SPI;
        //SPIClass pVSPI(VSPI);
      #endif
    #if ( DEV_HSPI > OFF )
        //SPIClass pHSPI(HSPI);
      #endif
    #if ( USE_LED_BLINK_OUT > 0 )
        msTimer ledT   = msTimer(BLINKTIME_MS);
        uint8_t sysLED = ON;
      #endif
    #if ( USE_DISP > 0 )
        uint32_t      ze     = 1;      // aktuelle Schreibzeile
        #if (USE_OLED_I2C > OFF)
            char          outBuf[OLED1_MAXCOLS + 1] = "";
          #endif
        String        outStr;
      #endif
    #ifdef USE_STATUS
        msTimer     statT  = msTimer(STAT_DELTIME_MS);
        msTimer     statN  = msTimer(STAT_NEWTIME_MS);
        char        statOut[60 + 1] = "";
        bool        statOn = false;
        bool        statDate = false;
          //char        timeOut[STAT_LINELEN + 1] = "";
      #endif
    #if (USE_ESPHALL > OFF)
        int32_t valHall = 0;
      #endif
  // --- system cycles ---------------
    #ifdef USE_INPUT_CYCLE
        static msTimer inputT           = msTimer(INPUT_CYCLE_MS);
        static uint8_t inpIdx   = 0;
      #endif
    #ifdef USE_OUTPUT_CYCLE
        static msTimer outpT            = msTimer(OUTPUT_CYCLE_MS);
        static uint8_t outpIdx  = 0;
      #endif
    #if (USE_DISP > OFF)
        static msTimer dispT            = msTimer(DISP_CYCLE_MS);
        static uint8_t dispIdx  = 0;
      #endif
  // ------ user input ---------------
    #if (USE_FLASH_MEM > OFF)
        md_spiffs   spiffs;
        md_spiffs  *pspiffs = &spiffs;
      #endif
    #if ((USE_TOUCHSCREEN > OFF) || (USE_MD_ATSMARTHOME > OFF))
        // _pTFT     = new Adafruit_ILI9341(tft_CS, tft_DC, tft_RST);
        // _ptouch   = new XPT2046_Touchscreen(cspin);
        // _ptouchev = new md_TouchEvent(_ptouch);
        static Adafruit_ILI9341     mdTFT(TFT_CS, TFT_DC, TFT_RST);
        static Adafruit_ILI9341*    pmdTFT = &mdTFT;
        static XPT2046_Touchscreen  Touch(TOUCH_CS);
        static XPT2046_Touchscreen* pTouch = &Touch;
        static md_TouchEvent        mdTouchEv(pTouch);
        static md_TouchEvent*       pmdTouchEv = &mdTouchEv;
        static md_touch             mdtouch(pmdTFT, pTouch, pmdTouchEv, TFT_LED, LED_ON, pspiffs);
        static md_touch*            pmdtouch =  &mdtouch;
        static md_menu              mdmenu;
        static md_menu*             pmdmenu = &mdmenu;
        static md_list              menuItems;
        static md_list*             pmenuItems = &menuItems;
      #endif
    #if (USE_MD_ATSMARTHOME > OFF)
        #define ARDUITOUCH_VERSION 1 //0 for older 1 for new with PNP Transistor
        //data structures for database and message buffer
        AT_Database         database(ATDEVICEFILE,ATCONFIGFILE);
        AT_MessageBuffer    msg;
        ATSETUP             atdbSetup;
        //Adafruit_ILI9341    tft(TFT_CS,TFT_DC,TFT_RST);
        #if (USE_TOUCHSCREEN == OFF)
            XPT2046_Touchscreen mdtouch(TOUCH_CS,TOUCH_IRQ);
            md_TouchEvent       tevent(&mdtouch);
          #endif
        AT_Display          dsp(pmdTFT, &database, TFT_LED, LED_ON);
        #if (USE_ATWIFI > OFF)
            WebServer srv(80);
            AT_Webserver webserver(&srv,&database);
            #include "MyWifi.h"     //all associated to WiFi
          #endif
        //Shoud be defined if a Ardui-Touch version 01-02 or higher is used
        #define ARDUITOUCHB
      #endif
    #if (USE_KEYPADSHIELD > OFF)
        md_kpad kpad(KEYS_ADC);
        uint8_t key;
      #endif // USE_KEYPADSHIELD

    #if (USE_CTRL_POTI > OFF)
        uint16_t inpValADC[USE_CTRL_POTI];
      #endif

    #if (USE_DIG_INP > OFF)
        uint8_t  valInpDig[USE_DIG_INP];
        uint8_t  pinInpDig[USE_DIG_INP];
        uint8_t  polInpDig[USE_DIG_INP];
        uint8_t  modInpDig[USE_DIG_INP];
      #endif

    #if (USE_CNT_INP > OFF)
        const pcnt_config_t config_cnt[USE_CNT_INP] =
          {
            {
              PCNT1_INP_SIG_IO,      // Pulse input GPIO number, if you want to use GPIO16, enter pulse_gpio_num = 16, a negative value will be ignored
              PCNT_PIN_NOT_USED,     // Control signal input GPIO number, a negative value will be ignored
              PCNT_MODE_KEEP,        // PCNT low control mode
              PCNT_MODE_KEEP,        // PCNT high control mode
              PCNT_COUNT_DIS,        // PCNT positive edge count mode
              PCNT_COUNT_INC,        // PCNT negative edge count mode
              PCNT_H_LIM_VAL,        // Maximum counter value
              PCNT_L_LIM_VAL,        // Minimum counter value
              (pcnt_unit_t)    PCNT1_UNIDX,  // PCNT unit number
              (pcnt_channel_t) PCNT1_CHIDX  // the PCNT channel
            },
            #if (USE_CNT_INP > 1)
                {
                  PCNT2_INP_SIG_IO,   //< Pulse input GPIO number, if you want to use GPIO16, enter pulse_gpio_num = 16, a negative value will be ignored
                  PCNT_PIN_NOT_USED,  //< Control signal input GPIO number, a negative value will be ignored
                  PCNT_MODE_KEEP,     //< PCNT low control mode
                  PCNT_MODE_KEEP,     // PCNT high control mode
                  PCNT_COUNT_DIS,     // PCNT positive edge count mode
                  PCNT_COUNT_INC,     // PCNT negative edge count mode
                  PCNT_H_LIM_VAL,     // Maximum counter value
                  PCNT_L_LIM_VAL,     // Minimum counter value
                  (pcnt_unit_t)    PCNT2_UNIDX, // PCNT unit number
                  (pcnt_channel_t) PCNT2_CHIDX  // the PCNT channel
                },
              #endif
            #if (USE_CNT_INP > 2)
                {
                  PCNT2_SIO,         // Pulse input GPIO number, if you want to use GPIO16, enter pulse_gpio_num = 16, a negative value will be ignored
                  PCNT2_CIO,         // Control signal input GPIO number, a negative value will be ignored
                  PCNT2_LCTRL_MODE,  // PCNT low control mode
                  PCNT2_HCTRL_MODE,  // PCNT high control mode
                  PCNT2_POS_MODE,    // PCNT positive edge count mode
                  PCNT2_NEG_MODE,    // PCNT negative edge count mode
                  PCNT2_H_NUM,       // Maximum counter value
                  PCNT2_L_NUM,       // Minimum counter value
                  (pcnt_unit_t)    PCNT2_UNIDX,  // PCNT unit number
                  (pcnt_channel_t) PCNT2_CHIDX   // the PCNT channel
                },
              #endif
            #if (USE_CNT_INP > 3)
                {
                  PCNT3_SIO,         // Pulse input GPIO number, if you want to use GPIO16, enter pulse_gpio_num = 16, a negative value will be ignored
                  PCNT3_CIO,         // Control signal input GPIO number, a negative value will be ignored
                  PCNT3_LCTRL_MODE,  // PCNT low control mode
                  PCNT3_HCTRL_MODE,  // PCNT high control mode
                  PCNT3_POS_MODE,    // PCNT positive edge count mode
                  PCNT3_NEG_MODE,    // PCNT negative edge count mode
                  PCNT3_H_NUM,       // Maximum counter value
                  PCNT3_L_NUM,       // Minimum counter value
                  (pcnt_unit_t)    PCNT3_UNIDX,  // PCNT unit number
                  (pcnt_channel_t) PCNT3_CHIDX   // the PCNT channel
                },
              #endif
          };
        //typedef intr_handle_t pcnt_isr_handle_t;

        /* A sample structure to pass events from the PCNT
         * interrupt handler to the main program.
         */
        typedef struct
          {
            int16_t  pulsCnt;   // the PCNT unit that originated an interrupt
            uint16_t tres;
            uint32_t freq;   // information on the event type that caused the interrupt
            uint64_t usCnt;
          } pcnt_evt_t;

        static xQueueHandle pcnt_evt_queue[USE_CNT_INP];   // A queue to handle pulse counter events
        static pcnt_evt_t cntErg[USE_CNT_INP];
        static int8_t     cntFilt[USE_CNT_INP];
        static float      cntFakt[USE_CNT_INP];
        static uint16_t   cntThresh[USE_CNT_INP];

          //static pcnt_evt_t tmpErg;
          //static uint64_t oldClk[USE_CNT_INP] = {NULL};
        static uint64_t oldUs[USE_CNT_INP];

        /* Decode what PCNT's unit originated an interrupt
         * and pass this information together with the event type
         * the main program using a queue.
         */

        #ifndef USE_INT_EVTHDL
            #define USE_INT_EVTHDL
          #endif
        portBASE_TYPE pcnt_res;
        //static const char *PCNTTAG = "pcnt_int ";

        static void IRAM_ATTR pcnt0_intr_hdl(void *arg)
          {
            BaseType_t port_status;
            pcnt_evt_t event;
            port_status = pcnt_get_counter_value((pcnt_unit_t) PCNT0_UNIDX, &(event.pulsCnt));
            event.usCnt = micros() - oldUs[PCNT0_UNIDX];
            //event.count = isrCnt[PCNT0_UNIDX];
            pcnt_res = xQueueSendToBackFromISR(pcnt_evt_queue[PCNT0_UNIDX], &event, &port_status);
            oldUs[PCNT0_UNIDX] = micros();
            port_status = pcnt_counter_clear((pcnt_unit_t) PCNT0_UNIDX);
          }

        #if (USE_CNT_INP > 1)
            static void IRAM_ATTR pcnt1_intr_hdl(void *arg)
              {
                BaseType_t port_status;
                pcnt_evt_t event;
                port_status = pcnt_get_counter_value((pcnt_unit_t) PCNT1_UNIDX, &(event.pulsCnt));
                event.usCnt = micros() - oldUs[PCNT1_UNIDX];
                //event.count = isrCnt[PCNT1_UNIDX];
                pcnt_res = xQueueSendToBackFromISR(pcnt_evt_queue[PCNT1_UNIDX], &event, &port_status);
                oldUs[PCNT1_UNIDX] = micros();
                port_status = pcnt_counter_clear((pcnt_unit_t) PCNT1_UNIDX);
              }
          #endif
        #if (USE_CNT_INP > 2)
            static void IRAM_ATTR pcnt2_intr_hdl(void *arg)
              {
                BaseType_t port_status = pdFALSE;
                pcnt_evt_t event;
                port_status = pcnt_get_counter_value((pcnt_unit_t) PCNT2_UNIDX, &(event.pulsCnt));
                event.usCnt  = micros();
                //event.intCnt = intCnt[PCNT2_UNIDX];
                xQueueSendFromISR(pcnt_evt_queue[PCNT2_UNIDX], &event, &port_status);
                //xQueueOverwriteFromISR(pcnt_evt_queue[PCNT2_UNIDX], &event, &port_status);
                port_status = pcnt_counter_clear((pcnt_unit_t) PCNT2_UNIDX);
                //intCnt[0] = 0;
              }
          #endif
        #if (USE_CNT_INP > 3)
            static void IRAM_ATTR pcnt3_intr_hdl(void *arg)
              {
                BaseType_t port_status = pdFALSE;
                pcnt_evt_t event;
                port_status = pcnt_get_counter_value((pcnt_unit_t) PCNT3_UNIDX, &(event.pulsCnt));
                event.usCnt  = micros();
                //event.intCnt = intCnt[PCNT3_UNIDX];
                xQueueSendFromISR(pcnt_evt_queue[PCNT3_UNIDX], &event, &port_status);
                //xQueueOverwriteFromISR(pcnt_evt_queue[PCNT3_UNIDX], &event, &port_status);
                port_status = pcnt_counter_clear((pcnt_unit_t) PCNT3_UNIDX);
                //intCnt[0] = 0;
              }
          #endif
      #endif
    #if (USE_PWM_INP > OFF)
        typedef struct
          {
            uint32_t lowVal;
            uint32_t highVal;
          } pwm_val_t;
        pwm_val_t pwmInVal[USE_PWM_INP];
      #endif
  // ------ user output ---------------
    #if (USE_RGBLED_PWM > OFF)
        msTimer       rgbledT   = msTimer(PWM_LEDS_CYCLE_MS);
        outRGBVal_t   outValRGB[USE_RGBLED_PWM];
        md_LEDPix24*  RGBLED        = new md_LEDPix24((uint32_t) COL24_RGBLED_1);
        md_LEDPix24*  RGBLEDold     = new md_LEDPix24((uint32_t) COL24_RGBLED_1);
        static char   ctmp8[8]      = "";
        uint8_t       LEDout        = FALSE;
        static String valRGBBright  = "";
        static String valRGBCol     = "";
        #if (USE_MQTT > OFF)
            static String topRGBBright = MQTT_RGB_BRIGHT;
            static String topRGBCol    = MQTT_RGB_COLPICK;
          #endif
        #if (TEST_RGBLED_PWM > OFF)
            //uint8_t  colRGBLED = 0;
            //uint16_t incRGBLED = 10;
            //uint32_t RGBLED_gr = 64;
            //uint32_t RGBLED_bl = 128;
            //uint32_t RGBLED_rt = 192;
          #endif
      #endif
    #if (USE_WS2812_MATRIX_OUT > OFF)
        #if (ANZ_TILES_M1 > OFF)
            md_ws2812_matrix matrix_1 = md_ws2812_matrix
              ( COLPIX_2812_T1, ROWPIX_2812_T1,
                COLTIL_2812_M1, ROWTIL_2812_M1, PIN_WS2812_M1,
                ROW1_2812_T1  + COL1_2812_T1 +
                DIR_2812_T1   + ORI_2812_T1,
                (neoPixelType) COLORD_2812_M1 + NEO_KHZ800 );
            //md_LEDPix24* matrix2812[2] = { new md_LEDPix24(), new md_LEDPix24() };
        #else
            md_ws2812_matrix matrix_1 = md_ws2812_matrix
              ( COLPIX_2812_M1, ROWPIX_2812_M1,
                0, 0, PIN_WS2812_M1,
                ROW1_2812_M1  + COL1_2812_M1 +
                DIR_2812_M1   + ORI_2812_M1,
                (neoPixelType) COLORD_2812_M1 + NEO_KHZ800 );
          #endif
        msTimer ws2812T1   = msTimer(UPD_2812_M1_MS);
        #if (USE_WS2812_MATRIX_OUT > 1)
            md_ws2812_matrix matrix_2 = md_ws2812_matrix
              ( COLPIX_2812_M2, ROWPIX_2812_M2,
                COLTIL_2812_M2, ROWTIL_2812_M2, PIN_WS2812_M2,
                #if (ANZ_TILES_M2 > OFF)
                    NEO_TILE_TOP       + NEO_TILE_LEFT +
                    NEO_TILE_ROWS      + NEO_TILE_PROGRESSIVE +
                  #endif
                NEO_MATRIX_TOP     + NEO_MATRIX_LEFT +
                NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG,
                (neoPixelType) COLORD_2812_M1 + NEO_KHZ800 );
            msTimer ws2812MT   = msTimer(UPD_2812_M1_MS);
          #endif
        // const char text2812[] = " # YES We Care !!  Happy 2022 ";
        const char text2812[] = " Im Herzen die Sonne - willkommen im Weltladen ";
        static scroll2812_t outM2812[2] = { scroll2812_t(), scroll2812_t() } ;
        static int16_t posM2812 = (int16_t) (COLPIX_2812_M1 + OFFBEG_2812_M1);
        unsigned long  ws2812_Malt = 0;
        uint32_t       ws2812_Mcnt = 0;
        uint32_t       ws2812_Mv   = 0;
        bool           ws2812_Mpwr = false;
        #if (USE_MQTT > OFF)
            static String topMaxBright = MQTT_MAX_BRIGHT;
            static String topMaxCol    = MQTT_MAX_COLPICK;
            static String valMaxBright = "";
            static String valMaxCol    = "";
          #endif
      #endif

    #if (USE_WS2812_LINE_OUT > OFF)
        msTimer         ws2812LT    = msTimer(UPD_2812_L1_MS);
        Adafruit_NeoPixel strip = Adafruit_NeoPixel(COLPIX_2812_L1, PIN_WS2812_L1, (neoPixelType) COLORD_2812_L1 + NEO_KHZ800);
          //static uint8_t ws2812_Lbright = (uint8_t) BRIGHT_2812_L1;
          //static uint8_t ws2812_Lrt  = 100;
          //static uint8_t ws2812_Lbl  = 100;
          //static uint8_t ws2812_Lgr  = 100;
        unsigned long  ws2812_Lalt = 0;
        uint32_t       ws2812_Lcnt = 0;
        uint32_t       ws2812_Lv   = 0;
        bool           ws2812_pwr  = false;
        uint16_t       idx2812L1   = 0;
        md_LEDPix24*   line2812[2] = {NULL, NULL};
        #if (USE_MQTT > OFF)
            static String topLinBright = MQTT_LIN_BRIGHT;
            static String topLinCol    = MQTT_LIN_COLPICK;
            static String valLinBright = "";
            static String valLinCol    = "";
          #endif
      #endif

    #if (USE_BUZZER_PWM > OFF)
        md_buzzer     buzz       = md_buzzer();
        #ifdef PLAY_MUSIC
            tone_t test = {0,0,0};
          #endif
      #endif // USE_BUZZER_PWM

    #if (USE_FAN_PWM > OFF)
        msTimer      fanT   = msTimer(PWM_FAN_CYCLE_MS);
        #if (USE_POTICTRL_FAN > OFF)
          #endif
        uint32_t valFanPWM[USE_FAN_PWM];
        uint16_t fanIdx = 0;
      #endif
    #if (USE_GEN_DIG_OUT > OFF)
        uint8_t testLED          = OFF;
        uint8_t testLEDold       = ON;
        static String valtestLED = "";
        #if (USE_MQTT > OFF)
            static String toptestLED  = MQTT_TEST_LED;
          #endif
      #endif

    #if (OLED1_I2C > OFF)
        #if (OLED1_I2C > OFF)
            #if !(OLED1_DRV ^ OLED_DRV_1106)
                md_oled_1106 oled1 = md_oled_1106((uint8_t) OLED1_I2C_ADDR, (uint8_t) OLED1_I2C_SDA,
                                        (uint8_t) OLED1_I2C_SCL, (OLEDDISPLAY_GEOMETRY) OLED1_GEO);
            #else
                md_oled_1306 oled1 = md_oled_1306((uint8_t) OLED1_I2C_ADDR, (uint8_t) OLED1_I2C_SDA,
                                        (uint8_t) OLED1_I2C_SCL, (OLEDDISPLAY_GEOMETRY) OLED1_GEO);
              #endif
          #endif
        #if (DISP_I2C21 > OFF)
            #if !(OLED2_DRV ^ OLED_DRV_1106)
                md_oled_1106 oled2 = md_oled_1106((uint8_t) OLED2_I2C_ADDR, (uint8_t) OLED2_I2C_SDA,
                                        (uint8_t) OLED2_I2C_SCL, (OLEDDISPLAY_GEOMETRY) OLED2_GEO);
              #else
                md_oled_1306 oled2 = md_oled_1306((uint8_t) OLED2_I2C_ADDR, (uint8_t) OLED2_I2C_SDA,
                                        (uint8_t) OLED2_I2C_SCL, (OLEDDISPLAY_GEOMETRY) OLED2_GEO);
              #endif
          #endif
        msTimer oledT   = msTimer(DISP_CYCLE_MS);
      #endif
    #if (defined(USE_TFT1602_GPIO_RO_3V3) || defined(USE_TFT1602_GPIO_RO_3V3))
        LiquidCrystal  lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);
        void*          plcd = (void*) &lcd;
        md_lcd         mlcd(plcd);
      #endif
  // ------ network ----------------------
    #if (USE_WIFI > OFF)
        md_wifi wifi  = md_wifi();
        msTimer wifiT = msTimer(WIFI_CONN_CYCLE);
        #if (USE_LOCAL_IP > OFF)
          #endif // USE_LOCAL_IP
        #if (USE_NTP_SERVER > OFF)
            msTimer ntpT    = msTimer(NTPSERVER_CYCLE);
            time_t  ntpTime = 0;
            //bool    ntpGet  = true;
            uint8_t ntpOk   = FALSE;
          #endif // USE_WEBSERVER
      #endif
    #if (USE_WEBSERVER > OFF)
        #if (TEST_SOCKET_SERVER > OFF)
          /*
            const char index_html[] PROGMEM = R"rawliteral(
            <!DOCTYPE html>
            <html>
            <head>
              <meta name="viewport" content="width=device-width, initial-scale=1">  <title>ESP32 Websocket</title>
              <script src="https://ajax.googleapis.com/ajax/libs/jquery/3.5.1/jquery.min.js">
              <script src="https://cdn.jsdelivr.net/npm/spectrum-colorpicker2/dist/spectrum.min.js"></script>

              <link rel="stylesheet" type="text/css" href="https://cdn.jsdelivr.net/npm/spectrum-colorpicker2/dist/spectrum.min.css">
              <script language="javascript">

                window.alert(location.host);
                var gwUrl = "ws://" + location.host + "/ws";
                var webSocket = new WebSocket(gwUrl);
                webSocket.onopen = function(e) {
                    console.log("open");
                }
                webSocket.onclose = function(e) {
                    console.log("close");
                }

               webSocket.onmessage = function(e) {
                    console.log("message");
                }
                function handleColor() {
                  var val = document.getElementById('type-color-on-page').value;
                  webSocket.send(val.substring(1));
                }
              </script>

              <style>
                h2 {background: #3285DC;
                    color: #FFFFFF;
                    align:center;
                }

                .content {
                    border: 1px solid #164372;
                    padding: 5px;
                }

                .button {
                   background-color: #00b300;
                   border: none;
                   color: white;
                   padding: 8px 10px;
                   text-align: center;
                   text-decoration: none;
                   display: inline-block;
                   font-size: 14px;
              }
              </style>
            </head>
            <body>
              <h2>ESP32 Websocket</h2>
              <div class="content">
              <p>Pick a color</p>
              <div id="qunit"></div>

              <input type="color" id="type-color-on-page"  />
               <p>
                 <input type="button" class="button" value="Send to ESP32" id="btn" onclick="handleColor()" />
               </p>

              </div>
            </body>
            </html>
            )rawliteral";

            // Web server running on port 80
            AsyncWebServer serv(80);
            // Web socket
            AsyncWebSocket socket("/ws");
          */
        #else
            md_server*   pmdServ   = new md_server();
            static bool  newClient = false;
          #endif
        static msTimer   servT  = msTimer(WEBSERVER_CYCLE);
        static uint8_t   pubWeb = TRUE;
        static uint8_t   webOn  = OFF;
      #endif // USE_WEBSERVER
    #if (USE_MQTT > OFF)
        const char cerrMQTT[10][20]  =
          {
            "success",          "UnknownError",
            "TimedOut",         "AlreadyConnected",
            "BadParameter",     "BadProperties",
            "NetworkError",     "NotConnected",
            "TranscientPacket", "WaitingForResult"
          };
        const  String     mqttID       = MQTT_DEVICE;
        const  String     topDevice    = MQTT_TOPDEV;
        static char       cMQTT[20]    = "";
        static String     tmpMQTT      = "";
        static MQTTmsg_t  MQTTmsgs[MQTT_MSG_MAXANZ];
        static MQTTmsg_t* pMQTTWr      = &MQTTmsgs[0];
        static MQTTmsg_t* pMQTTRd      = &MQTTmsgs[0];
        static uint8_t    anzMQTTmsg   = 0;
        static uint8_t    pubMQTT      = TRUE;
        static int8_t     errMQTT      = 0;
        struct MessageReceiver : public Network::Client::MessageReceived
          {
            void messageReceived(const Network::Client::MQTTv5::DynamicStringView & topic,
                                 const Network::Client::MQTTv5::DynamicBinDataView & payload,
                                 const uint16 packetIdentifier,
                                 const Network::Client::MQTTv5::PropertiesView & properties)
              {
                fprintf(stdout, "  Topic: %.*s ", topic.length, topic.data);
                fprintf(stdout, "  Payload: %.*s\n", payload.length, payload.data);
                if (anzMQTTmsg < (MQTT_MSG_MAXANZ - 1))
                  {
                    sprintf(pMQTTWr->topic,   "%.*s", topic.length,   topic.data);
                    sprintf(pMQTTWr->payload, "%.*s", payload.length, payload.data);
                    pMQTTWr->topic[topic.length] = 0;
                    pMQTTWr->payload[payload.length] = 0;
                    anzMQTTmsg++;
                        S3VAL(" topic payload count", pMQTTWr->topic, pMQTTWr->payload, anzMQTTmsg);
                    pMQTTWr = (MQTTmsg_t*) pMQTTWr->pNext;
                  }
                //fprintf(stdout, "Msg received: (%04X)\n", packetIdentifier);
                //readMQTTmsg(topic, payload);
              }
          };
        MessageReceiver msgHdl;
        Network::Client::MQTTv5 mqtt(mqttID.c_str(), &msgHdl);
      #endif
  // ------ sensors ----------------------
    #if (USE_BME280_I2C > OFF) // 1
        static Adafruit_BME280  bme1;
        #if (BME280_I2C == I2C1)
            TwoWire* pbme1i2c = &i2c1;
          #else
            TwoWire* pbme1i2c = &i2c2;
          #endif
        //md_val<float> bmeTVal;
        //md_val<float> bmePVal;
        //md_val<float> bmeHVal;
        float         bmeT       = 0;
        float         bmeP       = 0;
        float         bmeH       = 0;
        float         bmeTold    = MD_F_MAX;
        float         bmePold    = MD_F_MAX;
        float         bmeHold    = MD_F_MAX;
        static String valBME280t = "";
        static String valBME280p = "";
        static String valBME280h = "";
        //static int8_t pubBME280t = OFF;
        //static int8_t pubBME280h = OFF;
        //static int8_t pubBME280p = OFF;
        static int8_t bmeda      = FALSE;
        #if (USE_MQTT > OFF)
            static String topBME280t = MQTT_BME280T;
            static String topBME280p = MQTT_BME280P;
            static String topBME280h = MQTT_BME280H;
          #endif
      #endif
    #if (USE_BME680_I2C > OFF) // 1
        #if (BME680_I2C == I2C1)
            TwoWire* pbme680i2c = &i2c1;
          #else
            TwoWire* pbme1i2c = &i2c2;
          #endif
        static Adafruit_BME680  bme680 = Adafruit_BME680(pbme680i2c);
        float         bme680T    = 0;
        float         bme680P    = 0;
        float         bme680H    = 0;
        float         bme680G    = 0;
        float         bme680Told = MD_F_MAX;
        float         bme680Pold = MD_F_MAX;
        float         bme680Hold = MD_F_MAX;
        float         bme680Gold = MD_F_MAX;
        static String valBME680t = "";
        static String valBME680p = "";
        static String valBME680h = "";
        static String valBME680g = "";
        //static int8_t pubBME680t = OFF;
        //static int8_t pubBME680h = OFF;
        //static int8_t pubBME680p = OFF;
        //static int8_t pubBME680g = OFF;
        static int8_t bme680da     = FALSE;
        #if (USE_MQTT > OFF)
            static String topBME680t = MQTT_BME680T;
            static String topBME680p = MQTT_BME680P;
            static String topBME680h = MQTT_BME680H;
            static String topBME680g = MQTT_BME680G;
          #endif
      #endif
    #if (USE_ADC1115_I2C > OFF)
        static md_ADS1115 ads[USE_ADC1115_I2C];
        #if (ADS_I2C == I2C1)
            TwoWire* pads0i2c = &i2c1;
          #else
            TwoWire* pads0i2c = &i2c2;
          #endif
      #endif
    #if (USE_CCS811_I2C > OFF) // 2
        msTimer ccs811T = msTimer(1200);
        Adafruit_CCS811  ccs811;
        #if (CCS811_I2C == I2C1)
            TwoWire* pcssi2c = &i2c1;
          #else
            TwoWire* pcssi2c = &i2c2;
          #endif
        //md_val<float>   ccsCVal;
        //md_val<float>   ccsTVal;
        //md_scale<float> ccsESkal;
        //md_scale<float> ccsVSkal;
        float           ccsC       = MD_F_MAX;
        float           ccsT       = MD_F_MAX;
        float           ccsCold    = MD_F_MAX;
        float           ccsTold    = MD_F_MAX;
        static String   valCCS811c = "";
        static String   valCCS811t = "";
        static int8_t   pubCCS811c = OFF;
        static int8_t   pubCCS811t = OFF;
        static int8_t   ccsda      = FALSE;
        #if (USE_MQTT > OFF)
            static String topCCS811c = MQTT_CCS811C;
            static String topCCS811t = MQTT_CCS811T;
          #endif
      #endif
    #if (USE_INA3221_I2C > OFF) // 3
        SDL_Arduino_INA3221 ina32211(INA32211_ADDR); // def 0.1 ohm
        #if (INA32211_I2C == I2C1)
            TwoWire* ina1i2c = &i2c1;
          #else
            TwoWire* ina1i2c = &i2c2;
          #endif
        //md_val<float>    inaIVal[USE_INA3221_I2C][3];
        //md_scale<float>  inaISkal[USE_INA3221_I2C][3];
        //md_val<float>    inaUVal[USE_INA3221_I2C][3];
        //md_scale<float>  inaUSkal[USE_INA3221_I2C][3];
        static float     inaI   [USE_INA3221_I2C][3];
        static float     inaU   [USE_INA3221_I2C][3];
        static float     inaP   [USE_INA3221_I2C][3];
        static float     inaIold[USE_INA3221_I2C][3];
        static float     inaUold[USE_INA3221_I2C][3];
        static float     inaPold[USE_INA3221_I2C][3];
        static String    valINA3221i[USE_INA3221_I2C][3];
        static String    valINA3221u[USE_INA3221_I2C][3];
        static String    valINA3221p[USE_INA3221_I2C][3];
        static int8_t    pubINA3221i[USE_INA3221_I2C][3];
        static int8_t    pubINA3221u[USE_INA3221_I2C][3];
        static int8_t    pubINA3221p[USE_INA3221_I2C][3];
        #if (USE_MQTT > OFF)
            static String topINA32211i[3] = { MQTT_INA32211I1, MQTT_INA32211I2, MQTT_INA32211I3 };
            static String topINA32211u[3] = { MQTT_INA32211U1, MQTT_INA32211U2, MQTT_INA32211U3 };
            static String topINA32211p[3] = { MQTT_INA32211P1, MQTT_INA32211P2, MQTT_INA32211P3 };
          #endif
        #if (USE_INA3221_I2C > 1)
            SDL_Arduino_INA3221 ina32212(INA32212_ADDR, 0.1f);
            #if (INA32212_I2C == I2C1)
                TwoWire* ina2i2c = &i2c1;
              #else
                TwoWire* ina2i2c = &i2c2;
              #endif
            #if (USE_MQTT > OFF)
                static String topINA32212i[3] = { MQTT_INA32212I1, MQTT_INA32212I2, MQTT_INA32212I3 };
                static String topINA32212u[3] = { MQTT_INA32212U1, MQTT_INA32212U2, MQTT_INA32212U3 };
              #endif
            #if (USE_INA3221_I2C > 2)
                SDL_Arduino_INA3221 ina32213(INA32213_ADDR, 0.1f);
                #if (INA32213_I2C == I2C1)
                    TwoWire* ina3i2c = &i2c1;
                  #else
                    TwoWire* ina3i2c = &i2c2;
                  #endif
                #if (USE_MQTT > OFF)
                    static String topINA32213i[3] = { MQTT_INA32213I1, MQTT_INA32213I2, MQTT_INA32213I3 };
                    static String topINA32213u[3] = { MQTT_INA32213U1, MQTT_INA32213U2, MQTT_INA32213U3 };
                  #endif
              #endif
          #endif
      #endif
    #if (USE_DS18B20_1W_IO > OFF) // 4
        OneWire             ds1OneWire(DS1_ONEWIRE_PIN);
        DallasTemperature   ds1Sensors(&ds1OneWire);
        //md_val<uint16_t>    dsTempVal[USE_DS18B20_1W_IO];
        md_scale            dsTempSkal[USE_DS18B20_1W_IO];
        float               dsTemp   [USE_DS18B20_1W_IO];
        float               dsTempold[USE_DS18B20_1W_IO];
        #if (USE_DS18B20_1W_IO > 1)
            OneWire           ds2OneWire(DS2_ONEWIRE_PIN);
            DallasTemperature ds2Sensors(&ds2OneWire);
          #endif
        #if (USE_MQTT > OFF)
            static String valDS18B20[USE_DS18B20_1W_IO];
            static String topDS18B201 = MQTT_DS18B201;
            #if (USE_DS18B20_1W_IO > 1)
                static String topDS18B202 = MQTT_DS18B202;
              #endif
          #endif
      #endif
    #if (USE_MQ135_GAS_ANA > OFF) // 5
        //filterValue tholdGas(MQ135_ThresFilt,1);
        //md_val<double> gasVal;
        //md_val<uint16_t> gasThres;
      #endif
    #if (USE_MQ3_ALK_ANA > OFF) // 6
        //md_val<int16_t> alkVal;
        md_scale<int16_t> alkScal;
        uint16_t alk;
        uint16_t alkold;
        #if (USE_MQTT > OFF)
            static String topMQ3alk = MQTT_MQ3;
            static String valMQ3alk;
          #endif
      #endif
    #if (USE_PHOTO_SENS_ANA > OFF) // 7
        //md_val   photoVal[USE_PHOTO_SENS_ANA];
        static md_scale   photoScal[USE_PHOTO_SENS_ANA];
        static float      photof[USE_PHOTO_SENS_ANA];
        static float      photofold[USE_PHOTO_SENS_ANA];
        static uint8_t    pubPhoto[USE_PHOTO_SENS_ANA];
        static String     valPhoto[USE_PHOTO_SENS_ANA];
        #if (USE_MQTT > OFF)
            static String topPhoto1 = MQTT_PHOTO1;
            #if (USE_MQTT > 1)
                static String topLicht2 = MQTT_PHOTO2;
              #endif
          #endif
      #endif
    #if (USE_POTI_ANA > OFF) // 8
        //md_val<int16_t> potiVal[USE_POTI_ANA];
        static md_scale   potifScal[USE_POTI_ANA];
        static uint16_t   poti[USE_POTI_ANA];
        static float      potif[USE_POTI_ANA];
        static float      potifold[USE_POTI_ANA];
        static uint8_t    pubPoti[USE_POTI_ANA];
        static String     valPoti[USE_POTI_ANA];
        #if (USE_MQTT > OFF)
            static String topPoti1 = MQTT_POTI1;
            #if (USE_POTI_ANA > 1)
                static String topPoti2      = MQTT_POTI2;
                static String valPoti2      = "";
              #endif
          #endif
      #endif
    #if (USE_VCC50_ANA > OFF) // 9
        //md_val<int16_t>   vcc50Val;
        //md_scale<int16_t> vcc50Scal;
        static int16_t    vcc50;
        //int16_t           vcc50old;
        static float      vcc50f;
        static float      vcc50fold;
        static md_scale   vcc50fScal;
        static String     valVCC50;
        static uint8_t    pubVCC50;
        #if (USE_MQTT > OFF)
            static String topVCC50 = MQTT_VCC50;
          #endif
      #endif
    #if (USE_VCC33_ANA > OFF) // 10
        //md_val<int16_t>   vcc33Val;
        //md_scale<int16_t> vcc33Scal;
        static int16_t    vcc33;
        //int16_t           vcc33old;
        static float      vcc33f;
        static float      vcc33fold;
        static md_scale   vcc33fScal;
        static String     valVCC33;
        static uint8_t    pubVCC33;
        #if (USE_MQTT > OFF)
            static String topVCC33     = MQTT_VCC33;
          #endif
      #endif
    #if (USE_ACS712_ANA > OFF) // 11
        //md_val<int16_t>   i712Val[USE_ACS712_ANA];
        static md_scale   i712Scal[USE_ACS712_ANA];
        static float*     pi712[USE_ACS712_ANA];
        #if (USE_ACS712_ANA > 1)
            float*  pi712[USE_ACS712_ANA];
            #if (USE_ACS712_ANA > 2)
                float*  pi712[USE_ACS712_ANA];
                #if (USE_ACS712_ANA > 3)
                    float*  pi712[USE_ACS712_ANA];
                  #endif
              #endif
          #endif
        static int16_t    i712[USE_ACS712_ANA];
        static float      i712f[USE_ACS712_ANA];
        static float      i712fold[USE_ACS712_ANA];
        static String     vali712[USE_ACS712_ANA];
        static String     pubi712[USE_ACS712_ANA];
        #if (USE_MQTT > OFF)
            static String topi7121 = MQTT_I712_1;
            #if (USE_ACS712_ANA > 1)
                static String topi7122 = MQTT_I712_2;
                #if (USE_ACS712_ANA > 2)
                    static String topi7123 = MQTT_I712_3;
                    #if (USE_ACS712_ANA > 3)
                        static String topi7124 = MQTT_I712_4;
                      #endif
                  #endif
              #endif
          #endif
      #endif
    #if (USE_TYPE_K_SPI > 0) // 12
        //SPIClass* tkSPI = new SPIClass();
        //md_31855_ktype TypeK1(TYPEK1_CS_PIN, tkSPI);
        md_31855_ktype TypeK1(TYPEK1_CS_PIN, TYPEK_CLK_PIN, TYPEK_DATA_PIN);
        filterValue valTK1(TYPEK_FILT, TYPEK_DROP_PEEK, TYPEK1_OFFSET, TYPEK1_GAIN);
        filterValue valTK1ref(TYPEK_FILT, TYPEK_DROP_PEEK, TYPEK1_OFFSET, TYPEK1_GAIN);
        int16_t     tk1Val;
        int16_t     tk1ValRef;
        #if ( USE_TYPE_K_SPI > 1)
            //md_31855_ktype TypeK2(TYPEK2_CS_PIN, tkSPI);
            md_31855_ktype TypeK2(TYPEK2_CS_PIN, TYPEK_CLK_PIN, TYPEK_DATA_PIN);
            filterValue valTK2(TYPEK_FILT, TYPEK_DROP_PEEK, TYPEK2_OFFSET, TYPEK2_GAIN);
            filterValue valTK2ref(TYPEK_FILT, TYPEK_DROP_PEEK, TYPEK2_OFFSET, TYPEK2_GAIN);
            int16_t     tk2Val;
            int16_t     tk2ValRef;
          #endif
      #endif
    #if (USE_CNT_INP > OFF) // 13
      #endif
    #if (USE_DIG_INP > OFF) // 14
      #endif
    #if (USE_ESPHALL > OFF) // 15
      #endif
    #if (USE_MCPWM > OFF) // 16
      #endif
    #if (USE_MQTT > 0) // 17
        uint8_t errMQTTold = MD_OK;
      #endif
  // ------ memories
    #if (USE_FLASH_MEM > OFF)

      #endif

    #if (USE_FRAM_I2C > OFF)
        md_FRAM fram = md_FRAM();
      #endif
    #if (USE_SD_SPI > OFF)
        //SPIClass psdSPI(VSPI);
        //SPIClass psdSPI(SD_SPI);
        File     sdFile;         // file object that is used to read and write data
        uint8_t  sdda = FALSE;
      #endif
// ----------------------------------------------------------------
// --- system setup -----------------------------------
// ----------------------------------------------------------------
  void setup()
    {
      //      uint32_t i32tmp = 0;
      // --- system
        // disable watchdog
          disableCore0WDT();
          disableLoopWDT();
        // start system
          Serial.begin(SER_BAUDRATE);
          usleep(3000); // power-up safety delay
          STXT(" setup start ...");
          #if (SCAN_I2C > OFF)
              scanI2C(&i2c1, PIN_I2C1_SDA, PIN_I2C1_SCL);
              #if (USE_I2C > 1)
                  scanI2C(&i2c2, 0, SCAN_I2C, PIN_I2C2_SDA, PIN_I2C2_SCL);
                #endif
            #endif
          #if (TEST_NUM_CONVERT > OFF)
              int32_t src32   = 200 * 2<<16;
              int32_t src32m  = -src32;
              int16_t src16   = 0x0100;
              int16_t src16m  = -src16;
              uint8_t bits    = 16;
              int16_t dest16cpp  = (int16_t) (src32/(2 << bits));
              int16_t dest16mcpp = (int16_t) (src32m/(pow(2, bits)));
              SOUTLN();
              SOUTLN(" test convertion NUM format");
              SOUT(" (cpp) int32 -> int16: ");
              SOUT(src32); SOUT(" ~ 0x") ; SOUTHEX(src32); SOUT(" -> ");
              SOUT(dest16cpp); SOUT(" ~ 0x") ; SOUTHEXLN(dest16cpp);
              SOUT(src32m); SOUT(" ~ 0x") ; SOUTHEX(src32m); SOUT(" -> ");
              SOUT(dest16mcpp); SOUT(" ~ 0x") ; SOUTHEXLN(dest16mcpp);
              SOUTLN();
            #endif
          #if (USE_LED_BLINK_OUT > 0)
              #if (PIN_BOARD_LED > NC)
                  pinMode(PIN_BOARD_LED, OUTPUT);
                  digitalWrite(PIN_BOARD_LED, sysLED);
                #endif
            #endif
        // start OLED - output to user
          #if (USE_OLED_I2C > OFF)
              startOLED();
            #endif
        // FLASH memory
          #if (USE_FLASH_MEM > OFF)
              testFlash();
            #endif
        // start touchscreen
          #if (USE_TOUCHSCREEN > OFF)
              STXT(" setup vor startTFT_TOUCH ");
              startTFT_TOUCH();
              STXT(" setup nach startTFT_TOUCH ");
            #endif
          #if (USE_MD_ATSMARTHOME > OFF)
              startTFT_TOUCH();
              initSmartHome();
            #endif

          dispStatus("setup start ...", -1);
      // --- memories
        // SD card
          #if (USE_SD_SPI > OFF)
              //File sdFile;
              if (sdda == FALSE)
                {
                  pinMode(SD_CS, OUTPUT); // chip select pin must be set to OUTPUT mode
                  STXT(" init SD ... ");
                  //pSPI->begin(SD_SCL, SD_MISO, SD_MOSI, SD_CS);
                    //psdSPI.end();
                    //if (SD.begin(SD_CS, psdSPI))
                  STXT("   SD.begin ... ");
                  //SD.end(); // end first to unmount in case of reboot online
                  if (SD.begin(SD_CS, *pSPI))
                    {
                      sdda = TRUE;
                      if (SD.exists("/test.txt"))
                        { // if "file.txt" exists, fill will be deleted
                          if (SD.remove("/test.txt") == true)
                            {
                              SVAL("   file removed ", "/test.txt");
                            }
                        }
                      else
                        {
                          SVAL("   file not found ", "/test.txt");
                        }
                      sdFile = SD.open("/test.txt", FILE_WRITE); // open "file.txt" to write data
                      if (sdFile)
                        {
                          sdFile.printf("test ok\n"); // write test text
                          sdFile.close();
                          //STXT("   wrote text ");
                          sdFile = SD.open("/test.txt", FILE_READ);
                          if (sdFile)
                            {
                              char c = 32;
                              SOUT(" read: ");
                              while (c >= ' ')
                                {
                                  c = sdFile.read();
                                  SOUT(c);
                                }
                              STXT(" SD ready ");
                            }
                          else
                            {
                              STXT(" ERR could not open file (read)");
                            }
                        }
                       else
                        {
                          STXT(" ERR could not open file (write)");
                        }
                      sdFile.close();
                    }
                  else
                    { STXT(" ERROR SD could not be initialised "); }
                }
            #endif
        // FRAM
          #if (USE_FRAM_I2C > OFF)
            // Read the first byte
            SHEXVAL("FRAM addr ",FRAM1_I2C_ADDR);
            dispStatus("init FRAM");
            bool ret = !fram.begin(FRAM1_I2C_SDA, FRAM1_I2C_SCL, FRAM1_I2C_ADDR);
            if (ret != MD_ERR)
              {
                SOUT(" ok ProdID= ");
                uint16_t prodID, manuID;
                fram.getDeviceID(&manuID, &prodID);
                SVAL(" product ", prodID); SVAL(" producer ", manuID);
                SVAL(" FRAM selftest ", fram.selftest());
              }
            #endif
      // --- network
        // start WIFI
          #if (USE_WIFI > OFF)
              uint8_t rep = WIFI_ANZ_LOGIN;
              while(rep > 0)
                {
                      //STXT(" setup   Start WiFi ");
                  iret = startWIFI(true);
                  if (iret == MD_OK)
                      {
                        dispStatus("WIFI connected",true);
                        break;
                      }
                    else
                      {
                        #if (WIFI_IS_DUTY > OFF)
                            dispStatus("WIFI error -> halted", true);
                        #else
                            rep--;
                            if (rep > 0)
                              { dispStatus("WIFI error ..."); }
                            else
                              { dispStatus("WIFI not connected"); }
                          #endif
                      }
                  //usleep(50000);
                }
            #endif // USE_WIFI
        // start Webserer
          #if (USE_WEBSERVER > OFF)
              {
                servT.startT();
                #if (TEST_SOCKET_SERVER > OFF)
                    //socket.onEvent(onEvent);
                    //serv.addHandler(&socket);

                    //serv.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
                    //  {
                    //    request->send_P(200, "text/html", index_html, NULL);
                    //  });

                    // serv.begin();
                #else
                    startWebServer();
                  #endif
              }
            #endif
        // start MQTT
          #if (USE_MQTT > OFF)
              startMQTT();
            #endif
      // --- user input
        // start digital inputs
          #if (USE_DIG_INP > OFF)
              STXT(" config digSW Pins " );
              #if (USE_WS2812_PWR_IN_SW > OFF)
                  pinMode(PIN_WS2812_PWR_IN_SW, INPUT_PULLUP);
                  SOUT(PIN_WS2812_PWR_IN_SW); SOUT(" ");
                #endif
              #if (USE_CTRL_SW_INP > OFF)
                  pinInpDig[INP_SW_CTRL] = PIN_INP_SW_1;
                  polmodInpDig[INP_SW_CTRL] = POL_SW_CTRL >> 4 + MOD_SW_CTRL;
                #endif
              #if (USE_GEN_SW_INP > OFF)
                  pinInpDig[INP_REED_1] = PIN_INP_REED_1;
                  polInpDig[INP_REED_1] = POL_REED_1;
                  modInpDig[INP_REED_1] = MOD_REED_1;
                  //SOUT(" polInpDig "); SOUT(polInpDig[INP_REED_1]); SOUT(" ");
                  //SOUT(" modInpDig "); SOUT(modInpDig[INP_REED_1]); SOUT(" ");
                  SVAL(" modInpDig ", modInpDig[INP_REED_1]);
                  SVAL(" polInpDig ", polInpDig[INP_REED_1]);
                #endif
              for (uint8_t i = 0 ; i < USE_DIG_INP ; i++ )
                {
                  pinMode(pinInpDig[i], modInpDig[i]);
                  SOUT(pinInpDig[i]); SOUT("-"); SOUTHEX(modInpDig[i] & 0x0F);
                }
              SOUTLN();
            #endif
        // start dutycycle (pwm) inputs
          #if (USE_PWM_INP > OFF)
              //mcpwm_gpio_init(MCPWM_UNIDX_0, MCPWM0A, GPIO_OUT);
              mcpwm_gpio_init(MCPWM_UNIDX_0, MCPWM_CAP_0, PIN_PWM_INP_1); // MAGIC LINE to define WHICH GPIO
              // gpio_pulldown_en(GPIO_CAP0_IN); //Enable pull down on CAP0 signal
              mcpwm_capture_enable(MCPWM_UNIDX_0, MCPWM_SELECT_CAP0, MCPWM_NEG_EDGE, 1);
            #endif
      // --- user output
        // WS2812 LEDs
          #if (USE_WS2812_MATRIX_OUT > OFF)
              STXT("start WS2812 matrix ...");
              dispStatus("start WS2812 Matrix");
              initWS2812Matrix();
              ws2812T1.startT();
              STXT(" ok");
            #endif

          #if (USE_WS2812_LINE_OUT > OFF)
              STXT("start NEOPIXEL LED strip ... ");
              initWS2812Line();
              sleep(1);
            #endif
        // RGB LED
          #if (USE_RGBLED_PWM > OFF)
              initRGBLED();
            #endif
          #if (USE_TRAFFIC_COL16_OUT > 0)
              pinMode(PIN_TL_GREEN, OUTPUT);
              pinMode(PIN_TL_YELLOW, OUTPUT);
              pinMode(PIN_TL_RED, OUTPUT);
              digitalWrite(PIN_TL_GREEN, ON);
              digitalWrite(PIN_TL_RED, ON);
              digitalWrite(PIN_TL_YELLOW, ON);
              usleep(500000);
              digitalWrite(PIN_TL_GREEN, OFF);
              digitalWrite(PIN_TL_RED, OFF);
              digitalWrite(PIN_TL_YELLOW, OFF);
            #endif
        // start buzzer (task)
          #if (USE_BUZZER_PWM > OFF)
              pinMode(PIN_BUZZ, OUTPUT);                                                                               // Setting pin 11 as output
              #ifdef PLAY_MUSIC
                buzz.initMusic(PIN_BUZZ, PWM_BUZZ);
                #if defined(PLAY_START_MUSIC)
                    playSong();
                  #endif
                #if defined(PLAY_START_DINGDONG)
                    buzz.playDingDong();
                  #endif
              #endif
            #endif
        // start key device
          #if defined(KEYS)
              startKeys();
            #endif
        // start fans
          #if (USE_FAN_PWM > OFF)
              // Fan 1
                pinMode(PIN_PWM_FAN_1, OUTPUT);
                ledcSetup(PWM_FAN_1, PWM_FAN_FREQ, PWM_FAN_RES);
                ledcAttachPin(PIN_PWM_FAN_1, PWM_FAN_1);
                ledcWrite(PWM_FAN_1, 255);
                STXT(" Test Fan 1");
                sleep(1);
                ledcWrite(PWM_FAN_1, 0);
              // Fan 2
                #if (USE_FAN_PWM > 1)
                    pinMode(PIN_PWM_FAN_2, OUTPUT);
                    ledcSetup(PWM_FAN_2, PWM_FAN_FREQ, PWM_FAN_RES);
                    ledcAttachPin(PIN_PWM_FAN_2, PWM_FAN_2);
                    ledcWrite(PWM_FAN_2, 255);
                    STXT(" Test Fan 2");
                    sleep(1);
                    ledcWrite(PWM_FAN_2, 0);
                  #endif
            #endif
        // start freq generator
          #if (USE_BUZZER_PWM > OFF)
              pinMode(PIN_BUZZ, OUTPUT);                                                                               // Setting pin 11 as output
              #ifdef PLAY_MUSIC
                buzz.initMusic(PIN_BUZZ, PWM_BUZZ);
                #if defined(PLAY_START_MUSIC)
                    playSong();
                  #endif
                #if defined(PLAY_START_DINGDONG)
                    buzz.playDingDong();
                  #endif
              #endif
            #endif
        // start digital output (test-led)
          #if (USE_GEN_DIG_OUT > OFF)
                      #define DIG_OUT_INV             ON   // Online controlled output
                      #if (USE_MQTT > OFF)
                          #define MQTT_TEST_LED       "test-led"
                        #endif
              STXT(" init testLED ... ");
              pinMode(PIN_GEN_DIG_OUT1, OUTPUT);
              #if (DIG_OUT1_INV == ON)
                  digitalWrite(PIN_GEN_DIG_OUT1, !testLED);
                #endif
              STXT(" testLED ready");
            #endif
      // --- sensors
        // ADC ADS1115
          #if (USE_ADC1115_I2C > OFF)
              initADS1115();
              startADS1115();
            #endif
        // BME280/BME680 temperature, pessure, humidity, (680 gas)
          #if (USE_BME280_I2C > OFF)
              initBME280();
            #endif
          #if (USE_BME680_I2C > OFF)
              initBME680();
            #endif
        // temp. air quality sensor CCS811
          #if (USE_CCS811_I2C > OFF)
              initCCS811();
            #endif
        // temp. current sensor INA3221
          #if (USE_INA3221_I2C > OFF)
              initINA3221();
            #endif
        // temp. temperature sensor DS18B20
          #if (USE_DS18B20_1W_IO > OFF)
              dispStatus("init DS18D20");
              ds1Sensors.begin();
              String DS18Str = getDS18D20Str();
              #if (USE_DS18B20_1W_IO > OFF)
                  ds2Sensors.begin();
                #endif
              dispStatus(DS18Str);
                  SVAL(" DS18D20 ... ", DS18Str);
            #endif
        // alcohol sensor
          #if (USE_MQ3_ALK_ANA > OFF)
              STXT(" init alc sensors ... ");
              alkVal.begin(MQ3_FILT, MQ3_DROP, FILT_FL_MEAN);
              alkScal.setScale(MQ3_OFFRAW, PHOTO1_SCAL_GAIN, PHOTO1_SCAL_OFFREAL);
              STXT(" alc sensors ready");
            #endif
        // photo sensor
          #if (USE_PHOTO_SENS_ANA > OFF)
              initPhoto();
            #endif
        // poti measure
          #if (USE_POTI_ANA > OFF)
              initPoti();
            #endif
        // vcc measure
          #if (USE_VCC50_ANA > OFF)
              initVCC50();
            #endif
          #if (USE_VCC33_ANA > OFF)
              initVCC33();
            #endif
        // ACS712 current measurement
          #if (USE_ACS712_ANA > OFF)
              initACS712();
            #endif
        // K-type thermoelementation
          #if ( USE_TYPE_K_SPI > 0)
                STXT(" Tcouple1 ... " );
                dispStatus("init TypeK");
                uint8_t tkerr = TypeK1.begin();
                if (!tkerr)
                    {
                      STXT(" TypeK gefunden TK1 ");
                      int16_t itmp = TypeK1.actT();
                      STXT(" TK1cold ", itmp);
                      itmp = TypeK1.refT();
                      STXT(" TK1ref ", itmp);
                    }
                  else
                    {
                      STXT(" TypeK nicht gefunden TK1 ");
                    }
                #if ( USE_TYPE_K_SPI > 1)
                      STXT(" Tcouple2 ... " );
                      int16_t itmp = 0;
                      tkerr = TypeK2.begin();
                      if (!tkerr)
                          {
                            STXT(" TypeK gefunden TK2 ");
                            itmp = TypeK2.actT();
                            STXT(" TK2cold ", itmp);
                            itmp = TypeK2.refT();
                            STXT(" TK2ref ", itmp);
                          }
                        else
                          {
                            STXT(" TypeK nicht gefunden TK2 ");
                          }
                  #endif
            #endif
      // --- services using interrupt
        // start counter
          #if (USE_CNT_INP > OFF)
              STXT("config counter Pins " );
              for (uint8_t i = 0 ; i < USE_CNT_INP ; i++ )
                {
                  switch (i)
                    {
                      case 0:
                        //pinMode(PCNT0_SIO, INPUT_PULLUP);     SOUT(PCNT0_SIO); SOUT(" - ");
                        pinMode(PCNT0_SIO, INPUT);     SOUT(PCNT0_SIO); SOUT(" - ");
                        //pinMode(PCNT0_CIO, OUTPUT);           SOUT(PCNT0_CIO); SOUT(" ");
                        break;
                      #if (USE_CNT_INP > 1)
                          case 1:
                          pinMode(PCNT1_SIO, INPUT_PULLUP);   SOUT(PCNT1_SIO); SOUT(" - ");
                          //pinMode(PCNT1_CIO, OUTPUT);         SOUT(PCNT1_CIO); SOUT(" ");
                          break;
                        #endif
                      #if (USE_CNT_INP > 2)
                          case 2:
                          pinMode(PCNT2_SIO, INPUT_PULLUP); SOUT(PCNT2_CIO); SOUT(" ");
                          //pinMode(PCNT2_CIO, OUTPUT); SOUT(PCNT2_CIO); SOUT(" ");
                          break;
                        #endif
                      #if (USE_CNT_INP > 3)
                          case 3:
                          pinMode(PCNT3_SIO, INPUT_PULLUP); SOUT(PCNT3_CIO); SOUT(" ");
                          //pinMode(PCNT3_CIO, OUTPUT); SOUT(PCNT3_CIO); SOUT(" ");
                          break;
                        #endif
                      default: break;
                    }
                }
              initGenPCNT();
                      //attachInterrupt(digitalPinToInterrupt(PIN_PWM_FAN_1), &pcnt_intr_handler, FALLING);

              #ifdef USE_MCPWM
                  mcpwm_gpio_init(MCPWM_UNIDX_0, MCPWM_CAP_0, PIN_CNT_FAN_1); // MAGIC LINE to define WHICH GPIO
                  // gpio_pulldown_en(GPIO_CAP0_IN); //Enable pull down on CAP0 signal
                  mcpwm_capture_enable(MCPWM_UNIDX_0, MCPWM_SELECT_CAP0, MCPWM_NEG_EDGE, 0);
                #endif
            #endif
      // --- finish setup
          #if (DEBUG_MODE >= CFG_DEBUG_STARTUP)
              #if (USE_LED_BLINK_OUT > 0)
                  sysLED = OFF;
                  digitalWrite(PIN_BOARD_LED, sysLED);
                #endif
              dispStatus("... end setup");
              heapFree(" end setup ");
              //usleep(400000);
            #endif
    }
// ----------------------------------------------------------------
// --- system run = endless loop
// ----------------------------------------------------------------
  void loop()
    {
      anzUsCycles++;
      //SOUT(" "); SOUT(millis());
      #if ( USE_DISP > 0 )
          outStr   = "";
        #endif
      tmpval32 = ESP.getFreeHeap();
      //heapFree("+loop");
      if (tmpval32 < freeHeap)
        {
          freeHeap = tmpval32;
          heapFree(" loop ");
        }
      if (firstrun == true)
        {
          SVAL("loop task running on core ", xPortGetCoreID());
          usLast = micros();
          sleep(5);
        }
      //uint16_t t_x = 0, t_y = 0; // To store the touch coordinates
      // --- network ---
        #if (USE_NTP_SERVER > OFF)   // get time from NTP server
            if (ntpT.TOut() == true)
              {
                setTime(++ntpTime);
                if (WiFi.status() == WL_CONNECTED)
                  { // WiFi online
                    if (!ntpOk)
                      {
                        initNTPTime();
                        iret = TRUE;
                        if (iret)
                          {
                            iret = wifi.getNTPTime(&ntpTime);
                            if (iret == WIFI_OK)
                              {
                                setTime(ntpTime);
                                STXT(" NTP time syncronized");
                                ntpOk = TRUE;
                              }
                          }
                      }
                  }
                ntpT.startT();
              }
          #endif // USE_NTP_SERVER
        #if (USE_WEBSERVER > OFF)    // run webserver -> restart/run not allowed in loop task
            // read only 1 message / cycle for cycle time
                //heapFree("webserv +readmsg");
            readWebMessage();
                //heapFree("webserv -readmsg");
          #endif
        #if (USE_MQTT > OFF)
            readMQTTmsg();
          #endif
      // --- trigger measurement ---
        #if (USE_ADC1115_I2C > OFF)
            md_ADS1115_run();
          #endif
      // --- direct input ---
        #if (USE_MD_ATSMARTHOME > OFF)
            //STXT(" run touch ");
            dsp.updateDisplay();
            mdTouchEv.pollTouchScreen();
          #endif
        #if (USE_KEYPADSHIELD > OFF)
          key = getKey();
          if (key)
            {
              sprintf(outBuf,"key %d", key);
              dispStatus(outBuf);
            }
          #endif
        #if (USE_CNT_INP > OFF)
            uint64_t        lim  = 0ul;
            pcnt_evt_type_t ev;
            uint8_t         doIt = false;
            pcnt_unit_t     unit;
            sprintf(cmsg,"  loop/cnt_inp");
            for ( uint8_t i = 0; i < USE_CNT_INP ; i++ )
              {
                switch (i)
                  {
                    case 0:
                      lim  = PCNT0_UFLOW;
                      ev   = PCNT0_EVT_0;
                      unit = PCNT0_UNIDX;
                      pcnt_res = xQueueReceive(pcnt_evt_queue[PCNT0_UNIDX], &cntErg[i], 0);
                      break;
                    #if (USE_CNT_INP > 1)
                        case 1:
                          lim  = PCNT1_UFLOW;
                          ev   = PCNT1_EVT_0;
                          unit = PCNT1_UNIDX;
                          pcnt_res = xQueueReceive(pcnt_evt_queue[PCNT1_UNIDX], &cntErg[i], 0);
                          break;
                      #endif
                    #if (USE_CNT_INP > 2)
                        case 2:
                          lim  = PCNT2_UFLOW;
                          ev   = PCNT2_EVT_0;
                          unit = PCNT2_UNIDX;
                          pcnt_res = xQueueReceive(pcnt_evt_queue[PCNT2_UNIDX], &tmpErg, 0);
                          break;
                      #endif
                    #if (USE_CNT_INP > 3)
                        case 3:
                          lim  = PCNT3_UFLOW;
                          ev   = PCNT3_EVT_0;
                          unit = PCNT3_UNIDX;
                          pcnt_res = xQueueReceive(pcnt_evt_queue[PCNT3_UNIDX], &tmpErg, 0);
                          break;
                      #endif
                    default:
                      break;
                  }
                if (pcnt_res == pdTRUE)
                  {
                            //if (i == 0) { SOUT(cmsg); }
                            //SOUT("  "); SOUT(millis()); SOUT("  "); SOUT(i);
                            //SOUT(" "); SOUT("usCnt"); SOUT(" "); SOUT(cntErg[i].usCnt);
                            //SOUT(" T "); SOUT(cntThresh[i]); Serial.flush();

                    if ( (cntErg[i].usCnt > 0) )
                      {
                        cntErg[i].freq = (uint16_t) (1000000ul * cntThresh[i] * cntFakt[i] / cntErg[i].usCnt);
                          //cntErg[i].freq = (uint16_t) (cntErg[i].pulsCnt * 1000000ul / cntErg[i].usCnt);
                        //SOUT(" "); SOUT(cntErg[i].freq); Serial.flush();
                      }
                    else
                      {
                        cntErg[i].freq = 0;
                      }
                            //SOUT(" "); SOUT(i); SOUT(" "); SOUT((uint32_t) cntErg[i].freq);
                  }
                // autorange
                #if (USE_CNT_AUTORANGE > OFF)
                    // check for auto range switching

                    if (cntErg[i].usCnt > PNCT_AUTO_SWDN)
                      { // low freq
                        if (cntFilt[i] > -5)
                          {
                            SVAL("SWDN filt ", cntFilt[i]);
                            cntFilt[i]--;
                            usleep(500000);
                          }
                        if ((cntThresh[i] > 1) && (cntFilt[i] > -5))
                          {
                            cntThresh[i] /= 2;
                            doIt = true;
                            STXT("SWDN new ", cntThresh[i]);
                            usleep(500000);
                          }
                      }
                    else if ( (cntErg[i].usCnt < PNCT_AUTO_SWUP) && (cntErg[i].pulsCnt > 0) )
                      { // high freq
                        if (cntFilt[i] < 5)
                          {
                            STXT("SWUP filt ", cntFilt[i]);
                            cntFilt[i]++;
                            usleep(500000);
                          }
                        if ((cntThresh[i] < 16) && (cntFilt[i] > 5))
                          {
                            cntThresh[i] *= 2;
                            doIt = true;
                            STXT("SWUP new ", cntThresh[i]);
                            usleep(500000);
                          }
                      }
                    else
                      {
                        cntFilt[i] = 0;
                      }

                    if (doIt)
                      {
                        pcnt_counter_pause(unit);
                        logESP(pcnt_event_disable  (unit, ev),            cmsg, i);
                        logESP(pcnt_set_event_value(unit, ev, cntThresh[i]), cmsg, i);
                        pcnt_counter_clear(unit);
                        pcnt_counter_resume(unit);
                        logESP(pcnt_event_enable   (unit, ev),            cmsg, i);
                        doIt = false;
                        cntThresh[i] = 0;
                      }
                  #endif // USE_CNT_AUTORANGE
              }
                      //Serial.flush();
          #endif
        #if (USE_PWM_INP > OFF)
            mcpwm_capture_enable(MCPWM_UNIDX_0, MCPWM_SELECT_CAP0, MCPWM_NEG_EDGE, 1);
            pwmInVal->lowVal = mcpwm_capture_signal_get_value(MCPWM_UNIDX_0, MCPWM_SELECT_CAP0);

            mcpwm_capture_enable(MCPWM_UNIDX_0, MCPWM_SELECT_CAP0, MCPWM_POS_EDGE, 1);
            pwmInVal->highVal = mcpwm_capture_signal_get_value(MCPWM_UNIDX_0, MCPWM_SELECT_CAP0);
          #endif
      // --- standard input cycle ---
        #ifdef USE_INPUT_CYCLE
            if (inputT.TOut())
              {
                inpIdx++;
                    //SOUT("b1a ");
                    //heapFree("+meascyc");
                    //STXT(" # MEASCYCLE ");
                inputT.startT();
                  //SOUT("b1b ");
                switch(inpIdx)
                  {
                    case 1: // BME280_I2C / BME680_I2C
                        //SOUT("c1 ");
                        #if (USE_BME280_I2C > OFF)
                            // BME280 temperature
                              bmeT = round(bme1.readTemperature() * 10) / 10;
                              #if (BME280T_FILT > 0)
                                  bmeT = bmeTVal.doVal(bmeT);
                                #endif
                            // BME280 humidity
                              bmeH = round(bme1.readHumidity() * 10) / 10;
                              #if (BME280H_FILT > 0)
                                  bmeH = bmeHVal[0].doVal( bmeH);
                                #endif
                            // BME280 envirement air pressure
                              bmeP = round((bme1.readPressure() / 100) + 0.5);
                              #if (BME280P_FILT > 0)
                                  bmeP = bmePVal[0].doVal(bmeP);
                                #endif
                          #endif
                        #if (USE_BME680_I2C > OFF)
                            // BME680 temperature
                              //bme680T = round((bme680.readTemperature() * 10 + 5)/10);
                              bme680T = round((bme680.temperature * 10 + 5) / 10);
                              #if (BME680T_FILT > 0)
                                  bme680T = bmeTVal.doVal(bme680T);
                                #endif
                            // BME680 humidity
                              //bme680H = round((bme680.readHumidity() * 10 + 5 ) / 10);
                              bme680H = round((bme680.humidity * 10 + 5 ) / 10);
                              #if (BME680H_FILT > 0)
                                  bme680H = bme680HVal[0].doVal( bme680H);
                                #endif
                            // BME680 envirement air pressure
                              //bme680P = round((bme680.readPressure() / 100) + 0.5);
                              bme680P = round((bme680.pressure / 100) + 0.5);
                              #if (BME680P_FILT > 0)
                                  bme680P = bme680PVal[0].doVal(bme680P);
                                #endif
                            // BME680 gas sensor resistance
                              //bme680G = (float) (bme680.readGas() / 100);
                              bme680G = (float) (bme680.gas_resistance / 100);
                                  //S4VAL(" BME680 values T H P G", bme680T, bme680H, bme680P, bme680G );
                              #if (BME680G_FILT > 0)
                                  bme680G = bme680GVal[0].doVal(bme680G);
                                #endif
                          #endif
                      break;
                    case 2: // CCS811_I2C
                        #if (USE_CCS811_I2C > OFF)
                            if (ccs811.available())
                              {
                                if (ccs811.readData());  // CSS811 internal read data
                                // CO2 value
                                  ccsC = ccs811.geteCO2();
                                  #if (CCS811C_FILT > OFF)
                                      ccsC = ccsCVal.doVal(ccsC);
                                    #endif
                                // TVOC value
                                  ccsT = ccs811.getTVOC();
                                  #if (CCS811T_FILT > 0)
                                      ccsT = ccsTVal.doVal(ccsT);
                                    #endif
                              }
                          #endif
                      break;
                    case 3: // INA3221_I2C 3x U+I measures
                        //SOUT(" c3");
                        #if (USE_INA3221_I2C > OFF)
                            // U 3.3V supply
                              inaU[0][0] = ina32211.getBusVoltage_V(1);
                              #if (INA3221I1_FILT > OFF)
                                  inaU[0][0] = ccsCVal.doVal(inaU[0][0]);
                                #endif
                              //S2VAL(" incycle 3221 ina[0][0] inaUold[0][0] ", inaUold[0][0], inaU[0][0]);
                            // I 3.3V not used
                              #ifdef NOTUSED
                                  inaI[0][0] = ina32211.getCurrent_mA(1);
                                #endif
                            // P 3.3V not used
                            // U 5V supply
                              inaU[0][1] = ina32211.getBusVoltage_V(2);
                              #if (INA3221I1_FILT > OFF)
                                  inaU[0][1] = ccsCVal.doVal(inaU[0][1]);
                                #endif
                            // I 5V supply
                              inaI[0][1] = -ina32211.getCurrent_mA(2);
                            // P 5V supply
                              inaP[0][1] = (inaU[0][1] * inaI[0][1]) / 1000;
                            // U main supply 12V/19V supply
                              inaU[0][2] = ina32211.getBusVoltage_V(3);
                              #if (INA3221U3_FILT > OFF)
                                  inaU[0][2] = ccsCVal.doVal(inaU[0][2]);
                                #endif
                            // I main supply 12V/19V supply
                              inaI[0][2] = -ina32211.getCurrent_mA(3);
                              #if (INA3221I3_FILT > OFF)
                                  inaI[0][2] = ccsCVal.doVal(inaI[0][2]);
                                #endif
                            // P main supply
                              inaP[0][2] = (inaU[0][2] * inaI[0][2]) / 1000;
                          #endif
                      break;
                    case 4: // DS18B20_1W
                        //SOUT(" c4");
                        #if (USE_DS18B20_1W_IO > OFF)
                            outStr = "";
                            //outStr = getDS18D20Str();
                            dispText(outStr ,  0, 1, outStr.length());
                          #endif
                      break;
                    case 5: // MQ135_GAS_ANA
                        //SOUT(" c5");
                        #if (USE_MQ135_GAS_ANA > OFF)
                            #if (MQ135_GAS_ADC > OFF)
                                gasVal.doVal(analogRead(PIN_MQ135));
                                      //STXT(" gas measurment val = ", gasValue);
                                gasValue = (int16_t) valGas.value((double) gasValue);
                                      //STXT("    gasValue = ", gasValue);
                              #endif
                            #if (MQ135_GAS_1115 > OFF)
                              #endif
                          #endif
                      break;
                    case 6: // MQ3_ALK_ANA
                        //SOUT(" c6");
                        #if (USE_MQ3_ALK_ANA > OFF)
                            #if (MQ3_ALK_ADC > OFF)
                              #endif
                            #if (MQ3_ALK_1115 > OFF)
                                //ads[0].setGain(MQ3_1115_ATT);
                                //ads[0].setDataRate(RATE_ADS1115_860SPS);
                                //ads[0].startADCReading(MUX_BY_CHANNEL[MQ3_1115_CHIDX], /*continuous=*/false);
                                usleep(1200); // Wait for the conversion to complete
                                //while (!ads[0].conversionComplete());
                                //alk = ads[0].getLastConversionResults();   // Read the conversion results
                                alkVal.doVal(alk);   // Read the conversion results
                                //alk[0] = (uint16_t) (1000 * ads[0].computeVolts(alkVal[0].doVal(ads->readADC_SingleEnded(MQ3_1115_CHIDX))));
                              #endif
                          #endif
                      break;
                    case 7: // PHOTO_SENS_ANA
                        //SOUT(" c7");
                        #if (USE_PHOTO_SENS_ANA > OFF)
                            #if (PHOTO1_ADC > OFF)
                                //photoVal[0].doVal(analogRead(PIN_PHOTO1_SENS));
                                photof[0] = (float) analogRead(PIN_PHOTO1_SENS);
                                        //SVAL(" photo1  new ", photof[0]);
                                //photof[0] = photof[0];
                                        //SVAL(" photo1  new ", photof[0]);
                                photof[0] = photoScal[0].scale(photof[0]);
                              #endif
                            #if (PHOTO1_1115 > OFF)
                              #endif
                          #endif
                      break;
                    case 8: // POTI_ANA
                        //SOUT(" c8");
                        #if (USE_POTI_ANA > OFF)
                            #if (POTI1_ADC > OFF)
                              #endif
                            #if (POTI1_1115 > OFF)
                                poti[0]  = ads[POTI1_1115_UNIDX].getResult(POTI1_1115_CHIDX);
                                potif[0] = ads[POTI1_1115_UNIDX].getVolts(POTI1_1115_CHIDX);
                                    //S3VAL(" main vcc50f unit chan Volts ", VCC_1115_UNIDX, VCC50_1115_CHIDX, vcc50f );
                                //potif[0] = potifScal[0].scale(potif[0]);
                              #endif
                          #endif
                      break;
                    case 9: // USE_VCC50_ANA
                        //SOUT(" c9");
                        #if (USE_VCC50_ANA > OFF)
                            #if (VCC50_ADC > OFF)
                              #endif
                            #if (VCC50_1115 > OFF)
                                vcc50  = ads[VCC_1115_UNIDX].getResult(VCC50_1115_CHIDX);
                                vcc50f = ads[VCC_1115_UNIDX].getVolts(VCC50_1115_CHIDX);
                                    //S3VAL(" main vcc50f unit chan Volts ", VCC_1115_UNIDX, VCC50_1115_CHIDX, vcc50f );
                                vcc50f = vcc50fScal.scale(vcc50f);
                              #endif
                          #endif
                      break;
                    case 10: // USE_VCC33_ANA
                        //SOUT(" c10");
                        #if (USE_VCC33_ANA > OFF)
                            #if (VCC33_ADC > OFF)
                              #endif
                            #if (VCC33_1115 > OFF)
                                vcc33  = ads[VCC_1115_UNIDX].getResult(VCC33_1115_CHIDX);
                                vcc33f = ads[VCC_1115_UNIDX].getVolts(VCC33_1115_CHIDX);
                                    //S3VAL(" main vcc33f unit chan Volts ", VCC_1115_UNIDX, VCC33_1115_CHIDX, vcc33f );
                                //vcc33f = vcc33fScal.scale(vcc33f);
                              #endif
                          #endif
                    case 11: // ACS712_ANA
                        //SOUT(" c11");
                        #if (USE_ACS712_ANA > OFF)
                            #if (I712_1_ADC > OFF)
                              #endif
                            #if (I712_1_1115 > OFF)
                                i712[0]  = ads[I712_1_1115_UNIDX].getResult(VCC33_1115_CHIDX);
                                i712f[0] = ads[I712_1_1115_UNIDX].getVolts(VCC33_1115_CHIDX);
                                i712f[0] -= vcc50f/2;
                                i712f[0] *= 185;
                                        //S2VAL(" 712 Isup     ", i712[0], i712f[0]);
                              #endif
                          #endif
                      break;
                    case 12: // TYPE_K_SPI
                        //SOUT(" c12");
                        #if (USE_TYPE_K_SPI > OFF)
                            int8_t  tkerr = (int8_t) ISOK;
                            int16_t ival = TypeK1.actT();
                            tkerr = TypeK1.readErr();
                                  //SVAL(" typeK1 err ", tkerr);
                            if (!tkerr)
                              {
                                tk1Val    = valTK1.value((double) ival);
                                    //SVAL(" k1val ", tk1Val);
                                ival      = TypeK1.refT();
                                    //SVAL(" k1ref raw = ", (int) ival);
                                tk1ValRef = valTK1ref.value((double) ival);
                                    //SVAL(" k1ref = ", (int) tk1ValRef);
                              }
                            #if (USE_TYPE_K_SPI > 1)
                                ival      = TypeK2.actT();
                                tkerr     = TypeK2.readErr() % 8;
                                    //SVAL(" typeK1 err ", tkerr);
                                if (!tkerr)
                                  {
                                    tk2Val    = valTK2.value((double) ival);
                                        //SVAL(" k2val ", tk2Val);
                                    ival      = TypeK2.refT();
                                        //SVAL(" k2ref raw = ", (int) ival);
                                    tk2ValRef = valTK2ref.value((double) ival);
                                        //SVAL(" k2ref = ", (int) tk2ValRef);
                                  }
                              #endif
                          #endif
                      break;
                    case 13: // CNT_INP
                        //SOUT(" c13");
                        #if (USE_CNT_INP > OFF)
                            #ifdef USE_PW
                                getCNTIn();
                              #endif
                          #endif
                      break;
                    case 14: // DIG_INP
                        //SOUT(" c14");
                        #if (USE_DIG_INP > OFF)
                            getDIGIn();
                          #endif
                      break;
                    case 15: // ESPHALL
                        //SOUT(" c15");
                        #if (USE_ESPHALL > OFF)
                            valHall = hallRead();
                          #endif
                      break;
                    case 16: // MCPWM
                        //SOUT(" c16");
                        #if (USE_MCPWM > OFF)
                            getCNTIn();
                          #endif
                      break;
                    case 17: // MQTT
                        //SOUT(" c17");
                        #if (USE_MQTT > OFF)
                            mqtt.eventLoop();
                          #endif
                        //SOUT(" c17a");
                      break;
                    default:
                        inpIdx = 0;
                      break;
                  }
                    //heapFree("-meascyc");
              }
          #endif
      // --- direct output ---
        #if (USE_WS2812_MATRIX_OUT > OFF)
            //if (ws2812T1.TOut())
              //{
                //ws2812T1.startT();
                  if (!(outM2812[0].bmpB->pix24 == outM2812[1].bmpB->pix24)) //{}
                    //else
                    {
                      outM2812[0].bmpB->pix24 = outM2812[1].bmpB->pix24;
                      outM2812[0].bmpE->pix24 = outM2812[1].bmpE->pix24;
                    }

                    //if (strcmp(outM2812[0].text->text, outM2812[1].text->text) > 0)
                  if (strncmp((char*) outM2812[0].text, (char*) outM2812[1].text, sizeof(outM2812[1].text)) > 0)
                    {
                      memcpy((char*) outM2812[0].text, (char*) outM2812[1].text, sizeof(outM2812[1].text));
                        //memcpy(outM2812[0].text->text, outM2812[1].text->text, sizeof((char*) outM2812[1].text->text));
                    }

                      //SOUT(" matrix 0/1  "); SOUTHEX((uint32_t)  outM2812[0].text->pix24); SOUT("/"); SOUTHEXLN((uint32_t)  outM2812[0].text->pix24);
                      //SOUT(" matrix 0/1 *"); SOUTHEX((uint32_t) *outM2812[0].text->pix24); SOUT("/"); SOUTHEXLN((uint32_t) *outM2812[0].text->pix24);
                  //md_LEDPix24* ppix = *outM2812[0].text->pix24
                  if (!(outM2812[0].text->pix24 == outM2812[1].text->pix24)) //{}
                    //else
                    {
                      //STXT(" changed matrix bright&color ");
                        //SOUT((uint32_t) *outM2812[0].text->pix24); SOUT(" / "); SOUTHEXLN((uint32_t) *outM2812[1].text->pix24);
                        //SOUTHEX(outM2812[0].text->pix24->bright()); SOUT(" "); SOUTHEX(  outM2812[0].text->pix24->col24());
                        //SOUT(" / ");
                        //SOUTHEX(outM2812[1].text->pix24->bright()); SOUT(" "); SOUTHEXLN(outM2812[1].text->pix24->col24());
                      outM2812[0].text->pix24 = outM2812[1].text->pix24;
                        //STXT(" neu ");
                        //SOUT((uint32_t) *outM2812[0].text->pix24); SOUT(" / "); SOUTHEXLN((uint32_t) *outM2812[1].text->pix24);
                        //SOUTHEX(outM2812[0].text->pix24->bright()); SOUT(" "); SOUTHEX(  outM2812[0].text->pix24->col24());
                        //SOUT(" / ");
                        //SOUTHEX(outM2812[1].text->pix24->bright()); SOUT(" "); SOUTHEXLN(outM2812[1].text->pix24->col24());
                    }

                      /*
                        if (outM2812[0] == outM2812[1]) {}
                        else
                          {
                            SVAL(" changed matrix bitmap ", outM2812[0].text.pix24.bright());
                            *outM2812[0].bmpE.pix24 = *outM2812[1].bmpE.pix24;
                          }

                        if (doIt == true)
                          {
                            //SOUT(" do start-matrix ");
                            matrix_1.start_scroll_matrix((scroll2812_t*) &outM2812);
                          }
                      */
                      //SVAL(" M2812 ", micros());
                matrix_1.scroll_matrix();
                    //SVAL(" matrix ready ", micros());
                ws2812_Mcnt++;
              //}
          #endif
        #if (USE_WS2812_LINE_OUT > OFF)
            //if (ws2812LT.TOut())
              //{
                //ws2812LT.startT();
                //SOUT(" outCycle line 0/1 "); SOUTHEX(*line2812[0]); SOUT("/"); SOUTHEX(*line2812[1]);
                //SOUT(" bright 0/1 "); SOUTHEX(line2812[0]->bright()); SOUT("/"); SOUTHEX(line2812[1]->bright());
                //SOUT(" col24 0/1 "); SOUTHEX(line2812[0]->col24()); SOUT("/"); SOUTHEXLN(line2812[1]->col24());
                if (*(line2812[1]) == *(line2812[0])) {}
                  else
                  {
                    STXT(" line changed ");
                    *line2812[0] = *line2812[1];
                    strip.setBrightness(line2812[0]->bright());
                    strip.fill(line2812[0]->col24());
                    strip.show();
                  }
              //}
          #endif
        #if (USE_TOUCHSCREEN > OFF)
            // Touch has own touch and is driven by menuTask
            //STXT(" touch out ");
            pmdmenu->run();
            //STXT(" touch out end");
          #endif
        #if (USE_BME680_I2C)
            tmpval16 = bme680.remainingReadingMillis();
            if (tmpval16 <= 0) { bme680.endReading(); }
          #endif
      // --- standard output cycle ---
        #ifdef USE_OUTPUT_CYCLE
            if (outpT.TOut())
              {
                outpIdx++;
                outpT.startT();
                switch(outpIdx)
                  {
                    case 1:  // BME280_I2C / BME680
                        #if (USE_BME280_I2C > OFF)
                            if (bmeT != bmeTold)
                              {
                                    //SVAL(" 280readT  new ", bmeT);
                                #if (USE_MQTT > OFF)
                                    if (errMQTT == MD_OK)
                                      {
                                        valBME280t = bmeT;
                                        errMQTT = (int8_t) mqtt.publish(topBME280t.c_str(), (uint8_t*) valBME280t.c_str(), valBME280t.length());
                                        soutMQTTerr(topBME280t.c_str(), errMQTT);
                                            //SVAL(topBME280t, valBME280t);
                                      }
                                  #endif
                                #if (USE_WEBSERVER > OFF)
                                    tmpStr = "SVA0";
                                    tmpval16 = (int16_t) (bmeT+ 0.5);
                                    tmpStr.concat(tmpval16);
                                    pmdServ->updateAll(tmpStr);
                                  #endif
                                bmeTold = bmeT;
                              }
                            if (bmeP != bmePold)
                              {
                                #if (USE_MQTT > OFF)
                                    if (errMQTT == MD_OK)
                                      {
                                        valBME280p = bmeP;
                                        errMQTT = (int8_t) mqtt.publish(topBME280p.c_str(), (uint8_t*) valBME280p.c_str(), valBME280p.length());
                                        soutMQTTerr(topBME280p.c_str(), errMQTT);
                                            //SVAL(topBME280p, valBME280p);
                                      }
                                  #endif
                                #if (USE_WEBSERVER > OFF)
                                    tmpStr = "SVA1";
                                    tmpval16 = (uint16_t) bmeP;
                                    tmpStr.concat(tmpval16);
                                    pmdServ->updateAll(tmpStr);
                                  #endif
                                bmePold = bmeP;
                              }
                            if (bmeH != bmeHold)
                              {
                                #if (USE_MQTT > OFF)
                                    if (errMQTT == MD_OK)
                                      {
                                        valBME280h = bmeH;
                                        errMQTT = (int8_t) mqtt.publish(topBME280h.c_str(), (uint8_t*) valBME280h.c_str(), valBME280h.length());
                                        soutMQTTerr(topBME280h.c_str(), errMQTT);
                                            //SVAL(topBME280h, valBME280h);
                                      }
                                  #endif
                                #if (USE_WEBSERVER > OFF)
                                    tmpStr = "SVA2";
                                    tmpval16 = (int16_t) bmeH;
                                    tmpStr.concat(tmpval16);
                                    pmdServ->updateAll(tmpStr);
                                  #endif
                                bmeHold = bmeH;
                              }
                          #endif
                        #if (USE_BME680_I2C > OFF)
                            if (bme680T != bme680Told)
                              {
                                    //SVAL(" 680readT  new ", bmeT);
                                #if (USE_MQTT > OFF)
                                    if (errMQTT == MD_OK)
                                      {
                                        valBME680t = bme680T;
                                        errMQTT = (int8_t) mqtt.publish(topBME680t.c_str(), (uint8_t*) valBME680t.c_str(), valBME680t.length());
                                        soutMQTTerr(topBME680t.c_str(), errMQTT);
                                            //SVAL(topBME280t, valBME280t);
                                      }
                                  #endif
                                #if (USE_WEBSERVER > OFF)
                                    tmpStr = "SVA0";
                                    tmpval16 = (int16_t) (bme680T+ 0.5);
                                    tmpStr.concat(tmpval16);
                                    pmdServ->updateAll(tmpStr);
                                  #endif
                                bme680Told = bme680T;
                              }
                            if (bme680P != bme680Pold)
                              {
                                #if (USE_MQTT > OFF)
                                    if (errMQTT == MD_OK)
                                      {
                                        valBME680p = bme680P;
                                        errMQTT = (int8_t) mqtt.publish(topBME680p.c_str(), (uint8_t*) valBME680p.c_str(), valBME680p.length());
                                        soutMQTTerr(topBME680p.c_str(), errMQTT);
                                            //SVAL(topBME680p, valBME680p);
                                      }
                                  #endif
                                #if (USE_WEBSERVER > OFF)
                                    tmpStr = "SVA1";
                                    tmpval16 = (uint16_t) bme680P;
                                    tmpStr.concat(tmpval16);
                                    pmdServ->updateAll(tmpStr);
                                  #endif
                                bme680Pold = bme680P;
                              }
                            if (bme680H != bme680Hold)
                              {
                                #if (USE_MQTT > OFF)
                                    if (errMQTT == MD_OK)
                                      {
                                        valBME680h = bme680H;
                                        errMQTT = (int8_t) mqtt.publish(topBME680h.c_str(), (uint8_t*) valBME680h.c_str(), valBME680h.length());
                                        soutMQTTerr(topBME680h.c_str(), errMQTT);
                                            //SVAL(topBME680h, valBME680h);
                                      }
                                  #endif
                                #if (USE_WEBSERVER > OFF)
                                    tmpStr = "SVA2";
                                    tmpval16 = (int16_t) bme680H;
                                    tmpStr.concat(tmpval16);
                                    pmdServ->updateAll(tmpStr);
                                  #endif
                                bme680Hold = bme680H;
                              }
                            if (bme680G != bme680Gold)
                              {
                                #if (USE_MQTT > OFF)
                                    if (errMQTT == MD_OK)
                                      {
                                        valBME680g = bme680G;
                                        errMQTT = (int8_t) mqtt.publish(topBME680g.c_str(), (uint8_t*) valBME680g.c_str(), valBME680g.length());
                                        soutMQTTerr(topBME680g.c_str(), errMQTT);
                                            //SVAL(topBME680g, valBME680g);
                                      }
                                  #endif
                                #if (USE_WEBSERVER > OFF)
                                    tmpStr = "SVA2";
                                    tmpval16 = (int16_t) bme680G;
                                    tmpStr.concat(tmpval16);
                                    pmdServ->updateAll(tmpStr);
                                  #endif
                                bme680Gold = bme680G;
                              }
                          #endif
                          //outpIdx++;
                      break;
                    case 2:  // CCS811_I2C
                        #if (USE_CCS811_I2C > OFF)
                            if (ccsC != ccsCold)
                              {
                                valCCS811c = ccsC;
                                //pubCCS811c = TRUE;
                                    //SVAL(" 811readC  new ", ccsC);
                                #if (USE_MQTT > OFF)
                                    if (errMQTT == MD_OK)
                                      {
                                        errMQTT = (int8_t) mqtt.publish(topCCS811c.c_str(), (uint8_t*) valCCS811c.c_str(), valCCS811c.length());
                                        soutMQTTerr(topCCS811c.c_str(), errMQTT);
                                            //SVAL(topCCS811c.c_str(), valCCS811c);
                                      }
                                  #endif
                                #if (USE_WEBSERVER > OFF)
                                  #endif
                                ccsCold    = ccsC;
                              }

                            if (ccsT != ccsTold)
                              {
                                valCCS811t = ccsT;
                                //pubCCS811t = TRUE;
                                #if (USE_MQTT > OFF)
                                    if (errMQTT == MD_OK)
                                      {
                                        errMQTT = (int8_t) mqtt.publish(topCCS811t.c_str(), (uint8_t*) valCCS811t.c_str(), valCCS811t.length());
                                        soutMQTTerr(topCCS811t.c_str(), errMQTT);
                                            //SVAL(topCCS811t.c_str(), valCCS811t);
                                      }
                                  #endif
                                #if (USE_WEBSERVER > OFF)
                                  #endif
                                ccsTold    = ccsT;
                                    //SVAL(" 811readT  new ", ccsT);
                              }
                          #endif
                        outpIdx++;
                      //break;
                    case 3:  // INA3221_I2C 3x U+I measures
                        #if (USE_INA3221_I2C > OFF)
                            // U 3.3V supply
                              if (inaU[0][0] != inaUold[0][0])
                                {
                                  valINA3221u[0][0] = inaU[0][0];
                                      //S2VAL(" incycle 3221 ina[0][0] inaUold[0][0] ", inaUold[0][0], inaU[0][0]);
                                      //SVAL(" U 3.3    new ", inaU[0][0]);
                                  #if (USE_MQTT > OFF)
                                      if (errMQTT == MD_OK)
                                        {
                                          errMQTT = (int8_t) mqtt.publish(topINA32211u[0].c_str(),
                                                                           (uint8_t*) valINA3221u[0][0].c_str(),
                                                                          valINA3221u[0][0].length());
                                          soutMQTTerr(topINA32211u[0].c_str(), errMQTT);
                                              //SVAL(topINA32211u[0].c_str(), valINA3221u[0][0]);
                                        }
                                    #endif
                                  #if (USE_WEBSERVER > OFF)
                                    #endif
                                  inaUold[0][0]     = inaU[0][0];
                                }
                            // I 3.3V not used
                              #ifdef NOTUSED
                                  if (inaI[0][0] != inaIold[0][0])
                                    {
                                      valINA3221i[0][0] = inaI[0][0];
                                          SVAL(" I 3.3 new ", inaI[0][0]);
                                      #if (USE_MQTT > OFF)
                                          if (errMQTT == MD_OK)
                                            {
                                              errMQTT = (int8_t) mqtt.publish(topINA32211i[0].c_str(),
                                                                              (uint8_t*) valINA3221i[0][0].c_str(),
                                                                              valINA3221i[0][0].length());
                                              soutMQTTerr(topINA32211i[0].c_str(), errMQTT);
                                                  SVAL(topINA32211i[0].c_str(), valINA3221i[0][0]);
                                            }
                                        #endif
                                    }
                                  #if (USE_WEBSERVER > OFF)
                                    #endif
                                  inaIold[0][0]     = inaI[0][0];
                                #endif
                            // P 3.3V not used
                                  #if (USE_MQTT > OFF)
                                      if (errMQTT == MD_OK)
                                        {
                                        }
                                    #endif
                                  #if (USE_WEBSERVER > OFF)
                                    #endif
                            // U 5V supply
                              if (inaU[0][1] != inaUold[0][1])
                                {
                                  valINA3221u[0][1] = inaU[0][1];
                                  //pubINA3221u[0][1] = TRUE;
                                      //SVAL(" U 5.0    new ", inaU[0][1]);
                                  #if (USE_MQTT > OFF)
                                      if (errMQTT == MD_OK)
                                        {
                                          errMQTT = (int8_t) mqtt.publish(topINA32211u[1].c_str(),
                                                                          (uint8_t*) valINA3221u[0][1].c_str(),
                                                                          valINA3221u[0][1].length());
                                          soutMQTTerr(topINA32211u[1].c_str(), errMQTT);
                                              //SVAL(topINA32211u[1].c_str(), valINA3221u[0][1]);
                                        }
                                    #endif
                                  #if (USE_WEBSERVER > OFF)
                                    #endif
                                  inaUold[0][1]     = inaU[0][1];
                                }
                            // I 5V supply
                              if (inaI[0][1] != inaIold[0][1])
                                {
                                  valINA3221i[0][1] = inaI[0][1];
                                  //pubINA3221i[0][1] = TRUE;
                                      //SVAL(" I 5.0    new ", inaI[0][1]);
                                  #if (USE_MQTT > OFF)
                                      if (errMQTT == MD_OK)
                                        {
                                          errMQTT = (int8_t) mqtt.publish(topINA32211i[1].c_str(),
                                                                          (uint8_t*) valINA3221i[0][1].c_str(),
                                                                          valINA3221i[0][1].length());
                                          soutMQTTerr(topINA32211i[1].c_str(), errMQTT);
                                              //SVAL(topINA32211i[1].c_str(), valINA3221i[0][1]);
                                        }
                                    #endif
                                  #if (USE_WEBSERVER > OFF)
                                    #endif
                                  inaIold[0][1]     = inaI[0][1];
                                }
                            // P 5V supply
                              if (inaP[0][1] != inaPold[0][1])
                                {
                                  valINA3221p[0][1] = inaP[0][1];
                                  //pubINA3221p[0][1] = TRUE;
                                      //SVAL(" P 5.0    new ", inaP[0][1]);
                                  #if (USE_MQTT > OFF)
                                      if (errMQTT == MD_OK)
                                        {
                                          errMQTT = (int8_t) mqtt.publish(topINA32211p[1].c_str(),
                                                                          (uint8_t*) valINA3221p[0][1].c_str(),
                                                                          valINA3221p[0][1].length());
                                          soutMQTTerr(topINA32211p[1].c_str(), errMQTT);
                                              //SVAL(topINA32211p[1].c_str(), valINA3221p[0][1]);
                                        }
                                    #endif
                                  #if (USE_WEBSERVER > OFF)
                                    #endif
                                }
                              inaPold[0][1]     = inaP[0][1];
                            // U main supply 12V/19V supply
                              if (inaU[0][2] != inaUold[0][2])
                                {
                                  valINA3221u[0][2] = inaU[0][2];
                                  //pubINA3221u[0][2] = TRUE;
                                      //SVAL(" U supply new ", inaU[0][2]);
                                  #if (USE_MQTT > OFF)
                                      if (errMQTT == MD_OK)
                                        {
                                          errMQTT = (int8_t) mqtt.publish(topINA32211u[2].c_str(),
                                                                          (uint8_t*) valINA3221u[0][2].c_str(),
                                                                          valINA3221u[0][2].length());
                                          soutMQTTerr(topINA32211u[2].c_str(), errMQTT);
                                              //SVAL(topINA32211u[2].c_str(), valINA3221u[0][2]);
                                        }
                                    #endif
                                  #if (USE_WEBSERVER > OFF)
                                    #endif
                                  inaUold[0][2]     = inaU[0][2];
                                }
                            // I main supply 12V/19V supply
                              if (inaI[0][2] != inaIold[0][2])
                                {
                                  valINA3221i[0][2] = inaI[0][2];
                                  //pubINA3221i[0][2] = TRUE;
                                  inaIold[0][2]     = inaI[0][2];
                                      //SVAL(" I supply new ", inaI[0][2]);
                                  #if (USE_MQTT > OFF)
                                      if (errMQTT == MD_OK)
                                        {
                                          errMQTT = (int8_t) mqtt.publish(topINA32211i[2].c_str(),
                                                                          (uint8_t*) valINA3221i[0][2].c_str(),
                                                                          valINA3221i[0][2].length());
                                          soutMQTTerr(topINA32211i[2].c_str(), errMQTT);
                                              //SVAL(topINA32211i[2].c_str(), valINA3221i[0][2]);
                                        }
                                    #endif
                                  #if (USE_WEBSERVER > OFF)
                                    #endif
                                  inaIold[0][2]     = inaI[0][2];
                                }
                            // P main supply
                              if (inaP[0][2] != inaPold[0][2])
                                {
                                  valINA3221p[0][2] = inaP[0][2];
                                  //pubINA3221p[0][2] = TRUE;
                                  inaPold[0][2]     = inaP[0][2];
                                      //SVAL(" P supply new ", inaP[0][2]);
                                  #if (USE_MQTT > OFF)
                                      if (errMQTT == MD_OK)
                                        {
                                          errMQTT = (int8_t) mqtt.publish(topINA32211p[2].c_str(),
                                                                          (uint8_t*) valINA3221p[0][2].c_str(),
                                                                          valINA3221p[0][2].length());
                                          soutMQTTerr(topINA32211p[2].c_str(), errMQTT);
                                              //SVAL(topINA32211p[2].c_str(), valINA3221p[0][2]);
                                        }
                                    #endif
                                }
                          #endif
                      //outpIdx++;
                      break;
                    case 4:  // DS18B20_1W temperature
                        #if (USE_DS18B20_1W_IO > OFF)
                            #if (USE_MQTT > OFF)
                                if (errMQTT == MD_OK)
                                  {
                                  }
                              #endif
                            #if (USE_WEBSERVER > OFF)
                              #endif
                          #endif
                        outpIdx++;
                      //break;
                    case 5:  // MQ135_GAS_ANA
                        #if (USE_MQ135_GAS_ANA > OFF)
                            #if (USE_MQTT > OFF)
                                if (errMQTT == MD_OK)
                                  {
                                  }
                              #endif
                            #if (USE_WEBSERVER > OFF)
                              #endif
                          #endif
                        outpIdx++;
                      //break;
                    case 6:  // MQ3_ALK_ANA
                        #if (USE_MQ3_ALK_ANA > OFF)
                            #if (USE_MQTT > OFF)
                                if (errMQTT == MD_OK)
                                  {
                                  }
                              #endif
                            #if (USE_WEBSERVER > OFF)
                              #endif
                          #endif
                        outpIdx++;
                      //break;
                    case 7:  // PHOTO_SENS_ANA
                        #if (USE_PHOTO_SENS_ANA > OFF)
                            if (photof[0] != photofold[0])
                              {
                                #if (USE_MQTT > OFF)
                                    if (errMQTT == MD_OK)
                                      {
                                        valPhoto[0]  = photof[0];
                                        errMQTT = (int8_t) mqtt.publish(topPhoto1.c_str(), (uint8_t*) valPhoto[0].c_str(), valPhoto[0].length());
                                        soutMQTTerr(topPhoto1.c_str(), errMQTT);
                                            //SVAL(topPhoto1, valPhoto[0]);
                                      }
                                  #endif
                                #if (USE_WEBSERVER > OFF)
                                    tmpStr = "SVA3";
                                    tmpval16 = valPhoto[0].toInt();
                                    tmpStr.concat(tmpval16);
                                    pmdServ->updateAll(tmpStr);
                                  #endif
                                photofold[0] = photof[0];
                                    //SVAL(" photo1  new ", photof[0]);
                              }
                          #endif
                        outpIdx++;
                      //break;
                    case 8:  // POTI_ANA
                        #if (USE_POTI_ANA > OFF)
                            if (potif[0] != potifold[0])
                              {
                                #if (USE_MQTT > OFF)
                                    if (errMQTT == MD_OK)
                                      {
                                        valPoti[0]  = potif[0];
                                        errMQTT = (int8_t) mqtt.publish(topPoti1.c_str(),
                                                                        (uint8_t*) valPoti[0].c_str(),
                                                                        valPoti[0].length());
                                        soutMQTTerr(topPoti1.c_str(), errMQTT);
                                            //SVAL(topINA32211u[1].c_str(), valINA3221u[0][1]);
                                      }
                                  #endif
                                #if (USE_WEBSERVER > OFF)
                                  #endif
                                potifold[0] = potif[0];
                              }
                          #endif
                        outpIdx++;
                      //break;
                    case 9:  // VCC50_ANA
                        #if (USE_VCC50_ANA > OFF)
                            if (vcc50f != vcc50fold)
                              {
                                #if (USE_MQTT > OFF)
                                    if (errMQTT == MD_OK)
                                      {
                                        valVCC50  = vcc50f;
                                        errMQTT = (int8_t) mqtt.publish(topVCC50.c_str(),
                                                                        (uint8_t*) valVCC50.c_str(),
                                                                        valVCC50.length());
                                        soutMQTTerr(topVCC50.c_str(), errMQTT);
                                            //SVAL(topVCC50.c_str(), valVCC50);
                                      }
                                  #endif
                                #if (USE_WEBSERVER > OFF)
                                  #endif
                                vcc50fold = vcc50f;
                              }
                          #endif
                        outpIdx++;
                      //break;
                    case 10: // VCC33_ANA
                        #if (USE_VCC33_ANA > OFF)
                            if (vcc33f != vcc33fold)
                              {
                                #if (USE_MQTT > OFF)
                                    if (errMQTT == MD_OK)
                                      {
                                        valVCC33  = vcc33f;
                                        errMQTT = (int8_t) mqtt.publish(topVCC33.c_str(),
                                                                        (uint8_t*) valVCC33.c_str(),
                                                                        valVCC33.length());
                                        soutMQTTerr(topVCC33.c_str(), errMQTT);
                                            //SVAL(topVCC33.c_str(), valVCC33);
                                      }
                                  #endif
                                #if (USE_WEBSERVER > OFF)
                                  #endif
                                vcc33fold = vcc33f;
                              }
                          #endif
                        outpIdx++;
                      //break;
                    case 11: // ACS712_ANA
                        #if (USE_ACS712_ANA > OFF)
                            if (i712f[0] != i712fold[0])
                              {
                                #if (USE_MQTT > OFF)
                                    if (errMQTT == MD_OK)
                                      {
                                        vali712[0]  = i712f[0];
                                        errMQTT = (int8_t) mqtt.publish(topi7121.c_str(),
                                                                        (uint8_t*) vali712[0].c_str(),
                                                                        vali712[0].length());
                                        soutMQTTerr(topi7121.c_str(), errMQTT);
                                            //SVAL(topi7121.c_str(), vali712[0]);
                                      }
                                  #endif
                                #if (USE_WEBSERVER > OFF)
                                  #endif
                                i712fold[0] = i712f[0];
                              }
                          #endif
                        outpIdx++;
                      //break;
                    case 12: // TYPE_K_SPI
                        #if (USE_TYPE_K_SPI > OFF)
                            #if (USE_MQTT > OFF)
                                if (errMQTT == MD_OK)
                                  {
                                  }
                              #endif
                            #if (USE_WEBSERVER > OFF)
                              #endif
                          #endif
                        outpIdx++;
                      //break;
                    case 13: // CNT_INP
                        #if (USE_CNT_INP > OFF)
                            #if (USE_MQTT > OFF)
                                if (errMQTT == MD_OK)
                                  {
                                  }
                              #endif
                            #if (USE_WEBSERVER > OFF)
                              #endif
                          #endif
                        outpIdx++;
                      //break;
                    case 14: // DIG_INP
                        #if (USE_DIG_INP > OFF)
                            #if (USE_MQTT > OFF)
                                if (errMQTT == MD_OK)
                                  {
                                  }
                              #endif
                            #if (USE_WEBSERVER > OFF)
                              #endif
                          #endif
                        outpIdx++;
                      //break;
                    case 15: // ESPHALL
                        #if (USE_ESPHALL > OFF)
                            #if (USE_MQTT > OFF)
                                if (errMQTT == MD_OK)
                                  {
                                  }
                              #endif
                            #if (USE_WEBSERVER > OFF)
                              #endif
                          #endif
                        outpIdx++;
                      //break;
                    case 16: // MCPWM
                        #if (USE_MCPWM > OFF)
                            #if (USE_MQTT > OFF)
                                if (errMQTT == MD_OK)
                                  {
                                  }
                              #endif
                            #if (USE_WEBSERVER > OFF)
                              #endif
                          #endif
                        outpIdx++;
                      //break;
                    case 17: // RGBLED_PWM
                      #if (USE_RGBLED_PWM > OFF)
                          if (rgbledT.TOut())
                            {
                                  //STXT(" # Out RGBLED");
                              rgbledT.startT();
                              #if (TEST_RGBLED_PWM > OFF)
                                /*
                                  switch (colRGBLED)
                                    {
                                      case 0:
                                        if (RGBLED_rt >= 254)
                                          {
                                            RGBLED_rt = 0;
                                            RGBLED_gr += incRGBLED;
                                            colRGBLED++;
                                          }
                                          else
                                          { RGBLED_rt += incRGBLED; }
                                        break;
                                      case 1:
                                        if (RGBLED_gr >= 254)
                                          {
                                            RGBLED_gr = 0;
                                            RGBLED_bl += incRGBLED;
                                            colRGBLED++;
                                          }
                                          else
                                          { RGBLED_gr += incRGBLED; }
                                        break;
                                      case 2:
                                        if (RGBLED_bl >= 254)
                                          {
                                            RGBLED_bl = 0;
                                            RGBLED_rt += incRGBLED;
                                            colRGBLED = 0;
                                          }
                                          else
                                          { RGBLED_bl += incRGBLED; }
                                        break;
                                      default:
                                        break;
                                    }
                                  */

                                  #if (USE_WEBCTRL_RGB > OFF)
                                      _tmp += 4;
                                      if (_tmp > 50)
                                        { _tmp = 0; }
                                          //SVAL(" _tmp = ", _tmp);
                                      ledcWrite(PWM_RGB_RED,   webMD.getDutyCycle(0));
                                      ledcWrite(PWM_RGB_GREEN, webMD.getDutyCycle(1));
                                      ledcWrite(PWM_RGB_BLUE,  webMD.getDutyCycle(2));
                                    #endif


                                  if(*RGBLED[0] == *RGBLED[1]) {}
                                    else
                                    {
                                      SOUT(" RGBLED changed 0/1 "); SOUTHEX((uint32_t) *RGBLED[0]);
                                      SOUT(" / "); SOUTHEXLN((uint32_t) *RGBLED[1]);
                                      *RGBLED[0] = *RGBLED[1];
                                      //ledcWrite(PWM_RGB_RED,   BrightCol(RGBLED[0][LED_RED],RGBLED[0][LED_BRIGHT]));
                                      //ledcWrite(PWM_RGB_GREEN, BrightCol(RGBLED[0][LED_GREEN],RGBLED[0][LED_BRIGHT]));
                                      //ledcWrite(PWM_RGB_BLUE, BrightCol()  BrightCol(RGBLED[0][LED_BLUE],RGBLED[0][LED_BRIGHT]));
                                      ledcWrite(PWM_RGB_RED,   Bright_x_Col(Red(RGBLED[0]->col24()),   RGBLED[0]->bright()));
                                      ledcWrite(PWM_RGB_GREEN, Bright_x_Col(Green(RGBLED[0]->col24()), RGBLED[0]->bright()));
                                      ledcWrite(PWM_RGB_BLUE,  Bright_x_Col(Blue(RGBLED[0]->col24()),  RGBLED[0]->bright()));
                                    }
                                #endif
                                    //S2HEXVAL(" outcycle vor udate RGBLEDold RGBLED ", RGBLEDold->toInt(), RGBLED->toInt());
                              if (RGBLED->toInt() != RGBLEDold->toInt())
                                {
                                  // update HW
                                    LEDout = (uint8_t) map(RGBLED->bright(), 0, 255, 0, Green(RGBLED->col24()));
                                    ledcWrite(PWM_RGB_GREEN, LEDout);
                                    LEDout = (uint8_t) map(RGBLED->bright(), 0, 255, 0, Red(RGBLED->col24()));
                                    ledcWrite(PWM_RGB_RED, LEDout);
                                    LEDout = (uint8_t) map(RGBLED->bright(), 0, 255, 0, Blue(RGBLED->col24()));
                                    ledcWrite(PWM_RGB_BLUE, LEDout);
                                    LEDout = FALSE;
                                  // update brightness
                                    if (RGBLED->bright() != RGBLEDold->bright())
                                      {
                                        #if (USE_MQTT > OFF)
                                            if (errMQTT == MD_OK)
                                              {
                                                valRGBBright = (RGBLED->bright());    // RGB-LED col24
                                                    //SVAL(topRGBBright, valRGBBright);
                                                errMQTT = (int8_t) mqtt.publish(topRGBBright.c_str(), (uint8_t*) valRGBBright.c_str(), valRGBBright.length());
                                                    soutMQTTerr(" MQTT publish RGBBright", errMQTT);
                                              }
                                          #endif

                                        #if (USE_WEBSERVER > OFF)
                                            outStr = "SVB1";
                                            outStr.concat(RGBLED->bright());    // RGB-LED col24
                                            pmdServ->updateAll(outStr);
                                              //STXT(outStr);
                                          #endif
                                      }
                                  // update color
                                    if (RGBLED->col24() != RGBLEDold->col24())
                                      {
                                        #if (USE_MQTT > OFF)
                                            colToHexStr(cMQTT, RGBLED->col24());
                                            valRGBCol = cMQTT;    // RGB-LED col24
                                                //SVAL(topRGBCol, valRGBCol);
                                            errMQTT = (int8_t) mqtt.publish(topRGBCol.c_str(), (uint8_t*) valRGBCol.c_str(), valRGBCol.length());
                                                soutMQTTerr(" MQTT publish RGBCol", errMQTT);
                                          #endif

                                        #if (USE_WEBSERVER > OFF)
                                            outStr = "SVC1";
                                            colToHexStr(ctmp8, RGBLED->col24());
                                            outStr.concat(ctmp8);    // RGB-LED col24
                                            pmdServ->updateAll(outStr);
                                                //STXT(outStr);
                                          #endif
                                      }
                                          //S2HEXVAL(" outcycle update RGBLEDold RGBLED ", RGBLEDold->toInt(), RGBLED->toInt());
                                    RGBLEDold->fromInt(RGBLED->toInt());
                                          //S2HEXVAL(" outcycle update RGBLEDold RGBLED ", RGBLEDold->toInt(), RGBLED->toInt());
                                }
                            }
                        #endif
                      break;
                    case 18: // FAN_PWM
                      #if (USE_FAN_PWM > OFF)
                          if (fanT.TOut())
                            {
                                  //STXT(" # Out FAN");
                              fanT.startT();
                              if (fanIdx++ > 1000)
                                {
                                  fanIdx = 0;
                                  for (uint8_t i=0 ; i < USE_FAN_PWM ; i++)
                                    {
                                      valFanPWM[i] += 1;
                                      if (valFanPWM[i] >= 255) { valFanPWM[i] = 0; } // -50%
                                    }
                                  #if (USE_POTICTRL_FAN > 0)
                                      valFan[INP_CNT_FAN_1] = map((long) -inpValADC[INP_POTI_CTRL], -4095, 0, 0, 255);
                                          //SVAL(" fan poti ", inpValADC[INP_POTI_CTRL]);
                                          //SVAL(" fan cnt ", valFan[INP_CNT_FAN_1]);
                                      valFanPWM[0] = valFan[INP_CNT_FAN_1];
                                      #if (USE_POTICTRL_FAN > 1)
                                          valFan[INP_CNT_FAN_2] = map((long) -inpValADC[INP_POTI_CTRL], -4095, 0, 0, 255);
                                          valFanPWM[1] = valFan[INP_CNT_FAN_2];
                                        #endif
                                    #endif
                                  ledcWrite(PWM_FAN_1, valFanPWM[0]);
                                  #if (USE_FAN_PWM > 1)
                                      ledcWrite(PWM_FAN_2, valFanPWM[1]);
                                    #endif
                                }
                              #if (USE_MQTT > OFF)
                                  if (errMQTT == MD_OK)
                                    {
                                    }
                                #endif
                              #if (USE_WEBSERVER > OFF)
                                #endif
                            }
                        #endif
                      break;
                    case 19: // digital output
                      #if (USE_GEN_DIG_OUT > OFF)
                          if (testLED != testLEDold)
                            {
                              valtestLED = testLED;
                              #if (DIG_OUT1_INV > OFF)
                                  digitalWrite(PIN_GEN_DIG_OUT1, !testLED);
                              #else
                                  digitalWrite(PIN_GEN_DIG_OUT1, testLED);
                                #endif
                              #if (USE_MQTT)
                                  if (errMQTT == MD_OK)
                                    {
                                      errMQTT = (int8_t) mqtt.publish(toptestLED.c_str(), (uint8_t*) valtestLED.c_str(),  valtestLED.length());
                                          soutMQTTerr(" MQTT subscribe testLED", errMQTT);
                                    }
                                #endif
                              #if (USE_WEBSERVER > OFF)
                                #endif
                              testLEDold = testLED;
                            }
                        #endif
                    case 20: // WEBSERVER
                      #if (USE_WEBSERVER > OFF)
                          if (newClient)
                            {
                              // EL_TSLIDER
                              #if (USE_WS2812_LINE_OUT > OFF)
                                  outStr = "SVB2";
                                  outStr.concat(line2812[0]->bright());    // RGB-LED col24
                                  pmdServ->updateAll(outStr);
                                  STXT(outStr);
                                #endif
                              #if (USE_WS2812_MATRIX_OUT > OFF)
                                  outStr = "SVB3";
                                  md_LEDPix24* ppix = outM2812[0].text->pix24;
                                  outStr.concat(ppix->bright());           // RGB-LED col24
                                  pmdServ->updateAll(outStr);
                                  STXT(outStr);                              outStr = "SVB3";
                                #endif
                                  //tmpStr = "SVB4";
                                  //tmpStr.concat(line2812[0]->bright());    // RGB-LED col24
                                  //pmdServ->updateAll(tmpStr);

                              // EL_TCOLOR
                              #if (USE_WS2812_LINE_OUT > OFF)
                                  outStr = "SVC2";
                                  colToHexStr(ctmp, line2812[0]->col24());
                                  outStr.concat(ctmp);    // RGB-LED col24
                                  pmdServ->updateAll(outStr);
                                  //STXT(outStr);
                                #endif
                              #if (USE_WS2812_MATRIX_OUT > OFF)
                                  outStr = "SVC3";
                                  ppix = outM2812[0].text->pix24;
                                  colToHexStr(ctmp, ppix->col24());
                                  outStr.concat(ctmp);    // RGB-LED col24
                                  pmdServ->updateAll(outStr);
                                  STXT(outStr);
                                  outStr = "SVC4";
                                  ppix = outM2812[0].bmpB->pix24;
                                  colToHexStr(ctmp, ppix->col24());
                                  outStr.concat(ctmp);    // RGB-LED col24
                                  pmdServ->updateAll(outStr);
                                  STXT(outStr);
                                #endif

                              newClient = false;
                            }
                        #endif
                      break;
                    default:
                      outpIdx = 0;
                      break;
                  }
              }
          #endif
      // --- Display -------------------
        #if (USE_DISP > 0)
          if (dispT.TOut())    // handle touch output
            {
              dispIdx++;
                    //SOUT(" #"); SOUT(millis()); SOUT(" Display dispIdx ... "); SOUT(dispIdx); SOUT(" ");
                    //heapFree("+disp");
              #ifdef RUN_OLED_TEST
                  oled.clearBuffer();
                  switch (dispIdx)
                    {
                      case 0:
                        oled.prepare();
                        oled.box_frame();
                        break;
                      case 1:
                        oled.disc_circle();
                        oled.sendBuffer();
                        break;
                      case 2:
                        oled.r_frame_box();
                        break;
                      case 3:
                        oled.prepare();
                        oled.string_orientation();
                        dispIdx--;
                        break;
                      case 4:
                        oled.line();
                        break;
                      case 5:
                        oled.triangle();
                        break;
                      case 6:
                        oled.bitmap();
                        break;
                      default:
                        break;
                    }
                  if (++dispIdx > 6) { dispIdx = 0; }
                  oled.sendBuffer();
                #endif // RUN_OLED_TEST
                    //heapFree("+dispIdx");
              switch (dispIdx)
                {
                case 1:  // BME280/680 temp, humidity, pressure, (680 gas resistance)
                  #if ( USE_BME280_I2C > OFF )
                      outStr = "";
                      for (uint8_t i = 0; i < 3 ; i++)
                        {
                          switch (i)
                            {
                              case 0:  // BME280 temperature
                                  tmpval16 = (int16_t) (bmeT+ 0.5);
                                  outStr.concat(tmpval16);
                                  #if (USE_TOUCHSCREEN > OFF)
                                      //strncpy(pVal_T280V, outStr.c_str(),MENU_VALMAX);
                                      sprintf(pVal_T280V, "%i",tmpval16);
                                      pitT280V->wrText();
                                      //SVAL(" disp BMET280V ", outStr);
                                    #endif
                                  outStr.concat("° ");
                                break;
                              case 1:  // BME280 air pressure
                                  tmpval16 = (int16_t) bmeP;
                                  outStr.concat(tmpval16);
                                  #if (USE_TOUCHSCREEN > OFF)
                                      //strncpy(pVal_P280V, outStr.c_str(),MENU_VALMAX);
                                      sprintf(pVal_P280V, "%i",tmpval16);
                                      pitP280V->wrText();
                                      //SVAL(" disp BMET280V ", outStr);
                                    #endif
                                  outStr.concat("mb ");
                                break;
                              case 2:   // BME280 humidity
                                  tmpval16 = (int16_t) bmeH;
                                  outStr.concat(tmpval16);
                                  #if (USE_TOUCHSCREEN > OFF)
                                      //strncpy(pVal_H280V, outStr.c_str(),MENU_VALMAX);
                                      sprintf(pVal_H280V, "%i",tmpval16);
                                      pitH280V->wrText();
                                      //SVAL(" disp BMET280V ", outStr);
                                    #endif
                                  outStr.concat("% ");
                                break;
                              default:
                                break;
                            }
                        }
                      dispText(outStr , 0, 3, outStr.length());
                              //STXT(outStr);
                    #endif
                  #if ( USE_BME680_I2C > OFF )
                      outStr = "";
                      for (uint8_t i = 0; i < 4 ; i++)
                        {
                          switch (i)
                            {
                              case 0:  // BME680 temperature
                                  tmpval16 = (int16_t) (bme680T+ 0.5);
                                  outStr.concat(tmpval16);
                                  #if (USE_TOUCHSCREEN > OFF)
                                      //strncpy(pVal_T680V, outStr.c_str(),MENU_VALMAX);
                                      sprintf(pVal_T680V, "%i",tmpval16);
                                      pitT680V->wrText();
                                      //SVAL(" disp BMET280V ", outStr);
                                    #endif
                                  outStr.concat("° ");
                                break;
                              case 1:  // BME280 air pressure
                                  tmpval16 = (int16_t) bme680P;
                                  outStr.concat(tmpval16);
                                  #if (USE_TOUCHSCREEN > OFF)
                                      //strncpy(pVal_P280V, outStr.c_str(),MENU_VALMAX);
                                      sprintf(pVal_P680V, "%i",tmpval16);
                                      pitP680V->wrText();
                                      //SVAL(" disp BMET280V ", outStr);
                                    #endif
                                  outStr.concat("mb ");
                                break;
                              case 2:   // BME280 humidity
                                  tmpval16 = (int16_t) bme680H;
                                  outStr.concat(tmpval16);
                                  #if (USE_TOUCHSCREEN > OFF)
                                      //strncpy(pVal_H280V, outStr.c_str(),MENU_VALMAX);
                                      sprintf(pVal_H680V, "%i",tmpval16);
                                      pitH680V->wrText();
                                      //SVAL(" disp BMET280V ", outStr);
                                    #endif
                                  outStr.concat("% ");
                                break;
                              case 3:   // BME280 gas resistance
                                  tmpval16 = (int16_t) bme680G;
                                  outStr.concat(tmpval16);
                                  #if (USE_TOUCHSCREEN > OFF)
                                      //strncpy(pVal_H680G, outStr.c_str(),MENU_VALMAX);
                                      sprintf(pVal_G680V, "%i",tmpval16);
                                      pitG680V->wrText();
                                      //SVAL(" disp BMET680G ", outStr);
                                    #endif
                                  outStr.concat("% ");
                                break;
                              default:
                                break;
                            }
                        }
                      dispText(outStr , 0, 3, outStr.length());
                              //STXT(outStr);
                    #endif
                	break;
                case 2:  // CCS811_I2C
                    dispIdx++;
                  //break;
                case 3:  // INA3221_I2C 3x U+I measures
                    dispIdx++;
                  //break;
                case 4:  // temp sensor
                    #if (USE_DS18B20_1W_IO > OFF)
                        outStr = "";
                        outStr = getDS18D20Str();
                        dispText(outStr ,  0, 4, outStr.length());
                      #endif
                  break;
                case 5:  // gas sensor MQ135
                  #if (USE_MQ135_GAS_ANA > OFF)
                      //_tmp = showTrafficLight(gasValue, gasThres); // -> rel to defined break point
                      outStr = "CO2 ";
                      outStr.concat(gasValue);
                      outStr.concat("  ");
                      dispText(outStr, 17, 3, outStr.length());
                            //STXT(outStr);
                    #endif
                  break;
                case 6:  // gas sensor MQ3 alcohol
                  #if (USE_MQ3_ALK_ANA > OFF)
                      tmpval16 = alk;
                      outStr = "  a ";
                      outStr.concat(alk);
                      outStr.concat("  ");
                      dispText(outStr, 1, 1, outStr.length());
                          //STXT(outStr);
                      #if (USE_MQTT > OFF)
                          valMQ3alk = tmpval16;
                              //SVAL(topMQ3alk, valMQ3alk);
                          errMQTT = (int8_t) mqtt.publish(topMQ3alk.c_str(), (uint8_t*) valMQ3alk.c_str(), valMQ3alk.length());
                              //soutMQTTerr(" MQTT publish MQ3alk", errMQTT);
                        #endif
                    #endif
                  break;
                case 7:  // light sensor
                  #if (USE_PHOTO_SENS_ANA > OFF)
                      outStr = "          ";
                      //outStr.concat(photoVal[0].getVal());
                      outStr.concat("  ");
                      dispText(outStr, 12, 4, outStr.length());
                          //STXT(outStr);
                    #endif
                  break;
                case 8:  // poti,
                    #if (USE_POTI_ANA > OFF)
                        tmpval16 = potifScal[0].scale((float) poti[0]);
                        #if (USE_MQTT > OFF)
                            //sprintf(tmpMQTT, "%s%s", MQTT_DEVICE, POTI1_MQTT);
                          #endif
                        outStr = "  P ";
                        outStr.concat(tmpval16);
                        outStr.concat("  ");
                        dispText(outStr, 15, 1, outStr.length());
                              //STXT(outStr);
                      #endif
                    break;
                case 9:  // voltage 5V
                    #if (USE_VCC_ANA > OFF)
                        //tmpval16 = vcc50;
                        outStr = "  V ";
                        //outStr.concat(tmpval16);
                        outStr.concat(vcc50f);
                        outStr.concat("  ");
                        dispText(outStr, 1, 2, outStr.length());
                              //STXT(outStr);
                      #endif
                    #if (USE_ACS712_ANA > OFF)
                        //tmpval16 = i712[0];
                        outStr = "  I ";
                        outStr.concat(i712[0]);
                        //outStr.concat(tmpval16);
                        outStr.concat("  ");
                        dispText(outStr, 15, 2, outStr.length());
                              //STXT(outStr);
                      #endif
                   	break;
                case 10: // voltage 3.3V
                    #if (USE_VCC_ANA > OFF)
                        //tmpval16 = vcc50;
                        outStr = "  V ";
                        //outStr.concat(tmpval16);
                        outStr.concat(vcc33f);
                        outStr.concat("  ");
                        dispText(outStr, 1, 2, outStr.length());
                              //STXT(outStr);
                      #endif
                    //dispIdx++;
                  break;
                case 11: // ACS712_ANA
                    dispIdx++;
                  //break;
                case 12: // k-type sensor
                  #if (USE_TYPE_K_SPI > OFF)
                      outStr = "";
                      outStr = "TK1 ";
                      outStr.concat(tk1Val);
                      outStr.concat("°");
                      //dispText(outStr ,  0, 1, outStr.length());
                      #if (USE_TYPE_K_SPI > 1)
                          //outStr = "";
                          outStr.concat(" TK2 ");
                          outStr.concat(tk2Val);
                          outStr.concat("° ");
                        #endif
                      dispText(outStr ,  0, 2, outStr.length());
                      outStr.concat(" (");
                      outStr.concat(tk1ValRef);
                      #if (USE_TYPE_K > 1)
                          outStr.concat("° / ");
                          outStr.concat(tk2ValRef);
                        #endif
                      outStr.concat("°)");
                              STXT(outStr);
                    #endif
                  break;
                case 13: // counter values
                  #if (USE_PWM_INP > OFF)
                        outStr = "              ";
                        dispText(outStr ,  0, 1, outStr.length());
                        outStr = "pwm ";
                        for (uint8_t i = 0; i < USE_PWM_INP ; i++ )
                          {
                            outStr += " ";
                            outStr += (String) pwmInVal[i].lowVal;
                            outStr += " ";
                            outStr += (String) pwmInVal[i].highVal;
                                  //SOUT("/"); SOUT(cntErg[i].pulsCnt); SOUT(" "); SOUT("/"); SOUT(cntErg[i].usCnt); SOUT(" ");
                          }
                        //STXT(outStr);
                        //dispText(outStr ,  0, 1, outStr.length());
                    #endif
                  #if (USE_CNT_INP > OFF)
                        outStr = "              ";
                        dispText(outStr ,  0, 2, outStr.length());
                        outStr = "f";
                        SOUT(millis());
                        for (uint8_t i = 0; i < USE_CNT_INP ; i++ )
                          {
                            outStr = "   f";
                            outStr += (String) i;
                            outStr += " = ";
                            outStr += (String) cntErg[i].freq;
                            outStr += "     ";
                            SOUT(outStr); SOUT(" Hz"); SOUT("/"); SOUT(" usCnt "); SOUT(cntErg[i].usCnt); SOUT(" ");
                            dispText(outStr ,  0, i+1, outStr.length());
                          }
                        SOUTLN();
                        //SOUT(outStr); SOUT(" "); //SOUT(valFanPWM[0]); SOUT(" ");
                      #ifdef USE_MCPWM
                          outStr = "              ";
                          dispText(outStr ,  0, 0, outStr.length());
                          outStr = "LH ";
                          outStr += (String) cntLowPulse[0]; outStr += (" ");
                          outStr += (String) cntHighPulse[0];
                                    SOUT(outStr); SOUT(" ");
                          dispText(outStr ,  0, 2, outStr.length());
                        #endif
                    #endif
                  break;
                case 14: // digital inputs
                    #if (USE_DIG_INP > OFF)
                      outStr = "";
                      for (uint8_t i = 0 ; i < USE_GEN_SW_INP; i++)
                        {
                          //S2VAL(" SWD", i, valInpDig[i]);
                          tmpStr = "SWD";
                          tmpStr.concat(i);
                          tmpStr.concat(valInpDig[i]);
                          outStr.concat(valInpDig[i]);
                          outStr.concat(" ");
                          // send to websocket
                          #if (USE_WEBSERVER > OFF)
                              pmdServ->updateAll(tmpStr);
                            #endif
                          #if (USE_MQTT > OFF)
                            #endif
                        }
                      dispText(outStr , 17, 3, outStr.length());
                            //STXT(outStr);
                    #endif
                  #if (USE_CTRL_POTI)
                      //SVAL("POT ", inpValADC[INP_POTI_CTRL]);
                    #endif
                  break;
                case 15: // ESPHALL
                    #if (USE_ESPHALL > OFF)
                        int32_t valHall = 0;
                      #endif
                    dispIdx++;
                  //break;
                case 16: // MCPWM
                    dispIdx++;
                  //break;
                case 17: // RGB-LED
                    dispIdx++;
                  //break;
                case 18: // FAN_PWM
                    dispIdx++;
                  //break;
                case 19: // digital output
                    dispIdx++;
                  //break;
                case 20: // webserver nu
                    #if (USE_WIFI > OFF)
                        outStr = WiFi.localIP().toString();
                    #else
                        outStr = "IP Offline";
                      #endif
                    dispText(outStr ,  0, 4, outStr.length());
                  break;
                case 21: // WS2812 lines
                    #if ((USE_WS2812_MATRIX_OUT > OFF) || (USE_WS2812_LINE_OUT > OFF))
                        outStr = "              ";
                        dispText(outStr ,  0, 0, outStr.length());
                        //outStr = "LED ";
                        outStr = "";
                            //outStr += (String) ws2812_Mcnt; outStr += " ";
                        ws2812_Mv = millis() - ws2812_Malt; // dispT.getTout();
                        ws2812_Malt = millis();
                        if (ws2812_Mcnt > 0)
                          {
                            ws2812_Mv = ws2812_Mv / ws2812_Mcnt;
                            ws2812_Mcnt = 0;
                          }
                        outStr += (String) ws2812_Mv;
                        outStr += ("ms");
                              //SOUT((uint32_t) millis()); SOUT(" ");
                                  //SOUT(outStr); SOUT(" ");
                        dispText(outStr ,  0, 0, outStr.length());
                      #endif
                  break;
                default: // system
                    usTmp      = micros();
                    usPerCycle = (usTmp - usLast) / anzUsCycles;
                    usLast      = usTmp;
                      //SOUT(usLast); SOUT(" "); SOUT(micros()); SOUT(" "); SOUTLN(usPerCycle);
                      //outStr = "          ";
                      //dispText(outStr ,  22, 4, outStr.length());
                      //outStr = "";
                      //outStr.concat((unsigned long) usPerCycle);
                      //outStr.concat("us    ");
                      //dispText(outStr ,  22, 4, outStr.length());
                      //SOUTLN(); SOUT(usLast); SOUT(" ms/cyc "); SOUT((uint32_t) usPerCycle); SOUT(" ");
                    anzUsCycles = 0ul;
                    dispIdx = 0;
                  break;
                }
              dispT.startT();
                      //heapFree("-dispIdx");
                      //STXT("  disp end ");

              #ifdef USE_STATUS
                  dispStatus("");
                #endif
            }
          #endif // defined(DISP)
      // --- system control --------------------------------
        #if (USE_LED_BLINK_OUT > 0)
            #if (PIN_BOARD_LED > NC)
                if (ledT.TOut())    // handle touch output
                  {
                    ledT.startT();
                    if (sysLED == TRUE)
                        {
                          digitalWrite(PIN_BOARD_LED, OFF);
                          sysLED = OFF;
                        }
                      else
                        {
                          digitalWrite(PIN_BOARD_LED, ON);
                          sysLED = ON;
                        }
                  }
              #endif
          #endif
        if (firstrun == true)
            {
              String taskMessage = "loop task running on core ";
              taskMessage = taskMessage + xPortGetCoreID();
              STXT(taskMessage);
              usLast = micros();
              firstrun = false;
            }
        anzUsCycles++;
          //usleep(20);
          //delay(1);
    }
// ----------------------------------------------------------------
// --- subroutine and drivers ----------------
// ----------------------------------------------------------------
  // --- system --------------------------
    // --- heap output
      void heapFree(const char* text)
        {
          uint32_t tmp32 = ESP.getFreeHeap();
          //uint32_t tmp32 = heap_caps_get_free_size(MALLOC_CAP_8BIT | MALLOC_CAP_32BIT);
          SVAL(text, tmp32);
        }
  // --- user output ---------------------
    // --- display
      void startOLED()
        {
          #if (USE_OLED_I2C > OFF)
              #ifdef USE_STATUS
                statOut[OLED1_MAXCOLS] = 0;  // limit strlen
                #endif
              oled1.begin((uint8_t) OLED1_MAXCOLS, (uint8_t) OLED1_MAXROWS);
              STXT(" oled1 gestartet");
              #if (USE_OLED_I2C > 1)
                  oled2.begin((uint8_t) OLED2_MAXCOLS, (uint8_t) OLED2_MAXROWS);
                #endif
            #endif
        }
      void startTFT_TOUCH()
        {
          #if (USE_TFT > 0)
              #if !(DISP_TFT ^ MC_UO_TFT1602_GPIO_RO)
                  mlcd.start(plcd);
                #endif
              #if (USE_TOUCHSCREEN > OFF)
                  mdtouch.start(TOUCH_ROTATION, DISP_BCOL);
                      #if (DEBUG_MODE >= CFG_DEBUG_DETAIL)
                        STXT(" .. touch started ");
                      #endif

                  configMenu(pmenuItems);
                  mdmenu.init(pmenuItems, pitTitle, pitStatus);
                  mdmenu.begin(TRUE);
                  //
                  //mdmenu.wrStatus();
                  //mdmenu.wrTitle();
                  //mdtouch.doCalibration();
                #endif
            #endif
        }
    // --- ATSmartHome ---------------------
      #if (USE_MD_ATSMARTHOME > OFF)
          void runSmartHome()
            {

            }
          void initSmartHome()
            {
              pinMode(TFT_LED,OUTPUT);
              int heap = ESP.getFreeHeap();
              SVAL(" RAM available ",heap);
              //touch.begin();
              //tevent.setResolution(tft.width(),tft.height());
              mdTouchEv.setDblClick(200);
              mdTouchEv.registerOnTouchLong(onLong);
              mdTouchEv.registerOnTouchClick(onClick);
              mdTouchEv.registerOnTouchSwipe(onSwipe);
              dsp.begin(1,TOUCH_ROTATION);
              dsp.display(true);
              //start flash file system
                  if (SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED))
                    { Serial.println(F("SPIFFS loaded")); }
                  //if (SPIFFS.begin()) Serial.println(F("SPIFFS loaded"));
              #if (USE_ATWIFI > OFF)
                  dsp.registerOnResultChange(sendData);
                  webserver.registerOnResultChange(sendData);
                  dsp.registerOnSystemChanged(systemChanged);
                  initWiFi();
                  connectWlan("","");
                  //delay(100);
                  if (!initESPNow()) dsp.setStatus("ESP NOW failed");
                #endif
              if (!database.readDevices()) STXT("Can not read devices");
              STXT(" devices gelesen ");
              if (!database.readConfig()) Serial.println("Can not read widget config");
              //if (!database.readSetup()) Serial.println("Can not read setup");
              STXT(" config gelesen ");
              //stp = database.getSetup();
              STXT(" getSetup pointer  gefunden ");
              mdTouchEv.setDblClick(200);
              mdTouchEv.registerOnTouchLong(onLong);
              mdTouchEv.registerOnTouchClick(onClick);
              mdTouchEv.registerOnTouchSwipe(onSwipe);
              dsp.showCurrentPage();
              #if (USE_ATWIFI > OFF)
                  if (stp->useWlan) connectWlan(stp->SSID, stp->password, stp->NTPserver);
                  srv.on("/",handleRoot);
                  srv.onNotFound(handleNotFound);
                  webserver.begin();
                  set callback funktion
                  rf.setCallback(rfCallback);
                  // initialize receiver
                  rf.initReceiver(RCPIN);
                  rf.limitProtocols("[\"arctech_switch\"]");
                #endif
            }
          void onClick(TS_Point p)
            {
              STXT("Click");
              dsp.onClick(p);
            }
          void onLong(TS_Point p)
            {
              dsp.onLongClick(p);
            }
          void onSwipe(uint8_t direction)
            {
              dsp.onSwipe(direction);
            }
          void handleRoot()
            {
              #if (USE_ATWIFI > OFF)
                  webserver.handleRoot(stp->refresh);
                #endif
            }
          void handleNotFound()
            {
              #if (USE_ATWIFI > OFF)
                  webserver.handleNotFound();
                #endif
            }
          //this function is called if system setup has changed
          void systemChanged()
            {
              #if (USE_ATWIFI > OFF)
                  ATSETUP * stp=database.getSetup();
                  if (stp->useWlan)
                    {
                      connectWlan(stp->SSID,stp->password,stp->NTPserver);
                    }
                  else
                    {
                      disconnectWlan();
                    }
                #endif
            }
          // callback function. It is called on successfully received and parsed rc signal
          void rfCallback(const String &protocol, const String &message, int status,
                          size_t repeats, const String &deviceID)
            {
              /*
                Serial.print("RF signal arrived [");
                Serial.print(protocol);  // protocoll used to parse
                Serial.print("][");
                Serial.print(deviceID);  // value of id key in json message
                Serial.print("] (");
                Serial.print(status);  // status of message, depending on repeat, either:
                                       // FIRST   - first message of this protocoll within the
                                       //           last 0.5 s
                                       // INVALID - message repeat is not equal to the
                                       //           previous message
                                       // VALID   - message is equal to the previous message
                                       // KNOWN   - repeat of a already valid message
                Serial.print(") ");
                Serial.print(message);  // message in json format
                Serial.println();
                */
              // check if message is valid and process it
              #if (USE_ATWIFI > OFF)
                  if (status == FIRST)
                    {
                      Serial.println("**************************************************");
                      Serial.print("Valid message: [");
                      Serial.print(protocol);
                      Serial.print("] ");
                      Serial.print(message);
                      Serial.print(" ");
                      Serial.print(deviceID);
                      Serial.println();
                      Serial.println("**************************************************");
                    }
                #endif
            }
        #endif
      void clearDisp()
        {
          #if (USE_DISP > 0)
              #if (USE_OLED_I2C > OFF)
                  oled1.clear();
                  #if (USE_OLED_I2C > 1)
                      oled2.clear();
                    #endif
                #endif
            #endif
        }
      void dispStatus(String msg, int16_t mstout)
        {
          #ifdef USE_STATUS
              size_t statLen = msg.length();
              bool   doIt    = false;
              bool   info    = false;

              //STXT(" dispStatus .. ");
              if (statLen)
                {
                  #if (USE_OLED_I2C > OFF)
                      if ( statLen > OLED1_MAXCOLS)
                        {
                          msg.remove(OLED1_MAXCOLS);
                        }
                    #endif
                  statOn = true;
                  statT.startT();
                  doIt = true;    // output statOut
                  statT.startT();
                }
              else // empty input
                {
                  if (statOn && statT.TOut())
                    statOn = false;
                }
              if (!statOn) // disp def val and actual time
                {
                  if (statN.TOut())
                    {
                      statN.startT(6000);
                      #if (USE_NTP_SERVER > OFF)
                          sprintf(statOut,"%02d.%02d. %02d:%02d", day(), month(), hour(), minute());
                          msg = statOut;
                          msg.concat("  ");
                          msg.concat((unsigned long) usPerCycle);
                          msg.concat("us");
                          info = true;
                          #if (USE_TOUCHSCREEN > OFF)
                              pmdmenu->wrStatus(msg, 1);
                              doIt = false;
                            #endif
                        #endif
                    }
                }
              if (doIt)
                {
                  #if (USE_TOUCHSCREEN > OFF)
                      //SHEXVAL(" dispStatus vor wrStatus pmdmenu ", (uint32_t) pmdmenu);
                      pmdmenu->wrStatus(msg, mstout);
                      //SHEXVAL(" dispStatus nach wrStatus pmdmenu ",(uint32_t) pmdmenu);
                    #endif
                  #if (USE_MD_ATSMARTHOME > OFF)
                    //touch.runTouch(outBuf);
                    #endif
                  #if (USE_OLED_I2C > OFF)
                      #if ( OLED1_STATUS > OFF )
                          oled1.wrStatus(msg);
                        #endif
                      #if ( USE_STATUS2 > OFF)
                          oled2.wrStatus(msg);
                        #endif
                    #endif
                  #if (USE_TFT > 0)
                      #if !(DISP_TFT ^ MC_UO_TFT1602_GPIO_RO)
                          mlcd.wrStatus((char*) statOut);
                        #endif
                              //#if !(DISP_TFT ^ MC_UO_TOUCHXPT2046_AZ)
                                   //if (info)
                                   //  {
                                   //    #if ( USE_BME280_I2C > OFF )
                                   //        outStr[0] = 0;
                                   //        outStr.concat(bmeT.getVal());
                                   //        outStr.concat("° ");
                                   //        outStr.concat(bmeH.getVal());
                                   //        outStr.concat("% ");
                                   //        outStr.concat(bmeP.getVal());
                                   //        outStr.concat("mb  ");
                                   //      #endif
                                   //  }
                                   // outStr.concat((char*) statOut);
                                   // if (info)
                                   // {
                                   //   #if (USE_WEBSERVER > OFF)
                                   //       outStr.concat(" ");
                                   //       outStr.concat(WiFi.localIP().toString());
                                   //     #endif
                                   // }
                                  // #endif
                    #endif // USE_DISP
                  info = false;
                }
              //STXT(" dispStatus end ");
            #endif // USE_STATUS
        }
      void dispStatus(const char* msg, int16_t mstout)
        {
          dispStatus((String) msg, mstout);
        }
      void dispText(String msg, uint8_t col, uint8_t row, uint8_t len)
        {
          #if (USE_DISP > 0)
              #if (USE_OLED_I2C > OFF)
                  oled1.wrText(msg, col, row, len);
                            //SVAL(" dispText oled1 '", msg);
                  #if (USE_OLED_I2C > 1)
                      oled2.wrText(msg, col, row, len);
                            //SVAL(" dispText oled2 '", msg);
                    #endif
                #endif
              #if (USE_TFT > 0)
                  #if !(DISP_TFT ^ MC_UO_TFT1602_GPIO_RO)
                      mlcd.wrText(msg, row, col);
                    #endif
                  #if (USE_TOUCHSCREEN > OFF)
                      mdtouch.wrText(msg, col, row, len);
                    #endif
                  #if (USE_MD_ATSMARTHOME > OFF)
                    //touch.runTouch(outBuf);
                    #endif
                #endif
            #endif
        }
      void dispText(char* msg, uint8_t col, uint8_t row, uint8_t len)
        {
          dispText((String) msg, col, row, len);
            /*
                #if (USE_DISP > 0)
                    #if (USE_OLED1_I2C > OFF)
                        oled1.wrText(msg, col, row, len);
                      #endif
                    #if defined(OLED2)
                        oled2.wrText(msg, col, row, len);
                      #endif
                    #if (USE_TFT > 0)
                        #if !(DISP_TFT ^ MC_UO_TFT1602_GPIO_RO)
                            mlcd.wrText(msg, row, col);
                          #endif
                        #if !(DISP_TFT ^ MC_UO_TOUCHXPT2046_AZ)
                            touch.wrText(msg, col, row, len);
                          #endif
                      #endif
                    #if (USE_TOUCHSCREEN > OFF)
                      #endif
                    #if (USE_MD_ATSMARTHOME > OFF)
                      #endif
                  #endif
              */
        }
    // --- WS2812 LED
      #if (USE_WS2812_LINE_OUT > OFF)
          void initWS2812Line()
            {
              uint32_t i32tmp = 0;
              strip.begin();
              strip.setBrightness(255);
              strip.fill(strip.Color(200,0,0));
              strip.show();
              usleep(400000);
              i32tmp = COL24_2812_L1 + (BRI_2812_L1 << 24);
              SHEXVAL(" strip BriCol org ", i32tmp);

              line2812[0] = new md_LEDPix24();
              line2812[1] = new md_LEDPix24(i32tmp);
                  //(BRI_2812_L1,
                  // (uint8_t) ((COL24_2812_L1 & 0x00ff0000) >> 16),
                  // (uint8_t) ((COL24_2812_L1 & 0x0000ff00) >> 8),
                  // (uint8_t) ( COL24_2812_L1 & 0x000000ff)
                  //);
                  //SOUT(" line[0]/[1] vor "); SOUTHEX((uint32_t)*line2812[0]); SOUT(" "); SOUTHEX((uint32_t)*line2812[1]);
              *line2812[0] = *line2812[1];
                  //SOUT(" line[0]/[1] nach "); SOUTHEX((uint32_t)*line2812[0]); SOUT(" "); SOUTHEXLN((uint32_t)*line2812[1]);
                  //SOUT(" line[0].bright() "); SOUTHEX(line2812[0]->bright());
                  //SOUT(" line[0].col24() "); SOUTHEXLN(line2812[0]->col24());
                  //strip.setBrightness(line2812[0]->bright());
              strip.setBrightness(5);
              strip.fill(line2812[0]->col24());
                  //strip.fill(strip.Color((uint8_t) ((line2812[0]->col24() & 0x00FF0000u) >> 16),
                  //                       (uint8_t) ((line2812[0]->col24() & 0x0000FF00u) >> 8 ),
                  //                       (uint8_t)  (line2812[0]->col24() & 0x000000FFu) ) );
              strip.show();
              STXT("ok");
            }
        #endif
      #if (USE_WS2812_MATRIX_OUT > OFF)
          void initWS2812Matrix()
            {
              md_LEDPix24* ppix = outM2812[1].bmpB->pix24;
                //SOUT(" p_outM 0/1 "); SOUTHEX((uint32_t) &outM2812[0]);            SOUT(" / "); SOUTHEXLN ((uint32_t) &outM2812[1]);
                //SOUT(" p_text 0/1 "); SOUTHEX((uint32_t) &outM2812[0].text);       SOUT(" / "); SOUTHEXLN ((uint32_t) outM2812[1].text);
                //SOUT(" p_pix24 0/1 "); SOUTHEX((uint32_t) outM2812[0].text->pix24); SOUT(" / "); SOUTHEXLN ((uint32_t) outM2812[1].bmpB->pix24);
              ppix->col16(COL24_2812_BM1);
              ppix->bright(BRI_2812_BM1);
              outM2812[1].bmpB->bmp_num = MD_BITMAP_SMILY;
              outM2812[0].bmpB = outM2812[1].bmpB;

              ppix = outM2812[1].text->pix24;
              ppix->col16(COL24_2812_M1);
              ppix->bright(BRI_2812_M1);
              outM2812[1].text->text = (char*) text2812;
              outM2812[0].text = outM2812[1].text;

              ppix = outM2812[1].bmpE->pix24;
              ppix->col16(COL24_2812_BM1);
              ppix->bright(BRI_2812_BM1);
              outM2812[1].bmpE->bmp_num = MD_BITMAP_SMILY;
              outM2812[0].bmpE = outM2812[1].bmpE;
                //memcpy((char*) &outM2812[0], (char*) &outM2812[1], sizeof(outM2812[1]));
                //SOUT(" outM bmpB 0/1 ");
                //SOUTHEX(outM2812[0].bmpB->bmp_num); SOUT(" / "); SOUTHEX(outM2812[1].bmpB->bmp_num); SOUT(" - ");
                //SOUTHEX((uint32_t) outM2812[0].bmpB->pix24);   SOUT(" / "); SOUTHEXLN ((uint32_t) outM2812[1].bmpB->pix24);
                //SOUTHEX(*outM2812[0].bmpB->pix24);  SOUT(" / "); SOUTHEXLN(*outM2812[1].bmpB->pix24);
                //SOUT(" outM text 0/1 ");
                //SOUT(outM2812[0].text->text);       SOUT(" / "); SOUT(outM2812[1].text->text); SOUT(" - ");
                //SOUTHEX((uint32_t) outM2812[0].text->pix24);   SOUT(" / "); SOUTHEXLN((uint32_t) outM2812[1].text->pix24);
                //SOUTHEX(*outM2812[0].text->pix24);  SOUT(" / "); SOUTHEXLN(*outM2812[1].text->pix24);
                //SOUT(" outM bmpE 0/1 ");
                //SOUTHEX(outM2812[0].bmpE->bmp_num); SOUT(" / "); SOUTHEX(outM2812[1].bmpE->bmp_num); SOUT(" - ");
                //SOUTHEX(*outM2812[0].bmpE->pix24);  SOUT(" / "); SOUTHEXLN(*outM2812[1].bmpE->pix24);

                //SOUT(" p_outM 0/1 "); SOUTHEX((uint32_t) &outM2812[0]);            SOUT(" / "); SOUTHEXLN ((uint32_t) &outM2812[1]);
                //SOUT(" p_text 0/1 "); SOUTHEX((uint32_t) &outM2812[0].text);       SOUT(" / "); SOUTHEXLN ((uint32_t) outM2812[1].text);
                //SOUT(" p_pix24 0/1 "); SOUTHEX((uint32_t) outM2812[0].text->pix24); SOUT(" / "); SOUTHEXLN ((uint32_t) outM2812[1].bmpB->pix24);

              usleep(10000);
                //outM2812[0].text = outM2812[1].text;
                //outM2812[0].bmpE = outM2812[1].bmpE;
              matrix_1.begin();
              usleep(50000);
              matrix_1.display_boxes();
              usleep(1500000);
                //matrix_1.start_scroll_task((scroll2812_t*) &outM2812, &posM2812);

              SOUT(" start = "); SOUT(posM2812);
              int16_t tmp = (strlen(text2812) * (uint8_t) COLCHAR_2812 + (uint8_t) OFFEND_2812_M1);
              matrix_1.start_scroll_matrix(  (scroll2812_t*) outM2812, &posM2812, - tmp);
            }
        #endif
    // --- RGB LED
      #if (USE_RGBLED_PWM > OFF)
          void initRGBLED()
            {
              // RGB red
                pinMode(PIN_RGB_RED, OUTPUT);
                ledcSetup(PWM_RGB_RED,    PWM_LEDS_FREQ, PWM_LEDS_RES);
                ledcAttachPin(PIN_RGB_RED,   PWM_RGB_RED);
                ledcWrite(PWM_RGB_RED, 255);
                //STXT(" LED rot");
                usleep(5000);
                ledcWrite(PWM_RGB_RED, 0);
              // RGB green
                pinMode(PIN_RGB_GREEN, OUTPUT);
                ledcSetup(PWM_RGB_GREEN,  PWM_LEDS_FREQ, PWM_LEDS_RES);
                ledcAttachPin(PIN_RGB_GREEN, PWM_RGB_GREEN);
                ledcWrite(PWM_RGB_GREEN, 255);
                //STXT(" LED gruen");
                usleep(5000);
                ledcWrite(PWM_RGB_GREEN, 0);
              // RGB blue
                pinMode(PIN_RGB_BLUE, OUTPUT);
                ledcSetup(PWM_RGB_BLUE,   PWM_LEDS_FREQ, PWM_LEDS_RES);
                ledcAttachPin(PIN_RGB_BLUE,  PWM_RGB_BLUE);
                ledcWrite(PWM_RGB_BLUE, 255);
                //STXT(" LED blau");
                usleep(5000);
                ledcWrite(PWM_RGB_BLUE, 0);
              // init LED
                RGBLED->bright(BRI_RGBLED_1);
                RGBLED->col24 (COL24_RGBLED_1);
                RGBLEDold->bright(0);
                RGBLEDold->col24 (0);
            }
        #endif
    // --- passive buzzer
      #ifdef PLAY_MUSIC
          void playSong(int8_t songIdx)
            {
              if (buzz.setSong(SONG0_LEN,(void*) SONG0_NOTES) == ISOK)
                {
                  #ifdef USE_SONGTASK
                    buzz.playSong();
                  #endif
                }
            }

          void playSong()
            { playSong(0); }

        #endif

    // --- frequency generator
      #if (_USE_OUT_FREQ_PWM > OFF)
          void init_oscillator ()                                              // Inicializa gerador de pulsos
            {
              resolucao = (log (80000000 / oscilador)  / log(2)) / 2 ;                // Calculo da resolucao para o oscilador
              if (resolucao < 1) resolucao = 1;                                       // Resolu�ao m�nima
              // Serial.println(resolucao);                                           // Print
              mDuty = (pow(2, resolucao)) / 2;                                        // Calculo do ciclo de carga 50% do pulso
              // Serial.println(mDuty);                                               // Print

              ledc_timer_config_t ledc_timer = {};                                    // Instancia a configuracao do timer do LEDC

              ledc_timer.duty_resolution =  ledc_timer_bit_t(resolucao);              // Configura resolucao
              ledc_timer.freq_hz    = oscilador;                                      // Configura a frequencia do oscilador
              ledc_timer.speed_mode = LEDC_HIGH_SPEED_MODE;                           // Modo de operacao em alta velocidade
              ledc_timer.timer_num = LEDC_TIMER_0;                                    // Usar timer0 do LEDC
              ledc_timer_config(&ledc_timer);                                         // Configura o timer do LEDC

              ledc_channel_config_t ledc_channel = {};                                // Instancia a configuracao canal do LEDC

              ledc_channel.channel    = LEDC_CHANNEL_0;                               // Configura canal 0
              ledc_channel.duty       = mDuty;                                        // Configura o ciclo de carga
              ledc_channel.gpio_num   = LEDC_HS_CH0_GPIO;                             // Configura GPIO da saida do LEDC - oscilador
              ledc_channel.intr_type  = LEDC_INTR_DISABLE;                            // Desabilita interrup��o do LEDC
              ledc_channel.speed_mode = LEDC_HIGH_SPEED_MODE;                         // Modo de operacao do canal em alta velocidade
              ledc_channel.timer_sel  = LEDC_TIMER_0;                                 // Seleciona timer 0 do LEDC
              ledc_channel_config(&ledc_channel);                                     // Configura o canal do LEDC
            }
        #endif
  // --- user input ----------------------
    // --- keypad
      #if defined(KEYS)
          void startKeys()
            {
              #if (USE_KEYPADSHIELD > OFF)
                  kpad.init(KEYS_ADC);
                #endif // USE_KEYPADSHIELD
            }

          uint8_t getKey()
            {
              #if (USE_KEYPADSHIELD > OFF)
                  return kpad.getKey();
                #else
                  return NOKEY;
                #endif // USE_KEYPADSHIELD
            }
        #endif

    // --- counter
      #if (USE_CNT_INP > OFF)
          static void initGenPCNT()
            {
              /* Initialize PCNT event queue and PCNT functions */
              pcnt_unit_t unit;
              esp_err_t   retESP;
              SOUT("init pcnt ");
              for (uint8_t i = 0 ; i < USE_CNT_INP ; i++)
                {
                  sprintf(cmsg, "pcnt_unit_config[%1i]",i);
                  // Initialize PCNT unit and  queue
                  cntFilt[i] = 0;
                  //retESP = pcnt_unit_config(&config_cnt[i]);
                  logESP(pcnt_unit_config(&config_cnt[i]), cmsg, i);
                  pcnt_evt_queue[i] = xQueueCreate(1, sizeof(pcnt_evt_t));
                  switch (i)
                    {
                      case 0:
                        unit       = (pcnt_unit_t) PCNT0_UNIDX;
                        cntThresh[i]  = (uint16_t)    PCNT0_THRESH0_VAL;
                        cntFakt[i] = (float)    PCNT0_CNT_FAKT;
                        // Configure and enable the input filter
                          pcnt_set_filter_value(unit, PCNT0_INP_FILT);
                          pcnt_filter_enable(unit);
                        // Set threshold 0 and 1 values and enable events to watch //
                          pcnt_set_event_value(unit, PCNT0_EVT_0, PCNT0_THRESH0_VAL);
                          pcnt_event_enable(unit, PCNT0_EVT_0);
                              //pcnt_set_event_value(unit, PCNT_EVT_THRES_1, PCNT1_THRESH0_VAL);
                              //pcnt_event_enable(unit, PCNT_EVT_THRES_1);
                            // Enable events on zero, maximum and minimum limit values //
                              //pcnt_event_enable(unit, PCNT1_EVT_ZERO);
                              //pcnt_event_enable(unit, PCNT1_EVT_H_LIM);
                              //pcnt_event_enable(unit, PCNT1_EVT_L_LIM);
                        break;
                      #if (USE_CNT_INP > 1)
                          case 1:
                            unit = (pcnt_unit_t) PCNT1_UNIDX;
                            cntThresh[i] = (uint16_t) PCNT1_THRESH0_VAL;
                            // Configure and enable the input filter
                              pcnt_set_filter_value(unit, PCNT1_INP_FILT);
                              pcnt_filter_enable(unit);
                            // Set threshold 0 and 1 values and enable events to watch //
                              pcnt_set_event_value(unit, PCNT1_EVT_0, PCNT1_THRESH0_VAL);
                              pcnt_event_enable(unit, PCNT1_EVT_0);
                            break;
                        #endif
                      #if (USE_CNT_INP > 2)
                          case 2:
                            unit = (pcnt_unit_t) PCNT2_UNIDX;
                            // Configure and enable the input filter
                              pcnt_set_filter_value(unit, PCNT2_INP_FILT);
                              pcnt_filter_enable(unit);
                            // Set threshold 0 and 1 values and enable events to watch //
                              pcnt_set_event_value(unit, PCNT_EVT_0, PCNT2_THRESH0_VAL);
                              pcnt_event_enable(unit, PCNT_EVT_0);
                            break;
                        #endif
                      #if (USE_CNT_INP > 3)
                          case 1:
                            unit = (pcnt_unit_t) PCNT3_UNIDX;
                            // Configure and enable the input filter
                              pcnt_set_filter_value(unit, PCNT3_INP_FILT);
                              pcnt_filter_enable(unit);
                            // Set threshold 0 and 1 values and enable events to watch //
                              pcnt_set_event_value(unit, PCNT3_EVT_0, PCNT3_THRESH0_VAL);
                              pcnt_event_enable(unit, PCNT3_EVT_0);
                            break;
                        #endif
                      default:
                        break;
                    }
                  // Initialize PCNT's counter //
                  pcnt_counter_pause(unit);
                  pcnt_counter_clear(unit);

                  // Install interrupt service and add isr callback handler //
                  sprintf(cmsg, "_unit %i pcnt_isr_service_install_ ", i);
                  logESP(pcnt_isr_service_install(unit), cmsg, i);
                  switch (i)
                    {
                      case 0:
                        STXT("  pcnt_isr_handler unit 0");
                        sprintf(cmsg, "_unit %i pcnt_isr_handler_add_ ", i);
                        logESP(pcnt_isr_handler_add(unit, pcnt0_intr_hdl, &unit), cmsg, i);
                        break;
                      #if (USE_CNT_INP > 1)
                          case 1:
                            STXT("  pcnt_isr_handler unit 1");
                            sprintf(cmsg, "_unit %i pcnt_isr_handler_add_ ", i);
                            logESP(pcnt_isr_handler_add(unit, pcnt1_intr_hdl, &unit), cmsg, i);
                            break;
                        #endif
                      #if (USE_CNT_INP > 2)
                          case 2:
                            pcnt_isr_handler_add(unit, pcnt2_intr_hdl, &unit);
                            break;
                        #endif
                      #if (USE_CNT_INP > 3)
                          case 3:
                            pcnt_isr_handler_add(unit, pcnt3_intr_hdl, &unit);
                            break;
                        #endif
                      default:
                        break;
                    }
                  // Everything is set up, now go to counting //
                  pcnt_counter_resume(unit);
                }
            }
          #ifdef USE_MCPWM
              static void getCNTIn()
                {
                      // count LOW width
                      mcpwm_capture_enable(MCPWM_UNIDX_0, MCPWM_SELECT_CAP0, MCPWM_NEG_EDGE, 0);
                      cntLowPulse[0] = mcpwm_capture_signal_get_value(MCPWM_UNIDX_0, MCPWM_SELECT_CAP0) / 1000;
                      // count HIGH width
                      mcpwm_capture_enable(MCPWM_UNIDX_0, MCPWM_SELECT_CAP0, MCPWM_POS_EDGE, 0);
                      cntHighPulse[0] = mcpwm_capture_signal_get_value(MCPWM_UNIDX_0, MCPWM_SELECT_CAP0) /1000 ;
                }
            #endif
        #endif
    // --- digital input
      #if (USE_DIG_INP > OFF)
          void getDIGIn()
            {
              #if USE_WS2812_PWR_IN_SW
                  ws2812_pwr = digitalRead(PIN_WS2812_PWR_IN_SW);
                #endif
              #if (USE_GEN_SW_INP > OFF)
                  uint8_t tmp;
                  for (uint8_t i = 0; i < USE_GEN_SW_INP ; i++)
                    {
                      tmp = digitalRead(pinInpDig[i]);
                      valInpDig[i] = tmp;
                      if (!polInpDig[i])
                        if (tmp == valInpDig[i])
                          if (tmp == digitalRead(pinInpDig[i]))
                            valInpDig[i] = !tmp;
                      else
                        if (tmp != valInpDig[i])
                          if (tmp == digitalRead(pinInpDig[i]))
                            valInpDig[i] = tmp;
                    }
                #endif
            }
        #endif
    // --- analog input
      #if (USE_CTRL_POTI > OFF)
          void getADCIn()
            {
              for (uint8_t i = 0 ; i < USE_CTRL_SW_INP ; i++ )
                {
                  inpValADC[i] = analogRead(PIN_ADC_CONF[i].pin);
                }
            }
        #endif

  // --- sensors -------------------------
    // --- BME280
      #if (USE_BME280_I2C > OFF)
          static void initBME280()
            {
              dispStatus("init BME280");
              STXT(" init BME280 ...");
              bmeda = bme1.begin(I2C_BME280_76, pbme1i2c);
              if (bmeda)
                {
                  bme1.setSampling(bme1.MODE_FORCED);
                  STXT(" BME280(1) gefunden");
                  #if (BME280T_FILT > OFF)
                      bmeTVal.begin(BME280T_FILT, BME280T_Drop);
                    #endif
                  #if (BME280P_FILT > OFF)
                      bmePVal.begin(BME280P_FILT, BME280P_Drop);
                    #endif
                  #if (BME280H_FILT > OFF)
                      bmeHVal.begin(BME280H_FILT, BME280H_Drop);
                    #endif
                }
                else
                {
                  STXT(" BME280(1) nicht gefunden");
                }
            }
        #endif
    // --- BME680
      #if (USE_BME680_I2C > OFF)
          static void initBME680()
            {
              dispStatus("init BME680");
              STXT(" init BME680 ...");
              bme680da = bme680.begin(I2C_BME680_77, pbme680i2c);
              if (bme680da)
                {
                  STXT(" BME680 gefunden");
                  #if (BME680T_FILT > OFF)
                      bmeTVal.begin(BME680T_FILT, BME680T_Drop);
                    #endif
                  #if (BME680P_FILT > OFF)
                      bmePVal.begin(BME680P_FILT, BME680P_Drop);
                    #endif
                  #if (BME680H_FILT > OFF)
                      bmeHVal.begin(BME680H_FILT, BME680H_Drop);
                    #endif
                  #if (BME680G_FILT > OFF)
                      bmeHVal.begin(BME680G_FILT, BME680G_Drop);
                    #endif
                }
                else
                {
                  STXT(" BME680 nicht gefunden");
                }
            }
        #endif
    // --- 4x analog input ADS1115
      #if (USE_ADC1115_I2C > OFF)
          static void initADS1115()
            {
              // init unit 1
                STXT(" init ADS1115_1");
                ads[0].init(0, ADS1_RUNMODE);       // init unit 1
                // init channels
                  /* unit 1 - channel 1 is always configured and always measured
                     in case ADS11_MUX is not defined (this is allowed)
                     - channel 1 unit 0
                       - is initialized to
                         - fastest mode and
                         - smallest attenuation
                       - is always activ measured
                   */
                  #if (ADS11_MUX == ADS1X15_MUX_SINGLE)
                      STXT(" init ADS1115_1 chan 1");
                      ads[0].initChan(0, ADS11_RATE, ADS11_GAIN, ADS1X15_REG_CONFIG_MUX_SINGLE_0);
                    #else
                      STXT(" auto init ADS1115_1 chan 1");
                      ads[0].initChan(0, RATE_ADS1115_860SPS, GAIN_TWOTHIRDS, ADS1X15_REG_CONFIG_MUX_SINGLE_0);
                    #endif
                  #if (ADS12_MUX == ADS1X15_MUX_SINGLE)
                      STXT(" init ADS1115_1 chan 2");
                      ads[0].initChan(1, ADS12_RATE, ADS12_GAIN, ADS1X15_REG_CONFIG_MUX_SINGLE_1);
                    #endif
                  #if (ADS13_MUX == ADS1X15_MUX_SINGLE)
                      STXT(" init ADS1115_1 chan 3");
                      ads[0].initChan(2, ADS13_RATE, ADS13_GAIN, ADS1X15_REG_CONFIG_MUX_SINGLE_2);
                    #endif
                  #if (ADS14_MUX == ADS1X15_MUX_SINGLE)
                      STXT(" init ADS1115_1 chan 4");
                      ads[0].initChan(3, ADS13_RATE, ADS13_GAIN, ADS1X15_REG_CONFIG_MUX_SINGLE_3);
                    #endif
                // init unit 2
                #if (USE_ADC1115_I2C > 1) // 1
                    STXT(" init ADS1115_2");
                    ads[1].init(1);       // init unit 2
                    // init channels
                      #ifdef ADS21_MUX
                          STXT(" init ADS1115_2 chan 1");
                          ads[1].initChan(0, ADS21_RATE, ADS21_GAIN, ADS21_MUX);
                      #else
                          STXT(" auto init ADS1115_2 chan 1");
                          ads[1].initChan(0, RATE_ADS1115_860SPS, GAIN_TWOTHIRDS, ADS1X15_REG_CONFIG_MUX_SINGLE_0);
                        #endif
                      #ifdef ADS22_MUX
                          STXT(" init ADS1115_2 chan 2");
                          ads[1].initChan(1, ADS22_RATE, ADS22_GAIN, ADS12_MUX);
                        #endif
                      #ifdef ADS23_MUX
                          STXT(" init ADS1115_2 chan 3");
                          ads[1].initChan(2, ADS23_RATE, ADS23_GAIN, ADS13_MUX);
                        #endif
                      #ifdef ADS24_MUX
                          STXT(" init ADS1115_2 chan 4");
                          ads[1].initChan(3, ADS24_RATE, ADS24_GAIN, ADS14_MUX);
                        #endif
                    // init unit 3
                    #if (USE_ADC1115_I2C > 2) // 2
                        STXT(" init ADS1115_3");
                        ads[2].init(2);       // init unit 3
                        // init channels
                          #ifdef ADS31_MUX
                              STXT(" init ADS1115_3 chan 1");
                              ads[2].initChan(0, ADS31_RATE, ADS31_GAIN, ADS31_MUX);
                          #else
                              STXT(" auto init ADS1115_3 chan 1");
                              ads[2].initChan(0, RATE_ADS1115_860SPS, GAIN_TWOTHIRDS, ADS1X15_REG_CONFIG_MUX_SINGLE_0);
                            #endif
                          #ifdef ADS32_MUX
                              STXT(" init ADS1115_3 chan 2");
                              ads[2].initChan(1, ADS32_RATE, ADS32_GAIN, ADS32_MUX);
                            #endif
                          #ifdef ADS33_MUX
                              STXT(" init ADS1115_3 chan 3");
                              ads[2].initChan(2, ADS33_RATE, ADS33_GAIN, ADS33_MUX);
                            #endif
                          #ifdef ADS34_MUX
                              STXT(" init ADS1115_3 chan 4");
                              ads[2].initChan(3, ADS34_RATE, ADS34_GAIN, ADS34_MUX);
                            #endif
                        // init unit 4
                        #if (USE_ADC1115_I2C > 3) // 3
                             ads[3].init(3);       // init unit 3
                            STXT(" init ADS1115_4 chan 1");
                            // init channels
                              #ifdef ADS41_MUX
                                  STXT(" init ADS1115_4 chan 1");
                                  ads[3].initChan(0, ADS41_RATE, ADS41_GAIN, ADS41_MUX);
                              #else
                                  STXT(" auto init ADS1115_4 chan 1");
                                  ads[3].initChan(0, RATE_ADS1115_860SPS, GAIN_TWOTHIRDS, ADS1X15_REG_CONFIG_MUX_SINGLE_0);
                                #endif
                              #ifdef ADS42_MUX
                                  STXT(" init ADS1115_4 chan 2");
                                  ads[3].initChan(1, ADS42_RATE, ADS42_GAIN, ADS42_MUX);
                                #endif
                              #ifdef ADS43_MUX
                                  STXT(" init ADS1115_4 chan 3");
                                  ads[3].initChan(2, ADS43_RATE, ADS43_GAIN, ADS43_MUX);
                                #endif
                              #ifdef ADS44_MUX
                                  STXT(" init ADS1115_4 chan 4");
                                  ads[3].initChan(3, ADS44_RATE, ADS44_GAIN, ADS44_MUX);
                                #endif
                          #endif
                      #endif
                  #endif
            }
          static void startADS1115()
            {
              uint8_t _addr = ADS1_ADDR;
              SVAL(" start ADS1115_1 ... prj ADS1", ADS1_RUNMODE);
              if (ads[0].begin(ADS1_ADDR, pads0i2c))
                { STXT(" ADS1115_1 started "); }
                else
                {
                  if(ADS1_RUNMODE == MD_NORM)
                    { STXT(" could not start ADS1115_1 "); }
                    else
                    { STXT(" simulation start ADS1115_1 "); }
                }
              #if (USE_ADC1115_I2C > 1) // 1
                  _addr = ADS1_ADDR;
                  #ifdef ADS2_ADDR
                      _addr = ADS2_ADDR;
                    #endif
                  if (ads[1].begin(_addr, pads0i2c))
                    { STXT(" ADS1115_2 started "); }
                    else
                    { STXT(" could not start ADS1115_2 "); }
                  #if (USE_ADC1115_I2C > 2) // 2
                      _addr = ADS1_ADDR;
                      #ifdef ADS3_ADDR
                          _addr = ADS3_ADDR;
                        #endif
                      if (ads[2].begin(_addr, pads0i2c))
                        { STXT(" ADS1115_3 started "); }
                        else
                        { STXT(" could not start ADS1115_3 "); }
                      #if (USE_ADC1115_I2C > 3) // 3
                          _addr = ADS1_ADDR;
                          #ifdef ADS4_ADDR
                              _addr = ADS4_ADDR;
                            #endif
                          if (ads[3].begin(_addr, pads0i2c))
                            { STXT(" ADS1115_4 started "); }
                            else
                            { STXT(" could not start ADS1115_4 "); }
                        #endif
                    #endif
                #endif
            }
        #endif
    // --- CCS811
      #if (USE_CCS811_I2C > OFF)
          void initCCS811()
            {
              dispStatus("init CCS811");
              STXT(" init CCS811 ...");
              ccsda = ccs811.begin(I2C_CCS811_AQ_5A, pbme1i2c);
              if (ccsda)
                  {
                    STXT(" CCS811 gefunden");
                    #if (CCS811T_FILT > OFF)
                        ccsTVal.begin(CCS811T_FILT, CCS811T_Drop);
                      #endif
                    #if (CCS811C_FILT > OFF)
                        ccsCVal.begin(CCS811C_FILT, CCS811C_Drop);
                      #endif
                    ccs811T.startT();
                    //while(!ccs811.available() && !ccs811T.TOut()); // wait until init
                  }
                else
                  {
                    STXT(" CCS811 nicht gefunden");
                  }
            }
        #endif
    // --- INA3221
      #if (USE_INA3221_I2C > OFF)
          void initINA3221()
            {
              dispStatus("init INA32211");
              STX(" init INA32211 ID "); SOUTHEXLN((ina32211.getManufID()));
              ina32211.begin();
              for (uint8_t i = 0; i < 3 ; i++ )
                {
                  pubINA3221i[0][i] = OFF;
                  pubINA3221u[0][i] = OFF;
                }
              // channel 1
                #if (INA3221U1_FILT > OFF)
                    inaUVal[0][0].begin(INA3221U1_FILT, INA3221U1_DROP);
                  #endif
                #if (INA3221I1_FILT > OFF)
                    inaIVal[0][0].begin(INA3221I1_FILT, INA3221I1_DROP);
                  #endif
              // channel 2
                #if (INA3221U2_FILT > OFF)
                    inaUVal[0][1].begin(INA3221U2_FILT, INA3221U2_DROP);
                  #endif
                #if (INA3221I2_FILT > OFF)
                    inaIVal[0][1].begin(INA3221I2_FILT, INA3221I2_DROP);
                  #endif
              // channel 3
                #if (INA3221U3_FILT > OFF)
                   inaUVal[0][2].begin(INA3221U3_FILT, INA3221U3_DROP);
                  #endif
                #if (INA3221I3_FILT > OFF)
                    inaIVal[0][2].begin(INA3221I3_FILT, INA3221I3_DROP);
                  #endif
              #if (USE_INA3221_I2C > 1)
                  dispStatus("init INA32212");
                  STXT(" init INA32212 ...");
                  ina32212.begin();
                  inaIVal[1][0].begin(INA3221I1_FILT, INA3221I1_DROP);
                  inaIVal[1][1].begin(INA3221I2_FILT, INA3221I2_DROP);
                  inaIVal[1][2].begin(INA3221I3_FILT, INA3221I3_DROP);
                  inaUVal[1][0].begin(INA3221U1_FILT, INA3221U1_DROP);
                  inaUVal[1][1].begin(INA3221U2_FILT, INA3221U2_DROP);
                  inaUVal[1][2].begin(INA3221U3_FILT, INA3221U3_DROP);
                  for (uint8_t i = 0; i < 3 ; i++ )
                    {
                      pubINA3221i[1][i] = OFF;
                      pubINA3221u[1][i] = OFF;
                    }
                  #if (USE_INA3221_I2C > 2)
                      dispStatus("init INA32213");
                      STXT(" init INA32213 ...");
                      ina32212.begin();
                      inaIVal[2][0].begin(INA3221I1_FILT, INA3221I1_DROP);
                      inaIVal[2][1].begin(INA3221I2_FILT, INA3221I2_DROP);
                      inaIVal[2][2].begin(INA3221I3_FILT, INA3221I3_DROP);
                      inaUVal[2][0].begin(INA3221U1_FILT, INA3221U1_DROP);
                      inaUVal[2][1].begin(INA3221U2_FILT, INA3221U2_DROP);
                      inaUVal[2][2].begin(INA3221U3_FILT, INA3221U3_DROP);
                      for (uint8_t i = 0; i < 3 ; i++ )
                        {
                          pubINA3221i[2][i] = OFF;
                          pubINA3221u[2][i] = OFF;
                        }
                    #endif
                #endif

            }
        #endif
    // --- DS18B20
      String getDS18D20Str()
        {
          String outS = "";
          #if (USE_DS18B20_1W_IO > OFF)
              for (uint8_t i = 0 ; i < USE_DS18B20_1W_IO ; i++ )
                {
                  switch (i)
                    {
                      case 0:
                          ds1Sensors.requestTemperatures(); // Send the command to get temperatures
                          dsTemp[i] = ds1Sensors.getTempCByIndex(i);
                        break;
                      case 1:
                        #if (USE_DS18B20_1W_IO > 1)
                            ds2Sensors.requestTemperatures(); // Send the command to get temperatures
                            dsTemp[i] = ds2Sensors.getTempCByIndex(i);
                          #endif
                        break;
                      default:
                        break;
                    }
                        //S2VAL(" DS18B20",i, dsTemp[i]);
                  if (i < 1) { outS  = "T1 "; }
                  else       { outS += "    T2  ";}
                  outS += (String) ((int) (dsTemp[i] + 0.5)) + "°";
                }
            #endif
          return outS;
        }
    // --- T-element type K
    // --- photo sensor
      #if (USE_PHOTO_SENS_ANA > OFF)
          void initPhoto()
            {
              STXT(" init photo sensors ...");
              #if (PHOTO1_FILT > OFF)
                  //photoVal[0].begin(PHOTO1_FILT, PHOTO1_DROP, FILT_FL_MEAN);
                #endif
              photoScal[0].setScale(PHOTO1_SCAL_OFFRAW, PHOTO1_SCAL_GAIN, PHOTO1_SCAL_OFFREAL);
              #if (PHOTO1_ADC > OFF)
                  pinMode(PIN_PHOTO1_SENS, INPUT);
                  adc1_config_channel_atten((adc1_channel_t) ADC_PHOTO1_SENS,
                                            (adc_atten_t)    PHOTO1_ADC_ATT);
                #endif
              #if (PHOTO1_1115 > OFF)
                #endif
              STXT(" photo sensors  ready");
            }
        #endif
    // --- poti measure
      #if (USE_POTI_ANA > OFF)
          void initPoti()
            {
              STXT(" init poti ... ");
              #if (POTI1_FILT > OFF)
                  //potiVal[0].begin(POTI1_FILT, POTI1_DROP, FILT_FL_MEAN);
                #endif
              potifScal[0].setScale(POTI1_OFFRAW, POTI1_GAIN, POTI1_OFFREAL);
              STXT(" poti ready");
            }
        #endif
    // --- vcc measure
      #if (USE_VCC50_ANA > OFF)
          void initVCC50()
            {
              STXT(" init vcc measure ... ");
              #if (VCC_FILT > OFF)
                  vcc50Val.begin(VCC_FILT, VCC_DROP, FILT_FL_MEAN);
                #endif
              vcc50fScal.setScale(VCC50_OFFRAW, VCC50_GAIN, VCC50_OFFREAL);
              STXT(" vcc measure ready");
            }
        #endif
      #if (USE_VCC33_ANA > OFF)
          void initVCC33()
            {
              STXT(" init vcc measure ... ");
              #if (VCC_FILT > OFF)
                  vcc33Val.begin(VCC_FILT, VCC_DROP, FILT_FL_MEAN);
                #endif
              vcc33fScal.setScale(VCC33_OFFRAW, VCC33_GAIN, VCC33_OFFREAL);
              STXT(" vcc measure ready");
            }
        #endif

      #if (USE_ACS712_ANA > OFF)
          void initACS712()
            {
              STXT("init current sensors ... ");
              i712Scal[0].setScale(I712_1_SCAL_OFFRAW, I712_1_SCAL_GAIN, I712_1_SCAL_OFFREAL);
              #if (I712_FILT > OFF)
                  //i712Val[0].begin(I712_FILT, I712_DROP, FILT_FL_MEAN);
                #endif
              #if (USE_ACS712_ANA > 1)
                  STXT("init current sensors ... ");
                  i712Scal[1].setScale(I712_2_SCAL_OFFRAW, I712_2_SCAL_GAIN, I712_2_SCAL_OFFREAL);
                  #if (I712_FILT > OFF)
                      //i712Val[1].begin(I712_FILT, I712_DROP, FILT_FL_MEAN);
                    #endif
                  #if (USE_ACS712_ANA > 2)
                      STXT("init current sensors ... ");
                      i712Scal[2].setScale(I712_3_SCAL_OFFRAW, I712_3_SCAL_GAIN, I712_3_SCAL_OFFREAL);
                      #if (I712_FILT > OFF)
                          //i712Val[2].begin(I712_FILT, I712_DROP, FILT_FL_MEAN);
                        #endif
                      #if (USE_ACS712_ANA > 3)
                          STXT("init current sensors ... ");
                          i712Scal[3].setScale(I712_4_SCAL_OFFRAW, I712_4_SCAL_GAIN, I712_4_SCAL_OFFREAL);
                          #if (I712_FILT > OFF)
                              //i712Val[3].begin(I712_FILT, I712_DROP, FILT_FL_MEAN);
                            #endif
                        #endif
                    #endif
                  #endif
              STXT(" current sensors ready");
            }
        #endif
  // --- memory --------------------------
    // --- flash
      void testFlash()
        {
          STXT(" mounting SPIFFS ... ");
          /*
            if(!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED))
              { STXT(" ERROR"); return; }
            uint32_t bFree = SPIFFS.totalBytes();
           */
          spiffs.init(pspiffs);
          uint32_t bFree = spiffs.totalBytes();
          SVAL(" found bytes free ", bFree);
          STXT(" dir: test_example.txt ");
          File file = spiffs.open("/test_example.txt");
          if(!file)
            {
              STXT("   Failed to open file for reading");
              return;
            }
          STXT("   File Content: ");
          int8_t n = 0;
          while(n >= 0)
            {
              n = file.read();
              if (n > 0) SOUT((char) n);
            }
          SOUTLN();
          file.close();
          spiffs.end();
          //*/
        }
  // --- network -------------------------
    // --- WIFI
      uint8_t startWIFI(bool startup)
        {
          bool ret = MD_ERR;
          char _cssid[LOGINTXT_MAX_LEN + 1];
          char _cpw[LOGINTXT_MAX_LEN + 1];
              //SVAL(" startWIFI   Start WiFi ", startup);
          #if (USE_WIFI > OFF)
              dispStatus("  start WIFI");
                  //heapFree(" before generating ipList ");
              if (startup)
                {
                  ip_list ipList = ip_list(); // temporary object
                            #if (DEBUG_MODE > CFG_DEBUG_STARTUP)
                                SHEXVAL(" setup startWIFI created ipList ", (int) &ipList);
                                STXT(" setup startWIFI add WIFI 0");
                              #endif
                  sprintf(_cssid, "%s\0", WIFI_SSID0);
                  sprintf(_cpw, "%s\0", WIFI_SSID0_PW);
                  ipList.append(WIFI_FIXIP0, WIFI_GATEWAY0, WIFI_SUBNET, _cssid, _cpw);
                  #if (WIFI_ANZ_LOGIN > 1)
                            #if (DEBUG_MODE > CFG_DEBUG_STARTUP)
                                STXT(" setup startWIFI add WIFI 1");
                              #endif
                      sprintf(_cssid, "%s\0", WIFI_SSID1);
                      sprintf(_cpw, "%s\0", WIFI_SSID1_PW);
                      ipList.append(WIFI_FIXIP1, WIFI_GATEWAY1, WIFI_SUBNET, _cssid, _cpw);
                    #endif
                  #if (WIFI_ANZ_LOGIN > 2)
                            #if (DEBUG_MODE > CFG_DEBUG_STARTUP)
                                STXT(" setup startWIFI add WIFI 2");
                              #endif
                      sprintf(_cssid, "%s\0", WIFI_SSID2);
                      sprintf(_cpw, "%s\0", WIFI_SSID2_PW);
                      ipList.append(WIFI_FIXIP2, WIFI_GATEWAY2, WIFI_SUBNET, _cssid, _cpw);
                    #endif
                  #if (WIFI_ANZ_LOGIN > 3)
                            #if (DEBUG_MODE > CFG_DEBUG_STARTUP)
                                STXT(" setup startWIFI add WIFI 3");
                              #endif
                      sprintf(_cssid, "%s\0", WIFI_SSID3);
                      sprintf(_cpw, "%s\0", WIFI_SSID3_PW);
                      ipList.append(WIFI_FIXIP3, WIFI_GATEWAY3, WIFI_SUBNET, _cssid, _cpw);
                    #endif
                  #if (WIFI_ANZ_LOGIN > 4)
                            #if (DEBUG_MODE > CFG_DEBUG_STARTUP)
                                STXT(" setup startWIFI add WIFI 4");
                              #endif
                      sprintf(_cssid, "%s\0", WIFI_SSID4);
                      sprintf(_cpw, "%s\0", WIFI_SSID4_PW);
                      ipList.append(WIFI_FIXIP4, WIFI_GATEWAY4, WIFI_SUBNET, _cssid, _cpw);
                    #endif
                  #if (WIFI_ANZ_LOGIN > 5)
                            #if (DEBUG_MODE > CFG_DEBUG_STARTUP)
                                STXT(" setup startWIFI add WIFI 5");
                              #endif
                      sprintf(_cssid, "%s\0", WIFI_SSID5);
                      sprintf(_cpw, "%s\0", WIFI_SSID5_PW);
                      ipList.append(WIFI_FIXIP5, WIFI_GATEWAY5, WIFI_SUBNET, _cssid, _cpw);
                    #endif
                  #if (WIFI_ANZ_LOGIN > 6)
                            #if (DEBUG_MODE > CFG_DEBUG_STARTUP)
                                STXT(" setup startWIFI add WIFI 6");
                              #endif
                      sprintf(_cssid, "%s\0", WIFI_SSID6);
                      sprintf(_cpw, "%s\0", WIFI_SSID6_PW);
                      ipList.append(WIFI_FIXIP6, WIFI_GATEWAY6, WIFI_SUBNET, _cssid, _cpw);
                    #endif
                  #if (WIFI_ANZ_LOGIN > 7)
                            #if (DEBUG_MODE > CFG_DEBUG_STARTUP)
                                STXT(" setup startWIFI add WIFI 7");
                              #endif
                      sprintf(_cssid, "%s\0", WIFI_SSID7);
                      sprintf(_cpw, "%s\0", WIFI_SSID7_PW);
                      ipList.append(WIFI_FIXIP7, WIFI_GATEWAY7, WIFI_SUBNET, _cssid, _cpw);
                    #endif
                  #if (WIFI_ANZ_LOGIN > 8)
                            #if (DEBUG_MODE > CFG_DEBUG_STARTUP)
                                STXT(" setup add WIFI 8");
                              #endif
                      sprintf(_cssid, "%s\0", WIFI_SSID8);
                      sprintf(_cpw, "%s\0", WIFI_SSID8_PW);
                      ipList.append(WIFI_FIXIP8, WIFI_GATEWAY8, WIFI_SUBNET, _cssid, _cpw);
                    #endif
                            //STXT(UTLN(" setup startWIFI locWIFI fertig");

                            //ip_cell* pip = (ip_cell*) ipList.pFirst();
                            //char stmp[NET_MAX_SSID_LEN] = "";
                                    /*
                                      SOUT(" setup ip_list addr "); SOUT((u_long) &ipList);
                                      SOUT(" count "); SOUTLN(ipList.count());
                                      SOUT(" ip1: addr "); SOUTHEX((u_long) pip);
                                      SOUT(" locIP "); SOUTHEX(pip->locIP());
                                      SOUT(" gwIP ");  SOUTHEX(pip->gwIP());
                                      SOUT(" snIP ");  SOUTHEX(pip->snIP());
                                      pip->getSSID(stmp); SOUT(" ssid "); SOUT(stmp);
                                      pip->getPW(stmp); SOUT(" pw "); SOUTLN(stmp);
                                      pip = (ip_cell*) pip->pNext();
                                      SOUT(" ip2: addr "); SOUTHEX((u_long) pip);
                                      SOUT(" locIP "); SOUTHEX(pip->locIP());
                                      SOUT(" gwIP ");  SOUTHEX(pip->gwIP());
                                      SOUT(" snIP ");  SOUTHEX(pip->snIP());
                                      pip->getSSID(stmp); SOUT(" ssid "); SOUT(stmp);
                                      pip->getPW(stmp); SOUT(" pw "); SOUTLN(stmp);
                                      pip = (ip_cell*) pip->pNext();
                                      SOUT(" ip3: addr "); SOUTHEX((u_long) pip);
                                      SOUT(" locIP "); SOUTHEX(pip->locIP());
                                      SOUT(" gwIP ");  SOUTHEX(pip->gwIP());
                                      SOUT(" snIP ");  SOUTHEX(pip->snIP());
                                      pip->getSSID(stmp); SOUT(" ssid "); SOUT(stmp);
                                      pip->getPW(stmp); SOUT(" pw "); SOUTLN(stmp);
                                    */

                      //heapFree(" ipList generated ");
                  ret = wifi.scanWIFI(&ipList);
                          //SVAL(" scanWIFI ret=", ret);
                      //heapFree(" before deleting ipList ");
                  ipList.~ip_list();
                      //heapFree(" after deleting ipList ");
                }
              ret = wifi.startWIFI();
                  //SVAL(" startWIFI ret ", ret);
              if (ret == MD_OK)
                {
                  #if (USE_MD_ATSMARTHOME > OFF)
                      ctmp30[0]=0;
                      iret               = wifi.getSSID(ctmp30);
                        //S2VAL(" SSID ", iret, ctmp30);
                      atdbSetup.SSID     = ctmp30;
                      ctmp30[0]=0;
                      iret               = wifi.getPW(ctmp30);
                        //S2VAL(" PW ", iret, ctmp30);
                      atdbSetup.password = ctmp30;
                      atdbSetup.useWlan  = true;
                        //database.setupWiFi(&atdbSetup);
                    #endif
                  dispStatus("WIFI connected");
                }
                else
                {
                  dispStatus("WIFI error");
                  #if (USE_MD_ATSMARTHOME > OFF)
                      atdbSetup.useWlan  = false;
                    #endif
                }
            #endif // USE_WIFI
          return ret;
        }
    // --- NTP server
      void initNTPTime()
        {
          #if (USE_NTP_SERVER > OFF)
              bool ret = wifi.initNTP();
                    #if (DEBUG_MODE >= CFG_DEBUG_DETAIL)
                      //Serial.print("initNTPTime ret="); Serial.print(ret);
                    #endif
              if (ret = MD_OK)
                {
                  dispStatus("NTPTime ok");
                }
                else
                {
                  dispStatus("NTPTime error");
                }
            #endif // USE_NTP_SERVER
        }
    // --- webserver
      #if (USE_WEBSERVER > OFF)
        void startWebServer()
          {
            iret = MD_ERR;
            if (!webOn)
              {
                dispStatus("start webserver");
                STXT(" startServer ... ");
                if (WiFi.status() == WL_CONNECTED)
                  {
                    iret = pmdServ->md_startServer();
                        #if (DEBUG_MODE >= CFG_DEBUG_DETAIL)
                            SVAL(" webserver ret ", iret);
                          #endif
                    if (iret == MD_OK)
                      {
                        webOn = TRUE;
                      }
                  }
                if (webOn)
                  {
                    dispStatus("Webserver online");
                    STXT("Webserver online");
                  }
                  else
                  {
                    dispStatus("Webserver ERROR");
                    STXT("Webserver ERROR");
                  }
              }
          }

        void readWebMessage()
          {
            md_message *pM   = NULL;
            int         itmp = 0;
            char*       ctmp = NULL;
            #if (USE_WS2812_MATRIX_OUT > OFF)
                md_LEDPix24* ppix = NULL;
              #endif
            uint8_t     idx;
            char        tval;
            char        tdata;

            if (inMsgs->count() > 0)
              {
                pM   = (md_message*) inMsgs->pFirst();
                switch (pM->msgType())
                  {
                    case ME_TSOCKET:
                      SOUT(" Socket ");
                      break;

                    case ME_TREQ:
                      pmdServ->isRequest = true;
                      break;

                    case ME_TCONN:
                      newClient = true;
                      break;

                    default:
                      ctmp  = pM->payload();
                      tdata = pM->dataType();
                      tval  = ctmp[0];
                      idx   = ctmp[1] - '0';
                      ctmp += 2;
                          SVAL(" msg client ", pM->client());
                          SVAL(" tMsg ",       pM->msgType());
                          SVAL(" tData ",      (char) pM->dataType());
                          SVAL(" tval ",       tval);
                          SVAL(" payload ",    pM->payload());
                          SVAL(" idx ",        idx);
                          SVAL(" ctmp '",      ctmp);
                      if (tdata == MD_SINGLE)
                        {
                          //STXT("---- switch(tval) ----");
                          switch (tval)
                            {
                              case EL_TANALOG:
                                  SOUT(" -- Analog nu ");
                                break;
                              case EL_TSLIDER:
                                  STXT(" -- Slider");
                                  itmp  = (int)strtol(ctmp, NULL, 10);
                                  switch (idx) // index of serverelement
                                    {
                                      case 1: // RGB-LED col24
                                        #if (USE_RGBLED_PWM > OFF)
                                            RGBLED->bright(itmp);
                                            //S2VAL("  -- rgbLED bright old new ", RGBLEDold->bright(), RGBLED->bright());
                                          #endif
                                        break;
                                      case 2: // 2821 line col24
                                        #if (USE_WS2812_LINE_OUT >OFF)
                                            //SVAL("  -- line bright old", line2812[1]->bright());
                                            line2812[1]->bright((uint8_t) itmp);
                                            //SVAL("  -- line bright new", line2812[1]->bright());
                                          #endif
                                        break;
                                      case 3:
                                        #if (USE_WS2812_MATRIX_OUT > OFF)
                                            ppix = outM2812[1].text->pix24;
                                            //SVAL("  -- matrix bright old", ppix->bright());
                                            ppix->bright((uint8_t) itmp);
                                            //SVAL("  -- matrix bright new", ppix->bright());
                                          #endif
                                        break;
                                      case 4:
                                        #if (USE_WS2812_MATRIX_OUT > OFF)
                                            ppix = outM2812[1].bmpB->pix24;
                                            //SVAL("  -- smilyB bright old"); SOUT(ppix->bright());
                                            ppix->bright((uint8_t) itmp);
                                            //SVAL("  -- smilyB bright new"); SOUT(ppix->bright());
                                            ppix = outM2812[1].bmpE->pix24;
                                            //SVAL("  -- smilyE bright old"); SOUT(ppix->bright());
                                            ppix->bright((uint8_t) itmp);
                                            //SVAL("  -- smilyE bright new"); SOUT(ppix->bright());
                                          #endif
                                        break;
                                    }
                                break;
                              case EL_TCOLOR:
                                  STXT(" ---- Color ----");
                                  itmp  = (int)strtol(ctmp, NULL, 16);
                                  switch (idx) // index of serverelement
                                    {
                                      case 1: // RGB-LED col24
                                        #if (USE_RGBLED_PWM > OFF)
                                            //SVAL("  -- RGBLED color old", RGBLED[1]->col24());
                                            RGBLED->col24(itmp);
                                            //SVAL("  -- RGBLED color new", RGBLED[1]->col24());
                                          #endif
                                        break;
                                      case 2: // 2821 line col24
                                        #if (USE_WS2812_LINE_OUT >OFF)
                                            SHEXVAL(" -- line2812 color24 old ", line2812[1]->col24());
                                            line2812[1]->col24(itmp);
                                            SHEXVAL(" -- line2812 color24 new ", line2812[1]->col24());
                                            SHEXVAL(" -- line2812 color16 new ", Col16(line2812[1]->col24()));
                                          #endif
                                        break;
                                      case 3: // 2821 matrix col16
                                        #if (USE_WS2812_MATRIX_OUT > OFF)
                                            ppix = outM2812[1].text->pix24;
                                            SHEXVAL(" -- matrix color24 old", ppix->col24());
                                            ppix->col24(itmp);
                                            SHEXVAL(" -- matrix color24 new", ppix->col24());
                                            SHEXVAL(" -- matrix color16 new", Col16(ppix->col24()));
                                          #endif
                                        break;
                                      case 4: // 2821 matrix col16 bmp
                                        #if (USE_WS2812_MATRIX_OUT > OFF)
                                            ppix = outM2812[1].bmpB->pix24;
                                            SHEXVAL(" -- matrixB color24 old", ppix->col24());
                                            ppix->col24(itmp);
                                            SHEXVAL(" -- matrixB color24 new", ppix->col24());
                                            SHEXVAL(" -- matrixB color16 new", Col16(ppix->col24()));
                                            SHEXVAL(" -- matrixB color24 old", ppix->col24());
                                            ppix = outM2812[1].bmpE->pix24;
                                            SHEXVAL(" -- matrixE color24 old", ppix->col24());
                                            ppix->col24(itmp);
                                            SHEXVAL(" -- matrixE color24 new", ppix->col24());
                                            SHEXVAL(" -- matrixE color16 new", Col16(ppix->col24()));
                                          #endif
                                        break;
                                    }
                                break;
                              case EL_TSWITCH:
                                  SOUT(" Switch ");
                                break;
                              case EL_TTEXT:
                                  SOUT(" Text ");
                                break;
                              case EL_TOFFSET:
                                  SOUT(" Offset ");
                                break;
                              case EL_TGRAPH:
                                  SOUT(" Graph ");
                                break;
                              case EL_TINDEX:
                                  SOUT(" Index ");
                                break;
                              default:
                                  SOUT(" ERROR ");
                                break;
                            }
                        }
                      break;
                  }
                SOUT("'"); SOUT(pM->payload()); SOUT("'");
                inMsgs->rem();
                SVAL(" inMsgs.count ", inMsgs->count());
              }
          }
          /*
            void    handleClient(AsyncWebSocketClient *client, void *arg, uint8_t *data, size_t len)
              {
                AwsFrameInfo *info = (AwsFrameInfo*)arg;
                char* txt = (char*) data;
                if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
                  { //  SOUT(" handleWebSocketMessage info->index "); SOUT(info->index); SOUT(" info->final "); SOUT(info->final); SOUT(" info->len "); SOUTLN(info->len);
                    data[len] = 0;
                    uint8_t type  = txt[0];  // extract obj type
                    uint8_t index = txt[1] - WS_IDX_OFFSET;  // extract index
                    int16_t value = atoi(&txt[2]);
                              //SOUT(" Payload type "); SOUT(type);
                              //SOUT(" index "); SOUT(index); SOUT(" len "); SOUT(len);
                              //SOUT(" data '"); SOUT(&txt[2]); SOUT(" = "); SOUT(value);
                              //SOUT(" ledList cnt "); SOUTLN(psliderList->count());

                    if (type == EL_TSLIDER)
                      {
                        md_slider* psl = (md_slider*) psliderList->pIndex(index);
                              //SOUT(" psl "); SOUTHEX((uint32_t) psl);
                        if (psl != NULL)
                          {
                            psl->destVal = value;
                            SOUT(" slider "); SOUT((index+1)); SOUT("="); SOUTLN(value);
                          }
                      }

                    else if (type == EL_TSWITCH)
                      {
                        md_switch* psw = (md_switch*) pswitchList->pIndex(index);
                        while (psw != NULL)
                          {
                            psw->destVal = value; SOUT(" switch "); SOUTLN(value);
                          }
                      }

                    else if (type == EL_TANALOG)
                      {
                        md_analog* pana = (md_analog*) panalogList->pIndex(index);
                        while (pana != NULL)
                          {
                            pana->destVal = value; SOUT(" analog "); SOUTLN(value);
                          }
                      }

                    else { }
                  }
              }
            */
          /*
            void handlingIncomingData()
              {


                AwsFrameInfo *info = (AwsFrameInfo*)arg;

                if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
                  {
                    String hexColor = "";
                    for (int i=0; i < len; i++)
                      hexColor += ((char) data[i]);

                    Serial.println("Hex Color: " + hexColor);

                    long n = strtol(&hexColor[0], NULL, 16);
                    Serial.println(n);
                    strip.fill(n);
                    strip.show();
                  }
              }
            */
            // Callback for incoming event
          /*
            void onEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type,
                           void * arg, uint8_t *data, size_t len)
              {
                switch(type)
                  {
                    case WS_EVT_CONNECT:
                      Serial.printf("Client connected: \n\tClient id:%u\n\tClient IP:%s\n",
                           client->id(), client->remoteIP().toString().c_str());
                      break;
                    case WS_EVT_DISCONNECT:
                      Serial.printf("Client disconnected:\n\tClient id:%u\n", client->id());
                      break;
                    case WS_EVT_DATA:
                      handlingIncomingData(client, arg, data, len);
                      break;
                    case WS_EVT_PONG:
                      Serial.printf("Pong:\n\tClient id:%u\n", client->id());
                      break;
                    case WS_EVT_ERROR:
                      Serial.printf("Error:\n\tClient id:%u\n", client->id());
                      break;
                  }
              }
            void configWebsite()
              {
                webMD.createElement(EL_TSLIDER, "LED red", "%");
                webMD.createElement(EL_TSLIDER, "LED green", "%");
                webMD.createElement(EL_TSLIDER, "LED blue", "%");

                webMD.createElement(EL_TANALOG, "DS18B20 Temp", "°C");
                webMD.createElement(EL_TANALOG, "Type-K Temp", "°C");
                webMD.createElement(EL_TANALOG, "BME_Temp", "°C");
                webMD.createElement(EL_TANALOG, "BME_Humidity", "%");
                webMD.createElement(EL_TANALOG, "BME_Pressure", "mb");
                webMD.createElement(EL_TANALOG, "Gaswert", "");
              }
            */

        #endif // USE_WEBSERVER
    // --- MQTT
      #if (USE_MQTT > OFF)
          void startMQTT()
            {
              STXT("Connecting to MQTT...");
                  S2VAL(" startMQTT msgs-len &msgs[0]", sizeof(MQTTmsg_t), (uint32_t) &MQTTmsgs[0]);
              for ( uint8_t i=0 ; i < MQTT_MSG_MAXANZ - 1; i++)
                {
                  MQTTmsgs[i].pNext = (void*) &MQTTmsgs[i+1];
                      //S3VAL(" startMQTT i msgs[i] pNext", i, (uint32_t) &MQTTmsgs[i], (uint32_t) MQTTmsgs[i].pNext);
                }
              MQTTmsgs[MQTT_MSG_MAXANZ-1].pNext = (void*) &MQTTmsgs[0];
                  //S3VAL(" startMQTT i msgs[i] pNext", MQTT_MSG_MAXANZ-1, (uint32_t) &MQTTmsgs[MQTT_MSG_MAXANZ-1], (uint32_t) MQTTmsgs[MQTT_MSG_MAXANZ-1].pNext);
              connectMQTT();
            } // tested -> ok

          void connectMQTT() // TODO: move all subcribes to here -> reconnect
            {
              errMQTT = (int8_t) mqtt.connectTo(MQTT_HOST, MQTT_PORT);
              soutMQTTerr(" MQTT connect", errMQTT);
              if (errMQTT == MD_OK)
                {
                  #if (USE_BME280_I2C > OFF) // 1
                      topBME280t = topDevice + topBME280t;
                      errMQTT = (int8_t) mqtt.subscribe(topBME280t.c_str());
                          soutMQTTerr(" MQTT subscribe BME280t", errMQTT);
                      topBME280p = topDevice + topBME280p;
                      errMQTT = (int8_t) mqtt.subscribe(topBME280p.c_str());
                          soutMQTTerr(" MQTT subscribe BME280p", errMQTT);
                      topBME280h = topDevice + topBME280h;
                      errMQTT = (int8_t) mqtt.subscribe(topBME280h.c_str());
                          soutMQTTerr(" MQTT subscribe BME280h", errMQTT);
                    #endif
                  #if (USE_BME680_I2C > OFF) // 1
                      topBME680t = topDevice + topBME680t;
                      errMQTT = (int8_t) mqtt.subscribe(topBME680t.c_str());
                          soutMQTTerr(" MQTT subscribe BME680t", errMQTT);
                      topBME680p = topDevice + topBME680p;
                      errMQTT = (int8_t) mqtt.subscribe(topBME680p.c_str());
                          soutMQTTerr(" MQTT subscribe BME680p", errMQTT);
                      topBME680h = topDevice + topBME680h;
                      errMQTT = (int8_t) mqtt.subscribe(topBME680h.c_str());
                          soutMQTTerr(" MQTT subscribe BME680h", errMQTT);
                      topBME680g = topDevice + topBME680g;
                      errMQTT = (int8_t) mqtt.subscribe(topBME680g.c_str());
                          soutMQTTerr(" MQTT subscribe BME680G", errMQTT);
                    #endif
                  #if (USE_CCS811_I2C > OFF)
                          topCCS811t = topDevice + topCCS811t;
                          errMQTT = (int8_t) mqtt.subscribe(topCCS811t.c_str());
                              soutMQTTerr(" MQTT subscribe CCS811t", errMQTT);
                          topCCS811c = topDevice + topCCS811c;
                          errMQTT = (int8_t) mqtt.subscribe(topCCS811c.c_str());
                              soutMQTTerr(" MQTT subscribe CCS811c", errMQTT);
                    #endif
                  #if (USE_INA3221_I2C > OFF) // unit 1
                      // channel 1
                        topINA32211i[0] = topDevice + topINA32211i[0];
                        errMQTT = (int8_t) mqtt.subscribe(topINA32211i[0].c_str());
                            soutMQTTerr(" MQTT subscribe topINA32211i[0]", errMQTT);

                        topINA32211u[0] = topDevice + topINA32211u[0];
                        errMQTT = (int8_t) mqtt.subscribe(topINA32211u[0].c_str());
                            soutMQTTerr(" MQTT subscribe topINA32211u[0]", errMQTT);

                        topINA32211p[0] = topDevice + topINA32211p[0];
                        errMQTT = (int8_t) mqtt.subscribe(topINA32211p[0].c_str());
                            soutMQTTerr(" MQTT subscribe topINA32211p[0]", errMQTT);
                      // channel 2
                        topINA32211i[1] = topDevice + topINA32211i[1];
                        errMQTT = (int8_t) mqtt.subscribe(topINA32211i[1].c_str());
                            soutMQTTerr(" MQTT subscribe topINA32211i[1]", errMQTT);

                        topINA32211u[1] = topDevice + topINA32211u[1];
                        errMQTT = (int8_t) mqtt.subscribe(topINA32211u[1].c_str());
                            soutMQTTerr(" MQTT subscribe topINA32211u[1]", errMQTT);

                        topINA32211p[1] = topDevice + topINA32211p[1];
                        errMQTT = (int8_t) mqtt.subscribe(topINA32211p[1].c_str());
                            soutMQTTerr(" MQTT subscribe topINA32211p[1]", errMQTT);
                      // channel 3
                        topINA32211i[2] = topDevice + topINA32211i[2];
                        errMQTT = (int8_t) mqtt.subscribe(topINA32211i[2].c_str());
                            soutMQTTerr(" MQTT subscribe topINA32211i[2]", errMQTT);
                        topINA32211u[2] = topDevice + topINA32211u[2];
                        errMQTT = (int8_t) mqtt.subscribe(topINA32211u[2].c_str());
                            soutMQTTerr(" MQTT subscribe topINA32211u[2]", errMQTT);
                        topINA32211p[2] = topDevice + topINA32211p[2];
                        errMQTT = (int8_t) mqtt.subscribe(topINA32211p[2].c_str());
                            soutMQTTerr(" MQTT subscribe topINA32211p[2]", errMQTT);
                      #if (USE_INA3221_I2C > 1)  // unit 2
                          // channel 1
                            topINA32212i[0] = topDevice + topINA32212i[0];
                            errMQTT = (int8_t) mqtt.subscribe(topINA32212i[0].c_str());
                                soutMQTTerr(" MQTT subscribe topINA32212i[0]", errMQTT);
                            topINA32212u[0] = topDevice + topINA32212u[0];
                            errMQTT = (int8_t) mqtt.subscribe(topINA32212u[0].c_str());
                                soutMQTTerr(" MQTT subscribe topINA32212u[0]", errMQTT);
                          // channel 2
                            topINA32212i[1] = topDevice + topINA32212i[1];
                            errMQTT = (int8_t) mqtt.subscribe(topINA32212i[1].c_str());
                                soutMQTTerr(" MQTT subscribe topINA32212i[1]", errMQTT);
                            topINA32212u[1] = topDevice + topINA32212u[1];
                            errMQTT = (int8_t) mqtt.subscribe(topINA32212u[1].c_str());
                                soutMQTTerr(" MQTT subscribe topINA32212u[1]", errMQTT);
                          // channel 3
                            topINA32212i[2] = topDevice + topINA32212i[2];
                            errMQTT = (int8_t) mqtt.subscribe(topINA32212i[2].c_str());
                                soutMQTTerr(" MQTT subscribe topINA32212i[2]", errMQTT);
                            topINA32212u[2] = topDevice + topINA32212u[2];
                            errMQTT = (int8_t) mqtt.subscribe(topINA32212u[2].c_str());
                                soutMQTTerr(" MQTT subscribe topINA32212u[2]", errMQTT);
                          #if (USE_INA3221_I2C > 2)  // unit 3
                              // channel 1
                                topINA32213i[0] = topDevice + topINA32213i[0];
                                errMQTT = (int8_t) mqtt.subscribe(topINA32213i[0].c_str());
                                    soutMQTTerr(" MQTT subscribe topINA32213i[0]", errMQTT);
                                topINA32213u[0] = topDevice + topINA32212u[0];
                                errMQTT = (int8_t) mqtt.subscribe(topINA32213u[0].c_str());
                                    soutMQTTerr(" MQTT subscribe topINA32213u[0]", errMQTT);
                              // channel 2
                                topINA32213i[1] = topDevice + topINA32213i[1];
                                errMQTT = (int8_t) mqtt.subscribe(topINA32213i[1].c_str());
                                    soutMQTTerr(" MQTT subscribe topINA32213i[1]", errMQTT);
                                topINA32213u[1] = topDevice + topINA32213u[1];
                                errMQTT = (int8_t) mqtt.subscribe(topINA32213u[1].c_str());
                                    soutMQTTerr(" MQTT subscribe topINA32213u[1]", errMQTT);
                              // channel 3
                                topINA32213i[2] = topDevice + topINA32213i[2];
                                errMQTT = (int8_t) mqtt.subscribe(topINA32213i[2].c_str());
                                    soutMQTTerr(" MQTT subscribe topINA32213i[2]", errMQTT);
                                topINA32213u[2] = topDevice + topINA32213u[2];
                                errMQTT = (int8_t) mqtt.subscribe(topINA32213u[2].c_str());
                                    soutMQTTerr(" MQTT subscribe topINA32213u[2]", errMQTT);
                            #endif
                        #endif
                    #endif
                  #if (USE_DS18B20_1W_IO > OFF)
                      topDS18B201 = topDevice + topDS18B201;
                      errMQTT = (int8_t) mqtt.subscribe(topDS18B201.c_str());
                          soutMQTTerr(" MQTT subscribe DS18B201 ", errMQTT);
                      #if (USE_DS18B20_1W_IO > 1)
                          topDS18B202 = topDevice + topDS18B202;
                          errMQTT = (int8_t) mqtt.subscribe(topDS18B202.c_str());
                              soutMQTTerr(" MQTT subscribe DS18B202 ", errMQTT);
                        #endif
                    #endif
                  #if (USE_MQ135_GAS_ANA > OFF)
                    #endif
                  #if (USE_MQ3_ALK_ANA > OFF)
                      topMQ3alk = topDevice + topMQ3alk;
                      errMQTT = (int8_t) mqtt.subscribe(topMQ3alk.c_str());
                          soutMQTTerr(" MQTT subscribe MQ3alk", errMQTT);
                    #endif
                  #if (USE_PHOTO_SENS_ANA > OFF)
                      topPhoto1 = topDevice + topPhoto1;
                      errMQTT = (int8_t) mqtt.subscribe(topPhoto1.c_str());
                          soutMQTTerr(" MQTT subscribe Photo1 ", errMQTT);
                    #endif
                  #if (USE_POTI_ANA > OFF)
                      topPoti1 = topDevice + topPoti1;
                      errMQTT = (int8_t) mqtt.subscribe(topPoti1.c_str());
                          soutMQTTerr(" MQTT subscribe poti1", errMQTT);
                    #endif
                  #if (USE_VCC50_ANA > OFF)
                      topVCC50 = topDevice + topVCC50;
                      errMQTT = (int8_t) mqtt.subscribe(topVCC50.c_str());
                          soutMQTTerr(" MQTT subscribe VCC50", errMQTT);
                    #endif
                  #if (USE_VCC33_ANA > OFF)
                      topVCC33 = topDevice + topVCC33;
                      errMQTT = (int8_t) mqtt.subscribe(topVCC33.c_str());
                          soutMQTTerr(" MQTT subscribe VCC33", errMQTT);
                    #endif
                  #if (USE_ACS712_ANA > OFF)
                      topi7121 = topDevice + topi7121;
                      errMQTT = (int8_t) mqtt.subscribe(topi7121.c_str());
                          soutMQTTerr(" MQTT subscribe i7121", errMQTT);
                      #if (USE_ACS712_ANA > 1)
                          topi7122 = topDevice + topi7122;
                          errMQTT = (int8_t) mqtt.subscribe(topi7122.c_str());
                              soutMQTTerr(" MQTT subscribe i7122", errMQTT);
                          #if (USE_ACS712_ANA > 2)
                              topi7123 = topDevice + topi7123;
                              errMQTT = (int8_t) mqtt.subscribe(topi7123.c_str());
                                  soutMQTTerr(" MQTT subscribe i7123", errMQTT);
                              #if (USE_ACS712_ANA > 3)
                                  topi7124 = topDevice + topi7124;
                                  errMQTT = (int8_t) mqtt.subscribe(topi7124.c_str());
                                      soutMQTTerr(" MQTT subscribe i7124", errMQTT);
                                #endif
                            #endif
                        #endif
                    #endif
                  #if (USE_TYPE_K_SPI > 0)
                    #endif
                  #if (USE_CNT_INP > OFF)
                    #endif
                  #if (USE_DIG_INP > OFF)
                    #endif
                  #if (USE_ESPHALL > OFF)
                    #endif
                  #if (USE_MCPWM > OFF)
                    #endif
                  #if (USE_RGBLED_PWM > OFF)
                      topRGBBright = topDevice + topRGBBright;
                      errMQTT = (int8_t) mqtt.subscribe(topRGBBright.c_str());
                          soutMQTTerr(" MQTT subscribe LEDBright ", errMQTT);

                      topRGBCol = topDevice + topRGBCol;
                      errMQTT = (int8_t) mqtt.subscribe(topRGBCol.c_str());
                          soutMQTTerr(" MQTT subscribe LEDCol ", errMQTT);
                    #endif
                  #if (USE_WS2812_MATRIX_OUT > OFF)
                    #endif
                  #if (USE_WS2812_LINE_OUT > OFF)
                    #endif
                  #if (USE_BUZZER_PWM > OFF)
                    #endif
                  #if (USE_GEN_DIG_OUT > OFF)
                      toptestLED = topDevice + toptestLED;
                      errMQTT = (int8_t) mqtt.subscribe(toptestLED.c_str());
                          soutMQTTerr(" MQTT subscribe testLED", errMQTT);
                    #endif
                }
            }

          void soutMQTTerr(String text, int8_t errMQTT)
            {
              if (errMQTT == MD_OK)
                {
                  errMQTTold = errMQTT;
                }
              if (errMQTT < MD_OK)
                {
                  if ((errMQTT != errMQTTold) && (errMQTT != -3))
                    {
                      SVAL(text, cerrMQTT[(-1) * errMQTT]);
                    }
                  if (errMQTT != -7) // not connected stays
                    { errMQTT = MD_OK; }
                }
            }

          void readMQTTmsg()
            {
              if (errMQTT != MD_OK) // not connected
                {
                  connectMQTT();
                }

              if (errMQTT == MD_OK) // connected
                {
                  char* ptopic = NULL;
                  while (anzMQTTmsg > 0)
                    {
                      ptopic = pMQTTRd->topic + strlen(MQTT_TOPDEV); // remove device ID
                          //S3VAL(" readMQTT pMQTTRd ptopic payload ", (uint32_t) pMQTTRd->topic, ptopic, pMQTTRd->payload);
                          //S3VAL(" readMQTT Bright  result ", topRGBBright, pMQTTRd->topic, topRGBBright.equals(pMQTTRd->topic));
                          //S3VAL(" readMQTT Color   result ", topRGBCol,    pMQTTRd->topic, topRGBCol.equals(pMQTTRd->topic));
                          //S3VAL(" readMQTT testLED result ", toptestLED,   pMQTTRd->topic, toptestLED.equals(pMQTTRd->topic));
                      #if (USE_RGBLED_PWM > OFF)
                          //if (strcmp(ptopic, topRGBBright.c_str())) // RGB LED bright
                          if (topRGBBright.equals(pMQTTRd->topic)) // RGB LED bright
                            {
                              RGBLED->bright(atoi(pMQTTRd->payload));
                              //S2VAL(" readMQTT RGBLED new bright payload ", RGBLED->bright(), pMQTTRd->payload);
                            }
                          else if (topRGBCol.equals(pMQTTRd->topic)) // RGB LED bright
                            {
                              tmpMQTT = pMQTTRd->payload;
                              sscanf(tmpMQTT.c_str(), "%x", &tmpval32);
                              RGBLED->col24(tmpval32);
                              //SHEXVAL(" readMQTT RGBLED new color  payload ", RGBLED->col24());
                            }
                          else {}
                        #endif
                      #if (USE_GEN_DIG_OUT > OFF)
                          if (toptestLED.equals(pMQTTRd->topic)) // test-led
                            {
                              if (strcmp(pMQTTRd->payload, "false") == 0)
                                { testLED = OFF;}
                              else
                                { testLED = ON; }
                              //SVAL(" readMQTT testLED new val ", testLED);
                            }
                        #endif
                      pMQTTRd = (MQTTmsg_t*) pMQTTRd->pNext;
                      anzMQTTmsg--;
                    }
                }
            }
        #endif
  // --- error ESP -------------------------
    void logESP(const esp_err_t _err, const char *_msg, uint8_t nr, bool _stop)
      {
        if (_err == ESP_OK) { return; }  // no error
        // error
        Serial.printf("\n  %i  ", nr);
        SOUT(_msg);
        Serial.printf(" %i - 0x%H\n", _err, _err);
        usleep(500000);

        if (_stop)
          {
            STXT(" !!! STOP for ever ... goodby !!!");
            while (1)
              {
                sleep(1000);
              }
          }
      }
// ----------------------------------------------------------------
// --- callback functions -----------------------------------
// ----------------------------------------------------------------
  // ------ MQTT callback functions --------------------------------

// --- end of code -----------------------------------------------
#ifdef ORIGINAL
    void _loop()
      {
        //tft.fillScreen(ILI9341_BLACK);
        //tft.fillRect(0,0,240,20,BACKGROUND);
        //tft.fillRect(2,2,3,3,ILI9341_RED);
        tft.setTextSize(textSize);
        for (uint8_t i = 0 ; i < 4 ; i++)
          {
            setLimits(i);
            tft.setRotation(i);
            xpos = 10 + 10; ypos = 10;
            tft.drawRoundRect(10,      10, 3, 3, 1, ILI9341_YELLOW);
            sprintf(text,"r %1i", i);
            tft.setCursor(xpos,ypos);
            tft.getTextBounds(&text[0], xpos, ypos, &wx, &wy, &ww, &wh);
            tft.fillRect(wx, wy, ww, wh, ILI9341_BLACK);
            tft.print(text);
          }
        delay(3000);

        Serial.println(calWin[0]);
        Serial.println(calWin[1]);
        Serial.println(calWin[2]);
        Serial.println(calWin[3]);
        if (doCal == true)
          {
            Serial.println("draw Calib window");
            //tft.fillRoundRect(110,260,60,40,2,ILI9341_WHITE);
            tft.fillRoundRect(calWin[0], calWin[1], calWin[2], calWin[3], 2, ILI9341_GREEN);
            tft.drawRoundRect(calWin[0], calWin[1], calWin[2], calWin[3], 2, ILI9341_RED);
            tft.setCursor(calWin[0] + 2, calWin[1] + 10);
            tft.setTextSize(2);
            tft.setTextColor(ILI9341_BLUE);
            tft.print("Calib");
          }
        //get position and check if touched
        while (1)
          {
            tch = tevent.getTouchPos(pP, ppRaw);
            if (tch)
              {
                sprintf(text,"x %3i y %3i rx %5i ry %5i", p.x, p.y, pRaw.x, pRaw.y);
                xpos = 30;
                if ((pRaw.x < 1000) && (pRaw.y < 1000))
                  {
                    ypos = 30;
                    if (doCal == true) calP[0] = pRaw;
                  }
                else if ((pRaw.x < 1000) && (pRaw.y > 3000))
                  {
                    ypos = 90;
                    if (doCal == true) calP[2] = pRaw;
                  }
                else if ((pRaw.x > 3000) && (pRaw.y > 3000))
                  {
                    ypos = 120;
                    if (doCal == true) calP[3] = pRaw;
                  }
                else if ((pRaw.x > 3000) && (pRaw.y < 1000))
                  {
                    ypos = 60;
                    if (doCal == true) calP[1] = pRaw;
                  }
                else
                  {
                    ypos = 150;
                    if (doCal == true)
                      {
                        if (   (p.x > calWin[0]) && (p.x < calWin[0] + calWin[2])
                            && (p.y > calWin[1]) && (p.y < calWin[1] + calWin[3])
                           )
                            cxmin = (calP[0].x + calP[2].x)/2;
                          {
                            cxmax = (calP[1].x + calP[3].x)/2;
                            cymin = (calP[0].y + calP[1].y)/2;
                            cymax = (calP[2].y + calP[3].y)/2;
                            int16_t dRawX = cxmax - cxmin;
                            int16_t dRawY = cymax - cymin;
                            cxmin -= dRawX/30;
                            cxmax += dRawX/30;
                            cymin -= dRawY/22;
                            cymax += dRawY/22;
                            tevent.calibrate(cxmin, cymin, cxmax, cymax);
                            Serial.print("Calib done");
                            doCal = false;
                            tft.fillRoundRect(calWin[0], calWin[1], calWin[2], calWin[3], 2, ILI9341_BLACK);

                          }
                      }
                  }
                tft.setCursor(xpos,ypos);
                tft.setTextSize(1);
                tft.getTextBounds(&text[0], xpos, ypos, &wx, &wy, &ww, &wh);
                tft.fillRect(wx, wy, ww, wh, ILI9341_BLACK);
                tft.print(text);
                delay(500);
              }
            delay(500);

          }


        //tft.setTextxposor(ILI9341_GREEN);
        #ifdef XXXX
              tft.setCursor(myxpos, myypos);
              char buf[50];
              sprintf(buf,"xpos: %2i ypos: %2i",myxpos,myypos);
              tft.print(buf);
              delay (1000);
              myypos += 20;
              if (++myTextSize > 6)
                {
                  myTextSize = 1;
                  myypos += 20;
                  tft.fillScreen(ILI9341_BLACK);
                }
          //Auf Touch Ereignisse pruefen
          tevent.pollTouchScreen();
          /* Immer wenn die Zeit intterval erreicht ist, wird das aktuelle Tetris-Teil
             um eine Zeile nach unten geschoben falls das moeglich ist.
             Kommt es dabei zu einer Kollision oder wird der untere Rand erreicht,
             so wird das Teil nicht verschoben sondern am Spielfeld verankert.
             Vollstaendige Zeilen werden entfernt und ein neues Teil am oberen
             Rand eingefuegt
           */
          if ((curPiece > 0) && ((millis()-last) > interval))
            {
              last = millis();
              if (checkPiece(curPiece,curxpos,curypos+1))
                {
                  hidePiece(curPiece,curxpos,curypos);
                  curypos++;
                  showPiece(curPiece,curxpos,curypos);
                }
                else
                {
                  putPiece(curPiece,curxpos,curypos);
                  removeComplete();
                  newPiece();
                }
            }

          #endif
      }

    /* Funktion zeigt in der Kopfleiste den aktuellen Score und den Level an
       Abhaengig vom Score wird der Level hinaufgesetzt und das Intervall verringert
      */
    void _displayScore()
      {
        if      (score < 10)     {level = 1; interval = 900;}
        else if (score < 100)    {level = 2; interval = 700;}
        else if (score < 1000)   {level = 3; interval = 500;}
        else if (score < 10000)  {level = 4; interval = 300;}
        else if (score < 100000) {level = 5; interval = 100;}
        tft.fillRect(0,0,240,20,BACKGROUND);
        tft.setTextSize(2);
        tft.setTextColor(ILI9341_BLACK);
        tft.setCursor(5,4);
        char buf[50];
        sprintf(buf,"SC: %8i LV: %i",score,level);
        tft.print(buf);
      }

    /*  Funktion um ein Tetris-Teil zu drehen. Der Parameter ist die Nummer des Teils
       das gedreht werden soll. R�ckgabewert ist der Index des gedrehten Teils
      */
    uint8_t _rotate(uint8_t pc)
      {
        uint8_t res = 0;
        switch (pc) {
          case 1:  res = 1; break;
          case 2:  res = 3; break;
          case 3:  res = 2; break;
          case 4:  res = 5; break;
          case 5:  res = 4; break;
          case 6:  res = 7; break;
          case 7:  res = 6; break;
          case 8:  res = 9; break;
          case 9:  res = 10; break;
          case 10: res = 11; break;
          case 11: res = 8; break;
          case 12: res = 13; break;
          case 13: res = 14; break;
          case 14: res = 15; break;
          case 15: res = 12; break;
          case 16: res = 17; break;
          case 17: res = 18; break;
          case 18: res = 19; break;
          case 19: res = 16; break;
        }
        return res;
      }

    //Funktion testet ob eine Zeile voll belegt ist
    boolean _yposComplete(int8_t rpg)
      {
        if ((rpg >= 0) && (rpg < yposS))
          {
            boolean res = true;
            uint8_t c = 0;
            //wenn ein Block nicht belegt ist (Farbe 0), ist die Zeile nicht vollstaendig
            while (res && (c < xposUMNS))
              {
                if (playGround[rpg][c] == 0) res = false;
                c++;
              }
            return res;
          }
      }

    /* Funkzion prueft ob es zwischen der Zeile rpc des Tetris-Teils pc und
       der Zeile rpg des Spielfelds ab der Position cpg Kolklisionen gibt.
       Wenn eine Kollision auftritt oder die letzte Zeile des Spielfelds
       erreicht wurde wird falsch zurueckgegeben
      */
    boolean _checkypos(uint8_t pc, int8_t rpc, int8_t cpg, int8_t rpg)
      {
        boolean res = true;
        if (rpg >= yposS) return false;
        if (rpg < 0) return true;
        for (uint8_t i = 0; i<4; i++)
          {
            if (piece[pc][rpc*4 + i]>0)
              {
                if (((cpg+i) < 0) || ((cpg+i) >= xposUMNS))
                  {
                    res = false;
                  }
                  else
                  {
                    if (playGround[rpg][cpg+i] > 0) res = false;
                  }
              }
          }
        return res;
      }

    /* Funktion prueft ob das Tetris Teil pc am Spielfeld an der Position
       Zeile rpg Spalte cpg (linke untere Ecke des Teils) Kollisionen auftreten
      */
    boolean _checkPiece(uint8_t pc, int8_t cpg, int8_t rpg)
      {
        boolean res = true;
        uint8_t rpc = 0;
        while (res && (rpc < 4))
          {
            res = checkypos(pc,rpc,cpg,rpc+rpg-3);
            //Serial.printf("check %i = %i\n",rpc+rpg-3,res);
            rpc++;
          }
        return res;
      }

    /* Funktion zeigt einen Block des Spielfeldes in Zeile y Spalte x mit der Farbe xposor an
       xposor ist die Farbe im 565 Format fuer das Display */
    void _showBlock(uint8_t x, uint8_t y, uint16_t xposor)
      {
        tft.fillRect(LEFTMARGIN+x*BLOCKSIZE+2,TOPMARGIN+y*BLOCKSIZE+2,BLOCKSIZE-4,BLOCKSIZE-4,xposor);
        tft.drawRect(LEFTMARGIN+x*BLOCKSIZE+1,TOPMARGIN+y*BLOCKSIZE+1,BLOCKSIZE-2,BLOCKSIZE-2,BORDER);
      }

    // Funktion faellt einen Block des Spielfeldes in Zeile y Spalte x mit der Hintergrundfarbe
    void hideBlock(uint8_t x, uint8_t y)
      {
        tft.fillRect(LEFTMARGIN+x*BLOCKSIZE,TOPMARGIN+y*BLOCKSIZE,BLOCKSIZE,BLOCKSIZE,NOPIECE);
      }

    /* Funktion zeigt das Tetris-Teil pc in Zeile rpg, Spalte cpg (Linke untere Ecke) an
       Die Farbe wird der Definition des Tetris-Teils entnommen
      */
    void _showPiece(uint8_t pc, uint8_t cpg, uint8_t rpg)
      {
        uint8_t xposor;
        for (uint8_t r = 0; r<4; r++)
          {
            for (uint8_t c = 0; c<4; c++)
              {
                xposor = piece[pc][r*4+c];
                if ((xposor > 0) && ((3-r+rpg) >= 0))
                  showBlock(cpg+c,rpg-3+r,colorBlock[xposor]);
          }
        }
      }

    /* Funktion faellt die belegten Bloecke des Tetris-Teil pc in Zeile rpg,
       Spalte cpg (Linke untere Ecke) an mit Hintergrundfarbe
     */
    void _hidePiece(uint8_t pc, int8_t cpg, int8_t rpg)
      {
        uint8_t xposor;
        for (uint8_t r = 0; r<4; r++)
          {
            for (uint8_t c = 0; c<4; c++)
              {
                xposor = piece[pc][r*4+c];
                if ((xposor > 0) && ((3-r+rpg) >= 0)) hideBlock(cpg+c,rpg-3+r);
              }
          }
      }

    /*funktion faellt die Zeile ypos des Spielfelds mit Hintergrundfarbe und
      loescht alle Eintraege fuer diese Zeile im Spielfeld-Speicher
      */
    void _deleteypos(int8_t ypos)
      {
        tft.fillRect(LEFTMARGIN,TOPMARGIN+ypos*BLOCKSIZE,xposUMNS * BLOCKSIZE,BLOCKSIZE,NOPIECE);
        for (uint8_t i =0; i<xposUMNS; i++) playGround[ypos][i]=0;
      }

    /* Funktion kopiert die Zeile srcypos in die Zeile dstypos
       Die Anzeige der Zielzeile wird vorher geloescht. Beim
       kopieren wird die Quellzeile in der Zielzeile angezeigt
      */
    void _copyypos(int8_t srcypos, int8_t dstypos)
      {
        uint8_t xpos;
        deleteypos(dstypos);
        if ((srcypos < dstypos) && (srcypos >=0) && (dstypos < yposS)) {
          for (uint8_t c = 0; c < xposUMNS; c++) {
            xpos = playGround[srcypos][c];
            playGround[dstypos][c] = xpos;
            if (xpos > 0) showBlock(c,dstypos,colorBlock[xpos]);
          }
        }
      }

    /* Funktion zeigt alle Bloecke des Spielfeldes mit der Farbe ALLON an.
       Nach einer Pause von 500 ms wird das Sielfeld komplett geloescht
      */
    void _clearBoard()
      {
        for (uint8_t x = 0; x<xposUMNS; x++) {
          for (uint8_t y = 0; y<yposS; y++) {
            showBlock(x,y,ALLON);
          }
        }
        delay(500);
        for (uint8_t i = 0; i<yposS; i++) {
          deleteypos(i);
        }
      }

    /* Funktion uebertraegt das Tetris-Teil pc in den Spielfeldspeicher in der Zeile
       rpg an der Spalte cpg (linke untere Ecke)
      */
    void _putPiece(uint8_t pc, int8_t cpg, int8_t rpg)
      {
        uint8_t xposor;
        for (uint8_t r = 0; r<4; r++)
          {
            for (uint8_t c = 0; c<4; c++)
              {
                xposor = piece[pc][r*4+c];
                if ((xposor > 0) && ((3-r+rpg) >= 0)) playGround[rpg-3+r][cpg+c] = xposor;
              }
          }
      }

    /* Ein neues Tetristeil wird am oberen Rand des Spielfeldes eingefuegt.
       Welches Teil und in welcher Spalte wird als Zufallszahl ermittelt
       Hat das neue Teil keinen Platz am Spielfeld, so ist das Spiel zu Ende
      */
    boolean _newPiece()
      {
        uint8_t pc = random(1,20);
        uint8_t cpg = random(0,xposUMNS-4);
        boolean res = checkPiece(pc,cpg,3);
        curPiece=0;
        if (res)
          {
            curPiece = pc;
            curxpos = cpg;
            curypos = 0;
            showPiece(pc,cpg,0);
            score += 4;
            displayScore();
          }
          else
          {
            tft.setTextSize(3);
            tft.setCursor(LEFTMARGIN+xposUMNS*BLOCKSIZE/2-79,TOPMARGIN+yposS*BLOCKSIZE/2-10);
            tft.setTextColor(ILI9341_BLACK);
            tft.print("GAME OVER");
            tft.setCursor(LEFTMARGIN+xposUMNS*BLOCKSIZE/2-81,TOPMARGIN+yposS*BLOCKSIZE/2-12);
            tft.setTextColor(ILI9341_YELLOW);
            tft.print("GAME OVER");
          }
      }

    /* Funktion ermittelt komplett gef�llte Zeilen am Spielfeld und entfernt diese
       Darueberliegende Zeilen werden nach unten verschoben
     */
    void _removeComplete()
      {
        uint8_t s=yposS-1;
        int8_t d= yposS-1;
        while (d >= 0)
          {
            if (yposComplete(d))
              {
                s--;
                score += 10;
                copyypos(s,d);
              }
              else
              {
                if ((s < d) && (s >=0))
                  {
                    Serial.printf("copy %i to %i\n",s, d);
                    copyypos(s,d);
                  }
                s--;
                d--;
              }
          }
       displayScore();
      }

    /* Funktion beginnt ein neues Spiel. Der score wird auf 0 gesetzt, das Spielfeld
       geloescht und mit einem neuen Tetris Teil gestartet
     */
    void _newGame()
      {
        score=0;
        displayScore();
        clearBoard();
        newPiece();
      }

    /* Callbackfunktion fuer Touchscreen Ereignis Klick
       Diese Funktion wird immer dann aufgerufen, wenn der Bildschirm
       kurz beruehrt wird. p gibt die Position des Beruehrungspunktes an
     */
    void _onClick(TS_Point p)
      {
        if (p.y < 80)
          { //Klick im obersten Viertel des Displays
            newGame();
          }
          else if (p.y > 240)
          { //Klick im untersten Viertel
            uint8_t pc = curPiece;
            int8_t c = curxpos;
            if (p.x < 80)
              { //Klick im linken Drittel -> nach links schieben
                c--;
              }
              else if (p.x <160)
              { //Klick im mittleren Drittel -> drehen
                pc = rotate(pc);
              }
              else
              { //Klick im rechten Drittel -> nach rechts schieben
                c++;
              }
            /* nach Aenderung der Position wird auf Kollision geprueft
               nur wenn keine Kollision auftritt, wird die Bewegung
               ausgefuehrt
             */
            if (checkPiece(pc,c,curypos))
              {
                hidePiece(curPiece,curxpos,curypos);
                curPiece = pc;
                curxpos = c;
                showPiece(curPiece,curxpos,curypos);
              }
          }
      }
#endif // ORIGINAL
