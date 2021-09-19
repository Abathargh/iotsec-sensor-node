# IoT Security - A Proof of Concept architecture for a secure IoT sensor based on ESP8266 + Arduino-core

This repository contains a firmware based on the Arduino-core framework for ESP8266-01 boards, custom made to make it possible to securely send sensor data via MQTT to a Broker secured with TLS + username and password.

## Table of Contents

  - [Architecture](#architecture)
    - [The Connection Manager module](#the-connection-manager-module)
    - [The Service module](#the-service-module)
    - [Passing the secure data](#passing-the-secure-data)
    - [The board and the hardware](#the-board-and-the-hardware)
  - [Setup and configuration](#setup-and-configuration)
    - [Requirements](#requirements)
    - [Generating secrets and certs.h](#generating-secrets-and-certsh)
  - [Building](#building)
  - [License](#license)

## Architecture 

The project was developed using the Platformio environment and the Arduino-core FW framework, as a loop-based firmware, with an approach close to a bare-metal application, striving to have a completely statically allocated memory usage, to enforce security even more.

The firmware was written in C++11, with some python scripts that are used to both configure the private data to embed in the board flash memory and to pass custom build flags to the compiler. All the application code was organized in modules, which are all part of the **Iotsec** namespace.

### The Connection Manager module

The Connection Manager module is tasked with the management of the WiFi and MQTT connection and eventual re-connection. It internally uses a struct that can be passed at startup, to save the connection data and leverages it to reconnect if something goes wrong network-wise.

### The Service module

The Service module contains a number of utility classes that can be used to realize virtual entities that refer to the service offered by a sensor connected to the board. The **Service** class is a template class that can be used to abstract a sensor returning a specific type of data, through the use of the specified templated type. The user can then pass a custom acquisition of the ```T (*)()``` type, that is used within the loop to acquire data from the sensor periodically:

```C++
float acquireTemperature() {
	float temp = DhtSensor.readTemperature();
	return temp;
}
```

Another construct that is available from the Service module is the **ServicePool** class, a templated class with parametrized length usable through the template parameter. This allows the user to define a pool of schedulable services at compile-time, calling the acquisition functions and forwarding their payloads to the broker, with a period between one service acquisition+forwarding and the other that is user configurable. The schedule that is followed for these operations has a round-robin policy:

```C++
Iotsec::Service<String> presenceService("presence");
Iotsec::Service<float> temperatureService("temperature");
Iotsec::Service<float> humidityService("humidity");

Iotsec::ServicePool<numSensors> servicePool(poolPeriod);    

...

// in a setup function
servicePool.addService(&presenceService);
servicePool.addService(&humidityService);
servicePool.addService(&temperatureService);

...

// in your loop function
servicePool.loop();

```

When initializing a Service Object, the passed service name will be used to build a topic in the ```sensor/service/<serviceName>```, where \<serviceName\> is the passed name. This is the full topic that will be used by the MQTT client to publish messages.

### Passing the secure data

In order to build the firmware with the correct connection credentials, you have to generate a file containing your **secrets**. This is possible through the *conf_init.py*, that will prompt you with easy to follow instructions. This will result in creating two important files:

- **certs/certs.h**, which contains the chain certificates of the certificate authority formatted as c++ arrays of string literals, to be stored in the flash memory of the ESP (safer and less heavy on the already small RAM).
- **data.json**, which is the *secrets* file and contains confidential data that is passed securely directly to the compiler without embedding it into the source code.

This last point is implemented via passing the credentials as build flags for the C++ compiler, using the **Dynamic build flags** feature of the Platformio pipeline, where you can redirect the stdout of a script or executable towards the build flags in input to the compiler.


### The board and the hardware

The hardware used to implement the project is the following:

- An **ESP8266-01** chip, which is an inexpensive Wi-Fi enabled chip from Espressif systems, based around the Tensilica Diamond Standard 106Micro architecture, working with 3.3V;
- A **HC-SR501** PIR motion detection sensor, which is used as a presence sensor within the room where the node is deployed;
- A **DHT11** temperature and humidity sensor, which is used to collect data relative to these characteristics in the room where the node is deployed.

## Setup and configuration

### Requirements

- PlatformIO
- Python3.6+

### Generating secrets and certs.h

Before starting, you have to generate your certificates on the server, and copy your CA certificates and your server certificates to the **certs** directory, naming the CA cert as **chain.pem** and the server cert as **cert.pem**.

To setup the environment before building, the first thing to do is to call the configuration init python script:

```bash
cd scripts
python conf_init.py
```

Follow the instructions, and you will obtain a data.json secrets file and a certs.h file with your certificates in C++ form.

## Building 

You can build the firmware in debug mode (with debug symbols and log messages) or release mode, by using the platformio commands from terminal or directly from the IDE of your choice.

## License

This project is published under the Apache 2.0 open source license, a copy of which is available in the LICENSE file.