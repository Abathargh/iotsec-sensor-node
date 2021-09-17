#include <Arduino.h>

#include <DHT.h>

#include <Service.h>
#include <ConnectionManager.h>

#include <stdint.h>


#define TO_STR(str) _TO_STR(str)
#define _TO_STR(str) #str

#ifndef FINGERPRINT
#define FINGERPRINT "" 
#endif

#ifndef DOMAIN_NAME
#define DOMAIN_NAME ""
#endif

#ifndef PORT
#define PORT 0
#endif

#ifndef USERNAME
#define USERNAME ""
#endif

#ifndef PASSWORD
#define PASSWORD ""
#endif

#ifndef WIFI_SSID
#define WIFI_SSID ""
#endif

#ifndef KEY
#define KEY ""
#endif

#ifndef CLIENTID
#define CLIENTID ""
#endif


/* Connection defines */
const char ssid[] = TO_STR(WIFI_SSID);
const char key[] = TO_STR(KEY);

/* MQTT Server defines */
const char serverFingerprint[] = TO_STR(FINGERPRINT);
const char clientId[] = TO_STR(CLIENTID);
const char username[] = TO_STR(USERNAME);
const char password[] = TO_STR(PASSWORD);
const char domain[] = TO_STR(DOMAIN_NAME);
constexpr uint16_t port = PORT;
Iotsec::ConnectionConfig config;



/* Pin and sensor devices defines */
constexpr int presencePin = 0;
constexpr int dhtPin = 2;
constexpr int dhtType = DHT11;
constexpr int numSensors = 3;
constexpr int poolPeriod = 500;
constexpr int loopPeriod = 1000;

/* Device virtual entities */
DHT DhtSensor(dhtPin, dhtType);
Iotsec::Service<String> presenceService("presence");
Iotsec::Service<float> temperatureService("temperature");
Iotsec::Service<float> humidityService("humidity");

Iotsec::ServicePool<numSensors> servicePool(poolPeriod);

/* Acquisition callback for the presence sensor */
String acquirePresence() {
	int presence = digitalRead(presencePin);
	return presence == 0 ? String("no-presence") : String("presence");
}


/* Acquisition callback for the temperature sensor */
float acquireTemperature() {
	float temp = DhtSensor.readTemperature();
	return temp;
}


/* Acquisition callback for the humidity sensor */
float acquireHumidity() {
	float hum = DhtSensor.readHumidity();
	return hum;
}


void setup() {
	delay(3000);
	config.clientId          = clientId;
	config.domain            = domain;
	config.key               = key;
	config.password          = password;
	config.port              = port;
	config.serverFingerprint = serverFingerprint;
	config.ssid              = ssid;
	config.username          = username;

	Serial.begin(9600);

	Serial.println("id:" + config.clientId);
	Serial.println("user:" + config.domain);
	Serial.println("user:" + config.key);
	Serial.println("user:" + config.password);
	Serial.println("user:" + config.port);
	Serial.println("user:" + config.serverFingerprint);
	Serial.println("user:" + config.ssid);
	Serial.println("user:" + config.username);


	pinMode(presencePin, INPUT);
	DhtSensor.begin();
	
	Iotsec::connect(config);
	Iotsec::setMqttClient(Iotsec::mqttClientRef());
	
	presenceService.setAcquisitionCallback(acquirePresence);
	humidityService.setAcquisitionCallback(acquireHumidity);
	temperatureService.setAcquisitionCallback(acquireTemperature);

	servicePool.addService(&presenceService);
	servicePool.addService(&humidityService);
	servicePool.addService(&temperatureService);
}

void loop() {
	if(!Iotsec::isConnected()){
		Iotsec::reconnect(config);
	}
	servicePool.loop();
	delay(loopPeriod);
}
