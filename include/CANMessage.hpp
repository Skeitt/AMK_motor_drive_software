#ifndef CANMESSAGE_HPP
#define CANMESSAGE_HPP

#include <Arduino.h>

class CANMessage {
    private:
        uint16_t m_canId;
    public:
        uint8_t m_data[8]; ///< Dati del messaggio CAN
        CANMessage();
        CANMessage(uint16_t t_canId);

        void setCanId(uint16_t t_canId);
        uint16_t getCanId();

        virtual ~CANMessage() = default;
};

#endif // CANMESSAGE_H