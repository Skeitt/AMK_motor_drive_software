#include <Arduino.h>
#include <CANSAME5x.h>
#include "AMKUtils.hpp"
#include "Inverter.hpp"
#include "CANMessage.hpp"
#include "CANCommunication.hpp"
#include "DebugFlags.hpp"
#include "Potentiometer.hpp"

CANSAME5x CAN;
CANMessage canMsg;
Inverter inverters[2] = {
    Inverter(INVERTER_1_NODE_ADDRESS),
    Inverter(INVERTER_2_NODE_ADDRESS)};
Potentiometer pot;

unsigned long lastSendTime[2] = {0,0};

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
    DEBUG_PRINT(DEBUG_LEVEL_NONE, "Inverter 0x%X\n", inverter.getNodeAddress());

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
    DEBUG_PRINT(DEBUG_LEVEL_ERROR, "Starting CAN failed!\n");

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
      DEBUG_PRINT(DEBUG_LEVEL_ERROR, "Received message from unknown node\n");
      return;
    }
  }
  else
  {
    DEBUG_PRINT(DEBUG_LEVEL_ERROR, "Invalid packet size");
    return;
  }
}

bool sendMessage(CANMessage canMsg)
{
  CAN.beginPacket(canMsg.getCanId());
  size_t bytesSent = CAN.write(canMsg.m_data, 8);
  CAN.endPacket();

  unsigned long currentTime = millis();
  DEBUG_PRINT(DEBUG_LEVEL_NONE, "Tempo passato dall'ultimo invio: %d ms\n", currentTime - (canMsg.getCanId() == INVERTER_1_SETPOINTS_1 ? lastSendTime[0] : lastSendTime[1]));
  canMsg.getCanId() == INVERTER_1_SETPOINTS_1 ? lastSendTime[0] = currentTime : lastSendTime[1] = currentTime;

  return (bytesSent == 8);
}

void updateInverter(uint16_t nodeAddress, uint16_t baseAddress, CANMessage canMsg)
{
  for (Inverter &inverter : inverters)
  {
    if (inverter.getNodeAddress() == nodeAddress)
    {
      switch (baseAddress)
      {
      case ACTUAL_VALUES_1_BASE_ADDRESS:
        inverter.setActualValues1(parseActualValues1(canMsg.m_data));
        DEBUG_PRINT(DEBUG_LEVEL_VALUES,
                    "0x%X:\n Status: %d\n, Actual velocity: %d\n, Torque current: %f\n, Magnetizing current: %f\n",
                    canMsg.getCanId(),
                    inverter.getActualValues1().status,
                    inverter.getActualValues1().actualVelocity,
                    static_cast<float>(inverter.getActualValues1().torqueCurrent) * (ID110 / 16384),
                    static_cast<float>(inverter.getActualValues1().magnetizingCurrent) * (ID110 / 16384));
        break;
      case ACTUAL_VALUES_2_BASE_ADDRESS:
        inverter.setActualValues2(parseActualValues2(canMsg.m_data));
        DEBUG_PRINT(DEBUG_LEVEL_VALUES,
                    "0x%X:\n Motor temperature: %f\n, Inverter temperature: %f\n, Error info: %d\n, IGBT temperature: %f\n",
                    canMsg.getCanId(),
                    static_cast<float>(inverter.getActualValues2().tempMotor) * 0.1,
                    static_cast<float>(inverter.getActualValues2().tempInverter) * 0.1,
                    inverter.getActualValues2().errorInfo,
                    static_cast<float>(inverter.getActualValues2().tempIGBT) * 0.1);
        break;
      default:
        break;
      }
    }
  }
}