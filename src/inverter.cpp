#include "Inverter.hpp"

Inverter::Inverter(const uint16_t t_nodeAddress) { m_nodeAddress = t_nodeAddress; };

byte Inverter::getNodeAddress() const { return m_nodeAddress; };

ActualValues1 Inverter::getActualValues1() const { return *m_actualValues1; };

ActualValues2 Inverter::getActualValues2() const { return *m_actualValues2; };

Setpoints1 Inverter::getSetpoints1() const { return *m_setpoints1; };

void Inverter::setActualValues1(ActualValues1 t_actualValues1) { m_actualValues1 = &t_actualValues1; };

void Inverter::setActualValues2(ActualValues2 t_actualValues2) { m_actualValues2 = &t_actualValues2; };

void Inverter::setSetpoints1(Setpoints1 t_setpoints1) { m_setpoints1 = &t_setpoints1; };

void Inverter::setTargetParameters(uint16_t targetVel, int16_t torqueLimPos, int16_t torqueLimNeg)
{
    m_setpoints1->target_velocity = targetVel;
    m_setpoints1->torque_limit_positive = torqueLimPos;
    m_setpoints1->torque_limit_negative = torqueLimNeg;
};

void Inverter::checkStatus()
{
    uint16_t status = m_actualValues1->status;

    if ((status & (bSystemReady | bDerating)) == (bSystemReady | bDerating))
    {
        m_state = LV_ON;
    }
    if ((status & bSystemReady) == bSystemReady)
    {
        m_state = HV_ON;
    }
    if ((status & (bSystemReady | bDcOn | bQuitDcOn)) == (bSystemReady | bDcOn | bQuitDcOn))
    {
        if (m_state == READY)
            m_state = SECURITY_CHECK_DONE;
        else if (m_state == SECURITY_CHECK_DONE)
            m_state = DRIVER_ACTIVE;
        else
            m_state = READY;
    }
    if ((status & (bSystemReady | bDcOn | bQuitDcOn | bInverterOn | bQuitInverterOn)) == (bSystemReady | bDcOn | bQuitDcOn | bInverterOn | bQuitInverterOn))
    {
        if (m_state == DRIVER_ACTIVE)
            m_state = CONTROLLER_ACTIVE;
        else
            m_state = SENDING_SETPOINTS;
    }
    if ((status & (bError | bSystemReady)) == (bError | bSystemReady))
    {
        m_state = ERROR;
    }
    if ((status & (bError | bSystemReady | bInverterOn)) == (bError | bSystemReady | bInverterOn))
    {
        m_state = RESET_ERROR;
    }
}

void Inverter::activate()
{
    checkStatus();

    switch (m_state)
    {
    case LV_ON:
        Serial.printf("LV circuit enabled");
        break;
    case HV_ON:
        Serial.printf("HV circuit enabled");
        setSetpoints1(Setpoints1{cbDcOn, 0, 0, 0});
        break;
    case READY:
        // invio di torque limit pos e neg = 0
        Serial.printf("Sending torque limit pos and neg = 0");
        setSetpoints1(Setpoints1{cbDcOn, 0, 0, 0});
        m_state = SECURITY_CHECK_DONE;
        break;
    case SECURITY_CHECK_DONE:
        // invio di bEnable = 1
        Serial.printf("Sending driver enable = 1");
        setSetpoints1(Setpoints1{cbDcOn | cbEnable, 0, 0, 0});
        m_state = DRIVER_ACTIVE;
        break;
    case DRIVER_ACTIVE:
        // invio di bInverterOn = 1
        Serial.printf("Sending inverter on = 1");
        setSetpoints1(Setpoints1{cbDcOn | cbEnable | cbInverterOn, 0, 0, 0});
        // nel caso in cui la procedura di ready sia stata saltata, ricomincia da READY
        m_state = READY;
        break;
    case CONTROLLER_ACTIVE:
        setSetpoints1(Setpoints1{cbDcOn | cbEnable | cbInverterOn, 500, 100, 0});
        break;
    case SENDING_SETPOINTS:
        break;
    case ERROR:
        setSetpoints1(Setpoints1{cbDcOn | cbEnable, 0, 0, 0});
        break;
    case RESET_ERROR:
        setSetpoints1(Setpoints1{cbDcOn | cbEnable | cbErrorReset, 0, 0, 0});
        break;
    default:
        break;
    }
}

void Inverter::deactivate()
{
    // Implementare la disattivazione dell'inverter
    // inviando i setpoint corretti
    checkStatus();
    switch (m_state)
    {
    case SENDING_SETPOINTS:
        Serial.printf("switching off...");
        setSetpoints1(Setpoints1{cbDcOn | cbEnable | cbInverterOn, 0, 0, 0}); // setpoint nulli
        m_state = CONTROLLER_ACTIVE;
        break;
    case CONTROLLER_ACTIVE:
        Serial.printf("Disabling controller...");
        setSetpoints1(Setpoints1{cbDcOn | cbEnable, 0, 0, 0}); // InverterOn = 0
        m_state = DRIVER_ACTIVE;
        break;
    case DRIVER_ACTIVE:
        Serial.printf("Disabling driver...");
        setSetpoints1(Setpoints1{cbDcOn, 0, 0, 0}); // Enable = 0
        m_state = READY;
        break;

    case READY:
        Serial.printf("Disabling driver...");
        setSetpoints1(Setpoints1{0, 0, 0, 0}); // DcOn = 0
        m_state = HV_ON;
        break;

    case HV_ON:
        Serial.printf("switch off HV circuit...");
        break;
    case LV_ON:
        Serial.printf("switch off LV circuit...");
        break;

    default:
        break;
    }
}
