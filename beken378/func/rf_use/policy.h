typedef enum
{
    SCEN_BLE_NORMAL,
    SCEN_BLE_CONNECTING,
    SCEN_BLE_MAX,
}ble_scenarios_t;

typedef enum
{
    SCEN_WIFI_NORMAL,
    SCEN_WIFI_CONNECTING,
    SCEN_WIFI_MAX,
}wifi_scenarios_t;

typedef enum
{
    RF_EVENT_WIFI_DATA_SEND,
    RF_EVENT_MAX,
}rf_event_type;

OSStatus get_event_priority(uint8_t event, uint8_t *priority);
OSStatus set_event_priority(uint8_t event, uint8_t priority);
void set_ble_scenarios(ble_scenarios_t scenarios);
void set_wifi_scenarios(wifi_scenarios_t scenarios);
ble_scenarios_t get_ble_scanarios(void);
wifi_scenarios_t get_wifi_scanarios(void);
