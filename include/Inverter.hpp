#ifndef INVERTER_HPP
#define INVERTER_HPP

#include "utils.hpp"

class Inverter {
    private:
        uint16_t m_nodeAddress;
        ActualValues1* m_actualValues1;
        ActualValues2* m_actualValues2;
        Setpoints1* m_setpoints1;
        InverterState m_state;

    public:
        // Costruttore
        Inverter(const uint16_t t_nodeAddress);

        byte getNodeAddress() const;
        ActualValues1 getActualValues1() const;
        ActualValues2 getActualValues2() const;
        Setpoints1 getSetpoints1() const;
        void setActualValues1(ActualValues1 t_actualValues1);
        void setActualValues2(ActualValues2 t_actualValues2);
        void setSetpoints1(Setpoints1 t_setpoints1);
        void setTargetParameters(uint16_t target_velocity, int16_t torque_limit_positive, int16_t torque_limit_negative);

        void checkStatus();
        void activate();
        void deactivate();
};

#endif // INVERTER_H
