#ifndef SENSORLIMIT_H
#define SENSORLIMIT_H

#include <Arduino.h>

/* Button state enum */
typedef enum {
  eButtonReset,
  eButtonStart,
  eButtonShortPress,
  eButtonLongPress,
  eButtonReTrigger
} ButtonState_t;

/**
 * @brief Control Button class
 *
 */
class SensorLimit {
private:
  /**
   * @brief Button pin
   *
   */
  int _pin = -1;

  /**
   * @brief Timer for button sampling
   *
   */
  uint32_t _samplingTimer = 0;

  /**
   * @brief Timer for button actions
   *
   */
  uint32_t _actionTimer = 0;

  /**
   * @brief Button state
   *
   */
  ButtonState_t _state = eButtonReset;

  /**
   * @brief Pointer for on click event callback
   *
   */
  void (*_onClickCallback)(void);

  /**
   * @brief Pointer for on short press event callback
   *
   */
  void (*_onShortPressCallback)(void);

  /**
   * @brief Pointer for on long press event callback
   *
   */
  void (*_onLongPressCallback)(void);

  /**
   * @brief Button sampling interval in milliseconds
   *
   */
  const uint32_t BUTTON_SAMPLING_RATE_MS = 50;

  /**
   * @brief Duration for long press registration
   *
   */
  const uint32_t BUTTON_LONG_PRESS_DURATION_MS = 3000;

public:
  /**
   * @brief Construct a new Control Button object
   *
   * @param pin Button pin
   */
  SensorLimit(int pin);

  /**
   * @brief Destroy the Control Button object
   *
   */
  ~SensorLimit();

  /**
   * @brief Set the button pin
   *
   * @param pin Button pin
   * @return true If pin is valid
   * @return false If pin is not valid
   */
  bool setPin(int pin);

  /**
   * @brief Get the button pin
   *
   * @return int Button pin
   */
  int getPin(void);

  /**
   * @brief Set the pointer for the on click event callback
   *
   * @param onClickCallback On click callback function
   */
  void setOnClickCallback(void (*onClickCallback)(void));

  /**
   * @brief Set the pointer for the on short press event callback
   *
   * @param onShortPressCallback On short press callback function
   */
  void setOnShortPressCallback(void (*onShortPressCallback)(void));

  /**
   * @brief Set the pointer for the on long press event callback
   *
   * @param onLongPressCallback On long press callback function
   */
  void setOnLongPressCallback(void (*onLongPressCallback)(void));

  /**
   * @brief Initialize the button object
   *
   * @return true If initialization is successful
   * @return false If initialization is not successful
   */
  bool begin(void);

  /**
   * @brief Handle for the control button logic.
   *        Must be called as often as possible.
   *
   */
  void handle(void);
};

#endif // !SensorLimit_H