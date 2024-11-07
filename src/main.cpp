#include <Arduino.h>
#include <CANSAME5x.h>
#include "AMKUtils.hpp"
#include "Inverter.hpp"
#include "CANMessage.hpp"
#include "CANCommunication.hpp"
#include "Potentiometer.hpp"

CANSAME5x CAN;
CANMessage canMsg;
Inverter inverters[2] = {
    Inverter(INVERTER_1_NODE_ADDRESS),
    Inverter(INVERTER_2_NODE_ADDRESS)};
Potentiometer pot;

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
    Serial.printf("Inverter 0x%X\n", inverter.getNodeAddress());

    // if inverter is active, update setpoints based on potentiometer value
    if (inverter.getState() == CONTROLLER_ACTIVE)
    {
      pot.update();
      inverter
          .setSetpoints1(Setpoints1{cbDcOn | cbEnable | cbInverterOn,
                                    pot.getRpm(),
                                    pot.getTorquePos(),
                                    pot.getTorqueNeg()});
    }

    // if switch is on, activate inverter, else deactivate
    (activationValue) ? inverter.activate() : inverter.deactivate();
  }
}

void initDevice()
{
  Serial.begin(115200);

  // initialize CAN bus at 500 kbps
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
    canMsg.setCanId(CAN.packetId());
    uint16_t nodeAddress = getNodeAddressFromCANId(canMsg.getCanId());
    if (nodeAddress != 0)
    {
      uint16_t baseAddress = canMsg.getCanId() - nodeAddress;
      canMsg.m_data[0] = CAN.read();
      canMsg.m_data[1] = CAN.read();
      canMsg.m_data[2] = CAN.read();
      canMsg.m_data[3] = CAN.read();
      canMsg.m_data[4] = CAN.read();
      canMsg.m_data[5] = CAN.read();
      canMsg.m_data[6] = CAN.read();
      canMsg.m_data[7] = CAN.read();

      updateInverter(nodeAddress, baseAddress, canMsg);
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

void updateInverter(uint16_t nodeAddress, uint16_t baseAddress, CANMessage canMsg)
{
  Serial.printf("Received packet: %X %X %X %X %X %X %X %X\n",
                      canMsg.m_data[0],
                      canMsg.m_data[1],
                      canMsg.m_data[2],
                      canMsg.m_data[3],
                      canMsg.m_data[4],
                      canMsg.m_data[5],
                      canMsg.m_data[6],
                      canMsg.m_data[7]);
  
  for (Inverter &inverter : inverters)
  {
    if (inverter.getNodeAddress() == nodeAddress)
    {
      switch (baseAddress)
      {
      case ACTUAL_VALUES_1_BASE_ADDRESS:
        inverter.setActualValues1(parseActualValues1(canMsg.m_data));
        Serial.printf("ID: 0x%X\n", canMsg.getCanId());
        Serial.printf("Status: %X\n", inverter.getActualValues1().status);
        Serial.printf("Actual Velocity: %d\n", inverter.getActualValues1().actualVelocity);
        Serial.printf("Torque current: %f\n", (float)(inverter.getActualValues1().torqueCurrent * ID110) / 16384);
        Serial.printf("Magnetizing current: %f\n", (float)(inverter.getActualValues1().magnetizingCurrent * ID110) / 16384);
        break;
      case ACTUAL_VALUES_2_BASE_ADDRESS:
        inverter.setActualValues2(parseActualValues2(canMsg.m_data));

        Serial.printf("ID: 0x%X\n", canMsg.getCanId());
        Serial.printf("Motor temperature: %f\n", (float)inverter.getActualValues2().tempMotor * 0.1);
        Serial.printf("Inverter temperature: %f\n", (float)inverter.getActualValues2().tempInverter * 0.1);
        Serial.printf("Error: %d\n", inverter.getActualValues2().errorInfo);
        Serial.printf("IGBT temperature: %f\n", (float)inverter.getActualValues2().tempIGBT * 0.1);
        break;
      default:
        break;
      }
    }
  }
}