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

class PIDcontroller {
public:
    // Constructor with default parameters
    PIDcontroller(float kp, float ki, float kd,
                  float setpoint, float integralMin = 0.0,
                  float integralMax = 100.0, float outputMin = 0.0,
                  float outputMax = 100.0);

    // Set PID gains
    void setGains(float kp, float ki, float kd);

    // Set desired setpoint
    void setSetpoint(float setpoint);

    // Set integral windup limits
    void setIntegralLimits(float min, float max);

    // Set output limits (e.g., duty cycle 0-100%)
    void setOutputLimits(float min, float max);

    // Reset integral and previous error 
    void reset();

    // Compute PID output given the current measured value
    // to be called in the control task i think
    float calculatePID(float measuredValue){
    // Calculate error
    float error = pid->setpoint - measuredValue;

    // Proportional term
    float proportional = pid->kp * error;

    // Integral term
    pid->integral += pid->ki * error; // Accumulate the integral
    if (pid->integral > 100) pid->integral = 100; // Anti-windup
    if (pid->integral < 5) pid->integral = 5 ;

    // Derivative term
    float derivative = pid->kd * (error - pid->previousError);
    pid->previousError = error; // Store current error for next derivative calculation

    // Calculate total output
    float output = proportional + pid->integral + derivative;

    // Limit output to a valid range (#todo define this, might have to depend on each parementer)
    if (output > 100) output = 100;
    if (output < 0) output = 0;

    return output; // Return the adjustment value
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

