#pragma once

#include "esphome/components/esp32_ble_server/ble_characteristic.h"
#include "esphome/components/esp32_ble_server/ble_server.h"
#include "esphome/core/component.h"
#include "esphome/core/helpers.h"
#include "esphome/core/preferences.h"

#ifdef USE_ESP32

namespace esphome {
namespace ebe_ble_server {

using namespace esp32_ble_server;

//place your services and characteristics here. 
//my example code only implements one service and one characteristic.
static const char *const MY_SERVICE_UUID = "89a40b48-200b-47d5-9429-000000000000";
static const char *const MY_CHARACTERISTIC_UUID = "89a40b48-200b-47d5-9429-000900000000";

class EBEBleServer : public Component, public BLEServiceComponent {
 public:

  //this is the structure that is used to carry the data sent by the client.
  struct MyData {
  float float1_;  // 4 bytes for the first float
  float float2_;  
  float float3_;  
  float float4_;
  int int1_;
  };

  EBEBleServer();
  void dump_config() override;
  void loop() override;
  void setup() override;
  void setup_characteristics();
  void on_client_disconnect() override;
  void on_client_connect() override;
  const unsigned long check_interval = 60000;  // 60 seconds
 

  float get_setup_priority() const override;
  void start() override;
  void stop() override;

  MyData data_;
  long last_check_time;
  long last_data_time;
  //ESP32BLE *ble_;

 protected:
  enum State : uint8_t {
    STATE_STOPPED = 0x00,
    STATE_RUNNING = 0x01,
  };

  bool should_start_{true};
  bool setup_complete_{false};

  std::vector<uint8_t> incoming_data_;

  //std::shared_ptr<esphome::esp32_ble_server::BLEService> service_;
  BLEService *service_;
  BLECharacteristic *my_characteristic_;

  State state_{STATE_STOPPED};

  void set_state_(State state);
  void send_response_(std::vector<uint8_t> &response);
  void process_incoming_data_();
};

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
extern EBEBleServer *global_ebe_ble_server;

}  // namespace ebe_ble_server
}  // namespace esphome

#endif
