#define BLYNK_TEMPLATE_ID "TMPL6mNsKHWuW"
#define BLYNK_TEMPLATE_NAME "UAP IOT"
#define BLYNK_AUTH_TOKEN "_UdbeyM3FWleMDVeQi0zRQmpuemj7ZJA"
#define BLYNK_PRINT Serial
#define APP_DEBUG
#define USE_NODE_MCU_BOARD

#define chatID "6398443850"
#define botToken "7192459051:AAH50kN8Lba3En1Aa81oLVGtQttQArYnGBU"

#include <UniversalTelegramBot.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <Blynk.h>
#include <BlynkSimpleEsp8266.h>
#include <MQ2.h>

#define pinBuz D1 
#define ledMerah D8

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "RAIHANR";
char pass[] = "12345678";

int pinSmoke = A0;
float smoke;
MQ2 mq2(pinSmoke);

WiFiClientSecure client;
UniversalTelegramBot bot(botToken, client);

unsigned long lastSensorRead = 0;
const long sensorReadInterval = 2000; // Interval waktu dalam milidetik

bool isSmokeDetected = false; // Variabel status untuk mendeteksi perubahan kondisi
bool buzzerEnabled = true; // Variabel status untuk switch buzzer dan notifikasi

BLYNK_WRITE(V2) {
  int pin_value = param.asInt();
  buzzerEnabled = (pin_value == 1);
  if (!buzzerEnabled) {
    noTone(pinBuz);
  }else{
     tone(pinBuz, 400);
      Blynk.logEvent("smoke", "Bahaya, Ada Asap!!!!!");
      bot.sendMessage(chatID, "BAHAYA, ADA ASAP POTENSI KEBAKARAN!", "");
  }
}

void setup() {
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);

  pinMode(pinBuz, OUTPUT);
  pinMode(ledMerah, OUTPUT);
  digitalWrite(pinBuz, LOW);
  digitalWrite(ledMerah, LOW);
  mq2.begin();

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");
  client.setInsecure();

  // Inisialisasi LED merah di Blynk dalam keadaan mati
  Blynk.virtualWrite(V1, 0);
  // Inisialisasi switch buzzer di Blynk dalam keadaan aktif
  Blynk.virtualWrite(V2, 0);
}

void sendSensorData() {
  float* values = mq2.read(true); 
  smoke = mq2.readSmoke();
  Serial.print("Smoke Value: ");
  Serial.println(smoke); 
  Blynk.virtualWrite(V3, smoke);

  if (smoke > 1) {
    if (!isSmokeDetected) { 
      Serial.println("Smoke detected!"); 
      Blynk.logEvent("smoke", "Bahaya, Ada Asap!!!!!");
      bot.sendMessage(chatID, "BAHAYA, ADA ASAP POTENSI KEBAKARAN!", "");
      digitalWrite(pinBuz, HIGH);
      digitalWrite(ledMerah, HIGH);
      Blynk.virtualWrite(V1, 1); // LED merah menyala di Blynk
      Blynk.virtualWrite(V2, 1); // Switch buzzer ON di Blynk
      if (buzzerEnabled) {
        tone(pinBuz, 400);
      }
      isSmokeDetected = true; // Perbarui status kondisi
    }
  } else {
    if (isSmokeDetected) { // Periksa apakah kondisi berubah
      Serial.println("No smoke detected."); 
      digitalWrite(pinBuz, LOW);
      digitalWrite(ledMerah, LOW);
      Blynk.virtualWrite(V1, 0); // LED merah mati di Blynk
      noTone(pinBuz);
      isSmokeDetected = false; // Perbarui status kondisi
    }
  }
}

void loop() {
  Blynk.run();

  if (millis() - lastSensorRead >= sensorReadInterval) {
    lastSensorRead = millis();
    sendSensorData();
  }
}
