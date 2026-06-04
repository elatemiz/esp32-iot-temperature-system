#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>


#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1 
#define i2c_Address 0x3C 

Adafruit_SH1106G display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


const char* ssid = "SUPERONLINE_Wi-Fi_8901";
const char* password = "fYbE2YSsxK9G";

// PHP servisi adresi
const char* serverURL = "http://192.168.1.43/dht11_project/data.php";

// LED pinleri
#define RED_LED 18
#define BLUE_LED 2
#define YELLOW_LED 4

void setup() {
    Serial.begin(115200);

    Serial.println("WiFi'ye bağlanıyor...");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }
    Serial.println("\nWiFi bağlantısı başarılı!");

   
    if (!display.begin(i2c_Address, true)) {
        Serial.println("OLED ekran başlatılamadı! Program duruyor...");
        while (true);
    }
    display.clearDisplay();
    display.setTextSize(1);  
    display.setTextColor(SH110X_WHITE);  
    display.setCursor(0, 0);  
    display.println("Sistem Baslatildi...");
    display.display();  
    delay(2000);  

    
    pinMode(RED_LED, OUTPUT);
    pinMode(BLUE_LED, OUTPUT);
    pinMode(YELLOW_LED, OUTPUT);
    
    digitalWrite(RED_LED, LOW);
    digitalWrite(BLUE_LED, LOW);
    digitalWrite(YELLOW_LED, LOW);
}

void loop() {
    
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi bağlantısı kaybedildi, yeniden bağlanıyor...");
        WiFi.begin(ssid, password);
        while (WiFi.status() != WL_CONNECTED) {
            delay(1000);
            Serial.print(".");
        }
        Serial.println("\nWiFi bağlantısı tekrar kuruldu!");
    }

    
    HTTPClient http;
    http.begin(serverURL);
    http.setTimeout(5000); 

    int httpResponseCode = http.GET();
    if (httpResponseCode > 0) {
        String response = http.getString();
        Serial.println("Gelen yanit: ");
        Serial.println(response);

       
        StaticJsonDocument<200> doc;
        DeserializationError error = deserializeJson(doc, response);

        if (!error) {
            
            if (doc.containsKey("temperature") && doc.containsKey("humidity")) {
                float temperature = doc["temperature"];
                float humidity = doc["humidity"];

                
                display.clearDisplay();  
                display.setCursor(0, 0);  
                display.print("Sicaklik: ");
                display.print(temperature, 2); 
                display.println(" C");

                display.setCursor(0, 20); 
                display.print("Nem: ");
                display.print(humidity, 2);  
                display.println(" %");

                display.display();  

                
                if (temperature >= 25) {
                    digitalWrite(RED_LED, HIGH);
                    digitalWrite(BLUE_LED, LOW);
                    digitalWrite(YELLOW_LED, LOW);
                } else if (temperature <= 20) {
                    digitalWrite(RED_LED, LOW);
                    digitalWrite(BLUE_LED, HIGH);
                    digitalWrite(YELLOW_LED, LOW);
                } else if ( 21 >= temperature <= 24) {
                    digitalWrite(RED_LED, LOW);
                    digitalWrite(BLUE_LED, LOW);
                    digitalWrite(YELLOW_LED, HIGH);
                }
            } else {
                Serial.println("JSON anahtarları eksik!");
            }
        } else {
            Serial.println("JSON ayrıştırma hatası!");
        }
    } else {
        Serial.print("GET isteği hatası: ");
        Serial.println(httpResponseCode);
    }

    http.end();

    delay(5000); 
}
