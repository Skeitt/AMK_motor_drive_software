#ifndef INVERTER_H
#define INVERTER_H

#include "amk_specs.h"

class Inverter {
    private:
        byte node_address;
        ActualValues1* actual_values_1;
        ActualValues2* actual_values_2;
        Setpoints1* setpoints_1;

    public:
        // Costruttore
        Inverter(const byte node_address);

        byte get_node_address() const;
        void set_actual_values_1(ActualValues1 actual_values_1);
        void set_actual_values_2(ActualValues2 actual_values_2);
        void set_setpoints_1(Setpoints1 setpoints_1);
        void check_status();
};

#endif // INVERTER_H
