#ifndef CANMESSAGE_H
#define CANMESSAGE_H

#include <Arduino.h>

class CANMessage {
    public:
        uint16_t can_id; ///< ID del messaggio CAN
        uint8_t data[8]; ///< Dati del messaggio CAN
        CANMessage();
        CANMessage(uint16_t can_id);

        void set_can_id(uint16_t can_id);
        uint16_t get_can_id();

        void set_data(uint8_t data[8]);
        uint8_t* get_data();

        virtual ~CANMessage() = default;
};

#endif // CANMESSAGE_H