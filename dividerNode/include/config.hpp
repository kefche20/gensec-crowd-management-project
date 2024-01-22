#ifndef CONFIG
#define CONFIG

//NOTE - fixed divider ID and topic gate before upload
// Divider
#define DIVIDER_ID 100

// TODO: find a solution to store the topics temporary
static const char *topic_gates = "gates_100";
static const char *topic_gates_alloc = "gates_allocation";
static const char *topic_dividers_role = "dividers";
static const char *topic_dividers_alive = "dividerAlives";

static const char *topic_ui = "airport_ui";

// MQTT Broker variables
static const char *ssid = "ThisOne";       // Enter your WiFi name
static const char *password = "123456789"; // Enter WiFi password
static const int MAX_GATES = 3;

static const char *mqtt_broker = "broker.hivemq.com";

static const char *mqtt_username = "Nedyalko";
static const char *mqtt_password = "1234";

static const int mqtt_port = 1883;

#endif