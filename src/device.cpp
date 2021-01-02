#include <ESP8266WiFi.h>
class Device {
  private:
    uint8_t _pin; 
    bool _state;
  public:
    Device(uint8_t pin){
      _pin = pin;
      pinMode(_pin, OUTPUT);
      setPinState(LOW);
    }
    Device(uint8_t pin, bool state){
      _pin = pin;
      pinMode(_pin, OUTPUT);
      setPinState(state);
    }                          
    void setPinState(bool state){
      Serial.print("PIN: ");
      Serial.println(_pin);
      Serial.print("STATE: ");
      Serial.println(state);
      digitalWrite(_pin, !state);
      _state = state;
    }
    bool getPinState(){
      return _state;
    }
    void togglePinState(){
        setPinState(!_state);
    }
};