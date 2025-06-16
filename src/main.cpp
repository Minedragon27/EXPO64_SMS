#include <Arduino.h>
#include <MHZ19.h>
#include <hmi.h>
#include <CHT8305.h>
#include <Arduino_FreeRTOS.h>

// testing variables----------------------------------
short R[8] = {5, 6, 7, 8, 9, 10, 11, 12};
int LED[4] = {5, 6, 7, 12};

const short dataLen = 8 * 60; // how many data snapshots are kept, 10000 min=1 week 200kB
float dataLog[dataLen][5];    // 2d array to store the values
unsigned long timeOfLastLog = 0;
short posData = 0; // keeps track of which row of the array is the current one
//-----------------------------------------------------

// objects --------------------------------------------
CHT8305 tempHumSensor(0x40); // temperature and humidity sensor
HMI hmi1 = HMI(1, 2, 3, 4, R, LED, gfx); 
Arduino_DataBus *bus = new Arduino_SWSPI(TFT_DC, TFT_CS, TFT_SCK, TFT_MOSI, -1);
Arduino_GFX *gfx = new Arduino_ST7735(bus, TFT_RST, 1 /* rotation */, false /* IPS */); // objects used for LCD screen
MHZ19 sensorCO2(&Serial1); //to use: currentCO2=sensorCO2.getCO2(); can also use sensorCO2.getTemperature(); and senosrCO2.getAccuracy(); https://github.com/strange-v/MHZ19/blob/master/examples/hw_get_values/hw_get_values.ino
//-----------------------------------------------------

// global variables -----------------------------------
volatile float currentTemperature = 20;
volatile float targetTemperature = 20; // default value
volatile float currentHumidity = 20;
volatile float targetHumidity = 20;
volatile int currentCO2 = 20;
volatile int targetCO2 = 20;
volatile float currentLight = 20; // what
volatile float targetLight = 20;
//-----------------------------------------------------

// Mutex for protecting parameters data access --------
SemaphoreHandle_t xSensorDataMutex;

// tasks and functions ---------------------------------

void UpdateTargetParameters()
{
    byte buttons = hmi1.readButtons();
    bool firstLoopIteration = true; // used by loops to check if they have just started
    float shownTarget = 0;          // initialize shownTarget variable, changed when rotating the knob

    float scalingTemperature = 0.5; // how much should it change per step of the knob
    float scalingHumidity = 0.5;
    float scalingCO2 = 0.5;
    float scalingLight = 0.5;

    bool knobA, knobB = 0; // created just to find the current state
    hmi1.readRotaryEncoder(&knobA, &knobB);
    byte encState = 2 * knobA + knobB; // 0: 00; 1: 01; 2: 10; 3: 11; S: AB

    while (hmi1.readBit(buttons, 0)) // go to temperature menu
    {
        if (firstLoopIteration)
        {
            shownTarget = targetTemperature;
            firstLoopIteration = false;
        }

        byte increment = hmi1.incrementRotation(&encState);
        shownTarget = increment * scalingTemperature;
        buttons = hmi1.readButtons();
        if (hmi1.readBit(buttons, 6))
            targetTemperature = shownTarget; // confirm target value
        if (hmi1.readBit(buttons, 5))
            shownTarget = targetTemperature;                 // reset target value
        hmi1.writeToScreen(currentTemperature, shownTarget); // show values
        if (hmi1.readBit(buttons, 7))
            return; // exit; does not save automatically
    }
    while (hmi1.readBit(buttons, 1)) // go to humidity menu
    {
        if (firstLoopIteration)
        {
            shownTarget = targetHumidity;
            firstLoopIteration = false;
        }

        byte increment = hmi1.incrementRotation(&encState);
        shownTarget = increment * scalingHumidity;
        buttons = hmi1.readButtons();
        if (hmi1.readBit(buttons, 6))
            targetHumidity = shownTarget; // confirm target value
        if (hmi1.readBit(buttons, 5))
            shownTarget = targetHumidity;                 // reset target value
        hmi1.writeToScreen(currentHumidity, shownTarget); // show values
        if (hmi1.readBit(buttons, 7))
            return; // exit; does not save automatically;
    }
    while (hmi1.readBit(buttons, 2)) // go to CO2 menu
    {
        if (firstLoopIteration)
        {
            shownTarget = targetCO2;
            firstLoopIteration = false;
        }

        byte increment = hmi1.incrementRotation(&encState);
        shownTarget = increment * scalingCO2;
        buttons = hmi1.readButtons();
        if (hmi1.readBit(buttons, 6))
            targetCO2 = shownTarget; // confirm target value
        if (hmi1.readBit(buttons, 5))
            shownTarget = targetCO2;                 // reset target value
        hmi1.writeToScreen(currentCO2, shownTarget); // show values
        if (hmi1.readBit(buttons, 7))
            return; // exit; does not save automatically
    }
    while (hmi1.readBit(buttons, 3)) // go to light menu
    {
        if (firstLoopIteration)
        {
            shownTarget = targetLight;
            firstLoopIteration = false;
        }

        byte increment = hmi1.incrementRotation(&encState);
        shownTarget = increment * scalingLight;
        buttons = hmi1.readButtons();
        if (hmi1.readBit(buttons, 6))
            targetLight = shownTarget; // confirm target value
        if (hmi1.readBit(buttons, 5))
            shownTarget = targetLight;                 // reset target value
        hmi1.writeToScreen(currentLight, shownTarget); // show values
        if (hmi1.readBit(buttons, 7))
            return; // exit; does not save automatically
    }
}
void LogData()
{

    unsigned long time = millis(); // take the current time
    if (timeOfLastLog > time)
        timeOfLastLog = 0;                // if millis() overflows, reset the timeOfLastLog as well
    if (time - timeOfLastLog > 1000 * 60) // check if 1 minute has passed since last call
    {
        timeOfLastLog = time;
        if (posData == dataLen - 1) // array has been filled and must be shifted
        {
            for (int i = 0; i < dataLen - 1; i++) // shifted moves all data snapshots down by 1 row in the array, skips the last one since it is overwritten later
            {
                dataLog[i][0] = dataLog[i + 1][0];
                dataLog[i][1] = dataLog[i + 1][1];
                dataLog[i][2] = dataLog[i + 1][2];
                dataLog[i][3] = dataLog[i + 1][3];
                dataLog[i][4] = dataLog[i + 1][4];
            }
        }
        else
            posData++;                     // else increment to next pos
        dataLog[posData][0] = time / 1000; // save current values to arrays, time is saved in seconds
        dataLog[posData][1] = currentTemperature;
        dataLog[posData][2] = currentHumidity;
        dataLog[posData][3] = currentCO2;
        dataLog[posData][4] = currentLight;
    }
}
void dataOutput()
{
    int incomingMessage = 0;
    if (Serial.available() == 0)
        return; // no message received
    else
    {
        incomingMessage = Serial.read();
    }
    if (incomingMessage == int('p')) // trigger condition is receiving a 'p' on serial
    {
        for (int i = 0; i <= posData; i++)
        {
            Serial.println("----------------");
            Serial.println("Time: " + String(dataLog[i][0]));
            Serial.println("Temperature: " + String(dataLog[i][1]));
            Serial.println("Humidity: " + String(dataLog[i][2]));
            Serial.println("CO2: " + String(dataLog[i][3]));
            Serial.println("Light: " + String(dataLog[i][4]));
        }
    }
}
void getSensorData(void *parameters)
{
    for (;;)
    {
        // acquire the mutex before writing to shared variables
        if (xSemaphoreTake(xSensorDataMutex, portMAX_DELAY) == pdTRUE)
        {
            tempHumSensor.read();
            currentHumidity = tempHumSensor.getHumidity();
            //Serial.print("humidity (%): ");
            //Serial.println(tempHumSensor.getHumidity());
            currentTemperature = tempHumSensor.getTemperature();
            //Serial.print("temperature (℃): ");
            //Serial.println(currentTemperature);
            // currentCO2 ?

            xSemaphoreGive(xSensorDataMutex); // Release the mutex after writing
        }

        vTaskDelay(1000 / portTICK_PERIOD_MS); // Update every 1 second
    }
}
//------------------------------------------------------

void setup()
{
    // general setup --------------------------------------------
    Serial.begin(115200);
    xSensorDataMutex = xSemaphoreCreateMutex();
    if (xSensorDataMutex == NULL)
    {
        Serial.println("Error: Failed to create sensor data mutex.");
        // Handle error, maybe halt execution or retry
    }

    // setup for screen -----------------------------------------
    gfx->begin(); // initialize screen
    gfx->fillScreen(BLACK);
    gfx->setTextColor(RGB565(150, 150, 150));
    hmi1.drawText("test", 2, 50);

    // setup for temp_hum sensor ---------------------------------
    Wire.begin();
    Wire.setClock(400000);
    tempHumSensor.begin();

    // all tasks creations ---------------------------------------

    xTaskCreate(
        getSensorData,
        "get tempHum sensor data",
        configMINIMAL_STACK_SIZE, // stack size, this is 512 bytes
        NULL,                     // task parameters
        1,                        // task priority
        NULL                      // task handle
    );

    // end of tasks -----------------------------------------------

    // starts scheduler and never leaves it!
    vTaskStartScheduler();
}

void loop()
{
    /* THE SECRET MUSHROOM SOCIETY WILL LIVE FOREVER

           ____
        _.-'78o `"`--._
    ,o888o.  .o888o,   ''-.
  ,88888P  `78888P..______.]
 /_..__..----""        __.'
 `-._       /""| _..-''
     "`-----\  `\
             |   ;.-""--..
             | ,8o.  o88. `.
             `;888P  `788P  :
       .o""-.|`-._         ./
      J88 _.-/    ";"-P----'
      `--'\`|     /  /
          | /     |  |
          \|     /   |akn
           `-----`---'


    */
}
