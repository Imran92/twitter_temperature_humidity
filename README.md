# twitter_temperature_humidity
senses temperature and humidity through DHT11 sensor, tweets that. uses ESP8266 module to connect to wifi

To use it for yourself, change the ID and PASS in the definition section. 

Open a thingspeak account. Link your twitter with it. It'll generate and API key for you. Put that in the API key definition.

Take a resistor and connect one end of it to 5v and the other end to Button pin (see the definition above the code, in my arduino uno, pin 3 is the button pin). Now connect the button to pin 3 in such a way that the button port gets short with ground when the button is pressed. 

