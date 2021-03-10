#include "tachDriver.h"


TachDriver::TachDriver()
{

}

TachDriver* theInstance = NULL;

ICACHE_RAM_ATTR
void isr()
{
    theInstance->HandleInterrupt();
}

ICACHE_RAM_ATTR
void TachDriver::HandleInterrupt()
{
  int reading = digitalRead(TACH_PIN);

  // Ignore dupe readings
  if (reading == measurementState)
    return;

  // Persist the reading as the state.
  measurementState = reading;

  // We're debouncing on both rise and fall, but we only want to increment the counter on rise.
  if (!measurementState)
  {
    long currentMillis = millis();

    if (lastPulseMillis != 0)
    {
      lastPulseDelta = currentMillis - lastPulseMillis;
      receivedPulse = true;
    }

    lastPulseMillis = currentMillis;
  }
}


void TachDriver::Setup()
{
    hasEverReceivedPulse = false;

    measurementState = 0;

    lastPulseMillis = 0;
    lastPulseDelta = 0;
    receivedPulse = false;

    rpm = 0.0;
    counterSinceReceivedPulse = 0;

    pinMode(TACH_PIN, INPUT);

    theInstance = this;
    attachInterrupt(digitalPinToInterrupt(TACH_PIN), isr, CHANGE);
}


float TachDriver::GetWheelRpm()
{
    return rpm;
}


void TachDriver::ProcessStep()
{
    detachInterrupt(digitalPinToInterrupt(TACH_PIN));

    if (receivedPulse)
    {
        receivedPulse = false;
        hasEverReceivedPulse = true;
        counterSinceReceivedPulse = 0;
    }
    else
    {
        counterSinceReceivedPulse++;

        if (counterSinceReceivedPulse > 10)
        {
            lastPulseDelta = millis() - lastPulseMillis + counterSinceReceivedPulse * 150;
        }
    }

    float unfilteredRpm = 60.0 * 1000.0 / (float)lastPulseDelta / NUM_MAGNETS;

    rpm = rpm + LOW_PASS_FILTER_ALPHA * (unfilteredRpm - rpm);

    if (rpm < 0.0 || isnan(rpm) || !hasEverReceivedPulse || counterSinceReceivedPulse > 33)
    {
        rpm = 0.0;
    }

    attachInterrupt(digitalPinToInterrupt(TACH_PIN), isr, CHANGE);
}