#ifndef PWM_CONTROLLER_H
#define PWM_CONTROLLER_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief RC Servo PWM Controller Module
 * 
 * This module provides RC servo control via hardware PWM on P0.28.
 * Generates standard RC servo signals: 50Hz (20ms period), 1-2ms pulse width.
 * Input range: 0-1000 (0 = 1ms, 500 = 1.5ms, 1000 = 2ms)
 */

/**
 * @brief Initialize the PWM controller
 * 
 * Sets up the PWM device and configures the output pin.
 * 
 * @return int 0 on success, negative error code on failure
 */
int init_pwm_controller(void);

/**
 * @brief Set RC servo position
 * 
 * @param position Servo position (0-1000, where 0=1ms, 500=1.5ms, 1000=2ms)
 * @return int 0 on success, negative error code on failure
 */
int set_pwm_duty_cycle(uint16_t position);

/**
 * @brief Get current servo position
 * 
 * @return uint16_t Current position (0-1000)
 */
uint16_t get_pwm_duty_cycle(void);

/**
 * @brief Enable/disable PWM output
 * 
 * @param enable true to enable PWM, false to disable
 * @return int 0 on success, negative error code on failure
 */
int enable_pwm(bool enable);

#endif // PWM_CONTROLLER_H