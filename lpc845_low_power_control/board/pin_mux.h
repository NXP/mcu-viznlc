/***********************************************************************************************************************
 * This file was generated by the MCUXpresso Config Tools. Any manual edits made to this file
 * will be overwritten if the respective MCUXpresso Config Tools is used to update this file.
 **********************************************************************************************************************/

#ifndef _PIN_MUX_H_
#define _PIN_MUX_H_

/*!
 * @addtogroup pin_mux
 * @{
 */

/***********************************************************************************************************************
 * API
 **********************************************************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @brief Calls initialization functions.
 *
 */
void BOARD_InitBootPins(void);

#define IOCON_PIO_CLKDIV0 0x00u      /*!<@brief IOCONCLKDIV0 */
#define IOCON_PIO_HYS_EN 0x20u       /*!<@brief Enable hysteresis */
#define IOCON_PIO_INV_DI 0x00u       /*!<@brief Input not invert */
#define IOCON_PIO_MODE_PULLUP 0x10u  /*!<@brief Selects pull-up function */
#define IOCON_PIO_OD_DI 0x00u        /*!<@brief Disables Open-drain function */
#define IOCON_PIO_SMODE_BYPASS 0x00u /*!<@brief Bypass input filter */

/*! @name RESETN (number 5), MCU_RESET
  @{ */
#define BOARD_INITPINS_MCU_RESET_PORT 0U                  /*!<@brief PORT device index: 0 */
#define BOARD_INITPINS_MCU_RESET_PIN 5U                   /*!<@brief PORT pin number */
#define BOARD_INITPINS_MCU_RESET_PIN_MASK (1U << 5U)      /*!<@brief PORT pin mask */
                                                          /* @} */

/*! @name PIO0_4 (number 6), PIR_WAKEUP
  @{ */

/* Symbols to be used with GPIO driver */
#define BOARD_INITPINS_PIR_WAKEUP_GPIO GPIO                /*!<@brief GPIO peripheral base pointer */
#define BOARD_INITPINS_PIR_WAKEUP_GPIO_PIN_MASK (1U << 4U) /*!<@brief GPIO pin mask */
#define BOARD_INITPINS_PIR_WAKEUP_PORT 0U                  /*!<@brief PORT device index: 0 */
#define BOARD_INITPINS_PIR_WAKEUP_PIN 4U                   /*!<@brief PORT pin number */
#define BOARD_INITPINS_PIR_WAKEUP_PIN_MASK (1U << 4U)      /*!<@brief PORT pin mask */
                                                           /* @} */

/*! @name PIO1_3 (number 29), SYS_PWR_CTL
  @{ */

/* Symbols to be used with GPIO driver */
#define BOARD_INITPINS_SYS_PWR_CTL_GPIO GPIO                /*!<@brief GPIO peripheral base pointer */
#define BOARD_INITPINS_SYS_PWR_CTL_GPIO_PIN_MASK (1U << 3U) /*!<@brief GPIO pin mask */
#define BOARD_INITPINS_SYS_PWR_CTL_PORT 1U                  /*!<@brief PORT device index: 1 */
#define BOARD_INITPINS_SYS_PWR_CTL_PIN 3U                   /*!<@brief PORT pin number */
#define BOARD_INITPINS_SYS_PWR_CTL_PIN_MASK (1U << 3U)      /*!<@brief PORT pin mask */
                                                            /* @} */
/*! @name PIO1_2 (number 20), P3[46]/P1_2
  @{ */

/* Symbols to be used with GPIO driver */
#define BOARD_DCDC_PG_GPIO GPIO                /*!<@brief GPIO peripheral base pointer */
#define BOARD_DCDC_PG_GPIO_PIN_MASK (1U << 2U) /*!<@brief GPIO pin mask */
#define BOARD_DCDC_PG_PORT 1U                  /*!<@brief PORT device index: 1 */
#define BOARD_DCDC_PG_PIN 2U                   /*!<@brief PORT pin number */
#define BOARD_DCDC_PG_PIN_MASK (1U << 2U)      /*!<@brief PORT pin mask */
                                               /* @} */

/*! @name PIO0_24 (number 28), DEBUG_UART_RXD
  @{ */
#define BOARD_INITPINS_DEBUG_UART_RXD_PORT 0U                   /*!<@brief PORT device index: 0 */
#define BOARD_INITPINS_DEBUG_UART_RXD_PIN 24U                   /*!<@brief PORT pin number */
#define BOARD_INITPINS_DEBUG_UART_RXD_PIN_MASK (1U << 24U)      /*!<@brief PORT pin mask */
                                                                /* @} */

/*! @name PIO0_25 (number 27), DEBUG_UART_TXD
  @{ */
#define BOARD_INITPINS_DEBUG_UART_TXD_PORT 0U                   /*!<@brief PORT device index: 0 */
#define BOARD_INITPINS_DEBUG_UART_TXD_PIN 25U                   /*!<@brief PORT pin number */
#define BOARD_INITPINS_DEBUG_UART_TXD_PIN_MASK (1U << 25U)      /*!<@brief PORT pin mask */
                                                                /* @} */

/*! @name PIO1_9 (number 3), FACEID_UART_RXD
  @{ */
#define BOARD_INITPINS_FACEID_UART_RXD_PORT 1U                  /*!<@brief PORT device index: 1 */
#define BOARD_INITPINS_FACEID_UART_RXD_PIN 9U                   /*!<@brief PORT pin number */
#define BOARD_INITPINS_FACEID_UART_RXD_PIN_MASK (1U << 9U)      /*!<@brief PORT pin mask */
                                                              /* @} */

/*! @name PIO1_8 (number 1), FACEID_UART_TXD
  @{ */
#define BOARD_INITPINS_FACEID_UART_TXD_PORT 1U                  /*!<@brief PORT device index: 1 */
#define BOARD_INITPINS_FACEID_UART_TXD_PIN 8U                   /*!<@brief PORT pin number */
#define BOARD_INITPINS_FACEID_UART_TXD_PIN_MASK (1U << 8U)      /*!<@brief PORT pin mask */
                                                              /* @} */

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitPins(void); /* Function assigned for the Cortex-M0P */

#if defined(__cplusplus)
}
#endif

/*!
 * @}
 */
#endif /* _PIN_MUX_H_ */

/***********************************************************************************************************************
 * EOF
 **********************************************************************************************************************/
