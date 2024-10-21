#ifndef INVERTER_H
#define INVERTER_H

#include "utils.h"

class Inverter {
    private:
        uint16_t node_address;
        ActualValues1* actual_values_1;
        ActualValues2* actual_values_2;
        Setpoints1* setpoints_1;
        InverterState state;

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
        void set_target_parameters(uint16_t target_velocity, int16_t torque_limit_positive, int16_t torque_limit_negative);

        void check_status();
        void activate();
        void deactivate();
};

#endif // INVERTER_H
