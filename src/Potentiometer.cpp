#include "Potentiometer.hpp"
#include "PINOUT.hpp"

Potentiometer::Potentiometer() : m_value(0), m_rpm(0), m_torque_pos(0), m_torque_neg(0) {}

void Potentiometer::update()
{
    m_rpm = 100;
    m_torque_pos = 100;
    m_torque_neg = 0;
    // m_value = analogRead(POTENTIOMETER_PIN);

    // if (m_value > 180)
    // {
    //     m_rpm = map(m_value, 115, 970, 0, 6000);
    //     m_torque_pos = 100;
    //     m_torque_neg = 0;
    // }
    // else
    // {
    //     m_rpm = 0;
    //     m_torque_pos = 0;
    //     m_torque_neg = 0;
    // }
}

int16_t Potentiometer::getRpm() const { return m_rpm; }

int16_t Potentiometer::getTorquePos() const { return m_torque_pos; }

int16_t Potentiometer::getTorqueNeg() const { return m_torque_neg; }

int Potentiometer::getRawValue() const { return m_value; }
