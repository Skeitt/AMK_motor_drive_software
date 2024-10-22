#ifndef CANCOMMUNICATION_HPP
#define CANCOMMUNICATION_HPP

#include <Arduino.h>
#include "CANMessage.hpp"

void initDevice();
void startCANBus(long speed);
void receiveMessage(int packetSize);
bool sendMessage(CANMessage canMsg);
void update_inverter(uint16_t node_address, uint16_t base_address, CANMessage canMsg);

#endif // CANCOMMUNICATION_HPP