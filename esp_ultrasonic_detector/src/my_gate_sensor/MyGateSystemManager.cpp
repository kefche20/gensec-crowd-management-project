#include "..\include\my_gate_sensor\headers\MyGateSystemManager.hpp"

MyGateSystemManager::MyGateSystemManager() : ultrasonic1(ECHO_PIN_1, TRIGGER_PIN_1), ultrasonic2(ECHO_PIN_2, TRIGGER_PIN_2)
{
    ;
}

MyGateSystemManager::~MyGateSystemManager()
{
    ;
}

void MyGateSystemManager::myGateSetup()
{
    Serial.println("SETUP GATE");
    my_sensor_state = SensorState::WAITING;
    my_movement_state = MovementState::IDLE;

    ultrasonic1.setupSensor();
    ultrasonic2.setupSensor();

    open_state = 0;
    busy_state = 0;
    register_state = 0;

    pinMode(CLOSE_LED_PIN, OUTPUT);
    pinMode(OPEN_LED_PIN, OUTPUT);
    pinMode(BUSY_LED_PIN, OUTPUT);

    checkIfOpen();
    checkIfBusy();
}

void MyGateSystemManager::myGateLoop()
{
    first_catch_val = ultrasonic1.findDistance();
    second_catch_val = ultrasonic2.findDistance();

    // Serial.print("First sensor: ");
    // Serial.println(first_catch_val);
    // Serial.print("Second sensor: ");
    // Serial.println(second_catch_val);

    updateMyDetectorState();
}

void MyGateSystemManager::updateMyDetectorState()
{
    switch (my_sensor_state)
    {
    case SensorState::WAITING:
        if ((first_catch_val <= CATCH_DISTANCE) && (second_catch_val > CATCH_DISTANCE))
        {
            my_sensor_state = SensorState::DETECTING;
            my_movement_state = MovementState::FIRST_STEP;
        }
        else if ((first_catch_val > CATCH_DISTANCE) && (second_catch_val <= CATCH_DISTANCE))
        {
            my_sensor_state = SensorState::WAITING;
        }
        else if ((first_catch_val <= CATCH_DISTANCE) && (second_catch_val <= CATCH_DISTANCE))
        {
            my_sensor_state = SensorState::WAITING;
        }
        else if ((first_catch_val > CATCH_DISTANCE) && (second_catch_val > CATCH_DISTANCE))
        {
            my_sensor_state = SensorState::WAITING;
        }
        break;
    case SensorState::DETECTING:
        switch (my_movement_state)
        {
        case MovementState::FIRST_STEP:
            if ((first_catch_val <= CATCH_DISTANCE) && (second_catch_val <= CATCH_DISTANCE))
            {
                Serial.println("FIRST DETECT.");
                my_movement_state = MovementState::SECOND_STEP;
            }
            else if ((first_catch_val > CATCH_DISTANCE) && (second_catch_val <= CATCH_DISTANCE))
            {
                Serial.println("WAITING.");
                my_movement_state = MovementState::IDLE;
                my_sensor_state = SensorState::WAITING;
            }
            break;

        case MovementState::SECOND_STEP:
            if ((first_catch_val > CATCH_DISTANCE) && (second_catch_val <= CATCH_DISTANCE))
            {
                Serial.println("SECOND DETECT.");
                my_movement_state = MovementState::THIRD_STEP;
            }
            else if ((first_catch_val <= CATCH_DISTANCE) && (second_catch_val > CATCH_DISTANCE))
            {
                Serial.println("WAIT.");
                my_movement_state = MovementState::IDLE;
                my_sensor_state = SensorState::WAITING;
            }
            break;

        case MovementState::THIRD_STEP:
            if ((first_catch_val > CATCH_DISTANCE) && (second_catch_val > CATCH_DISTANCE))
            {
                Serial.println("THIRD DETECT.");
                my_movement_state = MovementState::IDLE;
                my_sensor_state = SensorState::CALCULATING;
            }
            else if ((first_catch_val <= CATCH_DISTANCE) && (second_catch_val > CATCH_DISTANCE))
            {
                Serial.println("WAIT.");
                my_movement_state = MovementState::IDLE;
                my_sensor_state = SensorState::WAITING;
            }
            break;
        default:
            break;
        }
        break;
    case SensorState::CALCULATING:
        Serial.println("JUST PASSED!");
        decreaseQueue();
        my_sensor_state = SensorState::WAITING;
        break;
    default:
        break;
    };
}

void MyGateSystemManager::decreaseQueue()
{
    if (my_queue > 0)
    {
        my_queue--;
    }

    if (my_queue < MAX_QUEUE_NUMBER)
    {
        busy_state = 0;
    }

    checkIfBusy();
}

int MyGateSystemManager::getQueueNr()
{
    return my_queue;
}

uint8_t MyGateSystemManager::getRegisterState()
{
    return register_state;
}

bool MyGateSystemManager::addQueueNr()
{
    bool allowed = false;
    int number = atoi(my_input_value.c_str());
    if ((busy_state == 1) || (my_queue >= MAX_QUEUE_NUMBER))
    {
        allowed = false;
    }
    else
    {
        Serial.println("ADD PERSON!");
        my_queue = my_queue + number;
        allowed = true;
    }
    return allowed;
}

void MyGateSystemManager::sortInputCommand(String message)
{
    static String message_buffer = "";
    for (char incoming_char : message)
    {
        if (incoming_char == START_CHAR)
        {
            // Start of the message
            message_buffer = "";
        }
        else if (incoming_char == END_CHAR)
        {
            // End of the message
            readInputSourceAndDestination(message_buffer);
            message_buffer = ""; // Clear the buffer for the next message
        }
        else
        {
            // Append the character to the buffer
            message_buffer += incoming_char;
        }
    }
}

void MyGateSystemManager::readInputSourceAndDestination(String message)
{
    int indicator_1;
    int indicator_2;

    my_source_id = "";
    my_destination_id = "";
    my_message = "";

    indicator_1 = message.indexOf(ARROW_CHAR);
    my_source_id = message.substring(0, indicator_1);
    indicator_2 = message.indexOf(SPLIT_CHAR, indicator_1 + 1);
    my_destination_id = message.substring(indicator_1 + 1, indicator_2);
    my_message = message.substring(indicator_2 + 1);
    Serial.print("source: ");
    Serial.println(my_source_id);
    Serial.print("to: ");
    Serial.println(my_destination_id);

    readCommandAndValue(my_message);
}

void MyGateSystemManager::readCommandAndValue(String message)
{
    int indicator_3;
    int indicator_4;

    my_input_command = "";
    my_input_value = "";

    indicator_3 = message.indexOf(DATA_CHAR);

    if (indicator_3 != -1)
    {
        my_input_command = message.substring(0, indicator_3);
        indicator_4 = message.indexOf(EXTRA_CHAR, indicator_3 + 1);
        my_input_value = message.substring(indicator_3 + 1, indicator_4);
    }
    else
    {
        my_input_command = message;
    }

    if ((my_destination_id == MY_ID) || (my_destination_id == ALL_ID))
    {
        operateQueue();
    }
}

void MyGateSystemManager::operateQueue()
{
    Serial.print("command: ");
    Serial.println(my_input_command);
    Serial.print("value: ");
    Serial.println(my_input_value);

    if (my_input_command == COMMAND_TO_OPEN)
    {
        if (open_state == 0)
        {
            Serial.println("OPENING GATE!");
            open_state = 1;
        }
    }

    if (my_input_command == COMMAND_TO_CLOSE)
    {
        if (open_state == 1)
        {
            Serial.println("CLOSING GATE!");
            open_state = 0;
        }
    }

    if (my_input_command == DEMAND_RECEIVED)
    {
        if (register_state == 0)
        {
            Serial.println("REGISTERED!");
            register_state = 1;
        }
    }

    if (my_input_command == COMMAND_TO_ADD)
    {
        if ((open_state == 1) && (register_state == 1))
        {
            bool to_add = addQueueNr();
            if (!to_add)
            {
                busy_state = 1;
            }
        }
    }
    checkIfBusy();
    checkIfOpen();
}

void MyGateSystemManager::checkIfOpen()
{
    if (open_state == 0)
    {
        digitalWrite(OPEN_LED_PIN, LOW);
        digitalWrite(CLOSE_LED_PIN, HIGH);
    }
    else
    {
        digitalWrite(CLOSE_LED_PIN, LOW);
        digitalWrite(OPEN_LED_PIN, HIGH);
    }
}

void MyGateSystemManager::checkIfBusy()
{
    if (my_queue >= MAX_QUEUE_NUMBER)
    {
        busy_state = 1;
    }

    switch (busy_state)
    {
    case 0:
        /* code */
        digitalWrite(BUSY_LED_PIN, LOW);
        break;

    case 1:
        /* code */
        digitalWrite(BUSY_LED_PIN, HIGH);
        Serial.println("GATE BUSY!");
        break;

    default:
        break;
    }
}