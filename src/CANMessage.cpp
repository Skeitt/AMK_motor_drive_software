#include "CANMessage.hpp"

CANMessage::CANMessage()
{
    m_canId = 0;
    for (int i = 0; i < 8; i++)
    {
        m_data[i] = 0;
    }
}

CANMessage::CANMessage(uint16_t t_canId)
{
    m_canId = t_canId;
    for (int i = 0; i < 8; i++)
    {
        m_data[i] = 0;
    }
}

void CANMessage::setCanId(uint16_t t_canId)
{
    m_canId = t_canId;
}

uint16_t CANMessage::getCanId()
{
    return m_canId;
}