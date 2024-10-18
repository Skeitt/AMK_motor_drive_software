#include "Inverter.h"

Inverter::Inverter(const uint16_t node_address) { this->node_address = node_address; };

byte Inverter::get_node_address() const { return this->node_address; };

ActualValues1 Inverter::get_actual_values_1() const { return *this->actual_values_1; };

ActualValues2 Inverter::get_actual_values_2() const { return *this->actual_values_2; };

Setpoints1 Inverter::get_setpoints_1() const { return *this->setpoints_1; };

void Inverter::set_actual_values_1(ActualValues1 actual_values_1) { this->actual_values_1 = &actual_values_1; };

void Inverter::set_actual_values_2(ActualValues2 actual_values_2) { this->actual_values_2 = &actual_values_2; };

void Inverter::set_setpoints_1(Setpoints1 setpoints_1) { this->setpoints_1 = &setpoints_1; };

void Inverter::set_target_parameters(uint16_t target_velocity, int16_t torque_limit_positive, int16_t torque_limit_negative)
{
    this->setpoints_1->target_velocity = target_velocity;
    this->setpoints_1->torque_limit_positive = torque_limit_positive;
    this->setpoints_1->torque_limit_negative = torque_limit_negative;
};

void Inverter::check_status()
{
    uint16_t status = this->actual_values_1->status;

    if ((status & (bSystemReady | bDerating)) == (bSystemReady | bDerating))
    {
        this->state = LV_ON;
    }
    if ((status & bSystemReady) == bSystemReady)
    {
        this->state = HV_ON;
    }
    if ((status & (bSystemReady | bDcOn | bQuitDcOn)) == (bSystemReady | bDcOn | bQuitDcOn))
    {
        if (this->state == READY)
            this->state = SECURITY_CHECK_DONE;
        else if (this->state == SECURITY_CHECK_DONE)
            this->state = DRIVER_ACTIVE;
        else
            this->state = READY;
    }
    if ((status & (bSystemReady | bDcOn | bQuitDcOn | bInverterOn | bQuitInverterOn)) == (bSystemReady | bDcOn | bQuitDcOn | bInverterOn | bQuitInverterOn))
    {
        if (this->state == DRIVER_ACTIVE)
            this->state = CONTROLLER_ACTIVE;
        else
            this->state = SENDING_SETPOINTS;
    }
    if ((status & (bError | bSystemReady)) == (bError | bSystemReady))
    {
        this->state = ERROR;
    }
    if ((status & (bError | bSystemReady | bInverterOn)) == (bError | bSystemReady | bInverterOn))
    {
        this->state = RESET_ERROR;
    }
}

void Inverter::activate()
{
    check_status();

    switch (this->state)
    {
    case LV_ON:
        Serial.printf("LV circuit enabled");
        break;
    case HV_ON:
        Serial.printf("HV circuit enabled");
        this->set_setpoints_1(Setpoints1{cbDcOn, 0, 0, 0});
        break;
    case READY:
        // invio di torque limit pos e neg = 0
        Serial.printf("Sending torque limit pos and neg = 0");
        this->set_setpoints_1(Setpoints1{cbDcOn, 0, 0, 0});
        this->state = SECURITY_CHECK_DONE;
        break;
    case SECURITY_CHECK_DONE:
        // invio di bEnable = 1
        Serial.printf("Sending driver enable = 1");
        this->set_setpoints_1(Setpoints1{cbDcOn | cbEnable, 0, 0, 0});
        this->state = DRIVER_ACTIVE;
        break;
    case DRIVER_ACTIVE:
        // invio di bInverterOn = 1
        Serial.printf("Sending inverter on = 1");
        this->set_setpoints_1(Setpoints1{cbDcOn | cbEnable | cbInverterOn, 0, 0, 0});
        // nel caso in cui la procedura di ready sia stata saltata, ricomincia da READY
        this->state = READY;
        break;
    case CONTROLLER_ACTIVE:
        this->set_setpoints_1(Setpoints1{cbDcOn | cbEnable | cbInverterOn, 500, 100, 0});
        break;
    case SENDING_SETPOINTS:
        break;
    case ERROR:
        this->set_setpoints_1(Setpoints1{cbDcOn | cbEnable, 0, 0, 0});
        break;
    case RESET_ERROR:
        this->set_setpoints_1(Setpoints1{cbDcOn | cbEnable | cbErrorReset, 0, 0, 0});
        break;
    default:
        break;
    }
}

void Inverter::deactivate()
{
    // Implementare la disattivazione dell'inverter
    // inviando i setpoint corretti
    check_status();
    switch (this->state)
    {
    case SENDING_SETPOINTS:
        Serial.printf("switching off...");
        this->set_setpoints_1(Setpoints1{cbDcOn | cbEnable | cbInverterOn, 0, 0, 0}); // setpoint nulli
        this->state = CONTROLLER_ACTIVE;
        break;
    case CONTROLLER_ACTIVE:
        Serial.printf("Disabling controller...");
        this->set_setpoints_1(Setpoints1{cbDcOn | cbEnable, 0, 0, 0}); // InverterOn = 0
        this->state = DRIVER_ACTIVE;
        break;
    case DRIVER_ACTIVE:
        Serial.printf("Disabling driver...");
        this->set_setpoints_1(Setpoints1{cbDcOn, 0, 0, 0}); // Enable = 0
        this->state = READY;
        break;

    case READY:
        Serial.printf("Disabling driver...");
        this->set_setpoints_1(Setpoints1{0, 0, 0, 0}); // DcOn = 0
        this->state = HV_ON;
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
