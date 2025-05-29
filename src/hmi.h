#include <Arduino.h>

class HMI
{
    enum parameter {none=0,temperature, humidity, co2, light};
    public: parameter currentParameter=none;

    private: short resistances[8];//resistances for the button circuit
    private: int pinA,pinB,pinC,pinButtons;
    private: int pinLED[4];//led pins

    

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
        memcpy(resistances, R, sizeof(R));//resistances in Ohms, code coppies R into resistances
        memcpy(pinLED,LED,sizeof(LED));

    }

    void readRotaryEncoder(bool* ptrA,bool* ptrB)
    {
        *ptrA=digitalRead(pinA);//writes value of A to address
        *ptrB=digitalRead(pinB);//writes value of B to address
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
    

    bool readBit(byte variable, int position) 
    {
        return (variable >> position) & 1;
    }
    void writeBit(byte &variable, byte position, bool bitValue) 
    {
        variable = (variable & ~(1 << position)) | ((bitValue & 1) << position);
    }

};