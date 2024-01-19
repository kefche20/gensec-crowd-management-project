#include "../include/my_gate_mqtt/MyAirportMQTT.hpp"

bool MyAirportMQTT::publishToMyNetwork(const char *net_topic, const char *net_message)
{
    return my_mqtt_client.publish(net_topic, net_message);
}