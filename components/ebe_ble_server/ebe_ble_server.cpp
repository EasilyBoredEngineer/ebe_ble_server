#include "ebe_ble_server.h"

#include "esphome/components/esp32_ble/ble.h"
#include "esphome/components/esp32_ble_server/ble_2902.h"
#include "esphome/core/application.h"
#include "esphome/core/log.h"

#ifdef USE_ESP32

namespace esphome {
namespace ebe_ble_server {

static const char *const TAG = "ebe_ble_server";

EBEBleServer::EBEBleServer() { global_ebe_ble_server = this; }

void EBEBleServer::setup() {

  global_ble_server->create_service(ESPBTUUID::from_raw(MY_SERVICE_UUID), true);
  this->service_ = global_ble_server->get_service(ESPBTUUID::from_raw(MY_SERVICE_UUID));
  this->setup_characteristics();

}

//add characteristics here
void EBEBleServer::setup_characteristics() {
  this->my_characteristic_ = this->service_->create_characteristic(MY_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_NOTIFY |
                                                                 BLECharacteristic::PROPERTY_WRITE |
                                                                 BLECharacteristic::PROPERTY_WRITE_NR);
                                                                 
  this->my_characteristic_->on_write([this](const std::vector<uint8_t> &data) {
  
  //if there's incoming data, add it to the array.
  if (!data.empty()) {
      this->incoming_data_.insert(this->incoming_data_.end(), data.begin(), data.end());
    }
  });

  //make a descriptor
  BLEDescriptor *my_characteristic_descriptor = new BLE2902();
  this->my_characteristic_->add_descriptor(my_characteristic_descriptor);

  //set a variable to record that setup has been run.
  this->setup_complete_ = true;
}

void EBEBleServer::loop() {

    unsigned long float3_time = millis();

    // Check if it's time to perform the action
    if (float3_time - last_check_time >= check_interval) {
        // Update the last check time
        last_check_time = float3_time;

        // Check the number of connected clients
        uint32_t connected_clients = global_ble_server->get_connected_client_count();

        if (connected_clients == 0) {
            // There are no connected clients, indicating a stale connection
            // Perform appropriate actions, such as cleaning up resources or restarting the server
            ESP_LOGW(TAG, "No connected clients, possible stale connection");
        } else {
            // There are connected clients, so the connection is active
            // No action needed
            ESP_LOGI(TAG, "There are %u connected clients", connected_clients);
        } }


  //call incoming data function if we see data incoming from client
  if (!this->incoming_data_.empty())
    this->process_incoming_data_();
  
  //handle server failures
  if (!global_ble_server->is_running()) {
     this->state_ = STATE_STOPPED;
     this->incoming_data_.clear();
     return;
   }

  switch (this->state_) {
    case STATE_STOPPED:
      if (this->service_->is_created() && this->should_start_ && this->setup_complete_) {
        if (this->service_->is_running()) {

          this->set_state_(STATE_RUNNING);
          this->should_start_ = false;
        } else {
          if (!this->service_->is_starting()) {
            ESP_LOGD(TAG, "Service running");
            this->service_->start();
          }
        }
      }
      break;
    case STATE_RUNNING:
        //do things here.
      break;
  }
}

void EBEBleServer::start() {
  if (this->state_ != STATE_STOPPED)
    return;

  ESP_LOGD(TAG, "Setting EBEBleServer to start");
  this->should_start_ = true;
}

void EBEBleServer::stop() {
  this->set_timeout("end-service", 1000, [this] {
    this->service_->stop();
    this->set_state_(STATE_STOPPED);
  });
}

//unimplimented response sending code.
void EBEBleServer::send_response_(std::vector<uint8_t> &response) {
  for (size_t i = 0; i < response.size(); i = i + 20) {
    auto last = std::min(response.size(), i + 20);
    std::vector<uint8_t> chunk = {response.begin() + i, response.begin() + last};
    ESP_LOGD(TAG, "  %s", format_hex_pretty(&chunk.front(), chunk.size()).c_str());
    this->my_characteristic_->set_value(chunk);
    this->my_characteristic_->notify();
  }
}

float EBEBleServer::get_setup_priority() const { return setup_priority::AFTER_BLUETOOTH; }

void EBEBleServer::dump_config() { ESP_LOGCONFIG(TAG, "EBEBleServer:"); }

void EBEBleServer::process_incoming_data_() {
  //ESP_LOGD(TAG, "Request received: %s", format_hex_pretty(this->incoming_data_).c_str());
    uint8_t byteArray[sizeof(MyData)];

    // Copy the contents of incoming_data_ to the byte array
    if (incoming_data_.size() <= sizeof(MyData)) {
        std::copy(this->incoming_data_.begin(), this->incoming_data_.end(), byteArray);
        std::memcpy(&data_, byteArray, sizeof(MyData));
        ESP_LOGD(TAG, "float one is: %f, float two is %f, float 3 is %f, float 4 is %f", data_.float1_, data_.float2_, data_.float3_, data_.float4_);
    } else {
        ESP_LOGD(TAG, "Too Large: ");
    }

  this->incoming_data_.clear();
  last_data_time=millis();

}

void EBEBleServer::set_state_(State state) {
  ESP_LOGV(TAG, "Setting state: %d", state);
  this->state_ = state;
}

//these are native parts of BLE_Server that are called by the underlying native esphome code
// when these events happen. see https://esphome.io/api/ble__server_8h_source

void EBEBleServer::on_client_disconnect() { 
  ESP_LOGD(TAG, "Emulator - BLE Client disconnected");

 };

void EBEBleServer::on_client_connect() { 
  ESP_LOGD(TAG, "Emulator - BLE Client connected");

 };


EBEBleServer *global_ebe_ble_server = nullptr;  // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

}  // namespace ebe_ble_server
}  // namespace esphome

#endif
