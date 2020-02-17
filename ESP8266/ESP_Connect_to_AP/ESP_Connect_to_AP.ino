/* Simple sketch to connect to an AP */

#include <ESP8266WiFi.h>

char *mySSID = "ssid";
char *myPass = "password";

void setup()
{
  Serial.begin(115200);
  Serial.println();
  WiFi.mode(WIFI_STA);
  WiFi.begin(mySSID, myPass);

  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());

  Serial.println();
  Serial.print("MAC: ");
  Serial.println(WiFi.macAddress());

  delay(5000);
}

void loop() {


  if ((WiFi.status() != WL_CONNECTED)) {

    Serial.println("Not Connected!");
    delay(1000);
  }
  else {
    Serial.println(WiFi.localIP());
    delay(1000);
  }
}
