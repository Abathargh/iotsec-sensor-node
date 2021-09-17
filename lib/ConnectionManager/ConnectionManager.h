#ifndef CONNECTION_MANAGER_H_
#define CONNECTION_MANAGER_H_

#include <stdint.h>
#include <PubSubClient.h>

namespace Iotsec {

struct ConnectionConfig {
    String ssid;
    String key;
    String serverFingerprint;

    String clientId;
    String username;
    String password;

    String domain;
    uint16_t port;
};


void connect(ConnectionConfig &config);
void reconnect(ConnectionConfig &config);
bool isConnected(void);

PubSubClient& mqttClientRef();

}

#endif