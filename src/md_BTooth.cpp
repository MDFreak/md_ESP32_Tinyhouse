    //#include <md_Bluetti.h>
    //#include <BLEDevice.h>
    //#include <md_defines.h>
#include <md_BTooth.h>
    //#include "utils.h"
    //#include "BWifi.h"

#ifndef DEBUG
    #define DEBUG
  #endif
// The remote Bluetti service we wish to connect to.
  static BLEUUID serviceUUID(BLUETTI_UUID_SERVICE);
// The characteristics of Bluetti Devices
  static BLEUUID WRITE_UUID(BLUETTI_UUID_WRITE);
  static BLEUUID NOTIFY_UUID(BLUETTI_UUID_NOTIFY);

static char bluetti_device_id[40] = "AC3002235000574654";
static ESPBluettiSettings _settings;

static boolean doConnect  = false;
static boolean connected  = false;
static boolean doScan     = false;

static BLERemoteCharacteristic* pRemoteWriteCharacteristic;
static BLERemoteCharacteristic* pRemoteNotifyCharacteristic;
static BLEAdvertisedDevice*     bluettiDevice;
QueueHandle_t commandHandleQueue;
QueueHandle_t sendQueue;
BLEAdvertisedDevice devBluetti = BLEAdvertisedDevice();
BLEScan* plocScan = NULL;
BLEScan* ptmpScan = NULL;

unsigned long lastBTMessage = 0;
int pollTick = 0;


struct command_handle
  {
    uint8_t page;
    uint8_t offset;
    int length;
  };
wrapper slice(const uint8_t* arr, int size, uint8_t include, uint8_t exclude)
  {
    wrapper result = { .myarr = {0}, .mysize = 0 };
    if (include >= 0 && exclude <= size)
      {
        int count = 0;
        for (int i = include; i < exclude; i++)
          {
            result.myarr[count] = arr[i];
            count++;
          }
        result.mysize = exclude - include;
        return result;
      }
    else
      {
        printf("Array index out-of-bounds\n");
        result.mysize = -1;
        return result;
      }
  }

String map_field_name(enum field_names f_name)
  {
   switch(f_name)
    {
      case DC_OUTPUT_POWER:
        return "dc_output_power";
        break;
      case AC_OUTPUT_POWER:
        return "ac_output_power";
        break;
      case DC_OUTPUT_ON:
        return "dc_output_on";
        break;
      case AC_OUTPUT_ON:
        return "ac_output_on";
        break;
      case POWER_GENERATION:
        return "power_generation";
        break;
      case TOTAL_BATTERY_PERCENT:
        return "total_battery_percent";
        break;
      case DC_INPUT_POWER:
        return "dc_input_power";
        break;
      case AC_INPUT_POWER:
        return "ac_input_power";
        break;
      case PACK_VOLTAGE:
        return "pack_voltage";
        break;
      case SERIAL_NUMBER:
        return "serial_number";
        break;
      case ARM_VERSION:
        return "arm_version";
        break;
      case DSP_VERSION:
        return "dsp_version";
        break;
      case DEVICE_TYPE:
        return "device_type";
        break;
      case UPS_MODE:
        return "ups_mode";
        break;
      case AUTO_SLEEP_MODE:
        return "auto_sleep_mode";
        break;
      case GRID_CHANGE_ON:
        return "grid_change_on";
        break;
      case INTERNAL_AC_VOLTAGE:
        return "internal_ac_voltage";
        break;
      case INTERNAL_CURRENT_ONE:
        return "internal_current_one";
        break;
      case PACK_NUM_MAX:
        return "pack_max_num";
        break;
      default:
        return "unknown";
        break;
    }
  }
class MyClientCallback : public BLEClientCallbacks
  {
    void onConnect(BLEClient* pclient)
      {
        STXT("  callback onConnect ");
      }

    void onDisconnect(BLEClient* pclient)
      {
        connected = false;
        Serial.println(F("onDisconnect"));
      }
  };
MyClientCallback  locClientCallback   = MyClientCallback();
MyClientCallback* plocClientCallback  = &locClientCallback;
/**
 * Scan for BLE servers and find the first one that advertises the service we are looking for.
 */
class BluettiAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks
  {
    void onResult(BLEAdvertisedDevice advertisedDevice)
      {
        SVAL("BLE Advertised Device found: ", advertisedDevice.toString().c_str());
              //Serial.print(F("BLE Advertised Device found: "));
              //Serial.println(advertisedDevice.toString().c_str());
              //ESPBluettiSettings settings = get_esp32_bluetti_settings();
        // We have found a device, let us now see if it contains the service we are looking for.
        if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID) && advertisedDevice.getName().compare(_settings.bluetti_device_id))
          {
            STXT(" onResult stop scan ");
            BLEDevice::getScan()->stop();
            bluettiDevice = new BLEAdvertisedDevice(advertisedDevice);
            // *bluettiDevice = advertisedDevice;
            SVAL("   onResult  Bluetti &Bluetti &Device  ", bluettiDevice->getName().c_str());
            doConnect = true;
            doScan = true;
          }
      }
  };
BluettiAdvertisedDeviceCallbacks  BluettiAdvDevCallbacks  =  BluettiAdvertisedDeviceCallbacks();
BluettiAdvertisedDeviceCallbacks* pBluettiAdvDevCallbacks = &BluettiAdvDevCallbacks;

void initBluetooth()
  {
    SVAL(" -> initBluetooth free ", (uint32_t) ESP.getFreeHeap());
    BLEDevice::init("");
    SVAL(" 0 initBluetooth free ", (uint32_t) ESP.getFreeHeap());
    //BLEScan* pBLEScan = BLEDevice::getScan();
    plocScan = BLEDevice::getScan();
    //pBLEScan->setAdvertisedDeviceCallbacks(new BluettiAdvertisedDeviceCallbacks());
    SVAL(" 1 initBluetooth free ", (uint32_t) ESP.getFreeHeap());
    //pBLEScan->setAdvertisedDeviceCallbacks(pBluettiAdvDevCallbacks);
    plocScan->setAdvertisedDeviceCallbacks(pBluettiAdvDevCallbacks);
    SVAL(" 2 initBluetooth free ", (uint32_t) ESP.getFreeHeap());
    //pBLEScan->setInterval(1349);
    plocScan->setInterval(1349);
    SVAL(" 3 initBluetooth free ", (uint32_t) ESP.getFreeHeap());
    //pBLEScan->setWindow(449);
    plocScan->setWindow(449);
    SVAL(" 4 initBluetooth free ", (uint32_t) ESP.getFreeHeap());
    //pBLEScan->setActiveScan(true);
    plocScan->setActiveScan(true);
    SVAL(" 5 initBluetooth free ", (uint32_t) ESP.getFreeHeap());
    //pBLEScan->start(5, false);
    plocScan->start(5, false);
    SVAL(" 6 initBluetooth free ", (uint32_t) ESP.getFreeHeap());
    //bluettiDevice = &devBluetti;
    commandHandleQueue = xQueueCreate( 5, sizeof(bt_command_t ) );
    sendQueue = xQueueCreate( 5, sizeof(bt_command_t) );
    STXT("    initBluetooth -> ");
  }
static void notifyCallback( BLERemoteCharacteristic* pBLERemoteCharacteristic,
                            uint8_t* pData, size_t length,   bool isNotify)
  {
    #ifdef DEBUG
        Serial.println("F01 - Write Response");
        /* pData Debug... */
        serHEXdump(pData, length);
                /*
                  for (int i=1; i<=length; i++)
                    {
                      Serial.printf("%02x", pData[i-1]);
                      if(i % 2 == 0) { Serial.print(" "); }
                      if(i % 16 == 0){ Serial.println();  }
                    }
                  Serial.println();
                 */
      #endif
    bt_command_t command_handle;
    if(xQueueReceive(commandHandleQueue, &command_handle, 500))
      {
        parse_bluetooth_data(command_handle.page, command_handle.offset, pData, length);
      }
  }
bool connectToServer()
  {
    // start connection
      SVAL("Forming a connection to ", bluettiDevice->getAddress().toString().c_str());
      BLEClient*  pClient  = BLEDevice::createClient();
      STXT("                        - Created client");
              //pClient->setClientCallbacks(new MyClientCallback());
      pClient->setClientCallbacks(plocClientCallback);
    // Connect to the remove BLE Server.
      pClient->connect(bluettiDevice);  // if you pass BLEAdvertisedDevice instead of address, it will be recognized type of peer device address (public or private)
      STXT("                        - Connected to server");
      pClient->setMTU(517); //set client to request maximum MTU from server (default is 23 otherwise)

    // Obtain a reference to the service we are after in the remote BLE server.
      BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
      if (pRemoteService == nullptr)
        {
          SVAL("Failed to find our service UUID: ", serviceUUID.toString().c_str());
          pClient->disconnect();
          return false;
        }
      STXT("                        - Found our service");
    // Obtain a reference to the characteristic in the service of the remote BLE server.
      pRemoteWriteCharacteristic = pRemoteService->getCharacteristic(WRITE_UUID);
      if (pRemoteWriteCharacteristic == nullptr)
        {
          SVAL("Failed to find our characteristic UUID: ", WRITE_UUID.toString().c_str());
          pClient->disconnect();
          return false;
        }
      STXT("                        - Found our Write characteristic");
    // Obtain a reference to the characteristic in the service of the remote BLE server.
      pRemoteNotifyCharacteristic = pRemoteService->getCharacteristic(NOTIFY_UUID);
      if (pRemoteNotifyCharacteristic == nullptr)
        {
          SVAL("Failed to find our characteristic UUID: ", NOTIFY_UUID.toString().c_str());
          pClient->disconnect();
          return false;
        }
      STXT("                        - Found our Notifyite characteristic");
    // Read the value of the characteristic.
      if(pRemoteWriteCharacteristic->canRead())
        {
          std::string value = pRemoteWriteCharacteristic->readValue();
          SVAL("The characteristic value was: ", value.c_str());
        }
      if(pRemoteNotifyCharacteristic->canNotify())
        { pRemoteNotifyCharacteristic->registerForNotify(notifyCallback); }
    connected = true;
    return true;
  }
void handleBTCommandQueue()
  {
    bt_command_t command;
    if(xQueueReceive(sendQueue, &command, 0))
      {
        #ifdef DEBUG
            Serial.print("Write Request FF02 - Value: ");
            for(int i=0; i<8; i++)
              {
                 if ( i % 2 == 0) { Serial.print(" "); };
                 Serial.printf("%02x", ((uint8_t*)&command)[i]);
              }
            Serial.println("");
        #endif
        pRemoteWriteCharacteristic->writeValue((uint8_t*)&command, sizeof(command),true);
      };
  }
void sendBTCommand(bt_command_t command)
  {
    bt_command_t cmd = command;
    xQueueSend(sendQueue, &cmd, 0);
  }
uint16_t bt_crc16_update(uint16_t crc, uint8_t a)
  {
    int i;
    crc ^= a;
    for (i = 0; i < 8; ++i)
      {
          if (crc & 1)
        crc = (crc >> 1) ^ 0xA001;
          else
        crc = (crc >> 1);
      }
    return crc;
  }
uint16_t bt_modbus_crc  (uint8_t buf[], int len)
  {
    unsigned int crc = 0xFFFF;
    for (unsigned int i = 0; i < len; i++)
     {
      crc = bt_crc16_update(crc, buf[i]);
     }
     return crc;
  }
void handleBluetooth()
  {
    STXT(" -> handleBluetooth ");
    if (doConnect == true)
      {
        if (connectToServer())
          {
            STXT("We are now connected to the Bluetti BLE Server.");
          }
          else
          {
            STXT("We have failed to connect to the server; there is nothing more we will do.");
          }
        doConnect = false;
      }
    if ((millis() - lastBTMessage) > (MAX_DISCONNECTED_TIME_UNTIL_REBOOT * 60000))
      {
        STXT("BT is disconnected over allowed limit, reboot device");
        #ifdef SLEEP_TIME_ON_BT_NOT_AVAIL
            //esp_deep_sleep_start();
        #else
            ESP.restart();
        #endif
      }
    if (connected)
      {
        STXT("   handleBluetooth connected " );
        // poll for device state
        if ( millis() - lastBTMessage > BLUETOOTH_QUERY_MESSAGE_DELAY)
          {
            SVAL("   handleBluetooth command tick ", pollTick);
            bt_command_t command;
            command.prefix = 0x01;
            command.field_update_cmd = 0x03;
            command.page = bluetti_polling_command[pollTick].f_page;
            command.offset = bluetti_polling_command[pollTick].f_offset;
            command.len = (uint16_t) bluetti_polling_command[pollTick].f_size << 8;
            command.check_sum = bt_modbus_crc((uint8_t*)&command,6);
            S2VAL(" command  pollTick  page ", pollTick, bluetti_polling_command[pollTick].f_page);
            xQueueSend(commandHandleQueue, &command, portMAX_DELAY);
            xQueueSend(sendQueue, &command, portMAX_DELAY);
            if (pollTick == sizeof(bluetti_polling_command)/sizeof(device_field_data_t)-1 )
              { pollTick = 0; }
            else
              { pollTick++; }
            lastBTMessage = millis();
          }
        handleBTCommandQueue();
      }
    else if(doScan)
      {
        BLEDevice::getScan()->start(0);
      }
    STXT("    handleBluetooth -> ");
  }
bool isBTconnected()
  { return connected; }
unsigned long getLastBTMessageTime()
  { return lastBTMessage; }


