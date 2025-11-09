#include <DHT.h> //library sensor DHT11
#include <ESP8266WiFi.h> //library konfigurasi Wi-Fi ESP8266
#include <Firebase_ESP_Client.h> //library konfigurasi Firebase terbaru untuk ESP8266

// Library tambahan wajib untuk Firebase
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

#define DHTPIN D4 // Pin sensor DHT11
#define DHTTYPE DHT11 // Jenis sensor DHT11
#define MQ_PIN A0 // Pin sensor MQ135
#define BUZZER D0 // Pin buzzer
#define LED_GREEN D6 // Pin LED hijau
#define LED_RED D5 // Pin LED merah

String path = "KELOMPOK-193"; //nama kelompok
const char* ssid = "Fahmy Almaliki"; // SSID WiFi Anda
const char* password = "123456788"; // Kata sandi WiFi Anda

// Konfigurasi Firebase
#define API_KEY "AIzaSyBmK0m-hMmzT8A9xCeacRVyEzhJKOGqDAk" // Kunci API Firebase Anda
#define DATABASE_URL "https://traininghardskill2025-default-rtdb.firebaseio.com/" // URL Firebase Realtime Database Anda

DHT dht(DHTPIN, DHTTYPE); // Membuat objek DHT untuk sensor DHT11
FirebaseData firebaseData; // Objek FirebaseData tambahan
FirebaseAuth auth; // Objek autentikasi Firebase
FirebaseConfig config; // Objek konfigurasi Firebase

void setup() {
  Serial.begin(115200); // Inisialisasi komunikasi Serial dengan baud rate 115200
  dht.begin(); // Inisialisasi sensor DHT11
  
  pinMode(BUZZER, OUTPUT); // Mengatur pin BUZZER sebagai OUTPUT
  pinMode(LED_GREEN, OUTPUT); // Mengatur pin LED_GREEN sebagai OUTPUT
  pinMode(LED_RED, OUTPUT); // Mengatur pin LED_RED sebagai OUTPUT

  WiFi.begin(ssid, password); // Mulai koneksi WiFi dengan SSID dan kata sandi
  Serial.print("Connecting to WiFi"); //Serial.print memunculkan teks di serial monitor
  while (WiFi.status() != WL_CONNECTED) { // Tunggu sampai terhubung ke WiFi
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi"); // Pesan konfirmasi jika terhubung ke WiFi

  // Konfigurasi Firebase
  config.api_key = API_KEY; // Set API Key Firebase
  config.database_url = DATABASE_URL; // Set URL Realtime Database Firebase

  // Sign in anonymous (tanpa akun)
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("Firebase sign-up sukses");
  } else {
    Serial.printf("Sign-up gagal, alasan: %s\n", config.signer.signupError.message.c_str());
  }

  Firebase.begin(&config, &auth); // Mulai koneksi ke Firebase
  Firebase.reconnectWiFi(true); // Mengatur ulang koneksi WiFi jika terputus
  Serial.println("Terhubung Firebase"); // Pesan konfirmasi jika terhubung ke Firebase
}

void loop() {
  float humidity = dht.readHumidity(); // Membaca kelembaban dari sensor DHT11
  float temperature = dht.readTemperature(); // Membaca suhu dari sensor DHT11
  
  if (isnan(humidity) || isnan(temperature)) { // Memeriksa apakah pembacaan DHT11 valid
    Serial.println("Error membaca DHT11"); // Pesan kesalahan jika pembacaan DHT11 tidak valid
  } else {
    Serial.print("Suhu: "); //Serial.print memunculkan teks nilai suhu
    Serial.print(temperature);
    Serial.println(" °C"); //Serial.print memunculkan teks satuan suhu
    Serial.print("Kelembaban: "); //Serial.print memunculkan teks nilai kelembaban
    Serial.print(humidity);
    Serial.println(" %"); 
  }

  // Membaca nilai dari sensor MQ135
  int mqValue = analogRead(MQ_PIN);
  Serial.print("Nilai MQ135: ");
  Serial.println(mqValue); //Serial.print memunculkan teks nilai MQ135

  // Mengirim data ke Firebase
  if (Firebase.RTDB.setFloat(&firebaseData, path + "/temperature", temperature))
    Serial.println("Suhu terkirim ke Firebase");
  else
    Serial.println("Gagal kirim suhu: " + firebaseData.errorReason());

  if (Firebase.RTDB.setFloat(&firebaseData, path + "/humidity", humidity))
    Serial.println("Kelembaban terkirim ke Firebase");
  else
    Serial.println("Gagal kirim kelembaban: " + firebaseData.errorReason());

  if (Firebase.RTDB.setFloat(&firebaseData, path + "/mq135", mqValue))
    Serial.println("Nilai MQ135 terkirim ke Firebase");
  else
    Serial.println("Gagal kirim MQ135: " + firebaseData.errorReason());
  
  // Mengaktifkan buzzer jika nilai MQ135 melebihi 500
  if (mqValue > 500) {
    digitalWrite(BUZZER, HIGH);
  } else {
    digitalWrite(BUZZER, LOW);
  }

  // Mengontrol dua LED sesuai dengan nilai suhu
  if (temperature < 28.0) {
    digitalWrite(LED_GREEN, HIGH);
    digitalWrite(LED_RED, LOW);
  } else {
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_RED, HIGH);
  }

  delay(2000); // Delay selama 2 detik
}
