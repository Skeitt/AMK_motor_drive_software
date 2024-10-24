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

    if(status & bError)
    {
        m_state = ERROR;
    }
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
        m_state = READY;
    }
    if ((status & (bSystemReady | bDcOn | bQuitDcOn | bInverterOn | bQuitInverterOn)) == (bSystemReady | bDcOn | bQuitDcOn | bInverterOn | bQuitInverterOn))
    {
        m_state = CONTROLLER_ACTIVE;
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
        sendMessage(parseSetpoints1(getSetpoints1(), getNodeAddress()));
        break;
    case READY:
        // invio di torque limit pos e neg = 0
        Serial.printf("Sending torque limit pos and neg = 0");
        setSetpoints1(Setpoints1{cbDcOn, 0, 0, 0});
        sendMessage(parseSetpoints1(getSetpoints1(), getNodeAddress()));

        Serial.printf("Sending driver enable = 1");
        setSetpoints1(Setpoints1{cbDcOn | cbEnable, 0, 0, 0});
        sendMessage(parseSetpoints1(getSetpoints1(), getNodeAddress()));

        // invio di bInverterOn = 1
        Serial.printf("Sending inverter on = 1");
        setSetpoints1(Setpoints1{cbDcOn | cbEnable | cbInverterOn, 0, 0, 0});
        sendMessage(parseSetpoints1(getSetpoints1(), getNodeAddress()));
        break;
    case CONTROLLER_ACTIVE:
        // i setpoint vanno settati nel main
        sendMessage(parseSetpoints1(getSetpoints1(), getNodeAddress()));
        break;
    case ERROR:
        Serial.printf("Error detected");
        setSetpoints1(Setpoints1{cbDcOn | cbEnable, 0, 0, 0}); // InverterOn = 0
        sendMessage(parseSetpoints1(getSetpoints1(), getNodeAddress()));

        // determina causa di errore e elimina

        // operazione di reset
        Serial.printf("Resetting error...");
        setSetpoints1(Setpoints1{cbDcOn | cbEnable | cbErrorReset, 0, 0, 0}); // ErrorReset = 1
        sendMessage(parseSetpoints1(getSetpoints1(), getNodeAddress()));

        setSetpoints1(Setpoints1{cbDcOn | cbEnable, 0, 0, 0}); // ErrorReset = 0
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
        Serial.printf("switching off...");
        setSetpoints1(Setpoints1{cbDcOn | cbEnable | cbInverterOn, 0, 0, 0}); // setpoint nulli
        sendMessage(parseSetpoints1(getSetpoints1(), getNodeAddress()));

        Serial.printf("Disabling controller...");
        setSetpoints1(Setpoints1{cbDcOn | cbEnable, 0, 0, 0}); // InverterOn = 0
        sendMessage(parseSetpoints1(getSetpoints1(), getNodeAddress()));

        Serial.printf("Disabling driver...");
        setSetpoints1(Setpoints1{cbDcOn, 0, 0, 0}); // Enable = 0
        sendMessage(parseSetpoints1(getSetpoints1(), getNodeAddress()));
        break;
    case READY:
        Serial.printf("Disabling driver...");
        setSetpoints1(Setpoints1{0, 0, 0, 0}); // DcOn = 0
        sendMessage(parseSetpoints1(getSetpoints1(), getNodeAddress()));
        break;
    case HV_ON:
        Serial.printf("SWITCH OFF HV circuit!");
        break;
    case LV_ON:
        Serial.printf("SWITCH OFF LV circuit!");
        break;
    default:
        break;
    }
}
