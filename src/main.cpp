#include <Arduino.h>
#include <CANSAME5x.h>
#include "amk_specs.h"
#include "Inverter.h"

CANSAME5x CAN;
CANMessage can_msg;
Inverter inverters[4] = {
    Inverter(AMK_INVERTER_1_NODE_ADDRESS),
    Inverter(AMK_INVERTER_2_NODE_ADDRESS),
    Inverter(AMK_INVERTER_3_NODE_ADDRESS),
    Inverter(AMK_INVERTER_4_NODE_ADDRESS)};

void init_device();
void start_can_bus(int speed);
void receive_message(int packetSize);
byte get_node_address_from_can_id(long can_id);

void setup()
{
  init_device();

  CAN.onReceive(receive_message);
}

void loop()
{
  //ogni ciclo controllo lo stato di ogni inverter
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
    byte node_address = get_node_address_from_can_id(can_msg.get_can_id());
    uint16_t base_address = can_msg.get_can_id() - node_address;
    can_msg.data[7] = CAN.read();
    can_msg.data[6] = CAN.read();
    can_msg.data[5] = CAN.read();
    can_msg.data[4] = CAN.read();
    can_msg.data[3] = CAN.read();
    can_msg.data[2] = CAN.read();
    can_msg.data[1] = CAN.read();
    can_msg.data[0] = CAN.read();

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
        }
      }
    }
  }
  else
  {
    Serial.println("Invalid packet size");
    return;
  }
}

// TODO: Implementare la funzione get_node_address_from_can_id
byte get_node_address_from_can_id(long can_id)
{
  // Implementare il codice per estrarre l'indirizzo del nodo da can_id
  // e restituirelo
  return 0; // Ritorna un indirizzo di default
}

ActualValues1 parse_actual_values_1(byte data[8])
{
  ActualValues1 actual_values_1;
  actual_values_1.status = data[7] | (data[6] << 8);
  actual_values_1.actual_velocity = data[5] | (data[4] << 8);
  actual_values_1.torque_current = data[3] | (data[2] << 8);
  actual_values_1.magnetizing_current = data[1] | (data[0] << 8);
  return actual_values_1;
}

ActualValues2 parse_actual_values_2(byte data[8])
{
  ActualValues2 actual_values_2;
  actual_values_2.temp_motor = data[7] | (data[6] << 8);
  actual_values_2.temp_inverter = data[5] | (data[4] << 8);
  actual_values_2.error_info = data[3] | (data[2] << 8);
  actual_values_2.temp_igbt = data[1] | (data[0] << 8);
  return actual_values_2;
}