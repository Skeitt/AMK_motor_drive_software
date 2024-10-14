#include <Arduino.h>
#include <CANSAME5x.h>
#include "amk_specs.h"
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
void check_status(Inverter& inverter);
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
  for(Inverter &inverter : inverters)
  {
    check_status(inverter);
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

bool send_message(CANMessage can_msg)
{
  CAN.beginPacket(can_msg.get_can_id());
  size_t bytesSent = CAN.write(can_msg.data, 8);
  CAN.endPacket();
  return (bytesSent == 8);
}

// TODO: Implementare la funzione get_node_address_from_can_id
uint16_t get_node_address_from_can_id(long can_id)
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

CANMessage parse_setpoints_1(Setpoints1 setpoints_1, uint16_t node_address)
{
  CANMessage can_msg;
  can_msg.set_can_id(SETPOINTS_1_BASE_ADDRESS + node_address);
  can_msg.data[7] = setpoints_1.control & 0xFF;
  can_msg.data[6] = (setpoints_1.control >> 8) & 0xFF;
  can_msg.data[5] = setpoints_1.target_velocity & 0xFF;
  can_msg.data[4] = (setpoints_1.target_velocity >> 8) & 0xFF;
  can_msg.data[3] = setpoints_1.torque_limit_positive & 0xFF;
  can_msg.data[2] = (setpoints_1.torque_limit_positive >> 8) & 0xFF;
  can_msg.data[1] = setpoints_1.torque_limit_negative & 0xFF;
  can_msg.data[0] = (setpoints_1.torque_limit_negative >> 8) & 0xFF;
  return can_msg;
}

void check_status(Inverter& inverter)
{
    // Implementare il controllo dello stato dell'inverter
    // usando i valori di actual_values_1

    uint16_t status = inverter.get_actual_values_1().status;

    if(status & bError)
    {
        Serial.printf("error");
        // controlla di che errore si tratta
        return;
    }
    if ((status & bSystemReady) == bSystemReady)
    {
        // invio di bDcOn = 1 dal control byte
        inverter.set_setpoints_1(Setpoints1{cbDcOn, 0, 0, 0});
        send_message(parse_setpoints_1(inverter.get_setpoints_1(), inverter.get_node_address()));
        return;
    }
    if ((status & (bSystemReady | bDcOn)) == (bSystemReady | bDcOn))
    {
        Serial.printf("DcOn activated");
        return;
    }
    if((status & (bSystemReady | bDcOn | bQuitDcOn)) == (bSystemReady | bDcOn | bQuitDcOn))
    {
        // invio di torque limit pos e neg = 0
        Serial.printf("Sending torque limit pos and neg = 0");
        inverter.set_setpoints_1(Setpoints1{cbDcOn, 0, 0, 0});
        send_message(parse_setpoints_1(inverter.get_setpoints_1(), inverter.get_node_address()));
        
        // invio di bEnable = 1
        Serial.printf("Sending driver enable = 1");
        inverter.set_setpoints_1(Setpoints1{cbDcOn | cbEnable, 0, 0, 0});
        send_message(parse_setpoints_1(inverter.get_setpoints_1(), inverter.get_node_address()));
        
        // invio di bInverterOn = 1
        Serial.printf("Sending inverter on = 1");
        inverter.set_setpoints_1(Setpoints1{cbDcOn | cbEnable | cbInverterOn, 0, 0, 0});
        send_message(parse_setpoints_1(inverter.get_setpoints_1(), inverter.get_node_address()));
        
        return;
    }
    if((status & (bSystemReady | bDcOn | bQuitDcOn | bInverterOn)) == (bSystemReady | bDcOn | bQuitDcOn | bInverterOn))
    {
        // invio dei setpoint
        Serial.printf("Sending setpoint");
        inverter.set_setpoints_1(Setpoints1{cbDcOn | cbEnable | cbInverterOn, 500, 100, 0});
        return;
    }

    // disattivazione dell'inverter
}
