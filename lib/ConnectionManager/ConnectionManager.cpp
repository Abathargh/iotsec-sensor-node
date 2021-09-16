#include "ConnectionManager.h"
#include "certs.h"

#include <PubSubClient.h>
#include <ESP8266WiFi.h>

namespace Iotsec {

static WiFiClientSecure wifiClient;
static PubSubClient mqttClient;
static X509List caList;

static ConnectionConfig config;


void connect(ConnectionConfig conf) {
    config = conf;

    caList.append(cert1);
	caList.append(cert2);
    wifiClient.setTrustAnchors(&caList);
	wifiClient.setFingerprint(config.serverFingerprint);

	WiFi.mode(WIFI_STA);
	WiFi.begin(config.ssid, config.key);
	
	int attempts = 0;
	while(WiFi.status() != WL_CONNECTED) {
		Serial.printf("Wifi connection: attempt %d\n", attempts);
		attempts++;
		delay(500);
	}

	attempts = 0;
	while(!mqttClient.connect(config.clientId, config.username, config.password)) {
		Serial.printf("Broker connection: attempt %d\n", attempts);
		attempts++;
		delay(500);
	}
}

bool isConnected(void) {
        return WiFi.isConnected() && mqttClient.connected();
}

void reconnect(void) {
    int attempts = 0;
	while(WiFi.status() != WL_CONNECTED) {
		WiFi.reconnect();
        Serial.printf("Wifi connection: attempt %d\n", attempts);
		attempts++;
		delay(500);
	}

	attempts = 0;
	while(!mqttClient.connect(config.clientId, config.username, config.password)) {
		Serial.printf("Broker connection: attempt %d\n", attempts);
		attempts++;
		delay(500);
	}
}

}
