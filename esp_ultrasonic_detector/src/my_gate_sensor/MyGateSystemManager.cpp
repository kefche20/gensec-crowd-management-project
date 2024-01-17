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
    my_sensor_state = SensorState::WAITING;
    my_movement_state = MovementState::IDLE;

    pinMode(CLOSE_LED_PIN, OUTPUT);
    pinMode(OPEN_LED_PIN, OUTPUT);
    pinMode(BUSY_LED_PIN, OUTPUT);
}

void MyGateSystemManager::myGateLoop()
{
    first_catch_val = ultrasonic1.findDistance();
    second_catch_val = ultrasonic2.findDistance();

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
        else
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
                my_movement_state = MovementState::SECOND_STEP;
            }
            else if ((first_catch_val > CATCH_DISTANCE) && (second_catch_val <= CATCH_DISTANCE))
            {
                my_movement_state = MovementState::IDLE;
                my_sensor_state = SensorState::WAITING;
            }
            break;

        case MovementState::SECOND_STEP:
            if ((first_catch_val > CATCH_DISTANCE) && (second_catch_val <= CATCH_DISTANCE))
            {
                my_movement_state = MovementState::THIRD_STEP;
            }
            break;

        case MovementState::THIRD_STEP:
            if ((first_catch_val > CATCH_DISTANCE) && (second_catch_val > CATCH_DISTANCE))
            {
                my_movement_state = MovementState::IDLE;
                my_sensor_state = SensorState::CALCULATING;
            }
            break;
        default:
            break;
        }
        break;
    case SensorState::CALCULATING:
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
        if (busyState == 1)
        {
            digitalWrite(BUSY_LED_PIN, LOW);
            busyState = 0;
        }
    }
}

int MyGateSystemManager::getQueueNr()
{
    return my_queue;
}

uint8_t MyGateSystemManager::getGateState()
{
    return openState;
}

bool MyGateSystemManager::addQueueNr()
{
    if (my_queue >= MAX_QUEUE_NUMBER)
    {
        return false;
    }

    my_queue = my_queue + 1;
    return true;
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

    indicator_1 = message.indexOf(SPLIT_CHAR);
    my_source_id = message.substring(0, indicator_1);
    indicator_2 = message.indexOf(SPLIT_CHAR, indicator_1 + 1);
    my_destination_id = message.substring(indicator_1 + 1, indicator_2 + 1);
    my_message = message.substring(indicator_2 + 1);

    readCommandAndValue(my_message);
}

void MyGateSystemManager::readCommandAndValue(String message)
{
    int indicator_3;
    indicator_3 = message.indexOf(DATA_CHAR);

    if (indicator_3 != -1)
    {
        my_input_command = message.substring(0, indicator_3);
        my_input_value = message.substring(indicator_3 + 1);
    }
    else
    {
        my_input_command = message;
    }

    operateQueue();
}

void MyGateSystemManager::operateQueue()
{
    if (my_input_command == COMMAND_TO_OPEN)
    {
        if (openState == 0)
        {
            digitalWrite(OPEN_LED_PIN, HIGH);
            digitalWrite(CLOSE_LED_PIN, LOW);
            openState = 1;
        }
    }
    if (my_input_command == COMMAND_TO_CLOSE)
    {
        if (openState == 1)
        {
            digitalWrite(OPEN_LED_PIN, LOW);
            digitalWrite(CLOSE_LED_PIN, HIGH);
            openState = 0;
        }
    }
    if (my_input_command == COMMAND_TO_ADD)
    {
        if (addQueueNr() == false)
        {
            if (busyState == 0)
            {
                digitalWrite(BUSY_LED_PIN, HIGH);
                busyState = 1;
            }
        }
    }
}
