#include "amk_specs.h"

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

uint16_t get_node_address_from_can_id(long can_id)
{
  // Implementare il codice per estrarre l'indirizzo del nodo da can_id
  // e restituirelo
  return 0; // Ritorna un indirizzo di default
}