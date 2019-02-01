/**
 * \file IfxIom_Driver.h
 * \brief IOM DRIVER details
 * \ingroup IfxLld_Iom
 *
 * \version iLLD_1_0_1_8_0
 * \copyright Copyright (c) 2018 Infineon Technologies AG. All rights reserved.
 *
 *
 *                                 IMPORTANT NOTICE
 *
 *
 * Infineon Technologies AG (Infineon) is supplying this file for use
 * exclusively with Infineon's microcontroller products. This file can be freely
 * distributed within development tools that are supporting such microcontroller
 * products.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 * OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * INFINEON SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 * OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 * The IOM driver enable easy usage of the IOM module, by abstracting the internal module structure.
 *  For it's configuration it requires the LAM channel to be used together with the reference and monitor signal. The muxes and filters
 *  are configured accordingly. The driver handles the event combiner "accumulated event timer" resources base on the single or multi-event
 *  configuration making use of the next free timer if required.
 *
 *  Double resource usage (LAM channel, accumulated event timers) is checked and reported at driver initialization .
 *
 *  The driver care for the timing and clock conversion, all timing configuration are to be given in second.
 *
 *  Example of use:
 *  - Initialization of the Iom
 *  - Initialization of LAM channel 0 with:
 *      - IOM monitor input: External pin P20.13, delay debounce filter both edge 0.5us, signal inverted,
 *      - IOM reference: GTM TOUT14, no filter, not inverted
 *      - Event window controlled by ref signal, with a free running timer and cleared on any edge of the reference signal, threshold set to 1us
 *      - Event source is XOR of monitor and reference signal, event is triggered on falling edge of the XOR
 *      - An alarm signal is generated if the monitor signal is not within the threshold, after two events.
 *
 *
 *  \code
 *  void InitLam(IfxIom_Driver *driver, IfxIom_Driver *lam){
 *        IfxIom_Driver_Config configDriver;
 *         IfxIom_Driver_LamConfig configLam;
 *
 *         IfxIom_Driver_initConfig(&configDriver, &MODULE_IOM);
 *         IfxIom_Driver_init(driver, &configDriver);
 *
 *         IfxIom_Driver_initLamConfig(&configLam, driver);
 *
 *         configLam.channel = IfxIom_LamId_0;
 *
 *         configLam.mon.input = IfxIom_MonInput_p20_13;
 *         configLam.mon.filter.mode = IfxIom_LamFilterMode_delayDebounceBothEdge;
 *         configLam.mon.filter.clearTimerOnGlitch = FALSE;
 *         configLam.mon.filter.fallingEdgeFilterTime = 0.5e-6;
 *         configLam.mon.filter.risingEdgeFilterTime = configLam.ref.filter.fallingEdgeFilterTime;
 *         configLam.mon.inverted = TRUE;
 *
 *         configLam.ref.input= IfxIom_RefInput_gtmTout14;
 *         configLam.ref.filter.mode = IfxIom_LamFilterMode_noFilter;
 *         configLam.ref.inverted = FALSE;
 *
 *         configLam.eventWindow.controlSource = IfxIom_LamEventWindowControlSource_ref;
 *         configLam.eventWindow.run = IfxIom_LamEventWindowRunControl_freeRunning;
 *         configLam.eventWindow.clearEvent = IfxIom_LamEventWindowClearEvent_anyEdge;
 *      configLam.eventWindow.threshold = 1e-6;
 *         configLam.event.source = IfxIom_LamEventSource_monXorRef;
 *         configLam.event.trigger = IfxIom_LamEventTrigger_fallingEdge;
 *
 *         configLam.systemEventTriggerThreshold = 2;
 *
 *         IfxIom_Driver_initLam(lam, &configLam);
 *
 *  }
 *  \endcode
 *
 * \defgroup IfxLld_Iom_Driver IOM Driver
 * \ingroup IfxLld_Iom
 * \defgroup IfxLld_Iom_Driver_func Driver Functions
 * \ingroup IfxLld_Iom_Driver
 * \defgroup IfxLld_Iom_Driver_struct Structures
 * \ingroup IfxLld_Iom_Driver
 */

#ifndef IFXIOM_DRIVER_H
#define IFXIOM_DRIVER_H 1

/******************************************************************************/
/*----------------------------------Includes----------------------------------*/
/******************************************************************************/

#include "Iom/Std/IfxIom.h"

/******************************************************************************/
/*-----------------------------Data Structures--------------------------------*/
/******************************************************************************/

/** \addtogroup IfxLld_Iom_Driver_struct
 * \{ */
/** \brief IOM LAM filter configuration
 */
typedef struct
{
    boolean              clearTimerOnGlitch;          /**< \brief If set, the timer is cleared on glitch, else it is decremented */
    float32              fallingEdgeFilterTime;       /**< \brief Falling edge filter time in second for immediate debounce filter mode. In delayed filter time this corresponds to the minimal filter time. */
    IfxIom_LamFilterMode mode;                        /**< \brief Filter mode */
    uint32               prescalerFactor;             /**< \brief Prescaler factor, must be > 0, only valid for prescaler mode */
    float32              risingEdgeFilterTime;        /**< \brief Rising edge filter time in second for immediate debounce filter mode. In delayed filter time this corresponds to the minimal filter time. */
} IfxIom_Driver_LamFilterConfig;

/** \} */

/** \addtogroup IfxLld_Iom_Driver_struct
 * \{ */
/** \brief IOM driver
 */
typedef struct
{
    Ifx_IOM *module;                         /**< \brief Pointer to IOM module SFR set */
    uint8    accumulatedEventUsedMask;       /**< \brief Indicates the used / unused accumulated counter ECM. bit0=CTS0, bit1=CTS2, ... 0=unused, 1=used */
    uint16   lamUsedMask;                    /**< \brief Indicates the used / unused LAM. bit0=LAM0, bit1=LAM2, ... 0=unused, 1=used */
} IfxIom_Driver;

/** \brief IOM LAM event configuration
 */
typedef struct
{
    IfxIom_LamEventSource  source;        /**< \brief Event source */
    IfxIom_LamEventTrigger trigger;       /**< \brief Event trigger */
} IfxIom_Driver_LamEventConfig;

/** \brief IOM LAM event window configuration
 */
typedef struct
{
    IfxIom_LamEventWindowClearEvent    clearEvent;          /**< \brief Timer clear event */
    IfxIom_LamEventWindowControlSource controlSource;       /**< \brief Timer control source */
    boolean                            inverted;            /**< \brief If TRUE, the event window is inverted */
    IfxIom_LamEventWindowRunControl    run;                 /**< \brief Timer run control */
    float32                            threshold;           /**< \brief Event window threshold in second. If 0, no event are generated */
} IfxIom_Driver_LamEventWindowConfig;

/** \brief IOM LAM monitor input configuration
 */
typedef struct
{
    IfxIom_Driver_LamFilterConfig filter;         /**< \brief Filter configuration */
    IfxIom_MonInput               input;          /**< \brief Monitor input */
    boolean                       inverted;       /**< \brief If TRUE, the signal is inverted */
} IfxIom_Driver_LamMonConfig;

/** \brief IOM LAM reference input configuration
 */
typedef struct
{
    IfxIom_Driver_LamFilterConfig filter;         /**< \brief Filter configuration */
    IfxIom_RefInput               input;          /**< \brief Reference input */
    boolean                       inverted;       /**< \brief If TRUE, the signal is inverted */
} IfxIom_Driver_LamRefConfig;

/** \} */

/** \addtogroup IfxLld_Iom_Driver_struct
 * \{ */
/** \brief IOM Driver Configuration
 */
typedef struct
{
    Ifx_IOM *module;       /**< \brief IOM module SFRs */
} IfxIom_Driver_Config;

/** \brief IOM LAM object
 */
typedef struct
{
    IfxIom_Driver        *iomDriver;                         /**< \brief Main IOM Driver */
    IfxIom_LamId          channel;                           /**< \brief LAM Channel */
    uint8                 monIndex;                          /**< \brief Monitor input index */
    uint8                 refIndex;                          /**< \brief Reference input index */
    IfxIom_RefInputSignal refInput;                          /**< \brief Reference input */
    IfxIom_MonInputSignal monInput;                          /**< \brief Monitor input */
    sint8                 accumulatedCounterIndex;           /**< \brief Accumulated counter used. Negative value means no counter used */
    uint8                 systemEventTriggerThreshold;       /**< \brief Specifies the number of LAM event that triggers the System Event. Value 0 disables the trigger event. Value one enables the trigger event. Value from 2 to 15 will use the counter to filter events, max 4 counters exists for the IOM. */
} IfxIom_Driver_Lam;

/** \brief IOM LAM configuration
 */
typedef struct
{
    IfxIom_Driver                     *iomDriver;                         /**< \brief Pointer to the IOM driver */
    IfxIom_LamId                       channel;                           /**< \brief LAM channel to be used */
    IfxIom_Driver_LamEventConfig       event;                             /**< \brief LAM event configuration */
    IfxIom_Driver_LamEventWindowConfig eventWindow;                       /**< \brief LAM eventWindow configuration */
    IfxIom_Driver_LamMonConfig         mon;                               /**< \brief LAM Monitor input configuration */
    IfxIom_Driver_LamRefConfig         ref;                               /**< \brief LAM reference input configuration */
    uint8                              systemEventTriggerThreshold;       /**< \brief Specifies the number of LAM event that triggers the System Event. Value 0 disables the trigger event. Value one enables the trigger event. Value from 2 to 15 will use the counter to filter events, max 4 counters exists for the IOM. */
} IfxIom_Driver_LamConfig;

/** \} */

/** \addtogroup IfxLld_Iom_Driver_func
 * \{ */

/******************************************************************************/
/*-------------------------Global Function Prototypes-------------------------*/
/******************************************************************************/

/** \brief Clear all LAM monitor and reference glitch flags
 * \param driver Pointer to the IOM driver object
 * \return None
 */
IFX_EXTERN void IfxIom_Driver_clearAllGlitch(IfxIom_Driver *driver);

/** \brief Clear the IOM event history
 * \param driver Pointer to the IOM driver object
 * \return None
 */
IFX_EXTERN void IfxIom_Driver_clearHistory(IfxIom_Driver *driver);

/** \brief Clear the LAM monitor signal glitch flag
 * \param driver Pointer to the LAM driver object
 * \return None
 */
IFX_EXTERN void IfxIom_Driver_clearLamMonGlitch(IfxIom_Driver_Lam *driver);

/** \brief Clear the LAM reference signal glitch flag
 * \param driver Pointer to the LAM driver object
 * \return None
 */
IFX_EXTERN void IfxIom_Driver_clearLamRefGlitch(IfxIom_Driver_Lam *driver);

/** \brief Disable all event generation
 * \param driver Pointer to the IOM driver object
 * \return Return the configured events
 */
IFX_EXTERN uint32 IfxIom_Driver_disableEvents(IfxIom_Driver *driver);

/** \brief Disable the event generation for the LAM
 * \param driver Pointer to the LAM driver object
 * \return None
 */
IFX_EXTERN void IfxIom_Driver_disableLamEvent(IfxIom_Driver_Lam *driver);

/** \brief Enable the event genration for the LAM
 * \param driver Pointer to the LAM driver object
 * \return None
 */
IFX_EXTERN void IfxIom_Driver_enableLamEvent(IfxIom_Driver_Lam *driver);

/** \brief Return the IOM event history.
 * In returned mask value, bit0= LAM0, bit1=LAM1, ...
 * \param driver Pointer to the IOM driver object
 * \param a Event mask of history level 0 (Last event)
 * \param b Event mask of history level 1
 * \param c Event mask of history level 2
 * \param d Event mask of history level 3 (oldest event)
 * \return None
 */
IFX_EXTERN void IfxIom_Driver_getHistory(IfxIom_Driver *driver, uint16 *a, uint16 *b, uint16 *c, uint16 *d);

/** \brief Initialize the IOM
 * Must be called before IfxIom_Driver_initLam()
 * \param driver Pointer to the IOM driver object. Will be initialized by the function
 * \param config IOM driver configuration
 * \return TRUE in case of success else FALSE
 */
IFX_EXTERN boolean IfxIom_Driver_init(IfxIom_Driver *driver, IfxIom_Driver_Config *config);

/** \brief Set the IOM default configuration
 * \param config IOM configuration. Will be initialized by the function
 * \param module Pointer to the IOM module
 * \return None
 */
IFX_EXTERN void IfxIom_Driver_initConfig(IfxIom_Driver_Config *config, Ifx_IOM *module);

/** \brief Initialize the LAM channel
 * \param driver Pointer to the LAM driver object. Will be initialized by the function
 * \param config LAM driver configuration
 * \return TRUE in case of success else FALSE
 */
IFX_EXTERN boolean IfxIom_Driver_initLam(IfxIom_Driver_Lam *driver, IfxIom_Driver_LamConfig *config);

/** \brief Set the LAM default configuration
 * \param config LAM configuration. Will be initialized by the function
 * \param driver Pointer to the IOM driver object
 * \return None
 */
IFX_EXTERN void IfxIom_Driver_initLamConfig(IfxIom_Driver_LamConfig *config, IfxIom_Driver *driver);

/** \brief Return the LAM monitor glitch flags
 * \param driver Pointer to the LAM driver object
 * \param risingEdgeGlitch Set to TRUE by the function if rising edge glitch were detected on the monitor signal
 * \param fallingEdgeGlitch Set to TRUE by the function if falling edge glitch were detected on the monitor signal
 * \return None
 */
IFX_EXTERN void IfxIom_Driver_isLamMonGlitch(IfxIom_Driver_Lam *driver, boolean *risingEdgeGlitch, boolean *fallingEdgeGlitch);

/** \brief Return the LAM reference glitch flags
 * \param driver Pointer to the LAM driver object
 * \param risingEdgeGlitch Set to TRUE by the function if rising edge glitch were detected on the reference signal
 * \param fallingEdgeGlitch Set to TRUE by the function if falling edge glitch were detected on the reference signal
 * \return None
 */
IFX_EXTERN void IfxIom_Driver_isLamRefGlitch(IfxIom_Driver_Lam *driver, boolean *risingEdgeGlitch, boolean *fallingEdgeGlitch);

/** \brief Restore the IOM event mask
 * \param driver Pointer to the IOM driver object
 * \param mask Event configuration as returned by IfxIom_Driver_disableEvents()
 * \return None
 */
IFX_EXTERN void IfxIom_Driver_restoreEvents(IfxIom_Driver *driver, uint32 mask);

/** \} */

#endif /* IFXIOM_DRIVER_H */
