#include "../include/my_gate_mqtt/MyAirportMQTT.hpp"

bool MyAirportMQTT::subscribeToMyNetwork(const char *net_topic)
{
    return my_mqtt_client.subscribe(net_topic);
}

void MyAirportMQTT::unsubscribeTheNetwork(const char *net_topic)
{
    my_mqtt_client.unsubscribe(net_topic);
}