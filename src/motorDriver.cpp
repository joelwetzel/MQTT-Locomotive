#include "Arduino.h"
#include "motorDriver.h"

MotorDriver::MotorDriver()
{

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
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    analogWrite(ENA, 0);
  }
  else {
    if (cappedPercent > 0.0)
    {
      digitalWrite(IN1, HIGH);
      digitalWrite(IN2, LOW);
    }
    else
    {
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, HIGH);
    }
    analogWrite(ENA, (MIN_SPEED_PWM + fabs(cappedPercent) / 100.0 * (MAX_SPEED_PWM - MIN_SPEED_PWM)));
  }
}