#include "SensorLimit.h"

SensorLimit::SensorLimit(int pin) { setPin(pin); }

SensorLimit::~SensorLimit() {}

bool SensorLimit::setPin(int pin) {
  if (pin >= 0) {
    _pin = pin;
    return (true);
  }
  return (false);
}

int SensorLimit::getPin(void) { return (_pin); }

void SensorLimit::setOnClickCallback(void (*onClickCallback)(void)) {
  _onClickCallback = onClickCallback;
}

void SensorLimit::setOnShortPressCallback(
    void (*onShortPressCallback)(void)) {
  _onShortPressCallback = onShortPressCallback;
}

void SensorLimit::setOnLongPressCallback(void (*onLongPressCallback)(void)) {
  _onLongPressCallback = onLongPressCallback;
}

bool SensorLimit::begin(void) {
  if (getPin() >= 0) {
    pinMode(getPin(), INPUT);
    return (true);
  }
  return (false);
}

void SensorLimit::handle(void) {
  if (millis() - _samplingTimer >= BUTTON_SAMPLING_RATE_MS) {
    switch (_state) {
    case eButtonReset:
      if (analogRead(getPin()) >= SENSOR_LIMIT) {
        _onClickCallback();
        _state = eButtonStart;
        _actionTimer = millis();
      }
      break;
    case eButtonStart:
      if ((millis() - _actionTimer) < BUTTON_LONG_PRESS_DURATION_MS) {
        if (analogRead(getPin()) == 0) {
          _state = eButtonShortPress;
        }
      } else {
        _state = eButtonLongPress;
      }
      break;
    case eButtonShortPress:
      _onShortPressCallback();
      _state = eButtonReset;
      break;
    case eButtonLongPress:
      _onLongPressCallback();
      _state = eButtonReTrigger;
      break;
    case eButtonReTrigger:
      if (analogRead(getPin()) == 0) {
        _state = eButtonReset;
      }
      break;
    default:
      _state = eButtonReset;
      break;
    }
    _samplingTimer = millis();
  }
}