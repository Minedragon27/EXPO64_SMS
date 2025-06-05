#include <Arduino.h>
#include <hmi.h>


//testing variables
short R[8]={5,6,7,8,9,10,11,12};
int LED[4]={13,14,15,16};
TFT TFTScreen;
HMI hmi1(1,2,3,4,R,LED,TFTScreen);
byte currentTemperature,targetTemperature,currentHumidity,targetHumidity,currentCO2,targetCO2,currentLight,targetLight=25;
void UpdateTargetParameters()
{
    byte buttons=hmi1.readButtons();
    bool firstLoopIteration=true;//used by loops to check if they have just started
    byte shownTarget=0; //initialize shownTarget variable, changed when rotating the knob
    while(hmi1.readBit(buttons,0))//go to temperature menu
    {
        if(firstLoopIteration)
        {
            shownTarget=targetTemperature;
            firstLoopIteration=false;
        }

        buttons=hmi1.readButtons();
        hmi1.writeToScreen(currentTemperature,shownTarget);
        if (hmi1.readBit(buttons,7)) return;
    }
    while(hmi1.readBit(buttons,1))//go to humidity menu
    {
        if(firstLoopIteration)
        {
            firstLoopIteration=false;
        }
        buttons=hmi1.readButtons();

        if (hmi1.readBit(buttons,7)) return;
    }
    while(hmi1.readBit(buttons,2))//go to CO2 menu
    {   
        if(firstLoopIteration)
        {
            firstLoopIteration=false;
        }
        buttons=hmi1.readButtons();

        if (hmi1.readBit(buttons,7)) return;
    }
    while(hmi1.readBit(buttons,3))//go to light menu
    {
        if(firstLoopIteration)
        {
            firstLoopIteration=false;
        }
        buttons=hmi1.readButtons();

        if (hmi1.readBit(buttons,7)) return;
    }
}