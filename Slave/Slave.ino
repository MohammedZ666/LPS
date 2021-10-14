/*
   ESPNOW - Basic communication - Slave
   Date: 26th September 2017
   Author: Arvind Ravulavaru <https://github.com/arvindr21>
   Purpose: ESPNow Communication between a Master ESP32 and multiple ESP32 Slaves
   Description: This sketch consists of the code for the Slave module.
   Resources: (A bit outdated)
   a. https://espressif.com/sites/default/files/documentation/esp-now_user_guide_en.pdf
   b. http://www.esploradores.com/practica-6-conexion-esp-now/

   << This Device Slave >>

   Flow: Master
   Step 1 : ESPNow Init on Master and set it in STA mode
   Step 2 : Start scanning for Slave ESP32 (we have added a prefix of `slave` to the SSID of slave for an easy setup)
   Step 3 : Once found, add Slave as peer
   Step 4 : Register for send callback
   Step 5 : Start Transmitting data from Master to Slave(s)

   Flow: Slave
   Step 1 : ESPNow Init on Slave
   Step 2 : Update the SSID of Slave with a prefix of `slave`
   Step 3 : Set Slave in AP mode
   Step 4 : Register for receive callback and wait for data
   Step 5 : Once data arrives, print it in the serial monitor

   Note: Master and Slave have been defined to easily understand the setup.
         Based on the ESPNOW API, there is no concept of Master and Slave.
         Any devices can act as master or salve.
*/



#ifdef ESP32
#include <WiFi.h>
#include <esp_now.h>
#define esp8266 false
#else
#include <ESP8266WiFi.h>
#include <espnow.h>
#endif


#define CHANNEL 0

char macStrPrev[18];


// Init ESP Now with fallback
void InitESPNow() {
  WiFi.disconnect();
  if (esp_now_init() == 0) {
    Serial.println("ESPNow Init Success");
  }
  else {
    Serial.println("ESPNow Init Failed");
    // Retry InitESPNow, add a counte and then restart?
    // InitESPNow();
    // or Simply Restart
    ESP.restart();
  }
}

// config AP SSID
void configDeviceAP() {
  String Prefix = "Slave:";
  String Mac = WiFi.macAddress();
  String SSID = Prefix + Mac;
  String Password = "123456789";
  bool result = WiFi.softAP(SSID.c_str(), Password.c_str(), CHANNEL, 0);
  if (!result) {
    Serial.println("AP Config failed.");
  } else {
    Serial.println("AP Config Success. Broadcasting with AP: " + String(SSID));
  }
}


float getX(float h0, float k0, float h1, float k1, float r0, float r1)
{
  float e{((h1 * h1 - h0 * h0 + k1 * k1 - k0 * k0 - (r1 * r1 - r0 * r0)) / (2 * (k1 - k0))) - k0};
  float f{ -(h1 - h0) / (k1 - k0)};
  float a{f * f + 1};
  float b{2 * ((f * e) - h0)};
  float c{e * e + h0 * h0 - r0 * r0};
  return getPositiveRoot(a, b, c);
}
float getY(float h0, float k0, float h1, float k1, float r0, float r1)
{
  float e{((h1 * h1 - h0 * h0 + k1 * k1 - k0 * k0 - (r1 * r1 - r0 * r0)) / (2 * (h1 - h0))) - h0};
  float f{ -(k1 - k0) / (h1 - h0)};
  float a{f * f + 1};
  float b{2 * ((f * e) - k0)};
  float c{e * e + k0 * k0 - r0 * r0};
  return getPositiveRoot(a, b, c);
}

float getPositiveRoot(float a, float b, float c)
{
  float discriminant{0};
  discriminant = b * b - 4 * a * c;
  if (discriminant < 0)
  {
    //cout << sqrt(-discriminant) / (2 * a) << "\n";
    discriminant = 0;
  }

  float y1 = (-b + sqrt(discriminant)) / (2 * a);
  float y2 = (-b - sqrt(discriminant)) / (2 * a);
  if (y2 > 0)
    return y2;
  return y1;
}




typedef struct payload {
  int   h{12};
  int   k{5};
  float r{0};
} payload;
payload data0;

//Overloading callback functions that will be executed when data is received on the basis of ESP chips used
//below one is for esp8266
void OnDataRecv(uint8_t * mac_addr, uint8_t *incomingData, uint8_t data_len) {
  payload data1;
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  if (macStrPrev[0] == 0) {
    strcpy(macStrPrev, macStr);
    memcpy(&data0, incomingData, sizeof(data0));
  }
  else if (String(macStrPrev).equals(String(macStr))) {
    memcpy(&data0, incomingData, sizeof(data0)); //getting the updated data
    return;
  }
  else {
    macStrPrev[0] = 0;
    memcpy(&data1, incomingData, sizeof(data1));    Serial.print("(X, Y) => ("); Serial.print(getX(data0.h, data0.k, data1.h, data1.k, data0.r, data1.r)); Serial.print(", "); Serial.print(getY(data0.h, data0.k, data1.h, data1.k, data0.r, data1.r)); Serial.println(") ");

  }

  Serial.print("Last Packet Recv from: "); Serial.println(macStr);
  //Serial.printf("h -> %d, k-> %d, r-> %f", data0.h, data0.k, data0.r);
  Serial.println("");
}


//below one is for esp32
void OnDataRecv(const uint8_t * mac_addr, const uint8_t *incomingData, int data_len) {
  payload data1;
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  if (macStrPrev[0] == 0) {
    strcpy(macStrPrev, macStr);
    memcpy(&data0, incomingData, sizeof(data0));
  }
  else if (String(macStrPrev).equals(String(macStr))) {
    memcpy(&data0, incomingData, sizeof(data0)); //getting the updated data
    return;
  }
  else {
    macStrPrev[0] = 0;
    memcpy(&data1, incomingData, sizeof(data1));    Serial.print("(X, Y) => ("); Serial.print(getX(data0.h, data0.k, data1.h, data1.k, data0.r, data1.r)); Serial.print(", "); Serial.print(getY(data0.h, data0.k, data1.h, data1.k, data0.r, data1.r)); Serial.println(") ");

  }

  Serial.print("Last Packet Recv from: "); Serial.println(macStr);
  //Serial.printf("h -> %d, k-> %d, r-> %f", data0.h, data0.k, data0.r);
  Serial.println("");
}
void setup() {
  Serial.begin(115200);
  Serial.println("ESPNow/Basic/Slave Example");
  //Set device in AP mode to begin with
  WiFi.mode(WIFI_AP);
  // configure device AP mode
  configDeviceAP();
  // This is the mac address of the Slave in AP Mode
  Serial.print("AP MAC: "); Serial.println(WiFi.softAPmacAddress());
  // Init ESPNow with a fallback logic
  InitESPNow();
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info.
  esp_now_register_recv_cb(OnDataRecv);

}


void loop() {
  // Chill
}
