#include <Wire.h>    // I2C library
#include "ccs811.h"  // CCS811 library
#include <BH1750.h>
#include <ESP8266WiFi.h>      //for wifi connectivity
#include <DHT.h>              //for Temp and Humidity Sensor Library
#include <FirebaseESP8266.h>  //Library used to connect ESP8266 to Firebase
#include <NTPClient.h>        //Network time protocol client, to get current time
#include <WiFiUdp.h>          //UDP protocol to receive data for NTP client
#include <Adafruit_BMP280.h>   //BMP sensor library
#include <BH1750.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define WIRE_HAS_END 1
Adafruit_BMP280 bmp; //Declaring object for bmp sensor
BH1750 light;
Adafruit_SSD1306 oled(128, 64, &Wire, -1);
CCS811 ccs811(0); // nWAKE on D3
DHT dht(14,DHT22);         //d3=3 and sensor type

WiFiUDP ntpUDP;         //UDP cliet for ntp
NTPClient timeClient(ntpUDP, "pool.ntp.org", 19800);     //declaring NTPclient object
FirebaseData fbdo;                                        //firebase database object to write data
FirebaseJson json;                                         //json objects to upload data as key-value pairs

   //to match the time zone of india
String rf = "";

void setup() {
  // Enable serial
  Serial.begin(9600);
   // Enable CCS811
  Wire.begin(); 
  ccs811.set_i2cdelay(50); // Needed for ESP8266 because it doesn't handle I2C clock stretch correctly
  ccs811.begin();
  ccs811.start(CCS811_MODE_1SEC);

  
  dht.begin();
  bmp.begin(0x76);
  light.begin();
  oled.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500);

  

  

  Serial.println("Connecting to WiFi...");
  WiFi.begin("JioFiber-2G", "11223344"); 
  int retries=0;
  while ((WiFi.status() != WL_CONNECTED)&&retries<21)//waiting to connect to wifi till 10secs
  {         
    retries++;
    delay(500);
   }
    
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi connected ! IP address :"+WiFi.localIP().toString());
  }else{
   Serial.println("WiFi connection FAILED");
  }

  Firebase.begin("https://weather-monitoring-56d1f-default-rtdb.firebaseio.com/", "Km0E6KCRw8AaACZBWI9VjRERRi9fVvuectdCYZyj");

}


void loop() {
  timeClient.update();            //updating time
int h = dht.readHumidity();       
int t1 = dht.readTemperature();
int t2 = bmp.readTemperature();
int t;
if(t1>100){
  t = t2;
}else t = t1;
int p = bmp.readPressure()/100;
int a = bmp.readAltitude();
int l = light.readLightLevel();
int r = analogRead(A0);
  if(r>800){
    rf = "No Rain";
  }else if(r<=800&&r>550)
{
  rf = "Light Rain";
}else{

  rf = "Heavy Rain";
}

 // Enable I2C
   Wire.endTransmission();
   Wire.begin(); 
  uint16_t eco2, etvoc, errstat, raw;
  ccs811.read(&eco2,&etvoc,&errstat,&raw); 


String tm = String(timeClient.getHours())+":"+String(timeClient.getMinutes())+":"+String(timeClient.getSeconds());


 String Temp = "Temp: " + String(t) + "C";
  String Humi = "Hum: " + String(h) + "%";
  String Pres = "Prss: "+ String(p)+"hPa";
  String Alti = "Alt: "+String(a)+" m";
  String Light = "LI: "+String(l)+"lux";
  String Rain = "Rain: "+rf;
  String Time = "Time= "+tm;
  String CO2 = "CO2 : "+String(eco2)+"ppm";
  String VOC = "VOC : "+String(etvoc)+"ppb";
   Serial.println("-----------------------------------------------\n");
  Serial.println(Time+"\n"+Temp+"\n"+Humi+"\n"+Pres+"\n"+Alti+"\n"+Light+"\n"+Rain+"\n"+CO2+"\n"+VOC);

 

 
   oled.clearDisplay(); // clear display

  oled.setTextSize(0.2);          // text size
  oled.setTextColor(WHITE);     // text color
  oled.setCursor(0, 0);        // position to display
  oled.println(Time+"\n"+Temp+"  "+Humi+"\n"+Pres+"  "+Alti+"\n"+Light+"\n"+Rain+"\n"+CO2+"  "+VOC);
 // text to display
  oled.display();     
 // oled.startscrolldiagright(0,0);// show on OLED

  


//creating JSON objects

json.set("time",tm);
json.set("Humidity",String(h));
json.set("Temperature",String(t));
json.set("Pressure",String(p));
json.set("Altitude",String(a));
json.set("Light",String(l));
json.set("Rain",rf);
json.set("CO2",String(eco2));
json.set("VOC",String(etvoc));



  //Uploading to firebase
  
  Firebase.RTDB.setJSON(&fbdo,"/ESP8266",&json);




  //5 seconds delay for every reading
  delay(5000);

   
  

  
  // Wait
  delay(1000); 
}
