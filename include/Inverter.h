#ifndef INVERTER_H
#define INVERTER_H

#include "amk_specs.h"

class Inverter {
    private:
        uint16_t node_address;
        ActualValues1* actual_values_1;
        ActualValues2* actual_values_2;
        Setpoints1* setpoints_1;

    public:
        // Costruttore
        Inverter(const uint16_t node_address);

        byte get_node_address() const;
        ActualValues1 get_actual_values_1() const;
        ActualValues2 get_actual_values_2() const;
        Setpoints1 get_setpoints_1() const;
        void set_actual_values_1(ActualValues1 actual_values_1);
        void set_actual_values_2(ActualValues2 actual_values_2);
        void set_setpoints_1(Setpoints1 setpoints_1);
};

#endif // INVERTER_H
