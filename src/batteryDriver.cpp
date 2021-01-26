#include "batteryDriver.h"



BatteryDriver::BatteryDriver()
{

}


void BatteryDriver::Setup()
{
    pinMode(TRACK_VOLTAGE_PIN, INPUT);
}


float BatteryDriver::GetVoltage()
{
    return analogRead(TRACK_VOLTAGE_PIN) / 1024.0 * 18.5;
}