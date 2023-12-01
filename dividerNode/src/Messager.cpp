#include "Messager.hpp"

    Messager::Messager(PubSubClient *mqttClient, MQTT_parameter *mqttParam, IDividerListener *divListener, int id) 
    {
        this->mqttClient = mqttClient;
        this->mqttParam = mqttParam;
        this->clientID = "esp32";

        this->divListener = divListener;

        // setup mqtt broker connection
        mqttClient->setServer(mqttParam->broker, MQTT_PORT);
        mqttClient->setCallback(callback);

        ConnecBroker();
    }

    int Messager::ConnectTopic(char *topic)
    {
        if (topic != nullptr)
        {
            return 0;
        }
        mqttClient->subscribe(topic);

        return 1;
    }
    
    void Messager::ConnecBroker()
    {
        // connect to mqtt
        while (!mqttClient->connected())
        {
            clientID += String(WiFi.macAddress());

            Serial.printf("%s is connecting...\n", clientID.c_str());
            if (mqttClient->connect(clientID.c_str(), mqttParam->username, mqttParam->password))
            {
                Serial.println("Connected to the broker!");
            }
            else
            {
                Serial.print("Error: ");
                Serial.print(mqttClient->state());
                delay(2000);
            }
        }
    }
