#ifndef POTENTIOMETER_HPP
#define POTENTIOMETER_HPP

#include <stdint.h>

/**
 * @class Potentiometer
 * @brief Class to represent a potentiometer, providing methods to read and calculate RPM and torque values.
 */
class Potentiometer {
private:
    int m_value;         ///< Raw analog value from the potentiometer
    int16_t m_rpm;           ///< Calculated RPM based on the potentiometer value
    int16_t m_torque_pos;    ///< Positive torque calculated based on the potentiometer value
    int16_t m_torque_neg;    ///< Negative torque calculated based on the potentiometer value

public:
    /**
     * @brief Constructor for the Potentiometer class.
     */
    Potentiometer();

    /**
     * @brief Updates the potentiometer reading and calculates RPM, positive torque, and negative torque.
     */
    void update();

    /**
     * @brief Gets the calculated RPM value.
     * @return The RPM value calculated based on the potentiometer reading.
     */
    int16_t getRpm() const;

    /**
     * @brief Gets the positive torque value.
     * @return The positive torque limit value calculated based on the potentiometer reading.
     */
    int16_t getTorquePos() const;

    /**
     * @brief Gets the negative torque value.
     * @return The negative torque limit value calculated based on the potentiometer reading.
     */
    int16_t getTorqueNeg() const;

    /**
     * @brief Gets the raw analog value from the potentiometer.
     * @return The raw analog value read from the potentiometer.
     */
    int getRawValue() const;
};

#endif // POTENTIOMETER_HPP