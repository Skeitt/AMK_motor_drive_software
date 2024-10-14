#include "Inverter.h"

byte Inverter::get_node_address() const { return this->node_address; };

void Inverter::set_actual_values_1(ActualValues1 actual_values_1) { this->actual_values_1 = &actual_values_1; };

void Inverter::set_actual_values_2(ActualValues2 actual_values_2) { this->actual_values_2 = &actual_values_2; };

void Inverter::set_setpoints_1(Setpoints1 setpoints_1) { this->setpoints_1 = &setpoints_1; };

void Inverter::check_status()
{
    // Implementare il controllo dello stato dell'inverter
    // usando i valori di actual_values_1


    // non va bene perchè anche se è stato attiovato restituirà 1
    // dovrei confrontare il messaggio vero e proprio
    if (this->actual_values_1->status & bDcOn)
    {
        // do some
    }
}