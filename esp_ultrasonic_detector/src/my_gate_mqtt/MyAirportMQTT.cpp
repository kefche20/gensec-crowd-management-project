#include "../include/my_gate_mqtt/MyAirportMQTT.hpp"

MyAirportMQTT::MyAirportMQTT(Client &the_client, const char *the_server, int the_port,
                           const char *the_id, const char *the_username, const char *the_password,
                           void (*externalCallback)(char *, byte *, unsigned int))
    : my_mqtt_client(the_client), esp_client(the_client), my_server(the_server),
      my_net_port(the_port), my_id(the_id), mqtt_username(the_username), mqtt_password(the_password)
{
    my_mqtt_client.setServer(my_server, my_net_port);
    my_mqtt_client.setCallback(externalCallback);
    // my_mqtt_client.setCredentials(mqtt_username, mqtt_password);
}

MyAirportMQTT::~MyAirportMQTT()
{
    ;
}

void MyAirportMQTT::setServer(const char *the_server)
{
    my_server = the_server;
    my_mqtt_client.setServer(my_server, my_net_port);
}

void MyAirportMQTT::setPort(int the_port)
{
    my_net_port = the_port;
    my_mqtt_client.setServer(my_server, my_net_port);
}

void MyAirportMQTT::myCallBackFunction(MQTT_CALLBACK_SIGNATURE)
{
    my_mqtt_client.setCallback(callback);
}

void MyAirportMQTT::connectToMyNetwork()
{
    static uint32_t lastConnectingAttempt = 0;

    if ((millis() - lastConnectingAttempt) >= COM_DELAY)
    {
        Serial.println("Connecting");
        if (my_mqtt_client.connect(my_id, mqtt_username, mqtt_password))
        {
            Serial.println("Connected!");
            my_mqtt_client.subscribe(MY_GATE_TOPIC);
        }
        else
        {
            Serial.print("fail status: ");
            Serial.print(my_mqtt_client.state());
        }
        lastConnectingAttempt = millis();
    }
}

void MyAirportMQTT::myNetLoop()
{
    if (!amIconnected())
    {
        connectToMyNetwork();
    }
    else
    {
        my_mqtt_client.loop();
    }
}

bool MyAirportMQTT::amIconnected()
{
    return my_mqtt_client.connected();
}