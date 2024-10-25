#ifndef AMK_UTILS_H
#define AMK_UTILS_H

#include <Arduino.h>
#include "CANMessage.hpp"
#include "amk_diagnostic.hpp"

/**
 * @brief Inverters node addresses
 */
#define INVERTER_1_NODE_ADDRESS 1 ///< Indirizzo nodo inverter 1.
#define INVERTER_2_NODE_ADDRESS 2 ///< Indirizzo nodo inverter 2.
#define INVERTER_3_NODE_ADDRESS 5 ///< Indirizzo nodo inverter 3.
#define INVERTER_4_NODE_ADDRESS 6 ///< Indirizzo nodo inverter 4.

/**
 * @brief Messages base addresses
 */
#define ACTUAL_VALUES_1_BASE_ADDRESS 0x282 ///< Base address AMK Actual Values 1 (inverter -> CAN controller).
#define ACTUAL_VALUES_2_BASE_ADDRESS 0x284 ///< Base address AMK Actual Values 2 (inverter -> CAN controller).
#define SETPOINTS_1_BASE_ADDRESS 0x183     ///< Base address AMK Setpoints 1 (CAN controller -> inverter).

/**
 * @brief Messages addresses
 */
/* INVERTER 1 */
#define INVERTER_1_ACTUAL_VALUES_1 (ACTUAL_VALUES_1_BASE_ADDRESS + INVERTER_1_NODE_ADDRESS) ///< AMK Actual Values 1 address for Inverter 1.
#define INVERTER_1_ACTUAL_VALUES_2 (ACTUAL_VALUES_2_BASE_ADDRESS + INVERTER_1_NODE_ADDRESS) ///< AMK Actual Values 2 address for Inverter 1.
#define INVERTER_1_SETPOINTS_1 (SETPOINTS_1_BASE_ADDRESS + INVERTER_1_NODE_ADDRESS)         ///< AMK Setpoints 1 address for Inverter 1.

/* INVERTER 2 */
#define INVERTER_2_ACTUAL_VALUES_1 (ACTUAL_VALUES_1_BASE_ADDRESS + INVERTER_2_NODE_ADDRESS) ///< AMK Actual Values 1 address for Inverter 2.
#define INVERTER_2_ACTUAL_VALUES_2 (ACTUAL_VALUES_2_BASE_ADDRESS + INVERTER_2_NODE_ADDRESS) ///< AMK Actual Values 2 address for Inverter 2.
#define INVERTER_2_SETPOINTS_1 (SETPOINTS_1_BASE_ADDRESS + INVERTER_2_NODE_ADDRESS)         ///< AMK Setpoints 1 address for Inverter 2.

/* INVERTER 3 */
#define INVERTER_3_ACTUAL_VALUES_1 (ACTUAL_VALUES_1_BASE_ADDRESS + INVERTER_3_NODE_ADDRESS) ///< AMK Actual Values 1 address for Inverter 3.
#define INVERTER_3_ACTUAL_VALUES_2 (ACTUAL_VALUES_2_BASE_ADDRESS + INVERTER_3_NODE_ADDRESS) ///< AMK Actual Values 2 address for Inverter 3.
#define INVERTER_3_SETPOINTS_1 (SETPOINTS_1_BASE_ADDRESS + INVERTER_3_NODE_ADDRESS)         ///< AMK Setpoints 1 address for Inverter 3.

/* INVERTER 4 */
#define INVERTER_4_ACTUAL_VALUES_1 (ACTUAL_VALUES_1_BASE_ADDRESS + INVERTER_4_NODE_ADDRESS) ///< AMK Actual Values 1 address for Inverter 4.
#define INVERTER_4_ACTUAL_VALUES_2 (ACTUAL_VALUES_2_BASE_ADDRESS + INVERTER_4_NODE_ADDRESS) ///< AMK Actual Values 2 address for Inverter 4.
#define INVERTER_4_SETPOINTS_1 (SETPOINTS_1_BASE_ADDRESS + INVERTER_4_NODE_ADDRESS)         ///< AMK Setpoints 1 address for Inverter 4.

/**
 *  @brief Control word values
 */
#define cbInverterOn 0x100    ///< Controller enable
#define cbDcOn 0x200          ///< HV activation
#define cbEnable 0x400 ///< Driver enable
#define cbErrorReset 0x800    ///< Remove error

/**
 * @brief Status word values
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
 * @brief static Parameters
 */
#define ID110 107 ///< Converter peak current

/**
 * @brief Contains the status word and setpoints
 */
typedef struct
{
    uint16_t control;              ///< Control word.
    int16_t targetVelocity;       ///< Speed setpoint.
    int16_t torqueLimitPositiv; ///< Positive torque limit.
    int16_t torqueLimitNegativ; ///< Negative torque limit.
} Setpoints1;

/**
 * @brief Contains the status word and actual values
 */
typedef struct
{
    uint16_t status;         ///< Status word.
    int16_t actualVelocity; ///< Actual speed value.
    int16_t torqueCurrent;      ///< Raw data for calculating 'actual torque current' Iq.
    int16_t magnetizingCurrent; ///< Raw data for calculating 'actual magnetizing current' Id.
} ActualValues1;

/**
 * @brief Contains actual values
 */
typedef struct
{
    int16_t tempMotor;    ///< Motor temperature.
    int16_t tempInverter; ///< Cold plate temperature.
    uint16_t errorInfo;   ///< Diagnostic number.
    int16_t tempIGBT;     ///< IGBT temperature.
} ActualValues2;

enum InverterState
{
    IDLE,
    LV_ON,
    HV_ON,
    READY,
    CONTROLLER_ACTIVE,
    ERROR,
};

/**
 * @brief Parses Actual Values 1 from a CAN message data array.
 * 
 * @param data An array of 8 bytes containing the raw CAN message data.
 * @return Parsed ActualValues1 structure containing the status, actual velocity, 
 *         torque current, and magnetizing current.
 */
ActualValues1 parseActualValues1(byte data[8]);

/**
 * @brief Parses Actual Values 2 from a CAN message data array.
 * 
 * @param data An array of 8 bytes containing the raw CAN message data.
 * @return Parsed ActualValues2 structure containing motor temperature, 
 *         inverter temperature, error info, and IGBT temperature.
 */
ActualValues2 parseActualValues2(byte data[8]);

/**
 * @brief Creates a CAN message with setpoints and a node address.
 * 
 * @param setpoints1 The Setpoints1 structure containing control, target velocity, 
 *                   and torque limits.
 * @param nodeAddress The node address of the inverter.
 * @return CANMessage structure containing the message to be sent to the inverter.
 */
CANMessage parseSetpoints1(Setpoints1 setpoints1, uint16_t nodeAddress);

/**
 * @brief Extracts the node address from a CAN message ID.
 * 
 * @param canId The CAN message ID from which to extract the node address.
 * @return The node address extracted from the CAN ID.
 */
uint16_t getNodeAddressFromCANId(long canId);

/**
 * @brief Retrieves the error message from the error map.
 * 
 * @param errorInfo The error code to retrieve the message for.
 * @return A pair of strings containing the error category and class.
 */
std::pair<const char*, const char*> getError(uint16_t errorInfo);

#endif // AMK_UTILS_H
