#include <SPI.h>
#include <TFT_eSPI.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

WiFiMulti wifiMulti;
char payload_chars[256];
TFT_eSPI tft = TFT_eSPI();  // Invoke custom library

void setup(void) {
  Serial.begin(115200);
  Serial.println("Init");

  tft.init();

  tft.setTextSize(1);
  tft.setRotation(3);
  tft.setTextColor(TFT_RED, TFT_BLACK);

  // tft.fillScreen(TFT_BLACK);
  // tft.setCursor(0, 100, 4);
  // tft.println("Connect");

  wifiMulti.addAP("SSID", "PASSWORD");
}

void loop()
{
  // wait for WiFi connection
  if((wifiMulti.run() == WL_CONNECTED))
  {
    Serial.println("connected");
    HTTPClient http;
    int httpCode;
    http.begin("http://192.168.1.123/cm?cmnd=cmd/tasmota_00000/stat/STATUS%208");
    http.addHeader("Content-Type", "text/plain");
    
    httpCode = http.GET();
    if(httpCode == HTTP_CODE_OK)
    {
      Serial.println("http get ok");
      char * pch;        
      char tc[6];
      char pc[6];
      char hc[8];
      StaticJsonDocument<256> doc;

      // payload = http.getString();
      http.getString().toCharArray(payload_chars, 256);
      Serial.println(payload_chars);

      DeserializationError error = deserializeJson(doc, payload_chars);

      if (!error)
      {
        //{"StatusSNS":{"Time":"2022-07-20T20:19:54","BME280":{"Temperature":31.9,"Humidity":35.7,"DewPoint":14.8,"Pressure":1002.9},"PressureUnit":"hPa","TempUnit":"C"}}

        double Temperature = doc["StatusSNS"]["BME280"]["Temperature"];
        double Humidity = doc["StatusSNS"]["BME280"]["Humidity"];
        double Pressure = doc["StatusSNS"]["BME280"]["Pressure"];

        tft.setTextSize(3);;
        tft.setCursor(0, 0, 4);
        tft.fillScreen(TFT_BLACK);
        tft.setTextColor(TFT_WHITE, TFT_BLACK);

        tft.println(String(Temperature,1) + " °C");
        tft.setTextSize(1);
        tft.setTextColor(TFT_ORANGE, TFT_BLACK);
        tft.print(String(Humidity,1) + " %  ");
        tft.println(String(Pressure,1) + " hPa");
        tft.drawFastHLine(0,70,TFT_HEIGHT,TFT_ORANGE);
        tft.drawFastHLine(0,71,TFT_HEIGHT,TFT_ORANGE);
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
      }
      else
      {
        pch = strstr (payload_chars,"Temperature");
        strncpy (tc,pch + 13,5);
        if (tc[4] == ',')
          tc[4] = 0;

        pch = strstr (payload_chars,"Humidity");
        strncpy (hc,pch + 10,4);

        pch = strstr (payload_chars,"Pressure");
        strncpy (pc,pch + 10,6);
        if (pc[5] == '}')
          pc[5] = 0;

        tft.setTextSize(3);
        tft.setCursor(0, 0, 4);
        tft.fillScreen(TFT_BLACK);
        tft.setTextColor(TFT_WHITE, TFT_BLACK);

        tft.println(String(tc) + " °C");
        tft.setTextSize(1);
        tft.setTextColor(TFT_ORANGE, TFT_BLACK);
        tft.println(String(hc) + " %");
        tft.println(String(pc) + " hPa");
        tft.drawFastHLine(0,72,TFT_HEIGHT,TFT_ORANGE);
        tft.drawFastHLine(0,73,TFT_HEIGHT,TFT_ORANGE);
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
      }
    }

    int i = 0;
    for (i=0; i<30; i++)
    {
      delay(1000);
      if (i%2)
      {
          tft.drawSpot(220,120,3,TFT_BLACK,TFT_ORANGE);
      }
      else
      {
         tft.drawSpot(220,120,3,TFT_ORANGE,TFT_BLACK);
      }
      
    }
    if (http.connected()) http.end();
  }
}
