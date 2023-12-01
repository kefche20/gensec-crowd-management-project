#ifndef CONFIG
#define CONFIG

// User config
const std::string MY_ID = "901";
static const int MAX_GATES = 3;
constexpr int MY_MAX_PEOPLE = 30;

// Wi-Fi consts
static const char *ssid = "14 Pro";            // Enter your WiFi name
static const char *password = "123456789"; // Enter WiFi password

// MQTT consts
const char *mqtt_broker = "broker.hivemq.com";
const char *topic_gates = "airportDemo";
const char *topic_dividers = "airportDemoDividers";
const char *mqtt_username = "Nedyalko";
const char *mqtt_password = "1234";
const int mqtt_port = 1883;

#endif