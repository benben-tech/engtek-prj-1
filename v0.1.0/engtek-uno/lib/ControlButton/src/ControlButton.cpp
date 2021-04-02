#include "ControlButton.h"

ControlButton::ControlButton(int pin) { setPin(pin); }

ControlButton::~ControlButton() {}

bool ControlButton::setPin(int pin) {
  if (pin >= 0) {
    _pin = pin;
    return (true);
  }
  return (false);
}

int ControlButton::getPin(void) { return (_pin); }

void ControlButton::setOnClickCallback(void (*onClickCallback)(void)) {
  _onClickCallback = onClickCallback;
}

void ControlButton::setOnShortPressCallback(
    void (*onShortPressCallback)(void)) {
  _onShortPressCallback = onShortPressCallback;
}

void ControlButton::setOnLongPressCallback(void (*onLongPressCallback)(void)) {
  _onLongPressCallback = onLongPressCallback;
}

bool ControlButton::begin(void) {
  if (getPin() >= 0) {
    pinMode(getPin(), INPUT);
    return (true);
  }
  return (false);
}

void ControlButton::handle(void) {
  if (millis() - _samplingTimer >= BUTTON_SAMPLING_RATE_MS) {
    switch (_state) {
    case _eButtonReset:
      if (digitalRead(getPin()) == LOW) {
        _onClickCallback();
        _state = _eButtonStart;
        _actionTimer = millis();
      }
      break;
    case _eButtonStart:
      if ((millis() - _actionTimer) < BUTTON_LONG_PRESS_DURATION_MS) {
        if (digitalRead(getPin()) == HIGH) {
          _state = _eButtonShortPress;
        }
      } else {
        _state = _eButtonLongPress;
      }
      break;
    case _eButtonShortPress:
      _onShortPressCallback();
      _state = _eButtonReset;
      break;
    case _eButtonLongPress:
      _onLongPressCallback();
      _state = _eButtonReTrigger;
      break;
    case _eButtonReTrigger:
      if (digitalRead(getPin()) == HIGH) {
        _state = _eButtonReset;
      }
      break;
    default:
      _state = _eButtonReset;
      break;
    }
    _samplingTimer = millis();
  }
}