# ebe_ble_server
 
Sends a series of sensor values from an existing esphome device to a device running this repository.

The values are received by this repository as a byte array, and then used to populate a struct called MyData -

  //this is the structure that is used to carry the data sent by the client.
  struct MyData {
  float float1_;  // 4 bytes for the first float
  float float2_;  
  float float3_;  
  float float4_;
  };

see examples folder for examples of client and server

