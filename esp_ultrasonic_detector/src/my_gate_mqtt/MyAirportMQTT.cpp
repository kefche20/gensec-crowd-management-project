#include "../include/my_gate_mqtt/MyAirportMQTT.hpp"

MyAirportMQTT::MyAirportMQTT(WiFiClient &the_client, const char *the_server, int the_port,
                             const char *the_id)
    : my_mqtt_client(the_client), esp_client(the_client), my_server(the_server),
      my_net_port(the_port), my_id(the_id)
{
    ;
}

MyAirportMQTT::~MyAirportMQTT()
{
    ;
}

void MyAirportMQTT::setupMqtt(void (*externalCallback)(char *, byte *, unsigned int))
{
    mqtt_username = MY_MQTT_USERNAME;
    mqtt_password = MY_MQTT_PASSWORD;
    my_mqtt_client.setServer(my_server, my_net_port);
    my_mqtt_client.setCallback(externalCallback);
    connectToMyNetwork();
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
    String id_to_connect = String(my_id);
    id_to_connect += String(WiFi.macAddress());
    Serial.println(id_to_connect);

    if ((millis() - lastConnectingAttempt) >= COM_DELAY)
    {
        Serial.println("Connecting");
        if (my_mqtt_client.connect(id_to_connect.c_str(), mqtt_username, mqtt_password))
        {
            Serial.println(id_to_connect);
            Serial.println("Connected!");
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