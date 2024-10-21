#include <Arduino.h>
#include <CANSAME5x.h>
#include "utils.h"
#include "Inverter.h"
#include "CANMessage.h"

CANSAME5x CAN;
CANMessage can_msg;
Inverter inverters[4] = {
    Inverter(INVERTER_1_NODE_ADDRESS),
    Inverter(INVERTER_2_NODE_ADDRESS),
    Inverter(INVERTER_3_NODE_ADDRESS),
    Inverter(INVERTER_4_NODE_ADDRESS)};

void init_device();
void start_can_bus(int speed);
void receive_message(int packetSize);
bool send_message(CANMessage can_msg);
void update_inverter(uint16_t node_address, uint16_t base_address, CANMessage can_msg);
uint16_t get_node_address_from_can_id(long can_id);
ActualValues1 parse_actual_values_1(byte data[8]);
ActualValues2 parse_actual_values_2(byte data[8]);

void setup()
{
  init_device();

  CAN.onReceive(receive_message);
}

void loop()
{
  bool switch_value = true;

  for (Inverter &inverter : inverters)
  {
    inverter.set_target_parameters(500, 100, 0);
    // se lo switch è attivo attiva l'inverter
    // altrimenti disattivalo
    if (switch_value)
      inverter.activate();
    else
      inverter.deactivate();

    send_message(parse_setpoints_1(inverter.get_setpoints_1(), inverter.get_node_address()));
  }
}

void init_device()
{
  Serial.begin(115200);

  // avvia il can bus a 500kbps
  start_can_bus(500E3);
}

void start_can_bus(int speed)
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

void receive_message(int packetSize)
{
  if (CAN.available() >= packetSize)
  {
    // memorizzazione del messaggio nel buffer data
    can_msg.set_can_id(CAN.packetId());
    uint16_t node_address = get_node_address_from_can_id(can_msg.get_can_id());
    if (node_address != 0)
    {
      uint16_t base_address = can_msg.get_can_id() - node_address;
      can_msg.data[7] = CAN.read();
      can_msg.data[6] = CAN.read();
      can_msg.data[5] = CAN.read();
      can_msg.data[4] = CAN.read();
      can_msg.data[3] = CAN.read();
      can_msg.data[2] = CAN.read();
      can_msg.data[1] = CAN.read();
      can_msg.data[0] = CAN.read();

      update_inverter(node_address, base_address, can_msg);
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

bool send_message(CANMessage can_msg)
{
  CAN.beginPacket(can_msg.get_can_id());
  size_t bytesSent = CAN.write(can_msg.data, 8);
  CAN.endPacket();
  return (bytesSent == 8);
}

void update_inverter(uint16_t node_address, uint16_t base_address, CANMessage can_msg)
{
  for (Inverter &inverter : inverters)
  {
    if (inverter.get_node_address() == node_address)
    {
      switch (base_address)
      {
      case ACTUAL_VALUES_1_BASE_ADDRESS:
        inverter.set_actual_values_1(parse_actual_values_1(can_msg.data));
        break;
      case ACTUAL_VALUES_2_BASE_ADDRESS:
        inverter.set_actual_values_2(parse_actual_values_2(can_msg.data));
        break;
      default:
        break;
      }
    }
  }
}