#ifndef _MD_BTOOTH_H_
  #define _MD_BTOOTH_H_
  #include <Arduino.h>
  #include <md_Bluetti.h>
  #include <prj_conf_tinybluetti.h>
  #include <BLEDevice.h>
  #include <md_PayloadParser.h>

  #define BLUETOOTH_QUERY_MESSAGE_DELAY 3000
  #define MAX_DISCONNECTED_TIME_UNTIL_REBOOT 5 //device will reboot when wlan/BT/MQTT is not connectet within x Minutes
  #define SLEEP_TIME_ON_BT_NOT_AVAIL 2 //device will sleep x minutes if restarted is triggered by bluetooth error
                                       //set to 0 to disable
  #define DEVICE_STATE_UPDATE  5
  #define MSG_VIEWER_ENTRY_COUNT 20 //number of lines for web message viewer
  #define MSG_VIEWER_REFRESH_CYCLE 5 //refresh time for website data in seconds

  typedef struct __attribute__ ((packed))
    {
      uint8_t prefix;            // 1 byte
      uint8_t field_update_cmd;  // 1 byte
      uint8_t page;              // 1 byte
      uint8_t offset;            // 1 byte
      uint16_t len;              // 2 bytes
      uint16_t check_sum;        // 2 bytes
    } bt_command_t;
  // --- prototypes
    void initBluetooth();
    void handleBluetooth();
    bool connectToServer();
    void handleBTCommandQueue();
    void sendBTCommand(bt_command_t command);
    bool isBTconnected();
    unsigned long getLastBTMessageTime();
    wrapper slice(const uint8_t* arr, int size, uint8_t include, uint8_t exclude);
    String map_field_name(enum field_names f_name);
#endif
