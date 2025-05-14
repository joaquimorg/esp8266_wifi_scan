/*
    Scan WiFi networks.
    (c)joaquim.org
*/

extern "C" {
  #include "user_interface.h"
}

#include <ESP8266WiFi.h>
#include "icons.h"

#include <TFT_eSPI.h> // Graphics and font library
#include <SPI.h>


extern "C" {
  uint16 readvdd33(void);
}

// For the Adafruit shield, these are the default.
//#define TFT_DC 4
//#define TFT_CS 5
//#define TFT_RST 16

// Use hardware SPI 
//Adafruit_TFT lcd = Adafruit_TFT(TFT_CS, TFT_DC, TFT_RST);

TFT_eSPI lcd = TFT_eSPI();  // Invoke library, pins defined in User_Setup.h


uint16_t  screen = 1;
uint16_t  screenCount = 1;

typedef struct {
  String SSID;
  uint8_t encryptionType;
  int32_t RSSI;
  int32_t Channel;
  uint8_t Active;
  uint8_t Update;
} NetInfo;

NetInfo networkInfo[30];

#define TEXT_SIZE 18

void setup() {
  system_update_cpu_freq(160);

  pinMode(A0, INPUT);

  Serial.begin(115200);
  lcd.begin();
  lcd.setRotation(2);
  lcd.setTextWrap(false);
  lcd.setTextSize(2);

  lcd.fillScreen(TFT_BLACK);
  lcd.drawBitmap(70, 136, wifiBitmap, 100, 70, TFT_WHITE);
  //lcd.setFont(&FreeMono9pt7b);
  lcd.setTextColor(TFT_YELLOW, TFT_BLACK);
  lcd.setCursor(65, 80);
  lcd.print("Wifi Scan");
  lcd.setTextColor(TFT_GREEN, TFT_BLACK);
  lcd.setCursor(52, 100);
  lcd.print("joaquim.org");
  //lcd.displayUpdate();
  lcd.setTextColor(TFT_WHITE, TFT_BLACK);
  Serial.println("esp8266 Wifi Scan ... ");

  // Set WiFi to station mode and disconnect from an AP if it was previously connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(5000);
  Serial.println("running... ");
  lcd.fillScreen(TFT_BLACK);
  lcd.setCursor(2, 2);
  lcd.println("Scanning WiFi...");
  //lcd.displayUpdate();
  memset( networkInfo, 0x0, sizeof(networkInfo) );
}


void showScreen1( int num ) {
  int pos = 0;
  
  for (int i = 0; i < num; ++i) {
    if (networkInfo[i].Active == 1) {
      
      lcd.fillRect(0, 20 + (TEXT_SIZE * pos), 240, TEXT_SIZE, TFT_BLACK);
      
      lcd.setCursor(0, 20 + (TEXT_SIZE * pos));      
      if (networkInfo[i].encryptionType == ENC_TYPE_NONE) {
        lcd.drawBitmap(0, 20 + (TEXT_SIZE * pos), unlockBitmap16, 16, 16, TFT_GREEN);
      } else {
        lcd.drawBitmap(0, 20 + (TEXT_SIZE * pos), lockBitmap16, 16, 16, TFT_RED);
      }
  
      lcd.setCursor(20, 20 + (TEXT_SIZE * pos));
  
      lcd.setTextColor(TFT_WHITE, TFT_BLACK);

      if (networkInfo[i].RSSI <= -80) {
        lcd.setTextColor(TFT_RED, TFT_BLACK);
      } else if (networkInfo[i].RSSI >= -80 && networkInfo[i].RSSI < -70) {
        lcd.setTextColor(TFT_YELLOW, TFT_BLACK);
      } else {
        lcd.setTextColor(TFT_GREEN, TFT_BLACK);
      }
      
      lcd.print(networkInfo[i].SSID);
  
      //lcd.fillRect(0, 20 + (TEXT_SIZE * pos), 240, TEXT_SIZE, TFT_BLACK);
      /*if (networkInfo[i].RSSI <= -80) {
        lcd.drawBitmap(120, 20 + (TEXT_SIZE * pos), level1Bitmap, 8, 8, TFT_RED);
      } else if (networkInfo[i].RSSI >= -80 && networkInfo[i].RSSI < -70) {
        lcd.drawBitmap(120, 20 + (TEXT_SIZE * pos), level2Bitmap, 8, 8, TFT_YELLOW);
      } else {
        lcd.drawBitmap(120, 20 + (TEXT_SIZE * pos), level3Bitmap, 8, 8, TFT_GREEN);
      }*/
      pos++;
      if ( pos > 10) break;
    }
  }
}

void showScreen2( int num ) {
  int pos = 0;
  for (int i = 0; i < num; ++i) {
    if (networkInfo[i].Active == 1) {
      lcd.fillRect(0, 20 + (TEXT_SIZE * pos), 240, TEXT_SIZE, TFT_BLACK);

      lcd.setCursor(0, 20 + (TEXT_SIZE * pos));
      
      if (networkInfo[i].encryptionType == ENC_TYPE_NONE) {
        lcd.drawBitmap(0, 20 + (TEXT_SIZE * pos), unlockBitmap16, 16, 16, TFT_GREEN);
      } else {
        lcd.drawBitmap(0, 20 + (TEXT_SIZE * pos), lockBitmap16, 16, 16, TFT_RED);
      }
  
      if (networkInfo[i].RSSI <= -80) {
        lcd.fillRect(20, 20 + (TEXT_SIZE * pos) + 4, (180 - abs(networkInfo[i].RSSI)), 8, TFT_RED);
      } else if (networkInfo[i].RSSI >= -80 && networkInfo[i].RSSI < -70) {
        lcd.fillRect(20, 20 + (TEXT_SIZE * pos) + 4, (180 - abs(networkInfo[i].RSSI)), 8, TFT_YELLOW);
      } else {
        lcd.fillRect(20, 20 + (TEXT_SIZE * pos) + 4, (180 - abs(networkInfo[i].RSSI)), 8, TFT_GREEN);
      }
  
      lcd.setCursor(200, 20 + (TEXT_SIZE * pos));
      lcd.printf("%02i", networkInfo[i].Channel);

      pos++;
      if ( pos > 10) break;
    }
  }
}

void showScreen3( int num ) {
  int pos = 1;
  
  lcd.setTextColor(TFT_GREEN, TFT_BLACK);
  lcd.setCursor(0, 20 + (TEXT_SIZE * 0));
  lcd.print("Free networks");

  for (int i = 0; i < num; ++i) {
    if (networkInfo[i].encryptionType == ENC_TYPE_NONE && networkInfo[i].Active == 1) {
      
      lcd.fillRect(0, 30 + (TEXT_SIZE * pos), 240, TEXT_SIZE, TFT_BLACK);
      
      lcd.setTextColor(TFT_WHITE, TFT_BLACK);

      if (networkInfo[i].RSSI <= -80) {
        lcd.setTextColor(TFT_RED, TFT_BLACK);
      } else if (networkInfo[i].RSSI >= -80 && networkInfo[i].RSSI < -70) {
        lcd.setTextColor(TFT_YELLOW, TFT_BLACK);
      } else {
        lcd.setTextColor(TFT_GREEN, TFT_BLACK);
      }
      
      lcd.setCursor(0, 30 + (TEXT_SIZE * pos));
      lcd.print(networkInfo[i].SSID);
      
      /*if (networkInfo[i].RSSI <= -80) {
        lcd.drawBitmap(120, 20 + (TEXT_SIZE * pos), level1Bitmap, 8, 8, TFT_RED);
      } else if (networkInfo[i].RSSI >= -80 && networkInfo[i].RSSI < -70) {
        lcd.drawBitmap(120, 20 + (TEXT_SIZE * pos), level2Bitmap, 8, 8, TFT_YELLOW);
      } else {
        lcd.drawBitmap(120, 20 + (TEXT_SIZE * pos), level3Bitmap, 8, 8, TFT_GREEN);
      }*/

      pos++;
      if ( pos > 10) break;
    }
  }
}


void BubbleSort(NetInfo netInfo[], int numLength) {
  uint8_t i, j, flag = 1;    // set flag to 1 to start first pass
  NetInfo temp;             // holding variable

  for (i = 1; (i <= numLength) && flag; i++)
  {
    flag = 0;
    for (j = 0; j < (numLength - 1); j++)
    {
      if (abs(netInfo[j + 1].RSSI) < abs(netInfo[j].RSSI))    // ascending order simply changes to <
      {
        temp = netInfo[j];             // swap elements
        netInfo[j] = netInfo[j + 1];
        netInfo[j + 1] = temp;
        flag = 1;               // indicates that a swap occurred.
      }
    }
  }
  return;   //arrays are passed to functions by address; nothing is returned
}

void addToArray( NetInfo netInfo ) {
  uint8_t total = 0;
  
  for (int i = 0; i < 30; ++i) {
    if ( networkInfo[i].Active == 1 && (networkInfo[i].SSID == netInfo.SSID) ) {
      networkInfo[i] = netInfo;
      //Serial.print("Found :" + netInfo.SSID + " ");
      //Serial.println(i);
      return;
    } 
  }

  //
  for (int i = 0; i < 30; ++i) {    
    /*Serial.print(i);
    Serial.print(" ");    
    Serial.print(networkInfo[i].Active);
    Serial.println(" " + networkInfo[i].SSID + " ");*/
      
    if ( networkInfo[i].Active == 1 ) {
      total++;
    } 
  }
  
  if ( total < 30 ) {
    networkInfo[total] = netInfo;
    //Serial.print("New :" + netInfo.SSID + " ");
    //Serial.println(total);
  }
  
}

/*
function readADC_avg()                 -- read 10 consecutive values and calculate average.
           ad1 = 0
           i=0
           while (i<10) do
                ad1=ad1+adc.read(0)*4/978 --calibrate based on your voltage divider AND Vref!
                --print(ad1)
                i=i+1
           end
           ad1 = ad1/10
           print(ad1)
           return ad1
      end
*/

float readADC_avg() {
  float adc = 0;
  //Serial.print("read adc :");
  for (int i = 0; i < 10; ++i) {
    adc = adc + analogRead(A0);
  }
  adc = adc / 10;
  /*Serial.print( adc * 9 / 978 );
  Serial.print(" - ");
  Serial.println(adc);*/
  return adc;
}

void loop() {

  int n = WiFi.scanNetworks();
  int m = n;
  NetInfo netInfo;

  float value = readADC_avg();
  
  //Serial.print("Vcc:");
  //Serial.println(value/1000);

  for (int i = 0; i < 30; ++i) {
    networkInfo[i].Update = 0;
  }

  if (n == 0) {
    lcd.fillScreen(TFT_BLACK);
    lcd.setTextColor(TFT_BLUE, TFT_BLACK);
    lcd.setCursor(0, 10);
    lcd.println();
    lcd.println();
    lcd.print("No networks found...");
    lcd.println();
    lcd.println("Scanning...");
  } else {

    if ( m > 30 ) m = 30;
    for (int i = 0; i < m; ++i) {
      netInfo.SSID = WiFi.SSID(i);
      netInfo.encryptionType = WiFi.encryptionType(i);
      netInfo.RSSI = WiFi.RSSI(i);
      netInfo.Channel = WiFi.channel(i);
      netInfo.Active = 1;
      netInfo.Update = 1;
      addToArray(netInfo);
    }
    for (int i = 0; i < 30; ++i) {
      if ( networkInfo[i].Update == 0 ) {
        networkInfo[i].SSID = "";
        networkInfo[i].RSSI = 255;
        networkInfo[i].Active = 0;
      }
    }

    BubbleSort(networkInfo, 30);
    if (screenCount == 0) {
      lcd.fillScreen(TFT_BLACK);
    }
    lcd.setCursor(2, 2);
    lcd.setTextColor(TFT_CYAN, TFT_BLACK);
    lcd.print(n);
    lcd.setTextColor(TFT_WHITE, TFT_BLACK);
    lcd.print(" networks found");
    
    //lcd.fillRect(0, 20, 240, 240, TFT_BLACK);
   
    if (screen == 1) {
      showScreen1(10);
      /*lcd.drawBitmap(50, 300, openBitmap, 8, 8, TFT_WHITE);
      lcd.drawBitmap(60, 300, closeBitmap, 8, 8, TFT_WHITE);
      lcd.drawBitmap(70, 300, closeBitmap, 8, 8, TFT_WHITE);*/
    } else if (screen == 2) {
      showScreen2(10);
      /*lcd.drawBitmap(50, 300, closeBitmap, 8, 8, TFT_WHITE);
      lcd.drawBitmap(60, 300, openBitmap, 8, 8, TFT_WHITE);
      lcd.drawBitmap(70, 300, closeBitmap, 8, 8, TFT_WHITE);*/
    } else if (screen == 3) {
      showScreen3(10);
      /*lcd.drawBitmap(50, 300, closeBitmap, 8, 8, TFT_WHITE);
      lcd.drawBitmap(60, 300, closeBitmap, 8, 8, TFT_WHITE);
      lcd.drawBitmap(70, 300, openBitmap, 8, 8, TFT_WHITE);*/
    }

    lcd.setTextSize(1);
    lcd.setCursor(1, 230);
    lcd.setTextColor(TFT_BLUE, TFT_BLACK);
    lcd.print("Batt: ");
    lcd.print(value * 9 / 945);
    lcd.print(" V");
    lcd.setTextSize(2);
    
    screenCount++;
    if ( screenCount > 5 ) {      
      screenCount = 0;
      screen++;
      if ( screen > 3 ) {
        screen = 1;
      }
    }
  }
  //lcd.displayUpdate();
  // Wait a bit before scanning again
  //delay(1000);
}
