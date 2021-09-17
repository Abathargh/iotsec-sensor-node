#include "ConnectionManager.h"
#include "../../certs/certs.h"

#include <ESP8266WiFi.h>

namespace Iotsec {

static WiFiClientSecure wifiClient;
static PubSubClient mqttClient;
static X509List caList;

void connect(ConnectionConfig &config) {
	caList.append(cert1);
	caList.append(cert2);
    wifiClient.setTrustAnchors(&caList);
	wifiClient.setFingerprint(config.serverFingerprint.c_str());

	mqttClient.setServer(config.domain.c_str(), config.port);
	mqttClient.setClient(wifiClient);

	WiFi.mode(WIFI_STA);
	WiFi.begin(config.ssid, config.key);
	
	int attempts = 0;
	while(WiFi.status() != WL_CONNECTED) {
		Serial.printf("Wifi connection: attempt %d\n", attempts);
		attempts++;
		delay(500);
	}

	attempts = 0;
	while(!mqttClient.connect(config.clientId.c_str(), config.username.c_str(), config.password.c_str())) {
		Serial.printf("Broker connection: attempt %d\n", attempts);
		attempts++;
		delay(500);
	}
}

bool isConnected(void) {
        return WiFi.isConnected() && mqttClient.connected();
}

void reconnect(ConnectionConfig &config) {
    int attempts = 0;
	while(WiFi.status() != WL_CONNECTED) {
		WiFi.reconnect();
        Serial.printf("Wifi connection: attempt %d\n", attempts);
		attempts++;
		delay(500);
	}

	attempts = 0;
	while(!mqttClient.connect(config.clientId.c_str(), config.username.c_str(), config.password.c_str())) {
		Serial.printf("Broker connection: attempt %d\n", attempts);
		attempts++;
		delay(2000);
	}
}

PubSubClient& mqttClientRef() {
	return mqttClient;
}

}
