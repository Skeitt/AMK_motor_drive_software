#include "AMKUtils.hpp"

ActualValues1 parseActualValues1(byte data[8])
{
  ActualValues1 actualValues1;
  actualValues1.status = data[0] | (data[1] << 8);
  actualValues1.actualVelocity = data[2] | (data[3] << 8);
  actualValues1.torqueCurrent = data[4] | (data[5] << 8);
  actualValues1.magnetizingCurrent = data[6] | (data[7] << 8);
  return actualValues1;
}

ActualValues2 parseActualValues2(byte data[8])
{
  ActualValues2 actualValues2;
  actualValues2.tempMotor = data[0] | (data[1] << 8);
  actualValues2.tempInverter = data[2] | (data[3] << 8);
  actualValues2.errorInfo = data[4] | (data[5] << 8);
  actualValues2.tempIGBT = data[6] | (data[7] << 8);
  return actualValues2;
}

CANMessage parseSetpoints1(Setpoints1 setpoints1, uint16_t nodeAddress)
{
  CANMessage canMsg;
  canMsg.setCanId(SETPOINTS_1_BASE_ADDRESS + nodeAddress);
  canMsg.m_data[0] = setpoints1.control & 0xFF;
  canMsg.m_data[1] = (setpoints1.control >> 8) & 0xFF;
  canMsg.m_data[2] = setpoints1.targetVelocity & 0xFF;
  canMsg.m_data[3] = (setpoints1.targetVelocity >> 8) & 0xFF;
  canMsg.m_data[4] = setpoints1.torqueLimitPositiv & 0xFF;
  canMsg.m_data[5] = (setpoints1.torqueLimitPositiv >> 8) & 0xFF;
  canMsg.m_data[6] = setpoints1.torqueLimitNegativ & 0xFF;
  canMsg.m_data[7] = (setpoints1.torqueLimitNegativ >> 8) & 0xFF;
  return canMsg;
}

uint16_t getNodeAddressFromCANId(long canId)
{
  switch (canId)
  {
  case INVERTER_1_ACTUAL_VALUES_1:
  case INVERTER_1_ACTUAL_VALUES_2:
  case INVERTER_1_SETPOINTS_1:
    return INVERTER_1_NODE_ADDRESS;
    break;
  case INVERTER_2_ACTUAL_VALUES_1:
  case INVERTER_2_ACTUAL_VALUES_2:
  case INVERTER_2_SETPOINTS_1:
    return INVERTER_2_NODE_ADDRESS;
    break;
  default:
    return 0;
    break;
  }
}

std::pair<const char *, const char *> getError(uint16_t errorInfo)
{
  auto it = ERROR_MAP.find(errorInfo);
  if (it != ERROR_MAP.end())
  {
    return it->second;
  }
  else{
    return {"Unknown error category", "Unknown class of error"};
  }
  
}