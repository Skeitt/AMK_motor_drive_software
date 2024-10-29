#include "CANMessage.hpp"

CANMessage::CANMessage()
{
    m_canId = 0;
    m_data[8] = {0};
}

CANMessage::CANMessage(uint16_t t_canId)
{
    m_canId = t_canId;
    m_data[8] = {0};
}

void CANMessage::setCanId(uint16_t t_canId)
{
    m_canId = t_canId;
}

uint16_t CANMessage::getCanId()
{
    return m_canId;
}