#include <Arduino.h>

#include <hmi.h>


//testing variables
short R[8]={5,6,7,8,9,10,11,12};
int LED[4]={13,14,15,16};
TFT TFTScreen;
HMI hmi1(1,2,3,4,R,LED,TFTScreen);

byte currentTemperature;
byte targetTemperature;
byte currentHumidity;
byte targetHumidity;
byte currentCO2;
byte targetCO2;
byte currentLight;
byte targetLight;

void UpdateTargetParameters()
{
    byte buttons=hmi1.readButtons();
    bool firstLoopIteration=true;//used by loops to check if they have just started
    byte shownTarget=0; //initialize shownTarget variable, changed when rotating the knob

    float scalingTemperature=0.5;//how much should it change per step of the knob
    float scalingHumidity=0.5;
    float scalingCO2=0.5;
    float scalingLight=0.5;

    bool knobA,knobB=0;//created just to find the current state
    hmi1.readRotaryEncoder(&knobA,&knobB);
    byte encState=2*knobA+knobB;//0: 00; 1: 01; 2: 10; 3: 11; S: AB

    while(hmi1.readBit(buttons,0))//go to temperature menu
    {
        if(firstLoopIteration)
        {
            shownTarget=targetTemperature;
            firstLoopIteration=false;
        }

        byte increment =hmi1.incrementRotation(&encState);
        shownTarget=increment*scalingTemperature;
        buttons=hmi1.readButtons();
        if (hmi1.readBit(buttons,6)) targetTemperature=shownTarget;//confirm target value
        if (hmi1.readBit(buttons,5)) shownTarget=targetTemperature;//reset target value
        hmi1.writeToScreen(currentTemperature,shownTarget);//show values
        if (hmi1.readBit(buttons,7)) return;//exit; does not save automatically
    }
    while(hmi1.readBit(buttons,1))//go to humidity menu
    {
        if(firstLoopIteration)
        {
            shownTarget=targetHumidity;
            firstLoopIteration=false;
        }

        byte increment =hmi1.incrementRotation(&encState);
        shownTarget=increment*scalingHumidity;
        buttons=hmi1.readButtons();
        if (hmi1.readBit(buttons,6)) targetHumidity=shownTarget;//confirm target value
        if (hmi1.readBit(buttons,5)) shownTarget=targetHumidity;//reset target value
        hmi1.writeToScreen(currentHumidity,shownTarget);//show values
        if (hmi1.readBit(buttons,7)) return;//exit; does not save automatically;
    }
    while(hmi1.readBit(buttons,2))//go to CO2 menu
    {   
        if(firstLoopIteration)
        {
            shownTarget=targetCO2;
            firstLoopIteration=false;
        }

        byte increment =hmi1.incrementRotation(&encState);
        shownTarget=increment*scalingCO2;
        buttons=hmi1.readButtons();
        if (hmi1.readBit(buttons,6)) targetCO2=shownTarget;//confirm target value
        if (hmi1.readBit(buttons,5)) shownTarget=targetCO2;//reset target value
        hmi1.writeToScreen(currentCO2,shownTarget);//show values
        if (hmi1.readBit(buttons,7)) return;//exit; does not save automatically
    }
    while(hmi1.readBit(buttons,3))//go to light menu
    {
        if(firstLoopIteration)
        {
            shownTarget=targetLight;
            firstLoopIteration=false;
        }

        byte increment =hmi1.incrementRotation(&encState);
        shownTarget=increment*scalingLight;
        buttons=hmi1.readButtons();
        if (hmi1.readBit(buttons,6)) targetLight=shownTarget;//confirm target value
        if (hmi1.readBit(buttons,5)) shownTarget=targetLight;//reset target value
        hmi1.writeToScreen(currentLight,shownTarget);//show values
        if (hmi1.readBit(buttons,7)) return;//exit; does not save automatically
    }


void setup() {
  // put your setup code here, to run once:
    //

}   

void loop() {
  // put your main code here, to run repeatedly:
}

