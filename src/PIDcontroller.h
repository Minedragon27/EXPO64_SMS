#ifndef PIDCONTROLLER_H
#define PIDCONTROLLER_H

/**
 * @class PIDcontroller
 * @brief Implements a PID (Proportional-Integral-Derivative) controller.
 *
 * This class provides a way to calculate a control output based on
 * the PID algorithm. It maintains internal state for integral and previous error
 * to compute the control signal for a given measured value.
 */

#include <Arduino.h>

class PIDcontroller
{
public:
    // Constructor declaration and inline definition
    PIDcontroller(float kp, float ki, float kd,
                  float setpoint, float integralMin = 0.0,
                  float integralMax = 100.0, float outputMin = 0.0,
                  float outputMax = 100.0)
        : _kp(kp),
          _ki(ki),
          _kd(kd),
          _setpoint(setpoint),
          _integral(0.0f),
          _previousError(0.0f),
          _integralMin(integralMin),
          _integralMax(integralMax),
          _outputMin(outputMin),
          _outputMax(outputMax)
    {
    }

    // Set PID gains
    void setGains(float kp, float ki, float kd)
    {
        _kp = kp;
        _ki = ki;
        _kd = kd;
    }

    // Set desired setpoint
    void setSetpoint(float setpoint)
    {
        _setpoint = setpoint;
    }

    // Set integral windup limits
    void setIntegralLimits(float min, float max)
    {
        _integralMin = min;
        _integralMax = max;
    }

    // Set output limits (e.g., duty cycle 0-100%)
    void setOutputLimits(float min, float max)
    {
        _outputMin = min;
        _outputMax = max;
    }
    // Reset integral and previous error
    void reset()
    {
        _integral = 0.0f;
        _previousError = 0.0f;
    }

    // Compute PID output given the current measured value
    // to be called in the control task i think
    float calculatePID(float measuredValue)
    {
        float error = _setpoint - measuredValue;

        float proportional = _kp * error;

        _integral += _ki * error; // Accumulate the integral

        // Anti-windup clamping
        if (_integral > _integralMax)
            _integral = _integralMax;
        if (_integral < _integralMin)
            _integral = _integralMin;

        float derivative = _kd * (error - _previousError);
        _previousError = error; // Store current error for next derivative calculation

        // Calculate total output
        float output = proportional + _integral + derivative;

        // Limit output to a valid range
        if (output > _outputMax)
            output = _outputMax;
        if (output < _outputMin)
            output = _outputMin;

        return output; // return the adjustment value
    };

private:
    float _kp;
    float _ki;
    float _kd;

    float _setpoint;

    float _integral;
    float _previousError;

    float _integralMin;
    float _integralMax;

    float _outputMin;
    float _outputMax;
};

#endif // PIDCONTROLLER_H
