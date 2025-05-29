#include <Arduino.h>

class HMI
{
    enum parameter {none=0,temperature, humidity, co2, light};
    public: parameter currentParameter=none;
    private: int pinA,pinB,pinC,pinButtons;

    HMI(int A,int B, int C, int Buttons)
    {
        pinA=A;
        pinMode(A,INPUT);
        pinB=B;
        pinMode(B,INPUT);
        pinC=C;
        pinMode(C,INPUT);
        pinButtons=Buttons;
        pinMode(Buttons,INPUT);

    }

    void readRotaryEncoder(bool* ptrA,bool* ptrB)
    {
        *ptrA=digitalRead(pinA);
        *ptrB=digitalRead(pinB);
    }

};