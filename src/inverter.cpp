#include "Inverter.hpp"
#include "CANCommunication.hpp"

Inverter::Inverter(const uint16_t t_nodeAddress) { m_nodeAddress = t_nodeAddress; };

byte Inverter::getNodeAddress() const { return m_nodeAddress; };

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
        Serial.printf("Error detected\n");
        m_state = ERROR;
    }
    else if ((status & (bSystemReady | bDerating)) == (bSystemReady | bDerating))
    {
        m_state = LV_ON;
        Serial.printf("LV circuit enabled\n");
    }
    else if ((status & bSystemReady) == bSystemReady)
    {
        m_state = HV_ON;
        Serial.printf("HV circuit enabled\n");
    }
    else if ((status & (bSystemReady | bDcOn | bQuitDcOn)) == (bSystemReady | bDcOn | bQuitDcOn))
    {
        m_state = READY;
        Serial.printf("DcOn enabled\n");
    }
    else if ((status & (bSystemReady | bDcOn | bQuitDcOn | bInverterOn | bQuitInverterOn)) == (bSystemReady | bDcOn | bQuitDcOn | bInverterOn | bQuitInverterOn))
    {
        m_state = CONTROLLER_ACTIVE;
        Serial.printf("Controller enabled\n");
    }
    else
    {
        m_state = IDLE;
        Serial.printf("System inactive (LV switched off)\n");
    }
}

void Inverter::activate()
{
    checkStatus();

    switch (m_state)
    {
    case IDLE:
        Serial.printf("Switch on LV circuit...\n");
        break;
    case LV_ON:
        setSetpoints1(Setpoints1{0, 0, 0, 0});
        sendMessage(parseSetpoints1(getSetpoints1(), getNodeAddress()));
        Serial.printf("Switch on HV circuit...\n");
        break;
    case HV_ON:
        Serial.printf("Enabling HV activation level...\n");
        setSetpoints1(Setpoints1{cbDcOn, 0, 0, 0});
        sendMessage(parseSetpoints1(getSetpoints1(), getNodeAddress()));
        break;
    case READY:
        Serial.printf("Performing security check (torque limit pos and neg = 0)\n");
        setSetpoints1(Setpoints1{cbDcOn, 0, 0, 0});
        sendMessage(parseSetpoints1(getSetpoints1(), getNodeAddress()));

        Serial.printf("Enabling driver...\n");
        setSetpoints1(Setpoints1{cbDcOn | cbEnable, 0, 0, 0});
        sendMessage(parseSetpoints1(getSetpoints1(), getNodeAddress()));

        Serial.printf("Enabling controller...\n");
        setSetpoints1(Setpoints1{cbDcOn | cbEnable | cbInverterOn, 0, 0, 0});
        sendMessage(parseSetpoints1(getSetpoints1(), getNodeAddress()));
        break;
    case CONTROLLER_ACTIVE:
        // setpoints must be set in main loop
        Serial.printf("Sending setpoints...\n");
        sendMessage(parseSetpoints1(getSetpoints1(), getNodeAddress()));
        break;
    case ERROR:
        Serial.printf("Disabling controller...\n");
        setSetpoints1(Setpoints1{cbDcOn | cbEnable, 0, 0, 0});
        sendMessage(parseSetpoints1(getSetpoints1(), getNodeAddress()));

        auto error = getError(m_actualValues2->errorInfo);
        Serial.printf("Error code: %d (%s), Error class: %s\n",
                      m_actualValues2->errorInfo, 
                      error.first,
                      error.second);
                      
        // TODO: remove error based on error removal message field

        Serial.printf("Resetting error...\n");
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
        Serial.printf("Switching off...\n");
        setSetpoints1(Setpoints1{cbDcOn | cbEnable | cbInverterOn, 0, 0, 0});
        sendMessage(parseSetpoints1(getSetpoints1(), getNodeAddress()));

        Serial.printf("Disabling controller...\n");
        setSetpoints1(Setpoints1{cbDcOn | cbEnable, 0, 0, 0});
        sendMessage(parseSetpoints1(getSetpoints1(), getNodeAddress()));

        Serial.printf("Disabling driver...\n");
        setSetpoints1(Setpoints1{cbDcOn, 0, 0, 0});
        sendMessage(parseSetpoints1(getSetpoints1(), getNodeAddress()));
        break;
    case READY:
        Serial.printf("Disabling HV activation level...\n");
        setSetpoints1(Setpoints1{0, 0, 0, 0});
        sendMessage(parseSetpoints1(getSetpoints1(), getNodeAddress()));
        break;
    case HV_ON:
        Serial.printf("Switch off HV circuit...\n");
        // TODO: send signal to battery pack to switch off HV
        setSetpoints1(Setpoints1{0, 0, 0, 0});
        sendMessage(parseSetpoints1(getSetpoints1(), getNodeAddress()));
        break;
    case LV_ON:
        Serial.printf("Switch off LV circuit...\n");
        // TODO: send signal to battery pack to switch off HV
        setSetpoints1(Setpoints1{0, 0, 0, 0});
        sendMessage(parseSetpoints1(getSetpoints1(), getNodeAddress()));
        break;
    case IDLE:
        break;
    default:
        break;
    }
}
