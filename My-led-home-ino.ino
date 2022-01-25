#include <ESP8266WiFi.h>
#include  <ArduinoOTA.h>
#include <WiFiUdp.h>
#include <ESP8266mDNS.h>

#include "FastLED.h"
#define NUM_LEDS 354 
#define DATA_PIN D4
CRGB leds[NUM_LEDS];
#define MAIN_LOOP 3
#define EXP 1
int MODE = 3;
CRGB CURRENT_COLOR = CRGB::White;

//update


unsigned long main_timer = 0, hue_timer, strobe_timer, running_timer, color_timer, rainbow_timer, eeprom_timer;
int RcurrentLevel,RsoundLevel;
uint16_t LOW_PASS = 0; 



WiFiServer server(80);

char dataR[100];
int indexWrite = 0;

void setup() {

pinMode(A0, INPUT);
  
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  
  leds[0] = CRGB::White; 

  FastLED.show();
  
  Serial.begin(115200);
  Serial.println();
  WiFi.begin("MGTS_256","moscow256");
  Serial.print("Connecting");  //  "Подключение"
  
  leds[1] = CRGB::Red;
  FastLED.show();
  
  while (WiFi.status() != WL_CONNECTED)
  {
    leds[0] = CRGB::Red;
    FastLED.show();
    delay(100);
    FastLED.clear();
    FastLED.show();
    delay(100);
    Serial.print(".");
  }
  Serial.println();
  leds[0] = CRGB::Green;
  leds[1] = CRGB::Green;
  leds[2] = CRGB::Green;
  FastLED.show();
  delay(300);
  FastLED.clear();
  FastLED.show();

  Serial.print("Connected, IP address: ");
           //  "Подключились, IP-адрес: "
  Serial.println(WiFi.localIP());
  server.begin();

  ArduinoOTA.setHostname("ESP8266-LED");
  ArduinoOTA.begin();
}

void loop() {

  ArduinoOTA.handle();
  
  WiFiClient client = server.available();
  if(client){
  Serial.println("if c");
  if(client.connected())
    {
      Serial.println("Client Connected");
    }
    while(client.available()>0){
        char str = client.read();
        Serial.print(str);
        dataR[indexWrite] = str;
        indexWrite ++;
        if(str == '\n'){
          readData(client);
          indexWrite = 0;
          }
      }
        
      
    }
//for(int i = 0; i < NUM_LEDS; i++){
//  leds[i].setRGB(255, 255, 255);
//}         // очистить массив пикселей
//  FastLED.show(); 
 if (millis() - main_timer > MAIN_LOOP) {
  
      if(MODE == 3){
   // сбрасываем значения
      RsoundLevel = 0;
        for (int i = 0; i < 50; i ++) {                                 // делаем 100 измерений
          RcurrentLevel = analogRead(A0);   
          if (RsoundLevel < RcurrentLevel) RsoundLevel = RcurrentLevel;   // ищем максимальное
        }

          int check_max_level = 400;
            if(RsoundLevel > check_max_level){
              check_max_level = RsoundLevel +1;
            }
        // фильтруем по нижнему порогу шумов
        RsoundLevel = map(RsoundLevel, LOW_PASS, check_max_level, 0, 500);
        

        // ограничиваем диапазон
        RsoundLevel = constrain(RsoundLevel, 0, 500);

        // возводим в степень (для большей чёткости работы)
        RsoundLevel = pow(RsoundLevel, EXP);

        //=== 1
//        for(int i = NUM_LEDS-1; i >  0; i--){
//          //leds[i] =  ;
//          if(i > 0){
//              leds[i] = leds[i-1];
//            }
//          }
//          
//          leds[0] = CHSV( map(RsoundLevel,0, 500, 200,300 ), 255, map(RsoundLevel,0, 500, 0,255 ));

// === 2
//          FastLED.clear();
//          for(int i = 0; i < map(RsoundLevel,0, 500, 0,NUM_LEDS/2 ); i++){
//            leds[NUM_LEDS/2-i] = CHSV( map(RsoundLevel,0, 500, 200,300 ), 255, map(RsoundLevel,0, 500, 0,255 ));
//            leds[NUM_LEDS/2+i] = CHSV( map(RsoundLevel,0, 500, 200,300 ), 255, map(RsoundLevel,0, 500, 0,255 ));
//            }



        for(int i = NUM_LEDS; i >  NUM_LEDS/2; i--){
          //leds[i] =  ;
          if(i > 0){
              leds[i] = leds[i-1];
            }
          }
         for(int i = 0; i <  NUM_LEDS/2-1; i++){
          //leds[i] =  ;
          if(i < NUM_LEDS/2-1){
              leds[i] = leds[i+1];
            }
          }
           leds[NUM_LEDS/2] = CHSV( map(RsoundLevel,0, 500, 200,300 ), 255, map(RsoundLevel,0, 500, 0,255 ));
           leds[NUM_LEDS/2-1] = CHSV( map(RsoundLevel,0, 500, 200,300 ), 255, map(RsoundLevel,0, 500, 0,255 ));
          FastLED.show();
          main_timer = millis();
          }else if(MODE == 1){
            for(int i = NUM_LEDS; i >  0; i--){
             //leds[i] =  ;
              leds[i] = CURRENT_COLOR;

          }
                         FastLED.show();
          main_timer = millis();
          }
  }
    
}

void readData(WiFiClient client){
  Serial.println(sizeof(dataR));
  Serial.println(strlen(dataR));
  Serial.println("end message. reading...");
  Serial.println("\n================\n");

  if(
    dataR[0] == 'e' &&
    dataR[1] == 's' &&
    dataR[2] == 'p'
    ){
      client.print("ok");
      Serial.println("send \"ok\"");
      }
      else{
        int index_read = 0;
        int step1 = dataR[0]- '0';
        MODE = step1 *100;
        step1 = dataR[1]- '0';
        MODE += step1 *10;
        step1 = dataR[2]- '0';
        MODE += step1 *1;
        index_read += 3;
        if(dataR[index_read] == 'c'){
          index_read +=1;
          int value = 0;
          int hue = 0;
          int str = 0;
          step1 = dataR[index_read+0]- '0';
          value = step1 *100;
          
          step1 = dataR[index_read+1]- '0';
          value += step1 *10;
          
          step1 = dataR[index_read+2]- '0';
          value += step1 *1;
          
          index_read +=3;
          
          step1 = dataR[index_read+0]- '0';
          hue = step1 *100;
          
          step1 = dataR[index_read+1]- '0';
          hue += step1 *10;
          
          step1 = dataR[index_read+2]- '0';
          hue += step1 *1;
          
          index_read +=3;

          step1 = dataR[index_read+0]- '0';
          str = step1 *100;
          
          step1 = dataR[index_read+1]- '0';
          str += step1 *10;
          
          step1 = dataR[index_read+2]- '0';
          str += step1 *1;
          
          index_read +=3;
          
          //CHSV( 224, 187, 255)
          
          CURRENT_COLOR = CRGB( hue, str, value);
        }


      }

  
  }
