#include <Arduino.h>
#include <Arduino_GFX_Library.h>

class HMI
{
    enum parameter {none=0,temperature, humidity, co2, light};
    public: parameter currentParameter=none;
    #define TFT_CS   10
    #define TFT_DC   9
    #define TFT_RST  8
    #define TFT_SCK  13
    #define TFT_MOSI 11
    Arduino_DataBus *bus = new Arduino_SWSPI(TFT_DC, TFT_CS, TFT_SCK, TFT_MOSI, -1);
    Arduino_GFX *gfx = new Arduino_ST7735(bus, TFT_RST, 1 /* rotation */, false /* IPS */);

    private: short resistances[8];//resistances for the button circuit
    private: int pinA,pinB,pinC,pinButtons;
    private: int pinLED[4];//led pins


    public:

    HMI(int A,int B, int C, int Buttons, short R[8], int LED[4])
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
        gfx->begin();//initialize screen
        gfx->fillScreen(BLACK);
        gfx->setTextColor(RGB565(150,150,150));

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

    void writeToScreen(float currentValue, float targetValue)
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
        drawText("Target:",3,128/2+5);
        drawText(String(targetValue)+unit,2,128/2+5+10+3*8);
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
        int textWidth = text.length() * 6 * textSize; // approx. width per character
        int x = (gfx->width() - textWidth) / 2;
        gfx->setTextSize(textSize);
        gfx->setCursor(x, y);
        gfx->print(text);
    }

};