#include <Arduino.h>
#include <TFT.h>
#include <SPI.h>

class HMI
{
    enum parameter {none=0,temperature, humidity, co2, light};
    public: parameter currentParameter=none;

    private: TFT &screen;
    private: short resistances[8];//resistances for the button circuit
    private: int pinA,pinB,pinC,pinButtons;
    private: int pinLED[4];//led pins


    public:

    HMI(int A,int B, int C, int Buttons, short R[8], int LED[4],TFT &TFTscreen): screen(TFTscreen)
    {
        pinA=A;//rotary enc A
        pinMode(A,INPUT);
        pinB=B;//rotary enc B
        pinMode(B,INPUT);
        pinC=C;//rotary enc Button
        pinMode(C,INPUT);
        pinButtons=Buttons;//analog pin for other buttons
        pinMode(Buttons,INPUT);
        memcpy(resistances, R, sizeof(R));//resistances in Ohms, code copies R into resistances

        memcpy(pinLED,LED,sizeof(LED));
        screen=TFTscreen;
    }

    void readRotaryEncoder(bool* ptrA,bool* ptrB)
    {
        *ptrA=digitalRead(pinA);//writes value of A to address
        *ptrB=digitalRead(pinB);//writes value of B to address
    }

    byte incrementRotation(byte* ptrEncState)
    {
        int encState=*ptrEncState;
        bool knobA,knobB=0;
        readRotaryEncoder(&knobA,&knobB);
        byte increment=0;
        switch (encState)
        {
        case 0://state 0            
            switch (knobA*2+knobB)
            {
            case 2:
                increment++;
                
                break;
            case 1:
                increment--;
                break;
            case 0:
                break;
            default://error
                break;
            }          
            break;
        case 1:
            switch (knobA*2+knobB)
            {
            case 0:
                increment++;
                break;
            case 3:
                increment--;
                break;
            case 1:
                break;
            default://error
                break;
            }          
            break;
        case 2:
            switch (knobA*2+knobB)
            {
            case 3:
                increment++;
                break;
            case 0:
                increment--;
                break;
            case 1:
                break;
            default://error
                break;
            }          
            break;
        case 3:
            switch (knobA*2+knobB)
            {
            case 1:
                increment++;
                break;
            case 2:
                increment--;
                break;
            case 3:
                break;
            default://error
                break;
            }          
            break;    
        default://error
            break;
        }
        *ptrEncState=2*knobA+knobB;
        return increment;
    }

    byte readButtons()
    {
        byte output=0;
        int buttons=analogRead(pinButtons);
        writeBit(output,0,digitalRead(pinC));
        float V=0.0049*analogRead(pinButtons);//convert analog signal to voltage
        if(V<0.05) return output;
        for(byte i=1;i<8;i++)
        {
            if(resistances[0]/(resistances[0]+resistances[i])*5<1.05*V) //checks if the measured voltage is more than the expected voltage for this button
            {
                writeBit(output,i-1,1);//if the voltage was more, then the previous button is pressed
                break;
            }
        }
        return output;
    }
    
    void setLEDs(byte LEDs)
    {
        for(int i=0;i<4;i++) digitalWrite(pinLED[i],readBit(LEDs,i));//writes each bit to its corresponding LED pin
    }

    void writeToScreen(byte currentValue, byte targetValue)
    {   
        String unit="";
        switch (currentParameter)//chooses the unit to be displayed
        {
        case 1:
            unit="C";
            break;
        case 2:
            unit="%";
            break;
        case 3:
            unit="ppm";
            break;
        case 4:
            unit="lux";
            break;    
        default:
            break;
        }


        drawText("Current:",3,5);
        drawText(String(currentValue)+unit,2,10+3*8);
        drawText("Target:",3,screen.height()/2+5);
        drawText(String(targetValue)+unit,2,screen.height()/2+5+10+3*8);
    }

    bool readBit(byte variable, int position) 
    {
        return (variable >> position) & 1;
    }
    void writeBit(byte &variable, byte position, bool bitValue) 
    {
        variable = (variable & ~(1 << position)) | ((bitValue & 1) << position);
    }
    void drawText( const String &text, int textSize, int y) //draws horizontally centered text
    {
        char buffer[text.length() + 1];
        text.toCharArray(buffer, sizeof(buffer));

        int textWidth = text.length() * 6 * textSize;
        int x = (screen.width() - textWidth) / 2;

        screen.setTextSize(textSize);
        screen.text(buffer, x, y);
    }

};