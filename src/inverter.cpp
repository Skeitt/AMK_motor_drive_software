#include "Inverter.hpp"
#include "CANCommunication.hpp"
#include "DebugFlags.hpp"

Inverter::Inverter(const uint16_t t_nodeAddress)
{
    m_nodeAddress = t_nodeAddress;
    m_actualValues1 = new ActualValues1();
    m_actualValues2 = new ActualValues2();
    m_setpoints1 = new Setpoints1();
    m_state = IDLE;
};

uint16_t Inverter::getNodeAddress() const { return m_nodeAddress; };

ActualValues1 Inverter::getActualValues1() const { return *m_actualValues1; };

ActualValues2 Inverter::getActualValues2() const { return *m_actualValues2; };

Setpoints1 Inverter::getSetpoints1() const { return *m_setpoints1; };

void Inverter::setActualValues1(ActualValues1 t_actualValues1) { m_actualValues1 = &t_actualValues1; };

void Inverter::setActualValues2(ActualValues2 t_actualValues2) { m_actualValues2 = &t_actualValues2; };

void Inverter::setSetpoints1(Setpoints1 t_setpoints1) { m_setpoints1 = &t_setpoints1; };

void Inverter::checkStatus()
{
    uint16_t status = m_actualValues1->status;

    if (status & bError)
    {
        m_state = ERROR;
        DEBUG_PRINT(DEBUG_LEVEL_ERROR, "Error detected\n");
    }
    else if ((status & (bSystemReady | bDerating)) == (bSystemReady | bDerating))
    {
        m_state = LV_ON;
        DEBUG_PRINT(DEBUG_LEVEL_STATE, "LV circuit enabled\n");
    }
    else if ((status & bSystemReady) == bSystemReady)
    {
        m_state = HV_ON;
        DEBUG_PRINT(DEBUG_LEVEL_STATE, "HV circuit enabled\n");
    }
    else if ((status & (bSystemReady | bDcOn | bQuitDcOn)) == (bSystemReady | bDcOn | bQuitDcOn))
    {
        m_state = READY;
        DEBUG_PRINT(DEBUG_LEVEL_STATE, "DcOn enabled\n");
    }
    else if ((status & (bSystemReady | bDcOn | bQuitDcOn | bInverterOn | bQuitInverterOn)) == (bSystemReady | bDcOn | bQuitDcOn | bInverterOn | bQuitInverterOn))
    {
        m_state = CONTROLLER_ACTIVE;
        DEBUG_PRINT(DEBUG_LEVEL_STATE, "Controller enabled\n");
    }
    else
    {
        m_state = IDLE;
        DEBUG_PRINT(DEBUG_LEVEL_STATE, "System inactive (LV switched off)\n");
    }
}

void Inverter::activate()
{
    checkStatus();
    std::pair<const char *, const char *> error;

    switch (m_state)
    {
    case IDLE:
        DEBUG_PRINT(DEBUG_LEVEL_STATE, "Switch on LV circuit...\n");
        break;
    case LV_ON:
        setSetpoints1(Setpoints1{0, 0, 0, 0});
        sendMessage(parseSetpoints1(getSetpoints1(), getNodeAddress()));
        DEBUG_PRINT(DEBUG_LEVEL_STATE, "Switch on HV circuit...\n");
        break;
    case HV_ON:
        DEBUG_PRINT(DEBUG_LEVEL_STATE, "Enabling HV activation level...\n");
        setSetpoints1(Setpoints1{cbDcOn, 0, 0, 0});
        sendMessage(parseSetpoints1(getSetpoints1(), getNodeAddress()));
        break;
    case READY:
        DEBUG_PRINT(DEBUG_LEVEL_STATE, "Performing security check (torque limit pos and neg = 0)\n");
        setSetpoints1(Setpoints1{cbDcOn, 0, 0, 0});
        sendMessage(parseSetpoints1(getSetpoints1(), getNodeAddress()));

        DEBUG_PRINT(DEBUG_LEVEL_STATE, "Enabling driver...\n");
        setSetpoints1(Setpoints1{cbDcOn | cbEnable, 0, 0, 0});
        sendMessage(parseSetpoints1(getSetpoints1(), getNodeAddress()));

        DEBUG_PRINT(DEBUG_LEVEL_STATE, "Enabling controller...\n");
        setSetpoints1(Setpoints1{cbDcOn | cbEnable | cbInverterOn, 0, 0, 0});
        sendMessage(parseSetpoints1(getSetpoints1(), getNodeAddress()));
        break;
    case CONTROLLER_ACTIVE:
        // setpoints must be set in main loop
        DEBUG_PRINT(DEBUG_LEVEL_STATE, "Sending setpoints...\n");
        sendMessage(parseSetpoints1(getSetpoints1(), getNodeAddress()));
        break;
    case ERROR:
        DEBUG_PRINT(DEBUG_LEVEL_STATE, "Disabling controller...\n");
        setSetpoints1(Setpoints1{cbDcOn | cbEnable, 0, 0, 0});
        sendMessage(parseSetpoints1(getSetpoints1(), getNodeAddress()));

        error = getError(m_actualValues2->errorInfo);
        DEBUG_PRINT(DEBUG_LEVEL_ERROR, "Error code: %d (%s), Error class: %s\n",
                    m_actualValues2->errorInfo,
                    error.first,
                    error.second);

        // TODO: remove error based on error removal message field

        DEBUG_PRINT(DEBUG_LEVEL_STATE, "Resetting error...\n");
        setSetpoints1(Setpoints1{cbDcOn | cbEnable | cbErrorReset, 0, 0, 0});
        sendMessage(parseSetpoints1(getSetpoints1(), getNodeAddress()));

        setSetpoints1(Setpoints1{cbDcOn | cbEnable, 0, 0, 0});
        sendMessage(parseSetpoints1(getSetpoints1(), getNodeAddress()));
        break;
    default:
        break;
    }
}

void Inverter::deactivate()
{
    checkStatus();

    switch (m_state)
    {
    case CONTROLLER_ACTIVE:
        DEBUG_PRINT(DEBUG_LEVEL_STATE, "Switching off...\n");
        setSetpoints1(Setpoints1{cbDcOn | cbEnable | cbInverterOn, 0, 0, 0});
        sendMessage(parseSetpoints1(getSetpoints1(), getNodeAddress()));

        DEBUG_PRINT(DEBUG_LEVEL_STATE, "Disabling controller...\n");
        setSetpoints1(Setpoints1{cbDcOn | cbEnable, 0, 0, 0});
        sendMessage(parseSetpoints1(getSetpoints1(), getNodeAddress()));

        DEBUG_PRINT(DEBUG_LEVEL_STATE, "Disabling driver...\n");
        setSetpoints1(Setpoints1{cbDcOn, 0, 0, 0});
        sendMessage(parseSetpoints1(getSetpoints1(), getNodeAddress()));
        break;
    case READY:
        DEBUG_PRINT(DEBUG_LEVEL_STATE, "Disabling HV activation level...\n");
        setSetpoints1(Setpoints1{0, 0, 0, 0});
        sendMessage(parseSetpoints1(getSetpoints1(), getNodeAddress()));
        break;
    case HV_ON:
        DEBUG_PRINT(DEBUG_LEVEL_STATE, "Switch off HV circuit...\n");
        // TODO: send signal to battery pack to switch off HV
        setSetpoints1(Setpoints1{0, 0, 0, 0});
        sendMessage(parseSetpoints1(getSetpoints1(), getNodeAddress()));
        break;
    case LV_ON:
        DEBUG_PRINT(DEBUG_LEVEL_STATE, "Switch off LV circuit...\n");
        // TODO: send signal to battery pack to switch off HV
        setSetpoints1(Setpoints1{0, 0, 0, 0});
        sendMessage(parseSetpoints1(getSetpoints1(), getNodeAddress()));
        break;
    case IDLE:
        setSetpoints1(Setpoints1{0, 0, 0, 0});
        break;
    default:
        break;
    }
}
