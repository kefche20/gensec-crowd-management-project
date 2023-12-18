#ifndef CONFIG
#define CONFIG
static const char *ssid = "ThisOne";       // Enter your WiFi name
static const char *password = "123456789"; // Enter WiFi password
static const int MAX_GATES = 3;

// TODO: find a solution to store the topics temporary
static const char *topic_gates = "airportDemo";
static const char *topic_dividers = "airportDemoDividers";

// MQTT Broker variables
static const char *mqtt_broker = "broker.hivemq.com";

static const char *mqtt_username = "Nedyalko";
static const char *mqtt_password = "1234";

static const int mqtt_port = 1883;

#endif