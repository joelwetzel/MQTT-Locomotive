#ifndef ICONTROLMODEL_H
#define ICONTROLMODEL_H

#define SPEEDPERCENT_TO_WHEEL_RPMS      0.325
#define WHEEL_RPM_TO_MPH                2.0


class IControlModel
{
public:
    IControlModel() { }
    virtual ~IControlModel() { }

    virtual int GetControlModelId() = 0;

    virtual float GetSpeedMph() = 0;
    virtual float GetSpeedPercent() = 0;
    virtual float GetEngineRpms() = 0;
    virtual float GetEnginePercent() = 0;
    virtual float GetSmokePercent() = 0;

    virtual void SetEngineOn(bool on) = 0;
    virtual bool GetEngineOn() = 0;
    virtual void SetThrottle(float throttle) = 0;
    virtual float GetThrottle() = 0;
    virtual void SetBrake(float brake) = 0;
    virtual float GetBrake() = 0;
    virtual void SetReverser(int direction) = 0;
    virtual int GetReverser() = 0;

    virtual void ProcessStep() = 0;
};

#endif