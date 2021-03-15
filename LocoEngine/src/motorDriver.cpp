#include "Arduino.h"
#include "motorDriver.h"

MotorDriver::MotorDriver()
{

}


void MotorDriver::Setup()
{
  pinMode(MOTORFORWARD_PIN, OUTPUT);
  pinMode(MOTORBACKWARD_PIN, OUTPUT);

  // Set the PWM (which goes out to the motor controller) to a high frequency.
  // If not done, you'll get a 1KHz tone from the motor.
  analogWriteFreq(16384);
}


// percent is from -100.0 to 100.0
void MotorDriver::SetMotorSpeed(float percent)
{
  float cappedPercent = percent;
  if (cappedPercent < -100.0)
  {
    cappedPercent = -100.0;
  }
  else if (cappedPercent > 100.0)
  {
    cappedPercent = 100.0;
  }

  if (cappedPercent < 0.05 && cappedPercent > -0.05) {
    digitalWrite(MOTORFORWARD_PIN, LOW);
    digitalWrite(MOTORBACKWARD_PIN, LOW);
 }
  else {
    if (cappedPercent > 0.0)
    {
      analogWrite(MOTORFORWARD_PIN, (MIN_SPEED_PWM + fabs(cappedPercent) / 100.0 * (MAX_SPEED_PWM - MIN_SPEED_PWM)));
      digitalWrite(MOTORBACKWARD_PIN, LOW);
    }
    else
    {
      digitalWrite(MOTORFORWARD_PIN, LOW);
      analogWrite(MOTORBACKWARD_PIN, (MIN_SPEED_PWM + fabs(cappedPercent) / 100.0 * (MAX_SPEED_PWM - MIN_SPEED_PWM)));
    }
  }
}