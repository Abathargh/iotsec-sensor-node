#ifndef SERVICE_H_
#define SERVICE_H_

#include <PubSubClient.h>
#include <assert.h>

namespace Iotsec
{

static PubSubClient *client = nullptr;
void setMqttClient(PubSubClient &mqttClient)
{
    client = &mqttClient;
}

class Loopable
{
    protected:
        Loopable() {};
    public:
        virtual void loop() = 0;
        ~Loopable() = default;
};

template<int size>
class ServicePool
{
    private:
        int nextFreeIndex;
        int period;
        Loopable* services[size];
    
    public:
        ServicePool(int period);
        bool addService(Loopable *service);
        void loop();
        ~ServicePool() = default;
};

template<int size>
ServicePool<size>::ServicePool(int loopPeriod) : nextFreeIndex(0)
{
    static_assert(size > 0, "The size template parameter and the period constructor parameter must be positive");
    assert(loopPeriod > 0);
    period = loopPeriod;
}

template<int size>
bool ServicePool<size>::addService(Loopable *service)
{
    if(nextFreeIndex < size)
    {
        services[nextFreeIndex++] = service;
        return true;
    }
    return false;
}

template<int size>
void ServicePool<size>::loop()
{
    for(int i = 0; i < nextFreeIndex; i++)
    {
        services[i]->loop();
        delay(period);
    }
}



template<typename T>
using SensorCallback = T (*)();


template<typename T>
class Service : public Loopable
{
    private:
        const char* serviceName;
        SensorCallback<T> callback;
    
    public:
        Service(const char* serviceName);
        Service(String serviceName);
        void setAcquisitionCallback(SensorCallback<T> acquisitionCallback);
        void loop() override;
};


template<typename T>
Service<T>::Service(const char* serviceName) : Loopable(), serviceName(serviceName) {
    static_assert(std::is_arithmetic<T>::value || std::is_same<T, String>::value, "T can only be a number or a String");
}

template<typename T>
Service<T>::Service(String serviceName) : Loopable(), serviceName(serviceName.c_str()) {
    static_assert(std::is_arithmetic<T>::value || std::is_same<T, String>::value, "T can only be a number or a String");
}

template<typename T>
void Service<T>::setAcquisitionCallback(SensorCallback<T> acquisitionCallback)
{
    callback = acquisitionCallback;
}

template<typename T>
void Service<T>::loop()
{
    if(callback != nullptr && client != nullptr)
    {
        T data = callback();
        client->publish(serviceName, String(data).c_str());
    }
}


} // namespace Iotsec



#endif