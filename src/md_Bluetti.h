#ifndef _MD_BLUETTI_H_
  #define _MD_BLUETTI_H_

  // --- includes
    #include <BLEDevice.h>
    #include <md_util.h>

  // --- Bluetti declarations
    #define UNKNOWN_DEVICE -1
    #define AC300           1
    #define AC200M          2
    #define EP500           3
    #define EB3A            4
    #define EP500P          5
    #define AC500           6
    //#define POWER_STATION(type) (BLUETTI_TYPE==BLUETTI_##type)
    #define POWER_STATION   AC300
    #ifndef BLUETTI_TYPE
        #define BLUETTI_TYPE BLUETTI_AC300
      #endif

    #define DEVICE_NAME "BLUETTI-MQTT"
    //#define BLUETTI_TYPE BLUETTI_EP500P

    typedef struct
      {
        //int  salt = EEPROM_SALT;
        //char mqtt_server[40] = "127.0.0.1";
        char mqtt_server[40] = "10.0.0.230";
        char mqtt_port[6]  = "1883";
        char mqtt_username[40] = "";
        char mqtt_password[40] = "";
        //char bluetti_device_id[40] = "Bluetti Blutetooth Id";
        char bluetti_device_id[40] = "AC3002235000574654";
        char ota_username[40] = "MAMD-HomeG";
        char ota_password[40] = "ElaNanniRalf3";
      } ESPBluettiSettings;

    #define WRAP_MAX 100
    typedef struct // wrapper
      {
        uint8_t myarr[WRAP_MAX];
        int mysize;
      } wrapper;

  // --- spezial field declarations
    enum field_types
      {
         UINT_FIELD,
         BOOL_FIELD,
         ENUM_FIELD,
         STRING_FIELD,
         DECIMAL_ARRAY_FIELD,
         DECIMAL_FIELD,
         VERSION_FIELD,
         SN_FIELD,
         TYPE_UNDEFINED
      };

    enum field_names
      {
        DC_OUTPUT_POWER,
        DC_OUTPUT_ON,
        AC_OUTPUT_POWER,
        AC_OUTPUT_ON,
        POWER_GENERATION,
        TOTAL_BATTERY_PERCENT,
        DC_INPUT_POWER,
        AC_INPUT_POWER,
        PACK_VOLTAGE,
        SERIAL_NUMBER,
        ARM_VERSION,
        DSP_VERSION,
        DEVICE_TYPE,
        UPS_MODE,
        AUTO_SLEEP_MODE,
        GRID_CHANGE_ON,
        FIELD_UNDEFINED,
        INTERNAL_AC_VOLTAGE,
        INTERNAL_CURRENT_ONE,
        PACK_NUM_MAX
      };

    typedef struct device_field_data
      {
        enum field_names f_name;
        uint8_t f_page;
        uint8_t f_offset;
        int8_t f_size;
        int8_t f_scale;
        int8_t f_enum;
        enum field_types f_type;
      } device_field_data_t;

    /* Not implemented yet
      enum output_mode
        {
            STOP = 0,
            INVERTER_OUTPUT = 1,
            BYPASS_OUTPUT_C = 2,
            BYPASS_OUTPUT_D = 3,
            LOAD_MATCHING = 4
        };

      enum ups_mode
        {
            CUSTOMIZED = 1,
            PV_PRIORITY = 2,
            STANDARD = 3,
            TIME_CONTROl = 4
        };

      enum auto_sleep_mode
        {
          THIRTY_SECONDS = 2,
          ONE_MINNUTE = 3,
          FIVE_MINUTES = 4,
          NEVER = 5
        };
     */

    #if (POWER_STATION == AC300)
        static device_field_data_t bluetti_device_state[] =
          {
            //{ FIELD_NAME,         PAGE, OFFS, SIZE, SCALE (e.g. decimal value, def 0) ,
                                                      // ENUM (if data is enum, defaults to 0),
                                                         // FIELD_TYPE }
            /*Page 0x00 Core */
            {DEVICE_TYPE,           0x00, 0x0A, 0x07, 0, 0, STRING_FIELD},
            {SERIAL_NUMBER,         0x00, 0x11, 0x04, 0 ,0, SN_FIELD},
            {ARM_VERSION,           0x00, 0x17, 0x02, 0, 0, VERSION_FIELD},
            {DSP_VERSION,           0x00, 0x19, 0x02, 0, 0, VERSION_FIELD},
            {DC_INPUT_POWER,        0x00, 0x24, 0x01, 0, 0, UINT_FIELD},
            {AC_INPUT_POWER,        0x00, 0x25, 0x01, 0, 0, UINT_FIELD},
            {AC_OUTPUT_POWER,       0x00, 0x26, 0x01, 0, 0, UINT_FIELD},
            {DC_OUTPUT_POWER,       0x00, 0x27, 0x01, 0, 0, UINT_FIELD},
            {POWER_GENERATION,      0x00, 0x29, 0x01, 1, 0, DECIMAL_FIELD},
            {TOTAL_BATTERY_PERCENT, 0x00, 0x2B, 0x01, 0, 0, UINT_FIELD},
            {AC_OUTPUT_ON,          0x00, 0x30, 0x01, 0, 0, BOOL_FIELD},
            {DC_OUTPUT_ON,          0x00, 0x31, 0x01, 0, 0, BOOL_FIELD},

            //Page 0x00 Details
            {INTERNAL_AC_VOLTAGE,   0x00, 0x47, 0x01, 1, 0, DECIMAL_FIELD},
            {INTERNAL_CURRENT_ONE,  0x00, 0x48, 0x01, 1, 0, DECIMAL_FIELD},

            //Page 0x00 Battery Details
            {PACK_NUM_MAX,          0x00, 0x5B, 0x01, 0, 0, UINT_FIELD },

            //Page 0x00 Battery Data
            {PACK_VOLTAGE,          0x00, 0x62, 0x01, 2 ,0 ,DECIMAL_FIELD},
          };

        static device_field_data_t bluetti_device_command[] =
          {
            /*Page 0x00 Core */
            {AC_OUTPUT_ON,          0x0B, 0xBF, 0x01, 0, 0, BOOL_FIELD},
            {DC_OUTPUT_ON,          0x0B, 0xC0, 0x01, 0, 0, BOOL_FIELD}
          };

        static device_field_data_t bluetti_polling_command[] =
          {
            {FIELD_UNDEFINED,       0x00, 0x0A, 0x28 ,0 , 0, TYPE_UNDEFINED},
            {FIELD_UNDEFINED,       0x00, 0x46, 0x15 ,0 , 0, TYPE_UNDEFINED},
            {FIELD_UNDEFINED,       0x0B, 0xB9, 0x3D ,0 , 0, TYPE_UNDEFINED}
          };
      #endif // AC300
#endif
