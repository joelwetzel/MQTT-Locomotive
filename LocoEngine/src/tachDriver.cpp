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
    unsigned long currentMicros = micros();

    if (lastPulseMicros != 0)
    {
      lastPulseDelta = currentMicros - lastPulseMicros;
      receivedPulse = true;
    }

    lastPulseMicros = currentMicros;
  }
}


void TachDriver::Setup()
{
    hasEverReceivedPulse = false;

    measurementState = 0;

    lastPulseMicros = 0;
    lastPulseDelta = 0;
    receivedPulse = false;

    rpm = 0.0;
    counterSinceReceivedPulse = 0;
    numAnomalousReadings = 0;

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
            lastPulseDelta = micros() - lastPulseMicros + counterSinceReceivedPulse * 150;
        }
    }

    float unfilteredRpm = 60.0 * 1000.0 * 1000.0 / (float)lastPulseDelta / NUM_MAGNETS;

    float rpmDelta = fabs(unfilteredRpm - rpm);
    if (rpmDelta > 200
        && numAnomalousReadings < 10)
    {
      // Throw out anomalous readings
      numAnomalousReadings++;
    }
    else
    {
      numAnomalousReadings = 0;

      // Use the new measurement, by applying a low pass filter
      rpm = rpm + LOW_PASS_FILTER_ALPHA * (unfilteredRpm - rpm);
    }

    if (rpm < 0.0 || isnan(rpm) || !hasEverReceivedPulse || counterSinceReceivedPulse > 33)
    {
        rpm = 0.0;
    }

    attachInterrupt(digitalPinToInterrupt(TACH_PIN), isr, CHANGE);
}