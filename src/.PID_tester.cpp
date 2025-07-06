#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include "PIDcontroller.h"

/**
 * @file main_PID_tester.cpp
 * @brief Simulates the implementation of the PID controller class. The sensor and actuator are simulated because they will be programmed separately later, based on the sensor/actuator.
 */


// Create a PIDcontroller instance with some initial gains and setpoint
PIDcontroller tempController(2.0, 0.5, 1.0, 50.0, 0.0, 100.0f, 0.0f, 100.0f);

// Simulated sensor reading function
float readThermometer() {
    // return a dummy sensor value that changes over time
    static float val = 0.0f;
    val += 0.1f;
    if (val > 100.0f) val = 0.0f;
    return val;
}

// Simulated actuator control function 
void setPeltier(uint8_t temperature) {
    // print the fake temperature
    Serial.print("Setting temperature to: ");
    Serial.print(temperature);
    Serial.println("°C");
}

// FreeRTOS task that runs the PID control loop
void vPIDControlTask(void *pvParameters) {
    (void)pvParameters;

    const TickType_t xDelay = pdMS_TO_TICKS(100); // 100 ms control loop

    for (;;) {
        // Read the current sensor value
        float measuredValue = readThermometer
    ();

        // Compute the PID output
        float output = tempController.calculatePID(measuredValue);

        // Convert output to uint8_t duty cycle (0-100%)
        uint8_t temperature = static_cast<uint8_t>(output);

        // Apply the control output to the actuator
        setPeltier(temperature);

        // Delay for next control cycle
        vTaskDelay(xDelay);
    }
}

void setup() {
    Serial.begin(115200);
    while (!Serial) {
        ; // Wait for serial port to connect (for native USB)
    }

    Serial.println("Starting PID control task...");

    // Optionally set PID parameters here if you want to override constructor defaults
    // myPID.setGains(2.0f, 0.5f, 1.0f);
    // myPID.setSetpoint(50.0f);

    // Create the PID control task
    xTaskCreate(
        vPIDControlTask,    // Task function
        "Temperature PID Control",      // Task name
        256,                // Stack size (words, not bytes)
        NULL,               // Task parameters
        1,                  // Task priority
        NULL                // Task handle
    );

    // Start the FreeRTOS scheduler
    vTaskStartScheduler();

    // Should never reach here
    Serial.println("Scheduler failed to start!");
}

void loop() {
    // Empty. All work done in tasks.
}
