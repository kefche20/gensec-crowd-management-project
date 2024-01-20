#pragma once

#define TRIGGER_PIN_1 13
#define ECHO_PIN_1 12

#define TRIGGER_PIN_2 26
#define ECHO_PIN_2 25

#define CLOSE_LED_PIN 15
#define BUSY_LED_PIN 2
#define OPEN_LED_PIN 4

// sensor constance
#define CATCH_DISTANCE 50
#define MAX_DETECT_DELAY 50

#define MY_PULSE_TIMEOUT 23529.4
#define MY_DISTANCE_DIVIDER 58.8235

// queue constance
#define MAX_QUEUE_NUMBER 10

// WiFi constance
#define MY_NET_SSID "R@mzy"
#define MY_NET_PASSWORD "yzmar252887!"
#define MY_NET_PORT 1883

// MQTT constance
#define MY_MQTT_BROKER "broker.hivemq.com"
#define MY_MQTT_USERNAME "Nedyalko"
#define MY_MQTT_PASSWORD "1234"

#define MY_GATE_TOPIC "gates_100"

#define EMPTY_STRING ""

#define START_CHAR '&'
#define ARROW_CHAR '>'
#define SPLIT_CHAR '-'
#define DATA_CHAR '+'
#define END_CHAR ';'

#define EXTRA_CHAR ':'

#define COMMAND_TO_REGISTER "6"
#define DEMAND_RECEIVED "0"
#define COMMAND_TO_OPEN "1"
#define COMMAND_TO_CLOSE "2"
#define COMMAND_TO_INFORM "3"
#define COMMAND_TO_ADD "5"

#define MY_ID "101"
#define DIV_ID "100"

#define COM_DELAY 1000
#define DETECT_DELAY 100
#define HEART_BEAT 5000
#define WIFI_CONNECT_TIMEOUT 10000

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "..\include\my_gate_sensor\enum\statesOfSensor.hpp"
#include "..\include\my_gate_sensor\enum\statesOfMovement.hpp"

void callback(char *, byte *, unsigned int);
String dataToSend(int);
String dataToRegister();
