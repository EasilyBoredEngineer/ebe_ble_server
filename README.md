# ebe_ble_server
 
Sends a series of sensor values from an existing esphome device to a device running this repository.

Inspired by a private repository created by @syssi, as well as @jesse_rocks esp32_controller. 

Lots more work required to make it more adaptable.

The values are received by this repository as a byte array, and then used to populate a struct called MyData -

  //this is the structure that is used to carry the data sent by the client.
  struct MyData {
  float float1_;  // 4 bytes for the first float
  float float2_;  
  float float3_;  
  float float4_;
  };

see examples folder for examples of client and server

