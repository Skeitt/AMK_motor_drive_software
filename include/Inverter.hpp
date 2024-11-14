#ifndef INVERTER_HPP
#define INVERTER_HPP

#include "AMKUtils.hpp"

/**
 * @brief The Inverter class represents an inverter in the system, handling
 * communication of actual values and setpoints via CAN.
 */
class Inverter
{
private:
    uint16_t m_nodeAddress;         ///< Node address of the inverter.
    ActualValues1 m_actualValues1; ///< Actual Values 1 data of the inverter.
    ActualValues2 m_actualValues2; ///< Actual Values 2 data of the inverter.
    Setpoints1 m_setpoints1;       ///< Setpoints 1 data of the inverter.
    InverterState m_state;          ///< Current state of the inverter.

public:
    /**
     * @brief Constructs an Inverter object with the specified node address.
     *
     * @param t_nodeAddress The CAN node address of the inverter.
     */
    Inverter(const uint16_t t_nodeAddress);

    /**
     * @brief Returns the node address of the inverter.
     *
     * @return The CAN node address of the inverter.
     */
    uint16_t getNodeAddress() const;


    /**
     * @brief Returns the state of the inverter.
     *
     * @return The current state of the inverter.
     */
    InverterState getState() const;

    /**
     * @brief Returns the actual values 1 of the inverter.
     *
     * @return A copy of the ActualValues1 structure of the inverter.
     */
    ActualValues1 getActualValues1() const;

    /**
     * @brief Returns the actual values 2 of the inverter.
     *
     * @return A copy of the ActualValues2 structure of the inverter.
     */
    ActualValues2 getActualValues2() const;

    /**
     * @brief Returns the setpoints 1 of the inverter.
     *
     * @return A copy of the Setpoints1 structure of the inverter.
     */
    Setpoints1 getSetpoints1() const;

    /**
     * @brief Sets the actual values 1 for the inverter.
     *
     * @param t_actualValues1 The ActualValues1 structure to be set.
     */
    void setActualValues1(ActualValues1 t_actualValues1);

    /**
     * @brief Sets the actual values 2 for the inverter.
     *
     * @param t_actualValues2 The ActualValues2 structure to be set.
     */
    void setActualValues2(ActualValues2 t_actualValues2);

    /**
     * @brief Sets the setpoints 1 for the inverter.
     *
     * @param t_setpoints1 The Setpoints1 structure to be set.
     */
    void setSetpoints1(Setpoints1 t_setpoints1);

    /**
     * @brief Checks the status bits of the inverter and updates its state.
     */
    void checkStatus();

    /**
     * @brief Activates the inverter, setting it to an operational state.
     */
    void activate();

    /**
     * @brief Deactivates the inverter, putting it into a safe, inactive state.
     */
    void deactivate();
};

#endif // INVERTER_HPP