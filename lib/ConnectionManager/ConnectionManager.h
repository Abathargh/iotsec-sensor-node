#ifndef CONNECTION_MANAGER_H_
#define CONNECTION_MANAGER_H_

namespace Iotsec {

struct ConnectionConfig {
    const char *ssid;
    const char *key;
    const char *serverFingerprint;
    
    const char *clientId;
    const char *username;
    const char *password;
};

void connect(ConnectionConfig config);
bool isConnected(void);
void reconnect(void);

}

#endif