#include <Arduino.h>

#define TRIGGER_PIN_1 2
#define TRIGGER_PIN_2 12

#define ECHO_PIN_1 15
#define ECHO_PIN_2 14

#define CATCH_DISTANCE 50
#define MAX_DETECT_DELAY 50

typedef enum
{
  WAITING,
  DETECTING,
  ADDING
} SensorState;

typedef enum
{
  IDLE,
  FIRST_STEP,
  SECOND_STEP,
  THIRD_STEP
} MovementState;

uint32_t start_time;
uint32_t current_time;

float first_catch_val;
float second_catch_val;
uint8_t nr_of_people;

// put function declarations here:
float findDistance(int, int);

SensorState sensor_state;
MovementState movement_state;

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(9600);

  pinMode(TRIGGER_PIN_1, OUTPUT);
  pinMode(TRIGGER_PIN_2, OUTPUT);
  pinMode(ECHO_PIN_1, INPUT);
  pinMode(ECHO_PIN_2, INPUT);

  sensor_state = WAITING;
  movement_state = IDLE;
  first_catch_val = 0;
  second_catch_val = 0;
  nr_of_people = 0;
  start_time = millis();
  Serial.println("Sensor Demo Start!");
}

void loop()
{
  // put your main code here, to run repeatedly:
  first_catch_val = findDistance(TRIGGER_PIN_1, ECHO_PIN_1);
  second_catch_val = findDistance(TRIGGER_PIN_2, ECHO_PIN_2);
  uint32_t current_time = millis();

  if (current_time - start_time >= MAX_DETECT_DELAY)
  {
    switch (sensor_state)
    {
    case WAITING:
      if ((first_catch_val <= CATCH_DISTANCE) && (second_catch_val > CATCH_DISTANCE))
      {
        sensor_state = DETECTING;
        movement_state = FIRST_STEP;
      }
      else if ((first_catch_val > CATCH_DISTANCE) && (second_catch_val <= CATCH_DISTANCE))
      {
        sensor_state = WAITING;
      }
      else if ((first_catch_val <= CATCH_DISTANCE) && (second_catch_val <= CATCH_DISTANCE))
      {
        sensor_state = WAITING;
      }
      else if ((first_catch_val > CATCH_DISTANCE) && (second_catch_val > CATCH_DISTANCE))
      {
        sensor_state = WAITING;
      }
      break;
    case DETECTING:
      switch (movement_state)
      {
      case FIRST_STEP:
        if ((first_catch_val <= CATCH_DISTANCE) && (second_catch_val <= CATCH_DISTANCE))
        {
          movement_state = SECOND_STEP;
        }
        else if ((first_catch_val > CATCH_DISTANCE) && (second_catch_val <= CATCH_DISTANCE))
        {
          movement_state = IDLE;
          sensor_state = WAITING;
        }
        break;
      case SECOND_STEP:
        if ((first_catch_val > CATCH_DISTANCE) && (second_catch_val <= CATCH_DISTANCE))
        {
          movement_state = THIRD_STEP;
        }
        break;
      case THIRD_STEP:
        if ((first_catch_val > CATCH_DISTANCE) && (second_catch_val > CATCH_DISTANCE))
        {
          movement_state = IDLE;
          sensor_state = ADDING;
        }
        break;
      default:
        break;
      }
      break;
    case ADDING:
      nr_of_people++;
      Serial.print("Number of people: ");
      Serial.println(nr_of_people);
      sensor_state = WAITING;
      break;

    default:
      break;
    }
    start_time = millis();
  }
}

// put function definitions here:
float findDistance(int trigger_pin, int echo_pin)
{
  digitalWrite(trigger_pin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigger_pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger_pin, LOW);
  noInterrupts();
  float duration = pulseIn(echo_pin, HIGH, 23529.4);
  interrupts();
  float distance = duration / 58.8235;
  return distance;
}