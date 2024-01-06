#ifndef ENTRY_DETECTOR_HPP 
#define ENTRY_DETECTOR_HPP

#define TRIGGER_PIN_1 2
#define TRIGGER_PIN_2 12

#define ECHO_PIN_1 15
#define ECHO_PIN_2 14

#define CATCH_DISTANCE 50
#define MAX_DETECT_DELAY 50

#define MY_NET_SSID "R@mzy"
#define NY_NET_PASSWORD "yzmar252887!"
#define NY_NET_PORT 1883

#define MY_MQTT_BROKER "broker.hivemq.com"
#define MY_MQTT_USERNAME "Nedyalko"
#define MY_MQTT_PASSWORD "1234"

#define MY_GATE_TOPIC "airportDemo"

#define START_CHAR '&'
#define SPLIT_CHAR '-'
#define END_CHAR ';'

#define DATA_CHAR "+"
#define EXTRA_CHAR ","

#define COMMAND_TO_OPEN "OPENGATE"
#define COMMAND_TO_CLOSE "CLOSEGATE"
#define COMMAND_TO_ADD "ALLOC"
#define COMMAND_TO_INFORM "NUMOFPEOPLE"

#define COM_DELAY 1000
#define WIFI_CONNECT_TIMEOUT 10000

#include <Arduino.h>
#include "..\include\my_gate_sensor\statesOfSensor.hpp"
#include "..\include\my_gate_sensor\statesOfMovement.hpp"

#endif // MACRO
