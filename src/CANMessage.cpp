#include "CANMessage.h"

CANMessage::CANMessage()
{
    this->can_id = 0;
    for (int i = 0; i < 8; i++)
    {
        this->data[i] = 0;
    }
}

CANMessage::CANMessage(uint16_t can_id)
{
    this->can_id = can_id;
    for (int i = 0; i < 8; i++)
    {
        this->data[i] = 0;
    }
}

void CANMessage::set_can_id(uint16_t can_id)
{
    this->can_id = can_id;
}

uint16_t CANMessage::get_can_id()
{
    return this->can_id;
}

void CANMessage::set_data(uint8_t data[8])
{
    for (int i = 7; i >= 0; i--)
    {
        this->data[i] = data[i];
    }
}