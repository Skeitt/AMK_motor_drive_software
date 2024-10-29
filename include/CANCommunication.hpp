#ifndef CAN_COMMUNICATION_HPP
#define CAN_COMMUNICATION_HPP

#include <stdint.h>
#include "CANMessage.hpp"

/**
 * @brief Initializes the device settings, such as serial communication and CAN bus.
 *
 * This function sets up the serial port and initializes the CAN bus at a specified baud rate.
 */
void initDevice();

/**
 * @brief Initializes the CAN bus with a given speed.
 *
 * @param speed The CAN bus speed in bits per second (bps). For example, 500E3 for 500 kbps.
 *
 * Configures the CAN bus speed and manages necessary pin settings to enable the CAN module.
 * If initialization fails, an error message is displayed, and the built-in LED flashes.
 */
void startCANBus(long speed);

/**
 * @brief Callback function to handle received CAN messages.
 *
 * @param packetSize The size of the incoming packet.
 *
 * Processes incoming CAN messages by extracting the CAN ID and data, updating inverter information,
 * and handling errors for unknown nodes or invalid packet sizes.
 */
void receiveMessage(int packetSize);

/**
 * @brief Sends a CAN message over the CAN bus.
 *
 * @param canMsg The CAN message object containing the ID and data to be sent.
 * @return true if the message was sent successfully; false otherwise.
 *
 * Initiates the sending of a CAN message with the specified ID and data.
 */
bool sendMessage(CANMessage canMsg);

/**
 * @brief Updates inverter data based on received CAN message information.
 *
 * @param nodeAddress The address of the inverter node to be updated.
 * @param baseAddress The base address used to identify the type of message.
 * @param canMsg The CAN message containing the data to update the inverter.
 *
 * Parses data from the received CAN message to update specific inverter parameters,
 * such as status, velocity, and temperature.
 */
void updateInverter(uint16_t nodeAddress, uint16_t baseAddress, CANMessage canMsg);

#endif // CAN_COMMUNICATION_HPP