#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <Arduino_FreeRTOS.h>

// pins ----------------------------------------------
const int LED_PIN = 6; // is this the one?
//-----------------------------------------------------

// global variables -----------------------------------
volatile uint8_t currentLight = 20; // 0-255
volatile uint8_t targetLight = 255;

const int LED_COUNT = 60;     // check this
const int BRIGHTNESS = 50;    //  (0-255)
const int FADE_DELAY_MS = 25; //
//-----------------------------------------------------

// objects --------------------------------------------
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRBW + NEO_KHZ800);
//-----------------------------------------------------

// Mutex for protecting parameters data access --------
SemaphoreHandle_t xSensorDataMutex;

void actuateLight(void *parameters)
{
    // parameter 4 of 4: light
    // actuator: LED strip
    for (;;)
    {
        float current_light_val = 0;
        float target_light_val = 0;

        // Acquire mutex to read shared data
        if (xSemaphoreTake(xSensorDataMutex, portMAX_DELAY) == pdTRUE)
        {
            current_light_val = currentLight;
            target_light_val = targetLight;
            xSemaphoreGive(xSensorDataMutex); // Release mutex immediately after reading
        }

        if (current_light_val != target_light_val)
        {
            if (current_light_val < target_light_val)
            {
                // Brighten
                for (int j = current_light_val; j <= target_light_val; j++)
                {
                    for (uint16_t i = 0; i < strip.numPixels(); i++)
                    {
                        strip.setPixelColor(i, j, j, j); // Set all channels to 'j' for grayscale
                    }
                    strip.show();
                    vTaskDelay(pdMS_TO_TICKS(FADE_DELAY_MS));
                    current_light_val = j; // Update currentLight as we fade
                }
            }
            else
            {
                // Darken
                for (int j = current_light_val; j >= target_light_val; j--)
                {
                    for (uint16_t i = 0; i < strip.numPixels(); i++)
                    {
                        strip.setPixelColor(i, j, j, j); // Set all channels to 'j' for grayscale
                    }
                    strip.show();
                    vTaskDelay(pdMS_TO_TICKS(FADE_DELAY_MS));
                    current_light_val = j; // Update currentLight as we fade
                }
            }
        }
    }
}

void setup()
{
    // general setup --------------------------------------------
    Serial.begin(115200);
    xSensorDataMutex = xSemaphoreCreateMutex();
    if (xSensorDataMutex == NULL)
    {
        Serial.println("Error: Failed to create sensor data mutex.");
    }

    strip.begin();
    strip.show();                    // Turn OFF all pixels
    strip.setBrightness(BRIGHTNESS); // Set overall brightness
    Serial.println("--- setup done ---");

    xTaskCreate(
        actuateLight,
        "Turns on the LEDs.",
        configMINIMAL_STACK_SIZE, // stack size, this is 512 bytes
        NULL,                     // task parameters
        1,                        // task priority
        NULL                      // task handle
    );

    vTaskStartScheduler();
}

void loop()
{
    // this test should make the led strip brighten

}