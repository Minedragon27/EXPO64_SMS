#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

// pins ----------------------------------------------
const int LED_PIN = 6; // is this the one?
//-----------------------------------------------------

// global variables -----------------------------------
const int LED_COUNT = 60;     // check this
const int BRIGHTNESS = 50;    //  (0-255)
const int FADE_DELAY_MS = 25; //
//-----------------------------------------------------

// objects --------------------------------------------
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRBW + NEO_KHZ800);
//-----------------------------------------------------

void setup()
{
    Serial.begin(115200);
    Serial.println("--- start setup ---");

    strip.begin();
    strip.show();                    // Turn OFF all pixels
    strip.setBrightness(BRIGHTNESS); // Set overall brightness
    Serial.println("--- setup done ---");
}

void loop()
{
    // this test should make the led strip brighten

    float currentLight = 10;
    float targetLight = 200;

    if (currentLight != targetLight)
    {
        if (currentLight < targetLight)
        {
            // Brighten
            for (int j = currentLight; j <= targetLight; j++)
            {
                for (uint16_t i = 0; i < strip.numPixels(); i++)
                {
                    strip.setPixelColor(i, j, j, j); // Set all channels to 'j' for grayscale
                }
                strip.show();
                delay(FADE_DELAY_MS);
                currentLight = j; // Update currentLight as we fade
            }
        }
        else
        {
            // Darken
            for (int j = currentLight; j >= targetLight; j--)
            {
                for (uint16_t i = 0; i < strip.numPixels(); i++)
                {
                    strip.setPixelColor(i, j, j, j); // Set all channels to 'j' for grayscale
                }
                strip.show();
                delay(FADE_DELAY_MS);
                currentLight = j; // Update currentLight as we fade
            }
        }
    }
}