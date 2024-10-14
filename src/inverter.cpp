#include "Inverter.h"
Inverter::Inverter(const uint16_t node_address) { this->node_address = node_address; };

byte Inverter::get_node_address() const { return this->node_address; };

ActualValues1 Inverter::get_actual_values_1() const { return *this->actual_values_1; };

ActualValues2 Inverter::get_actual_values_2() const { return *this->actual_values_2; };

Setpoints1 Inverter::get_setpoints_1() const { return *this->setpoints_1; };

void Inverter::set_actual_values_1(ActualValues1 actual_values_1) { this->actual_values_1 = &actual_values_1; };

void Inverter::set_actual_values_2(ActualValues2 actual_values_2) { this->actual_values_2 = &actual_values_2; };

void Inverter::set_setpoints_1(Setpoints1 setpoints_1) { this->setpoints_1 = &setpoints_1; };
