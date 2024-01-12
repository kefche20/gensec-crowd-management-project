#include "..\include\my_gate_sensor\MyGateSystemManager.hpp"

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
        calculateQueue();       
        my_sensor_state = SensorState::WAITING;
        break;
    default:
        break;
    };
}

void MyGateSystemManager::calculateQueue()
{
    if (my_queue > 0)
    {
        my_queue--;
    }
}