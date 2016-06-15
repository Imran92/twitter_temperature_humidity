#include <SoftwareSerial.h>

// Example testing sketch for various DHT humidity/temperature sensors
// Written by ladyada, public domain

#include "DHT.h"

#define DEBUG true
    
#define DHTPIN 2     // what digital pin we're connected to
#define button 3     // input of button

#define APIKEY "XXXXXXXXXXXXX" //  api key your twitter in thingspeak

#define TARGET_IP "184.106.153.149"//direccion IP thingspeak

#define TARGET_PORT "80"          ///  puerto 80

#define ID "XXXXXX"  //name of wireless access point to connect to
#define PASS "XXXXXXXX"  //wifi password


String getreq;
SoftwareSerial esp8266(10, 11);

// Uncomment whatever type you're using!
#define DHTTYPE DHT11   // DHT 11
//#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

// Connect pin 1 (on the left) of the sensor to +5V
// NOTE: If using a board with 3.3V logic like an Arduino Due connect pin 1
// to 3.3V instead of 5V!
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

// Initialize DHT sensor.
// Note that older versions of this library took an optional third parameter to
// tweak the timings for faster processors.  This parameter is no longer needed
// as the current DHT reading algorithm adjusts itself to work on faster procs.
DHT dht(DHTPIN, DHTTYPE);

void setup() {

  Serial.begin(9600);
  Serial.println("DHTxx test!");
  esp8266.begin(9600);
  sendData("AT+RST\r\n", 1000, DEBUG); //

  sendData("AT+CWMODE=1\r\n", 1000, DEBUG); //

  String cmd = "AT+CWJAP=\"";
  cmd += ID;
  cmd += "\",\"";
  cmd += PASS;
  cmd += "\"";
  sendData( cmd + "\r\n", 1000, DEBUG); //


  sendData("AT+CIPMUX=0\r\n", 1000, DEBUG); //

  pinMode(3, INPUT);
  pinMode(4, OUTPUT);

  dht.begin();
}

void loop() {
  digitalWrite(4 , HIGH);
  // Wait a few seconds between measurements.
  delay(2000);

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" *C ");
  Serial.print(f);
  Serial.print(" *F\t");
  Serial.print("Heat index: ");
  Serial.print(hic);
  Serial.print(" *C ");
  Serial.print(hif);
  Serial.println(" *F");

  if (digitalRead(3) == LOW)
  {
    char humstr[15];
    char tempstr[15];

    dtostrf(h,5,2,humstr);
    dtostrf(t,5,2,tempstr);

    String hum = humstr;
    String tem = tempstr;
    
    Serial.println("got a click");
    digitalWrite(4 , LOW);
    String  updateTwitterStatus = ("Temp " + tem + " degree C n humidity "+ hum+" percent.. Onak gorom lagtasa frans");

    String webpage = "AT+CIPSTART=\"TCP\",\"";
    webpage += TARGET_IP;
    webpage += "\",80\r\n";
    sendData(webpage, 1000, DEBUG);

    //getreq = "POST https://api.thingspeak.com/apps/thingtweet/1/statuses/update?api_key=XXXXXXXXXXX&status=I just posted this from my thing!\r\n";
    //sprintf(getreq,"POST https://api.thingspeak.com/apps/thingtweet/1/statuses/update?api_key=XXXXXXXXXXX&status=Humidity %f and temperature is %f at imran's house now", h, t );

    // Create HTTP POST Data

    String tsData = "api_key=" APIKEY "&status=" + updateTwitterStatus ;

    String webpage1 = "POST /apps/thingtweet/1/statuses/update HTTP/1.1\n";
    webpage1 += "Host: api.thingspeak.com\n";
    webpage1 += "Connection: close\n";
    webpage1 += "Content-Type: application/x-www-form-urlencoded\n";
    webpage1 += "Content-Length: ";
    webpage1 += tsData.length();
    webpage1 += "\n\n";
    webpage1 += tsData;

    //// declaring the number of bytes to be sent

    String cipsend = "AT+CIPSEND=";
    cipsend += webpage1.length();
    cipsend += "\r\n";

    sendData(cipsend, 1000, DEBUG);
    sendData(webpage1, 1000, DEBUG);

    //// closing the connection
    sendData("AT+CIPCLOSE=0\r\n", 1500, DEBUG);

    digitalWrite(4 , HIGH);
  }
}

String sendData(String command, const int timeout, boolean debug)
{
    String response = "";
    
    esp8266.print(command); // send the read character to the esp8266
    
    long int time = millis();
    
    while( (time+timeout) > millis())
    {
      while(esp8266.available())
      {
        // The esp has data so display its output to the serial window 
        char c = esp8266.read(); // read the next character.
        response+=c;
      }  
    }
    
    if(debug)
    {
      Serial.print(response);
    }
    
    return response;
}
