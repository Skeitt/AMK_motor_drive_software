#include "Inverter.hpp"
#include "CANCommunication.hpp"
#include <Arduino.h>

#define DELAY 10

Inverter::Inverter(const uint16_t t_nodeAddress)
{
    m_nodeAddress = t_nodeAddress;
    m_actualValues1 = ActualValues1();
    m_actualValues2 = ActualValues2();
    m_setpoints1 = Setpoints1();
    m_state = IDLE;
};

uint16_t Inverter::getNodeAddress() const { return m_nodeAddress; };

InverterState Inverter::getState() const { return m_state; };

ActualValues1 Inverter::getActualValues1() const { return m_actualValues1; };

ActualValues2 Inverter::getActualValues2() const { return m_actualValues2; };

Setpoints1 Inverter::getSetpoints1() const { return m_setpoints1; };

void Inverter::setActualValues1(ActualValues1 t_actualValues1) { m_actualValues1 = t_actualValues1; };

void Inverter::setActualValues2(ActualValues2 t_actualValues2) { m_actualValues2 = t_actualValues2; };

void Inverter::setSetpoints1(Setpoints1 t_setpoints1) { m_setpoints1 = t_setpoints1; };

void Inverter::checkStatus()
{
    uint16_t status = m_actualValues1.status;

    if(status == (bSystemReady | bDerating)) //0x8100
    {
        m_state = LV_ON;
        Serial.printf("LV circuit enabled\n");
    }
    else if ((status == (bSystemReady)) || (status == (bSystemReady | bDcOn))) //0x0100 o 0x1100
    {
        m_state = HV_ON;
        Serial.printf("HV circuit enabled\n");
    }
    else if ((status == (bSystemReady | bDcOn | bQuitDcOn)) || (status == (bSystemReady | bDcOn | bQuitDcOn | bInverterOn))) //0x1900 o 0x5900
    {
        m_state = READY;
        Serial.printf("DcOn enabled\n");
    }
    else if (status == (bSystemReady | bDcOn | bQuitDcOn | bInverterOn | bQuitInverterOn)) //0x7900
    {
        m_state = CONTROLLER_ACTIVE;
        Serial.printf("Controller enabled\n");
    }
    else if (status & bError) //0xY200
    {
        m_state = ERROR;
        Serial.printf("Error detected\n");
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
    std::pair<const char *, const char *> error;

    switch (m_state)
    {
    case IDLE:
        Serial.printf("Switch on LV circuit...\n");
        setSetpoints1(Setpoints1{0, 0, 0, 0});
        sendMessage(parseSetpoints1(getSetpoints1(), getNodeAddress()));
        delay(DELAY);
        break;
    case LV_ON:
        Serial.printf("Switch on HV circuit...\n");
        setSetpoints1(Setpoints1{0, 0, 0, 0});
        sendMessage(parseSetpoints1(getSetpoints1(), getNodeAddress()));
        delay(DELAY);
        break;
    case HV_ON:
        Serial.printf("Enabling HV activation level...\n");
        setSetpoints1(Setpoints1{cbDcOn, 0, 0, 0});
        sendMessage(parseSetpoints1(getSetpoints1(), getNodeAddress()));
        delay(DELAY);
        break;
    case READY:
        Serial.printf("Performing security check (torque limit pos and neg = 0)\n");
        setSetpoints1(Setpoints1{cbDcOn, 0, 0, 0});
        sendMessage(parseSetpoints1(getSetpoints1(), getNodeAddress()));
        delay(DELAY);

        Serial.printf("Enabling driver...\n");
        setSetpoints1(Setpoints1{cbDcOn | cbEnable, 0, 0, 0});
        sendMessage(parseSetpoints1(getSetpoints1(), getNodeAddress()));
        delay(DELAY);

        Serial.printf("Enabling controller...\n");
        setSetpoints1(Setpoints1{cbDcOn | cbEnable | cbInverterOn, 0, 0, 0});
        sendMessage(parseSetpoints1(getSetpoints1(), getNodeAddress()));
        delay(DELAY);
        break;
    case CONTROLLER_ACTIVE:
        Serial.printf("Sending setpoints...\n");
        sendMessage(parseSetpoints1(getSetpoints1(), getNodeAddress()));
        delay(DELAY);
        break;
    case ERROR:
        Serial.printf("Disabling controller...\n");
        setSetpoints1(Setpoints1{0, 0, 0, 0});
        sendMessage(parseSetpoints1(getSetpoints1(), getNodeAddress()));
        delay(DELAY);

        error = getError(m_actualValues2.errorInfo);
        Serial.printf("Error code: %d (%s), Error class: %s\n",
                      m_actualValues2.errorInfo,
                      error.first,
                      error.second);

        // TODO: remove error based on error removal message field

        Serial.printf("Resetting error...\n");
        setSetpoints1(Setpoints1{cbErrorReset, 0, 0, 0});
        sendMessage(parseSetpoints1(getSetpoints1(), getNodeAddress()));
        delay(DELAY);

        setSetpoints1(Setpoints1{0, 0, 0, 0});
        sendMessage(parseSetpoints1(getSetpoints1(), getNodeAddress()));
        delay(DELAY);
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
        delay(DELAY);

        Serial.printf("Disabling controller...\n");
        setSetpoints1(Setpoints1{cbDcOn | cbEnable, 0, 0, 0});
        sendMessage(parseSetpoints1(getSetpoints1(), getNodeAddress()));
        delay(DELAY);

        Serial.printf("Disabling driver...\n");
        setSetpoints1(Setpoints1{cbDcOn, 0, 0, 0});
        sendMessage(parseSetpoints1(getSetpoints1(), getNodeAddress()));
        delay(DELAY);
        break;
    case READY:
        Serial.printf("Disabling HV activation level...\n");
        setSetpoints1(Setpoints1{0, 0, 0, 0});
        sendMessage(parseSetpoints1(getSetpoints1(), getNodeAddress()));
        delay(DELAY);
        break;
    case HV_ON:
        Serial.printf("Switch off HV circuit...\n");
        // TODO: send signal to battery pack to switch off HV
        setSetpoints1(Setpoints1{0, 0, 0, 0});
        sendMessage(parseSetpoints1(getSetpoints1(), getNodeAddress()));
        delay(DELAY);
        break;
    case LV_ON:
        Serial.printf("Switch off LV circuit...\n");
        // TODO: send signal to battery pack to switch off HV
        setSetpoints1(Setpoints1{0, 0, 0, 0});
        sendMessage(parseSetpoints1(getSetpoints1(), getNodeAddress()));
        delay(DELAY);
        break;
    case IDLE:
        setSetpoints1(Setpoints1{0, 0, 0, 0});
        sendMessage(parseSetpoints1(getSetpoints1(), getNodeAddress()));
        delay(DELAY);
        break;
    default:
        break;
    }
}
