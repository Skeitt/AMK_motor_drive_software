// AMK.h
#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>
#include "CANMessage.hpp"

/**
 * @brief Indirizzi dei nodi degli inverter.
 */
#define INVERTER_1_NODE_ADDRESS 1 ///< Indirizzo nodo inverter 1.
#define INVERTER_2_NODE_ADDRESS 2 ///< Indirizzo nodo inverter 2.
#define INVERTER_3_NODE_ADDRESS 5 ///< Indirizzo nodo inverter 3.
#define INVERTER_4_NODE_ADDRESS 6 ///< Indirizzo nodo inverter 4.

/**
 * @brief Indirizzi base per i valori attuali e i setpoint.
 */
#define ACTUAL_VALUES_1_BASE_ADDRESS 0x282 ///< Indirizzo base per valori attuali tipo 1 (inverter -> CAN controller).
#define ACTUAL_VALUES_2_BASE_ADDRESS 0x284 ///< Indirizzo base per valori attuali tipo 2 (inverter -> CAN controller).
#define SETPOINTS_1_BASE_ADDRESS 0x183     ///< Indirizzo base per setpoint (CAN controller -> inverter).

/**
 * @brief Indirizzi calcolati per i pacchetti di ciascun inverter.
 */
/* INVERTER 1 */
#define INVERTER_1_ACTUAL_VALUES_1 (ACTUAL_VALUES_1_BASE_ADDRESS + INVERTER_1_NODE_ADDRESS) ///< Valori attuali tipo 1 per inverter 1.
#define INVERTER_1_ACTUAL_VALUES_2 (ACTUAL_VALUES_2_BASE_ADDRESS + INVERTER_1_NODE_ADDRESS) ///< Valori attuali tipo 2 per inverter 1.
#define INVERTER_1_SETPOINTS_1 (SETPOINTS_1_BASE_ADDRESS + INVERTER_1_NODE_ADDRESS)         ///< Setpoint per inverter 1.

/* INVERTER 2 */
#define INVERTER_2_ACTUAL_VALUES_1 (ACTUAL_VALUES_1_BASE_ADDRESS + INVERTER_2_NODE_ADDRESS) ///< Valori attuali tipo 1 per inverter 2.
#define INVERTER_2_ACTUAL_VALUES_2 (ACTUAL_VALUES_2_BASE_ADDRESS + INVERTER_2_NODE_ADDRESS) ///< Valori attuali tipo 2 per inverter 2.
#define INVERTER_2_SETPOINTS_1 (SETPOINTS_1_BASE_ADDRESS + INVERTER_2_NODE_ADDRESS)         ///< Setpoint per inverter 2.

/* INVERTER 3 */
#define INVERTER_3_ACTUAL_VALUES_1 (ACTUAL_VALUES_1_BASE_ADDRESS + INVERTER_3_NODE_ADDRESS) ///< Valori attuali tipo 1 per inverter 3.
#define INVERTER_3_ACTUAL_VALUES_2 (ACTUAL_VALUES_2_BASE_ADDRESS + INVERTER_3_NODE_ADDRESS) ///< Valori attuali tipo 2 per inverter 3.
#define INVERTER_3_SETPOINTS_1 (SETPOINTS_1_BASE_ADDRESS + INVERTER_3_NODE_ADDRESS)         ///< Setpoint per inverter 3.

/* INVERTER 4 */
#define INVERTER_4_ACTUAL_VALUES_1 (ACTUAL_VALUES_1_BASE_ADDRESS + INVERTER_4_NODE_ADDRESS) ///< Valori attuali tipo 1 per inverter 4.
#define INVERTER_4_ACTUAL_VALUES_2 (ACTUAL_VALUES_2_BASE_ADDRESS + INVERTER_4_NODE_ADDRESS) ///< Valori attuali tipo 2 per inverter 4.
#define INVERTER_4_SETPOINTS_1 (SETPOINTS_1_BASE_ADDRESS + INVERTER_4_NODE_ADDRESS)         ///< Setpoint per inverter 4.

/**
 *  @brief control word values
 */
#define cbInverterOn 0x01    ///< Controller enable
#define cbDcOn 0x02          ///< HV activation
#define cbEnable 0x04 ///< Driver enable
#define cbErrorReset 0x08    ///< Remove error

/**
 * @brief status word values
 */
#define bSystemReady 0x100 ///< System ready (SBM)
#define bError 0x200      ///< Error
#define bWarn 0x400      ///< Warning
#define bQuitDcOn 0x800   ///< HV activation acknowledgment
#define bDcOn 0x1000       ///< HV activation level
#define bQuitInverterOn 0x2000 ///< Controller enable acknowledgment
#define bInverterOn 0x4000     ///< Controller enable level
#define bDerating 0x8000       ///< Derating

/**
 * @brief parametri
 */
#define ID110 107 ///< Converter peak current

/**
 * @brief Contains the status word and setpoints
 */
typedef struct
{
    uint16_t control;              ///< Controllo.
    int16_t target_velocity;       ///< Setpoint di velocità.
    int16_t torque_limit_positive; ///< Limite di coppia positivo.
    int16_t torque_limit_negative; ///< Limite di coppia negativo.
} Setpoints1;

/**
 * @brief Contains the status word and actual values
 */
typedef struct
{
    uint16_t status;         ///< Stato.
    int16_t actual_velocity; ///< Velocità attuale.
    int16_t torque_current;      ///< Corrente di coppia.
    int16_t magnetizing_current; ///< Corrente di magnetizzazione.
} ActualValues1;

/**
 * @brief Contains actual values
 */
typedef struct
{
    int16_t temp_motor;    ///< Temperatura motore.
    int16_t temp_inverter; ///< Temperatura inverter.
    uint16_t error_info;   ///< Informazioni errori.
    int16_t temp_igbt;     ///< Temperatura IGBT.
} ActualValues2;

enum InverterState
{
    LV_ON,
    HV_ON,
    READY,
    SECURITY_CHECK_DONE,
    DRIVER_ACTIVE,
    CONTROLLER_ACTIVE,
    SENDING_SETPOINTS,
    ERROR,
    RESET_ERROR,
};

ActualValues1 parse_actual_values_1(byte data[8]);
ActualValues2 parse_actual_values_2(byte data[8]);
CANMessage parse_setpoints_1(Setpoints1 setpoints_1, uint16_t node_address);
uint16_t get_node_address_from_can_id(long can_id);

#endif // UTILS_H
