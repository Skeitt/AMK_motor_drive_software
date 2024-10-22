#include <Arduino.h>
#include <CANSAME5x.h>
#include "utils.hpp"
#include "Inverter.hpp"
#include "CANMessage.hpp"
#include "CANCommunication.hpp"

CANSAME5x CAN;
CANMessage canMsg;
Inverter inverters[4] = {
    Inverter(INVERTER_1_NODE_ADDRESS),
    Inverter(INVERTER_2_NODE_ADDRESS),
    Inverter(INVERTER_3_NODE_ADDRESS),
    Inverter(INVERTER_4_NODE_ADDRESS)
};

void setup()
{
  initDevice();

  CAN.onReceive(receiveMessage);
}

void loop()
{
  bool activationValue = true;

  for (Inverter &inverter : inverters)
  {
    inverter.setSetpoints1(Setpoints1{cbDcOn | cbEnable | cbInverterOn, 1000, 100, 0});
    // se lo switch è attivo attiva l'inverter
    // altrimenti lo disattiva
    if (activationValue)
    {
      inverter.activate();
    }
    else
    {
      inverter.deactivate();
    }
  }
}

void initDevice()
{
  Serial.begin(115200);

  // avvia il can bus a 500kbps
  startCANBus(500E3);
}

void startCANBus(long speed)
{
  pinMode(PIN_CAN_STANDBY, OUTPUT);
  digitalWrite(PIN_CAN_STANDBY, false); // turn off STANDBY
  pinMode(PIN_CAN_BOOSTEN, OUTPUT);
  digitalWrite(PIN_CAN_BOOSTEN, true); // turn on booster

  if (!CAN.begin(speed))
  {
    Serial.printf("Starting CAN failed!\n");
    // se la comunicazione non avviene correttamente il led lampeggia
    while (1)
    {
      digitalWrite(LED_BUILTIN, HIGH);
      delay(200);
      digitalWrite(LED_BUILTIN, LOW);
      delay(200);
    }
  }
}

void receiveMessage(int packetSize)
{
  if (CAN.available() >= packetSize)
  {
    // memorizzazione del messaggio nel buffer data
    canMsg.setCanId(CAN.packetId());
    uint16_t node_address = get_node_address_from_can_id(canMsg.getCanId());
    if (node_address != 0)
    {
      uint16_t base_address = canMsg.getCanId() - node_address;
      canMsg.m_data[0] = CAN.read();
      canMsg.m_data[1] = CAN.read();
      canMsg.m_data[2] = CAN.read();
      canMsg.m_data[3] = CAN.read();
      canMsg.m_data[4] = CAN.read();
      canMsg.m_data[5] = CAN.read();
      canMsg.m_data[6] = CAN.read();
      canMsg.m_data[7] = CAN.read();

      update_inverter(node_address, base_address, canMsg);
    }
    else
    {
      Serial.printf("Received message from unknown node\n");
      return;
    }
  }
  else
  {
    Serial.printf("Invalid packet size");
    return;
  }
}

bool sendMessage(CANMessage canMsg)
{
  CAN.beginPacket(canMsg.getCanId());
  size_t bytesSent = CAN.write(canMsg.m_data, 8);
  CAN.endPacket();
  return (bytesSent == 8);
}

void update_inverter(uint16_t node_address, uint16_t base_address, CANMessage canMsg)
{
  for (Inverter &inverter : inverters)
  {
    if (inverter.getNodeAddress() == node_address)
    {
      switch (base_address)
      {
      case ACTUAL_VALUES_1_BASE_ADDRESS:
        inverter.setActualValues1(parse_actual_values_1(canMsg.m_data));
        break;
      case ACTUAL_VALUES_2_BASE_ADDRESS:
        inverter.setActualValues2(parse_actual_values_2(canMsg.m_data));
        break;
      default:
        break;
      }
    }
  }
}