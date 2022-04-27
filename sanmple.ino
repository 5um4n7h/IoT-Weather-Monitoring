#include <ESP8266WiFi.h>
#include <DHT.h>  // there are multiple kinds of DHT sensors
#include <FirebaseESP8266.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#define API_KEY "AIzaSyAvFnAIPZTPWjLfqFYmm-_cBk3bviU0Qm4"
#define DHTTYPE DHT22 //--> Defines the type of DHT sensor used (DHT11, DHT21, and DHT22), in this project the sensor used is DHT11.
#define DATABASE_URL "https://signindemo-47c49-default-rtdb.firebaseio.com" 


FirebaseData firebaseData;
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
FirebaseJson json;
unsigned long sendDataPrevMillis = 0;
int count = 0;
bool signupOK = false;
const int DHTPin = 05; //--> The pin used for the DHT11 sensor is Pin D1 = GPIO5

DHT dht(DHTPin,DHT22);
const char *ssid = "JioFiber-2G";
const char *password = "11223344";
const long utcOffsetInSeconds = 19800;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

WiFiClient client;

void setup() {
  
//Connect to WiFi Network
   Serial.begin(9600);
   Serial.print("Connecting to WiFi");
   Serial.println("...");
   WiFi.begin(ssid, password);
   int retries = 0;
   while ((WiFi.status() != WL_CONNECTED) && (retries < 15)) {
   retries++;
  
   delay(500);
   Serial.print(".");
}
if (retries > 14) {
    Serial.println(F("WiFi connection FAILED"));
}
if (WiFi.status() == WL_CONNECTED) {
    Serial.println(F("WiFi connected!"));
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}
    Serial.println(F("Setup ready"));



  
  dht.begin();

  Serial.println("Running DHT!");
  Serial.println("-------------------------------------");

     /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Sign up */
  Serial.printf("Status :",Firebase.signUp(&config, &auth, "", ""));
  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

 timeClient.begin();
 Firebase.begin(&config, &auth);
  Serial.print("Done");

}
void loop() {
  dht.begin();
  timeClient.update();
int h = dht.readHumidity();
Serial.println(h);
int t = dht.readTemperature();
String tm = String(timeClient.getHours())+":"+String(timeClient.getMinutes())+":"+String(timeClient.getSeconds());
Serial.println("Time = "+tm);

 String Temp = "Temperature : " + String(t) + " C";
  String Humi = "Humidity : " + String(h) + " %";
  Serial.println(Temp);
  Serial.println(Humi);
  
  //Firebase.reconnectWiFi(true);
//
//  Firebase.RTDB.setString(&fbdo,"/ESP8266/time",tm);
//  Firebase.RTDB.setString(&fbdo, "/ESP8266/time/Humidity/value", h);

  json.set("time",tm);
  json.set("value",String(h));
  Firebase.RTDB.setJSON(&fbdo,"/ESP8266/Humidity",&json);



  
  delay(2500);


  
}
