#include <ESP8266WiFi.h>
#include <DHT.h>  // there are multiple kinds of DHT sensors
#include <FirebaseESP8266.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_BMP280.h>



#define API_KEY "AIzaSyAvFnAIPZTPWjLfqFYmm-_cBk3bviU0Qm4"
//#define DHTTYPE DHT22 //--> Defines the type of DHT sensor used (DHT11, DHT21, and DHT22), in this project the sensor used is DHT11.
#define DATABASE_URL "https://signindemo-47c49-default-rtdb.firebaseio.com" 
#define rainDigital 16

Adafruit_BMP280 bmp; // I2C
FirebaseData firebaseData;
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
FirebaseJson json;
FirebaseJson json2;
unsigned long sendDataPrevMillis = 0;
int count = 0;
bool signupOK = false;
const int DHTPin = 5; //--> The pin used for the DHT11 sensor is Pin D1 = GPIO5


//
DHT dht(0,DHT22);
const char *ssid = "JioFiber-2G";
const char *password = "11223344";
const long utcOffsetInSeconds = 19800;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

WiFiClient client;

void setup() {
  
//Connect to WiFi Network
   Serial.begin(9600);
   delay(500);
   Serial.print("Connecting to WiFi");
   Serial.println("...");
   WiFi.begin(ssid, password);
   int retries = 0;
   while ((WiFi.status() != WL_CONNECTED) && (retries < 5)) 
   {
   retries++;
   delay(500);
    }
if (retries > 14) {
    Serial.println(F("WiFi connection FAILED"));
}
if (WiFi.status() == WL_CONNECTED) {
    Serial.println(F("WiFi connected!"));
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}


Serial.println("Running DHT22");
  Serial.println("-------------------------------------");
  dht.begin();



  Serial.println("Running BMP280!");
  Serial.println("-------------------------------------");
  bmp.begin(0x76);
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500);
  

  Serial.println("Running BMP280!");
  Serial.println("-------------------------------------");

     /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Sign up */
  Serial.printf("Firebase Status :",Firebase.signUp(&config, &auth, "", ""));
  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("Connected");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

 timeClient.begin();
 Firebase.begin(&config, &auth);
    Serial.println(F("Setup ready"));

}
void loop() {
  
  timeClient.update();
int h = dht.readHumidity();
int t = dht.readTemperature();
int p = bmp.readPressure()/100;
int a = bmp.readAltitude(1000);
String tm = String(timeClient.getHours())+":"+String(timeClient.getMinutes())+":"+String(timeClient.getSeconds());
   Serial.println("-----------------------------------------------");
Serial.println("Time = "+tm);

 String Temp = "Temperature : " + String(t) + " C";
  String Humi = "Humidity : " + String(h) + " %";
  String Pres = "Pressure : "+ String(p)+" hPa";
  String Alti = "Altitude : "+String(a)+" m";
  Serial.println(Temp);
  Serial.println(Humi);
  Serial.println(Pres);
  Serial.println(Alti);
  
  //Firebase.reconnectWiFi(true);
//
//  Firebase.RTDB.setString(&fbdo,"/ESP8266/time",tm);
//  Firebase.RTDB.setString(&fbdo, "/ESP8266/time/Humidity/value", h);

  json.set("time",tm);
  json.set("value",String(h));
  json2.set("time",tm);
  json2.set("value",String(t));
  Firebase.RTDB.setJSON(&fbdo,"/ESP8266/Humidity",&json);
  Firebase.RTDB.setJSON(&fbdo,"/ESP8266/Temperature",&json2);


  delay(500);



  
  delay(2000);


  
}
