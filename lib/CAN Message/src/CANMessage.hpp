#ifndef CAN_MESSAGE_HPP
#define CAN_MESSAGE_HPP

#include <stdint.h>

/**
 * @brief The CANMessage class represents a CAN message, containing an ID and data.
 * 
 * This class provides a structure to store and manipulate CAN messages, which
 * include a 16-bit CAN identifier and up to 8 bytes of data.
 */
class CANMessage {
    private:
        uint16_t m_canId; ///< The CAN ID associated with the message.

    public:
        uint8_t m_data[8]; ///< Array of 8 bytes representing the data of the CAN message.

        /**
         * @brief Default constructor for CANMessage.
         * 
         * Initializes a CAN message with no specific CAN ID.
         */
        CANMessage();

        /**
         * @brief Constructs a CANMessage with a specified CAN ID.
         * 
         * @param t_canId The CAN ID to associate with this message.
         */
        CANMessage(uint16_t t_canId);

        /**
         * @brief Sets the CAN ID of the message.
         * 
         * @param t_canId The CAN ID to be set for this message.
         */
        void setCanId(uint16_t t_canId);

        /**
         * @brief Retrieves the CAN ID of the message.
         * 
         * @return The CAN ID currently associated with the message.
         */
        uint16_t getCanId();
};

#endif // CAN_MESSAGE_HPP