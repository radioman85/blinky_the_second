#ifndef PWM_CONTROLLER_H
#define PWM_CONTROLLER_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief PWM Controller Module
 * 
 * This module provides a clean interface for controlling PWM output
 * on the nRF5340dk board. It uses P0.13 as the default PWM pin.
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
 * @brief Set PWM duty cycle
 * 
 * @param duty_cycle Duty cycle value (0-255, where 0=0% and 255=100%)
 * @return int 0 on success, negative error code on failure
 */
int set_pwm_duty_cycle(uint8_t duty_cycle);

/**
 * @brief Get current PWM duty cycle
 * 
 * @return uint8_t Current duty cycle (0-255)
 */
uint8_t get_pwm_duty_cycle(void);

/**
 * @brief Enable/disable PWM output
 * 
 * @param enable true to enable PWM, false to disable
 * @return int 0 on success, negative error code on failure
 */
int enable_pwm(bool enable);

#endif // PWM_CONTROLLER_H