    
    /**
     * @brief       Start the measure indicated by operating mode
     * @param[in]   operating_mode specifies requested measure
     * @param[in]   fptr specifies call back function must be !NULL in case of interrupt measure
     * @return      "0" on success
     */
    int start_measurement(OperatingMode operating_mode, void (*fptr)(void));

    /**
     * @brief       Get results for the measure indicated by operating mode
     * @param[in]   operating_mode specifies requested measure results
     * @param[out]  p_data pointer to the MeasureData_t structure to read data in to
     * @return      "0" on success
     */
    int get_measurement(OperatingMode operating_mode, VL53L0X_RangingMeasurementData_t *p_data);

    /**
     * @brief       Stop the currently running measure indicate by operating_mode
     * @param[in]   operating_mode specifies requested measure to stop
     * @return      "0" on success
     */
    int stop_measurement(OperatingMode operating_mode);

    /**
     * @brief       Interrupt handling func to be called by user after an INT is occourred
     * @param[in]   opeating_mode indicating the in progress measure
     * @param[out]  Data pointer to the MeasureData_t structure to read data in to
     * @return      "0" on success
     */
    int handle_irq(OperatingMode operating_mode, VL53L0X_RangingMeasurementData_t *data);

    /**
     * @brief       Enable interrupt measure IRQ
     * @return      "0" on success
     */
    void enable_interrupt_measure_detection_irq(void)
    {
        if (_gpio1Int != NULL) {
            _gpio1Int->enable_irq();
        }
    }

    /**
     * @brief       Disable interrupt measure IRQ
     * @return      "0" on success
     */
    void disable_interrupt_measure_detection_irq(void)
    {
        if (_gpio1Int != NULL) {
            _gpio1Int->disable_irq();
        }
    }

    /**
     * @brief       Attach a function to call when an interrupt is detected, i.e. measurement is ready
     * @param[in]   fptr pointer to call back function to be called whenever an interrupt occours
     * @return      "0" on success
     */
    void attach_interrupt_measure_detection_irq(void (*fptr)(void))
    {
        if (_gpio1Int != NULL) {
            _gpio1Int->rise(fptr);
        }
    }


    /** Wrapper functions */
    /** @defgroup api_init Init functions
     *  @brief    API init functions
     *  @ingroup  api_hl
     *  @{
     */

    /**
     *
     * @brief One time device initialization
     *
     * To be called once and only once after device is brought out of reset (Chip enable) and booted.
     *
     * @par Function Description
     * When not used after a fresh device "power up" or reset, it may return @a #CALIBRATION_WARNING
     * meaning wrong calibration data may have been fetched from device that can result in ranging offset error\n
     * If application cannot execute device reset or need to run VL53L0X_data_init  multiple time
     * then it  must ensure proper offset calibration saving and restore on its own
     * by using @a VL53L0X_get_offset_calibration_data_micro_meter() on first power up and then @a VL53L0X_set_offset_calibration_data_micro_meter() all all subsequent init
     *
     * @param void
     * @return     "0" on success,  @a #CALIBRATION_WARNING if failed
     */
    virtual int init(void *init)
    {
        return VL53L0X_data_init(_device);
    }

    /**
      * @brief  Prepare device for operation
      * @par Function Description
      * Does static initialization and reprogram common default settings \n
      * Device is prepared for new measure, ready single shot ranging or ALS typical polling operation\n
      * After prepare user can : \n
      * @li Call other API function to set other settings\n
      * @li Configure the interrupt pins, etc... \n
      * @li Then start ranging or ALS operations in single shot or continuous mode
      *
      * @param void
      * @return  "0" on success
      */
    int prepare()
    {
        VL53L0X_Error status = VL53L0X_ERROR_NONE;
        uint32_t ref_spad_count;
        uint8_t is_aperture_spads;
        uint8_t vhv_settings;
        uint8_t phase_cal;

        if (status == VL53L0X_ERROR_NONE) {
            //printf("Call of VL53L0X_StaticInit\r\n");
            status = VL53L0X_static_init(_device);   // Device Initialization
        }

        if (status == VL53L0X_ERROR_NONE) {
            //printf("Call of VL53L0X_PerformRefCalibration\r\n");
            status = VL53L0X_perform_ref_calibration(_device,
                     &vhv_settings, &phase_cal);  // Device Initialization
        }

        if (status == VL53L0X_ERROR_NONE) {
            //printf("Call of VL53L0X_PerformRefSpadManagement\r\n");
            status = VL53L0X_perform_ref_spad_management(_device,
                     &ref_spad_count, &is_aperture_spads);  // Device Initialization
//            printf ("refSpadCount = %d, isApertureSpads = %d\r\n", refSpadCount, isApertureSpads);
        }

        return status;
    }

    /**
    * @brief Start continuous ranging mode
    *
    * @details End user should ensure device is in idle state and not already running
    * @return  "0" on success
    */
    int range_start_continuous_mode()
    {
        int status;
        status = VL53L0X_set_device_mode(_device, VL53L0X_DEVICEMODE_CONTINUOUS_RANGING);

        if (status == VL53L0X_ERROR_NONE) {
            //printf ("Call of VL53L0X_StartMeasurement\r\n");
            status = VL53L0X_start_measurement(_device);
        }

        return status;
    }

//------- Feburary 4th, 2018 by JH1PJL / K.Arai --------------------------------
#if ORIGINAL
    /**
     * @brief Get ranging result and only that
     *
     * @par Function Description
     * Unlike @a VL53L0X_get_ranging_measurement_data() this function only retrieves the range in millimeter \n
     * It does any required up-scale translation\n
     * It can be called after success status polling or in interrupt mode \n
     * @warning these function is not doing wrap around filtering \n
     * This function doesn't perform any data ready check!
     *
     * @param p_data  Pointer to range distance
     * @return        "0" on success
     */
    virtual int get_distance(uint32_t *p_data)
    {
        int status = 0;
        VL53L0X_RangingMeasurementData_t p_ranging_measurement_data;

        status = start_measurement(range_single_shot_polling, NULL);
        if (!status) {
            status = get_measurement(range_single_shot_polling, &p_ranging_measurement_data);
        }
        if (p_ranging_measurement_data.RangeStatus == 0) {
            // we have a valid range.
            *p_data = p_ranging_measurement_data.RangeMilliMeter;
        } else {
            *p_data = 0;
            status = VL53L0X_ERROR_RANGE_ERROR;
        }
        stop_measurement(range_single_shot_polling);
        return status;
    }
#endif

    /** @}  */

    /**
     * @brief Set new device i2c address
     *
     * After completion the device will answer to the new address programmed.
     *
     * @param new_addr  The new i2c address (7bit)
     * @return          "0" on success
     */
    int set_device_address(int new_addr)
    {
        int status;

        status = VL53L0X_set_device_address(_device, new_addr);
        if (!status) {
            _device->I2cDevAddr = new_addr;
        }
        return status;

    }

    /**
     * @brief Clear given system interrupt condition
     *
     * @par Function Description
     * Clear given interrupt cause by writing into register #SYSTEM_INTERRUPT_CLEAR register.
     * @param dev        The device
     * @param int_clear  Which interrupt source to clear. Use any combinations of #INTERRUPT_CLEAR_RANGING , #INTERRUPT_CLEAR_ALS , #INTERRUPT_CLEAR_ERROR.
     * @return           "0" on success
     */
    int clear_interrupt(uint8_t int_clear)
    {
        return VL53L0X_clear_interrupt_mask(_device, int_clear);
    }

    /**
     *
     * @brief Get the 53L0 device
     *
     * To be called to retrive the internal device descriptor to allow usage of 
     * low level API having device as parameter. To be called  after set_device_address()
     * (if any).
     *
     * @par Function Description
     * To be called if low level API usage is needed as those functions requires
     * device as a parameter.TICINIT.
     *
     * @note This function return a pointer to an object internal structure
     *
     * @param   dev                   ptr to ptr to Device Handle
     * @return  VL53L0X_ERROR_NONE     Success
     * @return  "Other error code"    See ::VL53L0X_Error
     */
    VL53L0X_Error vl53l0x_get_device(VL53L0X_DEV *dev)
{
   *dev = _device;
   return VL53L0X_ERROR_NONE;
}             

    /**
     *
     * @brief One time device initialization
     *
     * To be called once and only once after device is brought out of reset
     * (Chip enable) and booted see @a VL53L0X_WaitDeviceBooted()
     *
     * @par Function Description
     * When not used after a fresh device "power up" or reset, it may return
     * @a #VL53L0X_ERROR_CALIBRATION_WARNING meaning wrong calibration data
     * may have been fetched from device that can result in ranging offset error\n
     * If application cannot execute device reset or need to run VL53L0X_DataInit
     * multiple time then it  must ensure proper offset calibration saving and
     * restore on its own by using @a VL53L0X_GetOffsetCalibrationData() on first
     * power up and then @a VL53L0X_SetOffsetCalibrationData() in all subsequent init
     * This function will change the VL53L0X_State from VL53L0X_STATE_POWERDOWN to
     * VL53L0X_STATE_WAIT_STATICINIT.
     *
     * @note This function accesses to the device
     *
     * @param   dev                   Device Handle
     * @return  VL53L0X_ERROR_NONE    Success
     * @return  "Other error code"    See ::VL53L0X_Error
     */
    VL53L0X_Error VL53L0X_data_init(VL53L0X_DEV dev);

    /**
     * @brief Do basic device init (and eventually patch loading)
     * This function will change the VL53L0X_State from
     * VL53L0X_STATE_WAIT_STATICINIT to VL53L0X_STATE_IDLE.
     * In this stage all default setting will be applied.
     *
     * @note This function Access to the device
     *
     * @param   dev                   Device Handle
     * @return  VL53L0X_ERROR_NONE    Success
     * @return  "Other error code"    See ::VL53L0X_Error
     */
    VL53L0X_Error VL53L0X_static_init(VL53L0X_DEV dev);

    /**
     * @brief Perform Reference Calibration
     *
     * @details Perform a reference calibration of the Device.
     * This function should be run from time to time before doing
     * a ranging measurement.
     * This function will launch a special ranging measurement, so
     * if interrupt are enable an interrupt will be done.
     * This function will clear the interrupt generated automatically.
     *
     * @warning This function is a blocking function
     *
     * @note This function Access to the device
     *
     * @param   dev                  Device Handle
     * @param   p_vhv_settings       Pointer to vhv settings parameter.
     * @param   p_phase_cal          Pointer to PhaseCal parameter.
     * @return  VL53L0X_ERROR_NONE   Success
     * @return  "Other error code"   See ::VL53L0X_Error
     */
    VL53L0X_Error VL53L0X_perform_ref_calibration(VL53L0X_DEV dev, uint8_t *p_vhv_settings,
            uint8_t *p_phase_cal);

    /**
     * @brief Get Reference Calibration Parameters
     *
     * @par Function Description
     * Get Reference Calibration Parameters.
     *
     * @note This function Access to the device
     *
     * @param   dev                            Device Handle
     * @param   p_vhv_settings                 Pointer to VHV parameter
     * @param   p_phase_cal                    Pointer to PhaseCal Parameter
     * @return  VL53L0X_ERROR_NONE             Success
     * @return  "Other error code"             See ::VL53L0X_Error
     */
    VL53L0X_Error VL53L0X_get_ref_calibration(VL53L0X_DEV dev,
            uint8_t *p_vhv_settings, uint8_t *p_phase_cal);

    VL53L0X_Error VL53L0X_set_ref_calibration(VL53L0X_DEV dev,
            uint8_t vhv_settings, uint8_t phase_cal);

    /**
     * @brief Performs Reference Spad Management
     *
     * @par Function Description
     * The reference SPAD initialization procedure determines the minimum amount
     * of reference spads to be enables to achieve a target reference signal rate
     * and should be performed once during initialization.
     *
     * @note This function Access to the device
     *
     * @note This function change the device mode to
     * VL53L0X_DEVICEMODE_SINGLE_RANGING
     *
     * @param   dev                          Device Handle
     * @param   ref_spad_count               Reports ref Spad Count
     * @param   is_aperture_spads            Reports if spads are of type
     *                                       aperture or non-aperture.
     *                                       1:=aperture, 0:=Non-Aperture
     * @return  VL53L0X_ERROR_NONE           Success
     * @return  VL53L0X_ERROR_REF_SPAD_INIT  Error in the Ref Spad procedure.
     * @return  "Other error code"           See ::VL53L0X_Error
     */
    VL53L0X_Error VL53L0X_perform_ref_spad_management(VL53L0X_DEV dev,
            uint32_t *ref_spad_count, uint8_t *is_aperture_spads);

    /**
     * @brief Applies Reference SPAD configuration
     *
     * @par Function Description
     * This function applies a given number of reference spads, identified as
     * either Aperture or Non-Aperture.
     * The requested spad count and type are stored within the device specific
     * parameters data for access by the host.
     *
     * @note This function Access to the device
     *
     * @param   dev                          Device Handle
     * @param   refSpadCount                 Number of ref spads.
     * @param   is_aperture_spads            Defines if spads are of type
     *                                       aperture or non-aperture.
     *                                       1:=aperture, 0:=Non-Aperture
     * @return  VL53L0X_ERROR_NONE           Success
     * @return  VL53L0X_ERROR_REF_SPAD_INIT  Error in the in the reference
     *                                       spad configuration.
     * @return  "Other error code"           See ::VL53L0X_Error
     */
    VL53L0X_Error VL53L0X_set_reference_spads(VL53L0X_DEV dev,
            uint32_t refSpadCount, uint8_t is_aperture_spads);

    /**
     * @brief Retrieves SPAD configuration
     *
     * @par Function Description
     * This function retrieves the current number of applied reference spads
     * and also their type : Aperture or Non-Aperture.
     *
     * @note This function Access to the device
     *
     * @param   dev                          Device Handle
     * @param   p_spad_count                 Number ref Spad Count
     * @param   p_is_aperture_spads          Reports if spads are of type
     *                                       aperture or non-aperture.
     *                                       1:=aperture, 0:=Non-Aperture
     * @return  VL53L0X_ERROR_NONE           Success
     * @return  VL53L0X_ERROR_REF_SPAD_INIT  Error in the in the reference
     *                                       spad configuration.
     * @return  "Other error code"           See ::VL53L0X_Error
     */
    VL53L0X_Error VL53L0X_get_reference_spads(VL53L0X_DEV dev,
            uint32_t *p_spad_count, uint8_t *p_is_aperture_spads);

    /**
     * @brief Get part to part calibration offset
     *
     * @par Function Description
     * Should only be used after a successful call to @a VL53L0X_DataInit to backup
     * device NVM value
     *
     * @note This function Access to the device
     *
     * @param   dev                                     Device Handle
     * @param   p_offset_calibration_data_micro_meter   Return part to part
     * calibration offset from device (microns)
     * @return  VL53L0X_ERROR_NONE                      Success
     * @return  "Other error code"                      See ::VL53L0X_Error
     */
    VL53L0X_Error VL53L0X_get_offset_calibration_data_micro_meter(VL53L0X_DEV dev,
            int32_t *p_offset_calibration_data_micro_meter);
    /**
     * Set or over-hide part to part calibration offset
     * \sa VL53L0X_DataInit()   VL53L0X_GetOffsetCalibrationDataMicroMeter()
     *
     * @note This function Access to the device
     *
     * @param   dev                                      Device Handle
     * @param   p_offset_calibration_data_micro_meter    Offset (microns)
     * @return  VL53L0X_ERROR_NONE                       Success
     * @return  "Other error code"                       See ::VL53L0X_Error
     */
    VL53L0X_Error VL53L0X_set_offset_calibration_data_micro_meter(VL53L0X_DEV dev,
            int32_t offset_calibration_data_micro_meter);

    VL53L0X_Error VL53L0X_perform_offset_calibration(VL53L0X_DEV dev,
            FixPoint1616_t cal_distance_milli_meter,
            int32_t *p_offset_micro_meter);

    VL53L0X_Error VL53L0X_perform_xtalk_calibration(VL53L0X_DEV dev,
            FixPoint1616_t xtalk_cal_distance,
            FixPoint1616_t *p_xtalk_compensation_rate_mega_cps);

    /**
     * @brief Perform XTalk Measurement
     *
     * @details Measures the current cross talk from glass in front
     * of the sensor.
     * This functions performs a histogram measurement and uses the results
     * to measure the crosstalk. For the function to be successful, there
     * must be no target in front of the sensor.
     *
     * @warning This function is a blocking function
     *
     * @warning This function is not supported when the final range
     * vcsel clock period is set below 10 PCLKS.
     *
     * @note This function Access to the device
     *
     * @param   dev                    Device Handle
     * @param   timeout_ms             Histogram measurement duration.
     * @param   p_xtalk_per_spad       Output parameter containing the crosstalk
     *                                 measurement result, in MCPS/Spad.
     *                                 Format fixpoint 16:16.
     * @param   p_ambient_too_high     Output parameter which indicate that
     *                                 pXtalkPerSpad is not good if the Ambient
     *                                 is too high.
     * @return  VL53L0X_ERROR_NONE     Success
     * @return  VL53L0X_ERROR_INVALID_PARAMS vcsel clock period not supported
     *                                 for this operation.
     *                                 Must not be less than 10PCLKS.
     * @return  "Other error code"     See ::VL53L0X_Error
     */
    VL53L0X_Error VL53L0X_perform_xtalk_measurement(VL53L0X_DEV dev,
            uint32_t timeout_ms, FixPoint1616_t *p_xtalk_per_spad,
            uint8_t *p_ambient_too_high);

    /**
     * @brief Enable/Disable Cross talk compensation feature
     *
     * @note This function is not Implemented.
     * Enable/Disable Cross Talk by set to zero the Cross Talk value
     * by using @a VL53L0X_SetXTalkCompensationRateMegaCps().
     *
     * @param  dev                           Device Handle
     * @param  x_talk_compensation_enable    Cross talk compensation
     *                                       to be set 0=disabled else = enabled
     * @return VL53L0X_ERROR_NOT_IMPLEMENTED Not implemented
     */
    VL53L0X_Error VL53L0X_set_x_talk_compensation_enable(VL53L0X_DEV dev,
            uint8_t x_talk_compensation_enable);

    /**
     * @brief Get Cross talk compensation rate
     *
     * @note This function is not Implemented.
     * Enable/Disable Cross Talk by set to zero the Cross Talk value by
     * using @a VL53L0X_SetXTalkCompensationRateMegaCps().
     *
     * @param   dev                           Device Handle
     * @param   p_x_talk_compensation_enable  Pointer to the Cross talk compensation
     *                                        state 0=disabled or 1 = enabled
     * @return  VL53L0X_ERROR_NOT_IMPLEMENTED Not implemented
     */
    VL53L0X_Error VL53L0X_get_x_talk_compensation_enable(VL53L0X_DEV dev,
            uint8_t *p_x_talk_compensation_enable);
    /**
     * @brief Set Cross talk compensation rate
     *
     * @par Function Description
     * Set Cross talk compensation rate.
     *
     * @note This function Access to the device
     *
     * @param   dev                                 Device Handle
     * @param   x_talk_compensation_rate_mega_cps   Compensation rate in
     *                                              Mega counts per second
     *                                              (16.16 fix point) see
     *                                              datasheet for details
     * @return  VL53L0X_ERROR_NONE                  Success
     * @return  "Other error code"                  See ::VL53L0X_Error
     */
    VL53L0X_Error VL53L0X_set_x_talk_compensation_rate_mega_cps(VL53L0X_DEV dev,
            FixPoint1616_t x_talk_compensation_rate_mega_cps);

    /**
     * @brief Get Cross talk compensation rate
     *
     * @par Function Description
     * Get Cross talk compensation rate.
     *
     * @note This function Access to the device
     *
     * @param   dev                                 Device Handle
     * @param   p_xtalk_compensation_rate_mega_cps  Pointer to Compensation rate
     *                                              in Mega counts per second
     *                                              (16.16 fix point) see
     *                                              datasheet for details
     * @return  VL53L0X_ERROR_NONE                  Success
     * @return  "Other error code"                  See ::VL53L0X_Error
     */
    VL53L0X_Error VL53L0X_get_x_talk_compensation_rate_mega_cps(VL53L0X_DEV dev,
            FixPoint1616_t *p_xtalk_compensation_rate_mega_cps);

    /**
     * @brief  Set a new device mode
     * @par Function Description
     * Set device to a new mode (ranging, histogram ...)
     *
     * @note This function doesn't Access to the device
     *
     * @param   dev                   Device Handle
     * @param   device_mode           New device mode to apply
     *                                Valid values are:
     *                                VL53L0X_DEVICEMODE_SINGLE_RANGING
     *                                VL53L0X_DEVICEMODE_CONTINUOUS_RANGING
     *                                VL53L0X_DEVICEMODE_CONTINUOUS_TIMED_RANGING
     *                                VL53L0X_DEVICEMODE_SINGLE_HISTOGRAM
     *                                VL53L0X_HISTOGRAMMODE_REFERENCE_ONLY
     *                                VL53L0X_HISTOGRAMMODE_RETURN_ONLY
     *                                VL53L0X_HISTOGRAMMODE_BOTH
     *
     *
     * @return  VL53L0X_ERROR_NONE               Success
     * @return  VL53L0X_ERROR_MODE_NOT_SUPPORTED This error occurs when
     *                                           DeviceMode is not in the
     *                                           supported list
     */
    VL53L0X_Error VL53L0X_set_device_mode(VL53L0X_DEV dev, VL53L0X_DeviceModes device_mode);

    /**
     * @brief  Get current new device mode
     * @par Function Description
     * Get actual mode of the device(ranging, histogram ...)
     *
     * @note This function doesn't Access to the device
     *
     * @param   dev                   Device Handle
     * @param   p_device_mode         Pointer to current apply mode value
     *                                Valid values are:
     *                                VL53L0X_DEVICEMODE_SINGLE_RANGING
     *                                VL53L0X_DEVICEMODE_CONTINUOUS_RANGING
     *                                VL53L0X_DEVICEMODE_CONTINUOUS_TIMED_RANGING
     *                                VL53L0X_DEVICEMODE_SINGLE_HISTOGRAM
     *                                VL53L0X_HISTOGRAMMODE_REFERENCE_ONLY
     *                                VL53L0X_HISTOGRAMMODE_RETURN_ONLY
     *                                VL53L0X_HISTOGRAMMODE_BOTH
     *
     * @return  VL53L0X_ERROR_NONE                   Success
     * @return  VL53L0X_ERROR_MODE_NOT_SUPPORTED     This error occurs when
     *                                               DeviceMode is not in the
     *                                               supported list
     */
    VL53L0X_Error VL53L0X_get_device_mode(VL53L0X_DEV dev,
                                          VL53L0X_DeviceModes *p_device_mode);

    /**
    * @brief Get current configuration for GPIO pin for a given device
    *
    * @note This function Access to the device
    *
    * @param   dev                   Device Handle
    * @param   pin                   ID of the GPIO Pin
    * @param   p_device_mode         Pointer to Device Mode associated to the Gpio.
    * @param   p_functionality       Pointer to Pin functionality.
    *                                Refer to ::VL53L0X_GpioFunctionality
    * @param   p_polarity            Pointer to interrupt polarity.
    *                                Active high or active low see
    *                                ::VL53L0X_InterruptPolarity
    * @return  VL53L0X_ERROR_NONE    Success
    * @return  VL53L0X_ERROR_GPIO_NOT_EXISTING           Only Pin=0 is accepted.
    * @return  VL53L0X_ERROR_GPIO_FUNCTIONALITY_NOT_SUPPORTED
    *          This error occurs
    *          when Funcionality programmed is not in the supported list:
    *                      Supported value are:
    *                      VL53L0X_GPIOFUNCTIONALITY_OFF,
    *                      VL53L0X_GPIOFUNCTIONALITY_THRESHOLD_CROSSED_LOW,
    *                      VL53L0X_GPIOFUNCTIONALITY_THRESHOLD_CROSSED_HIGH,
    *                      VL53L0X_GPIOFUNCTIONALITY_THRESHOLD_CROSSED_OUT,
    *                      VL53L0X_GPIOFUNCTIONALITY_NEW_MEASURE_READY
    * @return  "Other error code"    See ::VL53L0X_Error
    */
    VL53L0X_Error VL53L0X_get_gpio_config(VL53L0X_DEV dev, uint8_t pin,
                                          VL53L0X_DeviceModes *p_device_mode,
                                          VL53L0X_GpioFunctionality *p_functionality,
                                          VL53L0X_InterruptPolarity *p_polarity);

    /**
     * @brief Set the configuration of GPIO pin for a given device
     *
     * @note This function Access to the device
     *
     * @param   dev                   Device Handle
     * @param   pin                   ID of the GPIO Pin
     * @param   functionality         Select Pin functionality.
     *  Refer to ::VL53L0X_GpioFunctionality
     * @param   device_mode            Device Mode associated to the Gpio.
     * @param   polarity              Set interrupt polarity. Active high
     *   or active low see ::VL53L0X_InterruptPolarity
     * @return  VL53L0X_ERROR_NONE                            Success
     * @return  VL53L0X_ERROR_GPIO_NOT_EXISTING               Only Pin=0 is accepted.
     * @return  VL53L0X_ERROR_GPIO_FUNCTIONALITY_NOT_SUPPORTED    This error occurs
     * when Functionality programmed is not in the supported list:
     *                             Supported value are:
     *                             VL53L0X_GPIOFUNCTIONALITY_OFF,
     *                             VL53L0X_GPIOFUNCTIONALITY_THRESHOLD_CROSSED_LOW,
     *                             VL53L0X_GPIOFUNCTIONALITY_THRESHOLD_CROSSED_HIGH,
     VL53L0X_GPIOFUNCTIONALITY_THRESHOLD_CROSSED_OUT,
     *                               VL53L0X_GPIOFUNCTIONALITY_NEW_MEASURE_READY
     * @return  "Other error code"    See ::VL53L0X_Error
     */
    VL53L0X_Error VL53L0X_set_gpio_config(VL53L0X_DEV dev, uint8_t pin,
                                          VL53L0X_DeviceModes device_mode, VL53L0X_GpioFunctionality functionality,
                                          VL53L0X_InterruptPolarity polarity);

    /**
     * @brief Start device measurement
     *
     * @details Started measurement will depend on device parameters set through
     * @a VL53L0X_SetParameters()
     * This is a non-blocking function.
     * This function will change the VL53L0X_State from VL53L0X_STATE_IDLE to
     * VL53L0X_STATE_RUNNING.
     *
     * @note This function Access to the device
     *

     * @param   dev                  Device Handle
     * @return  VL53L0X_ERROR_NONE                  Success
     * @return  VL53L0X_ERROR_MODE_NOT_SUPPORTED    This error occurs when
     * DeviceMode programmed with @a VL53L0X_SetDeviceMode is not in the supported
     * list:
     *                                   Supported mode are:
     *                                   VL53L0X_DEVICEMODE_SINGLE_RANGING,
     *                                   VL53L0X_DEVICEMODE_CONTINUOUS_RANGING,
     *                                   VL53L0X_DEVICEMODE_CONTINUOUS_TIMED_RANGING
     * @return  VL53L0X_ERROR_TIME_OUT    Time out on start measurement
     * @return  "Other error code"   See ::VL53L0X_Error
     */
    VL53L0X_Error VL53L0X_start_measurement(VL53L0X_DEV dev);

    /**
     * @brief Stop device measurement
     *
     * @details Will set the device in standby mode at end of current measurement\n
     *          Not necessary in single mode as device shall return automatically
     *          in standby mode at end of measurement.
     *          This function will change the VL53L0X_State from VL53L0X_STATE_RUNNING
     *          to VL53L0X_STATE_IDLE.
     *
     * @note This function Access to the device
     *
     * @param   dev                  Device Handle
     * @return  VL53L0X_ERROR_NONE    Success
     * @return  "Other error code"   See ::VL53L0X_Error
     */
    VL53L0X_Error VL53L0X_stop_measurement(VL53L0X_DEV dev);

    /**
     * @brief Return device stop completion status
     *
     * @par Function Description
     * Returns stop completiob status.
     * User shall call this function after a stop command
     *
     * @note This function Access to the device
     *
     * @param   dev                    Device Handle
     * @param   p_stop_status            Pointer to status variable to update
     * @return  VL53L0X_ERROR_NONE      Success
     * @return  "Other error code"     See ::VL53L0X_Error
     */
    VL53L0X_Error VL53L0X_get_stop_completed_status(VL53L0X_DEV dev,
            uint32_t *p_stop_status);

    /**
     * @brief Return Measurement Data Ready
     *
     * @par Function Description
     * This function indicate that a measurement data is ready.
     * This function check if interrupt mode is used then check is done accordingly.
     * If perform function clear the interrupt, this function will not work,
     * like in case of @a VL53L0X_PerformSingleRangingMeasurement().
     * The previous function is blocking function, VL53L0X_GetMeasurementDataReady
     * is used for non-blocking capture.
     *
     * @note This function Access to the device
     *
     * @param   dev                    Device Handle
     * @param   p_measurement_data_ready  Pointer to Measurement Data Ready.
     *  0=data not ready, 1 = data ready
     * @return  VL53L0X_ERROR_NONE      Success
     * @return  "Other error code"     See ::VL53L0X_Error
     */
    VL53L0X_Error VL53L0X_get_measurement_data_ready(VL53L0X_DEV dev,
            uint8_t *p_measurement_data_ready);

    /**
     * @brief Retrieve the measurements from device for a given setup
     *
     * @par Function Description
     * Get data from last successful Ranging measurement
     * @warning USER should take care about  @a VL53L0X_GetNumberOfROIZones()
     * before get data.
     * PAL will fill a NumberOfROIZones times the corresponding data
     * structure used in the measurement function.
     *
     * @note This function Access to the device
     *
     * @param   dev                      Device Handle
     * @param   p_ranging_measurement_data  Pointer to the data structure to fill up.
     * @return  VL53L0X_ERROR_NONE        Success
     * @return  "Other error code"       See ::VL53L0X_Error
     */
    VL53L0X_Error VL53L0X_get_ranging_measurement_data(VL53L0X_DEV dev,
            VL53L0X_RangingMeasurementData_t *p_ranging_measurement_data);

    /**
     * @brief Clear given system interrupt condition
     *
     * @par Function Description
     * Clear given interrupt(s).
     *
     * @note This function Access to the device
     *
     * @param   dev                  Device Handle
     * @param   interrupt_mask        Mask of interrupts to clear
     * @return  VL53L0X_ERROR_NONE    Success
     * @return  VL53L0X_ERROR_INTERRUPT_NOT_CLEARED    Cannot clear interrupts
     *
     * @return  "Other error code"   See ::VL53L0X_Error
     */
    VL53L0X_Error VL53L0X_clear_interrupt_mask(VL53L0X_DEV dev, uint32_t interrupt_mask);

    /**
     * @brief Return device interrupt status
     *
     * @par Function Description
     * Returns currently raised interrupts by the device.
     * User shall be able to activate/deactivate interrupts through
     * @a VL53L0X_SetGpioConfig()
     *
     * @note This function Access to the device
     *
     * @param   dev                    Device Handle
     * @param   p_interrupt_mask_status   Pointer to status variable to update
     * @return  VL53L0X_ERROR_NONE      Success
     * @return  "Other error code"     See ::VL53L0X_Error
     */
    VL53L0X_Error VL53L0X_get_interrupt_mask_status(VL53L0X_DEV dev,
            uint32_t *p_interrupt_mask_status);

    /**
     * @brief Performs a single ranging measurement and retrieve the ranging
     * measurement data
     *
     * @par Function Description
     * This function will change the device mode to VL53L0X_DEVICEMODE_SINGLE_RANGING
     * with @a VL53L0X_SetDeviceMode(),
     * It performs measurement with @a VL53L0X_PerformSingleMeasurement()
     * It get data from last successful Ranging measurement with
     * @a VL53L0X_GetRangingMeasurementData.
     * Finally it clear the interrupt with @a VL53L0X_ClearInterruptMask().
     *
     * @note This function Access to the device
     *
     * @note This function change the device mode to
     * VL53L0X_DEVICEMODE_SINGLE_RANGING
     *
     * @param   dev                       Device Handle
     * @param   p_ranging_measurement_data   Pointer to the data structure to fill up.
     * @return  VL53L0X_ERROR_NONE         Success
     * @return  "Other error code"        See ::VL53L0X_Error
     */
    VL53L0X_Error VL53L0X_perform_single_ranging_measurement(VL53L0X_DEV dev,
            VL53L0X_RangingMeasurementData_t *p_ranging_measurement_data);

    /**
     * @brief Single shot measurement.
     *
     * @par Function Description
     * Perform simple measurement sequence (Start measure, Wait measure to end,
     * and returns when measurement is done).
     * Once function returns, user can get valid data by calling
     * VL53L0X_GetRangingMeasurement or VL53L0X_GetHistogramMeasurement
     * depending on defined measurement mode
     * User should Clear the interrupt in case this are enabled by using the
     * function VL53L0X_ClearInterruptMask().
     *
     * @warning This function is a blocking function
     *
     * @note This function Access to the device
     *
     * @param   dev                  Device Handle
     * @return  VL53L0X_ERROR_NONE    Success
     * @return  "Other error code"   See ::VL53L0X_Error
     */
    VL53L0X_Error VL53L0X_perform_single_measurement(VL53L0X_DEV dev);

    /**
    * @brief Read current status of the error register for the selected device
    *
    * @note This function Access to the device
    *
    * @param   dev                   Device Handle
    * @param   p_device_error_status    Pointer to current error code of the device
    * @return  VL53L0X_ERROR_NONE     Success
    * @return  "Other error code"    See ::VL53L0X_Error
    */
    VL53L0X_Error VL53L0X_get_device_error_status(VL53L0X_DEV dev,
            VL53L0X_DeviceError *p_device_error_status);

    /**
    * @brief Human readable error string for a given Error Code
    *
    * @note This function doesn't access to the device
    *
    * @param   error_code           The error code as stored on ::VL53L0X_DeviceError
    * @param   p_device_error_string  The error string corresponding to the ErrorCode
    * @return  VL53L0X_ERROR_NONE   Success
    * @return  "Other error code"  See ::VL53L0X_Error
    */
    VL53L0X_Error VL53L0X_get_device_error_string(
        VL53L0X_DeviceError error_code, char *p_device_error_string);

    /**
     * @brief Human readable Range Status string for a given RangeStatus
     *
     * @note This function doesn't access to the device
     *
     * @param   range_status         The RangeStatus code as stored on
     * @a VL53L0X_RangingMeasurementData_t
     * @param   p_range_status_string  The returned RangeStatus string.
     * @return  VL53L0X_ERROR_NONE   Success
     * @return  "Other error code"  See ::VL53L0X_Error
     */
    VL53L0X_Error VL53L0X_get_range_status_string(uint8_t range_status,
            char *p_range_status_string);

    VL53L0X_Error VL53L0X_get_total_signal_rate(VL53L0X_DEV dev,
            VL53L0X_RangingMeasurementData_t *p_ranging_measurement_data,
            FixPoint1616_t *p_total_signal_rate_mcps);

    VL53L0X_Error VL53L0X_get_total_xtalk_rate(VL53L0X_DEV dev,
            VL53L0X_RangingMeasurementData_t *p_ranging_measurement_data,
            FixPoint1616_t *p_total_xtalk_rate_mcps);

    /**
     * @brief Get Ranging Timing Budget in microseconds
     *
     * @par Function Description
     * Returns the programmed the maximum time allowed by the user to the
     * device to run a full ranging sequence for the current mode
     * (ranging, histogram, ASL ...)
     *
     * @note This function Access to the device
     *
     * @param   dev                                    Device Handle
     * @param   p_measurement_timing_budget_micro_seconds   Max measurement time in
     * microseconds.
     *                                   Valid values are:
     *                                   >= 17000 microsecs when wraparound enabled
     *                                   >= 12000 microsecs when wraparound disabled
     * @return  VL53L0X_ERROR_NONE                      Success
     * @return  "Other error code"                     See ::VL53L0X_Error
     */
    VL53L0X_Error VL53L0X_get_measurement_timing_budget_micro_seconds(VL53L0X_DEV dev,
            uint32_t *p_measurement_timing_budget_micro_seconds);

    /**
     * @brief Set Ranging Timing Budget in microseconds
     *
     * @par Function Description
     * Defines the maximum time allowed by the user to the device to run a
     * full ranging sequence for the current mode (ranging, histogram, ASL ...)
     *
     * @note This function Access to the device
     *
     * @param   dev                                Device Handle
     * @param measurement_timing_budget_micro_seconds  Max measurement time in
     * microseconds.
     *                                   Valid values are:
     *                                   >= 17000 microsecs when wraparound enabled
     *                                   >= 12000 microsecs when wraparound disabled
     * @return  VL53L0X_ERROR_NONE             Success
     * @return  VL53L0X_ERROR_INVALID_PARAMS   This error is returned if
     MeasurementTimingBudgetMicroSeconds out of range
     * @return  "Other error code"            See ::VL53L0X_Error
     */
    VL53L0X_Error VL53L0X_set_measurement_timing_budget_micro_seconds(VL53L0X_DEV dev,
            uint32_t measurement_timing_budget_micro_seconds);

    /**
     * @brief  Get specific limit check enable state
     *
     * @par Function Description
     * This function get the enable state of a specific limit check.
     * The limit check is identified with the LimitCheckId.
     *
     * @note This function Access to the device
     *
     * @param   dev                           Device Handle
     * @param   limit_check_id                  Limit Check ID
     *  (0<= LimitCheckId < VL53L0X_GetNumberOfLimitCheck() ).
     * @param   p_limit_check_enable             Pointer to the check limit enable
     * value.
     *  if 1 the check limit
     *        corresponding to LimitCheckId is Enabled
     *  if 0 the check limit
     *        corresponding to LimitCheckId is disabled
     * @return  VL53L0X_ERROR_NONE             Success
     * @return  VL53L0X_ERROR_INVALID_PARAMS   This error is returned
     *  when LimitCheckId value is out of range.
     * @return  "Other error code"            See ::VL53L0X_Error
     */
    VL53L0X_Error VL53L0X_get_limit_check_enable(VL53L0X_DEV dev, uint16_t limit_check_id,
            uint8_t *p_limit_check_enable);

    /**
     * @brief  Enable/Disable a specific limit check
     *
     * @par Function Description
     * This function Enable/Disable a specific limit check.
     * The limit check is identified with the LimitCheckId.
     *
     * @note This function doesn't Access to the device
     *
     * @param   dev                           Device Handle
     * @param   limit_check_id                  Limit Check ID
     *  (0<= LimitCheckId < VL53L0X_GetNumberOfLimitCheck() ).
     * @param   limit_check_enable              if 1 the check limit
     *  corresponding to LimitCheckId is Enabled
     *                                        if 0 the check limit
     *  corresponding to LimitCheckId is disabled
     * @return  VL53L0X_ERROR_NONE             Success
     * @return  VL53L0X_ERROR_INVALID_PARAMS   This error is returned
     *  when LimitCheckId value is out of range.
     * @return  "Other error code"            See ::VL53L0X_Error
     */
    VL53L0X_Error VL53L0X_set_limit_check_enable(VL53L0X_DEV dev, uint16_t limit_check_id,
            uint8_t limit_check_enable);

    /**
     * @brief  Get a specific limit check value
     *
     * @par Function Description
     * This function get a specific limit check value from device then it updates
     * internal values and check enables.
     * The limit check is identified with the LimitCheckId.
     *
     * @note This function Access to the device
     *
     * @param   dev                           Device Handle
     * @param   limit_check_id                  Limit Check ID
     *  (0<= LimitCheckId < VL53L0X_GetNumberOfLimitCheck() ).
     * @param   p_limit_check_value              Pointer to Limit
     *  check Value for a given LimitCheckId.
     * @return  VL53L0X_ERROR_NONE             Success
     * @return  VL53L0X_ERROR_INVALID_PARAMS   This error is returned
     *  when LimitCheckId value is out of range.
     * @return  "Other error code"            See ::VL53L0X_Error
     */
    VL53L0X_Error VL53L0X_get_limit_check_value(VL53L0X_DEV dev, uint16_t limit_check_id,
            FixPoint1616_t *p_limit_check_value);

    /**
     * @brief  Set a specific limit check value
     *
     * @par Function Description
     * This function set a specific limit check value.
     * The limit check is identified with the LimitCheckId.
     *
     * @note This function Access to the device
     *
     * @param   dev                           Device Handle
     * @param   limit_check_id                  Limit Check ID
     *  (0<= LimitCheckId < VL53L0X_GetNumberOfLimitCheck() ).
     * @param   limit_check_value               Limit check Value for a given
     * LimitCheckId
     * @return  VL53L0X_ERROR_NONE             Success
     * @return  VL53L0X_ERROR_INVALID_PARAMS   This error is returned when either
     *  LimitCheckId or LimitCheckValue value is out of range.
     * @return  "Other error code"            See ::VL53L0X_Error
     */
    VL53L0X_Error VL53L0X_set_limit_check_value(VL53L0X_DEV dev, uint16_t limit_check_id,
            FixPoint1616_t limit_check_value);

    /**
     * @brief  Get the current value of the signal used for the limit check
     *
     * @par Function Description
     * This function get a the current value of the signal used for the limit check.
     * To obtain the latest value you should run a ranging before.
     * The value reported is linked to the limit check identified with the
     * LimitCheckId.
     *
     * @note This function Access to the device
     *
     * @param   dev                           Device Handle
     * @param   limit_check_id                  Limit Check ID
     *  (0<= LimitCheckId < VL53L0X_GetNumberOfLimitCheck() ).
     * @param   p_limit_check_current            Pointer to current Value for a
     * given LimitCheckId.
     * @return  VL53L0X_ERROR_NONE             Success
     * @return  VL53L0X_ERROR_INVALID_PARAMS   This error is returned when
     * LimitCheckId value is out of range.
     * @return  "Other error code"            See ::VL53L0X_Error
     */
    VL53L0X_Error VL53L0X_get_limit_check_current(VL53L0X_DEV dev, uint16_t limit_check_id,
            FixPoint1616_t *p_limit_check_current);

    /**
     * @brief  Return a the Status of the specified check limit
     *
     * @par Function Description
     * This function returns the Status of the specified check limit.
     * The value indicate if the check is fail or not.
     * The limit check is identified with the LimitCheckId.
     *
     * @note This function doesn't Access to the device
     *
     * @param   dev                           Device Handle
     * @param   limit_check_id                  Limit Check ID
     (0<= LimitCheckId < VL53L0X_GetNumberOfLimitCheck() ).
     * @param   p_limit_check_status             Pointer to the
     Limit Check Status of the given check limit.
     * LimitCheckStatus :
     * 0 the check is not fail
     * 1 the check if fail or not enabled
     *
     * @return  VL53L0X_ERROR_NONE             Success
     * @return  VL53L0X_ERROR_INVALID_PARAMS   This error is
     returned when LimitCheckId value is out of range.
     * @return  "Other error code"            See ::VL53L0X_Error
     */
    VL53L0X_Error VL53L0X_get_limit_check_status(VL53L0X_DEV dev,
            uint16_t limit_check_id, uint8_t *p_limit_check_status);

    /**
     * Get continuous mode Inter-Measurement period in milliseconds
     *
     * @par Function Description
     * When trying to set too short time return  INVALID_PARAMS minimal value
     *
     * @note This function Access to the device
     *
     * @param   dev                                  Device Handle
     * @param   p_inter_measurement_period_milli_seconds  Pointer to programmed
     *  Inter-Measurement Period in milliseconds.
     * @return  VL53L0X_ERROR_NONE                    Success
     * @return  "Other error code"                   See ::VL53L0X_Error
     */
    VL53L0X_Error VL53L0X_get_inter_measurement_period_milli_seconds(VL53L0X_DEV dev,
            uint32_t *p_inter_measurement_period_milli_seconds);

    /**
     * Program continuous mode Inter-Measurement period in milliseconds
     *
     * @par Function Description
     * When trying to set too short time return  INVALID_PARAMS minimal value
     *
     * @note This function Access to the device
     *
     * @param   dev                                  Device Handle
     * @param   inter_measurement_period_milli_seconds   Inter-Measurement Period in ms.
     * @return  VL53L0X_ERROR_NONE                    Success
     * @return  "Other error code"                   See ::VL53L0X_Error
     */
    VL53L0X_Error VL53L0X_set_inter_measurement_period_milli_seconds(
        VL53L0X_DEV dev, uint32_t inter_measurement_period_milli_seconds);

    /**
     * @brief Set new device address
     *
     * After completion the device will answer to the new address programmed.
     * This function should be called when several devices are used in parallel
     * before start programming the sensor.
     * When a single device us used, there is no need to call this function.
     *
     * @note This function Access to the device
     *
     * @param   dev                   Device Handle
     * @param   device_address         The new Device address
     * @return  VL53L0X_ERROR_NONE     Success
     * @return  "Other error code"    See ::VL53L0X_Error
     */
    VL53L0X_Error VL53L0X_set_device_address(VL53L0X_DEV dev, uint8_t device_address);

    /**
     * @brief Do an hard reset or soft reset (depending on implementation) of the
     * device \nAfter call of this function, device must be in same state as right
     * after a power-up sequence.This function will change the VL53L0X_State to
     * VL53L0X_STATE_POWERDOWN.
     *
     * @note This function Access to the device
     *
     * @param   dev                   Device Handle
     * @return  VL53L0X_ERROR_NONE     Success
     * @return  "Other error code"    See ::VL53L0X_Error
     */
    VL53L0X_Error VL53L0X_reset_device(VL53L0X_DEV dev);

    /**
     * @brief  Get setup of Wrap around Check
     *
     * @par Function Description
     * This function get the wrapAround check enable parameters
     *
     * @note This function Access to the device
     *
     * @param   dev                     Device Handle
     * @param   p_wrap_around_check_enable  Pointer to the Wrap around Check state
     *                                  0=disabled or 1 = enabled
     * @return  VL53L0X_ERROR_NONE       Success
     * @return  "Other error code"      See ::VL53L0X_Error
     */
    VL53L0X_Error VL53L0X_get_wrap_around_check_enable(VL53L0X_DEV dev,
            uint8_t *p_wrap_around_check_enable);

    /**
     * @brief  Enable (or disable) Wrap around Check
     *
     * @note This function Access to the device
     *
     * @param   dev                    Device Handle
     * @param   wrap_around_check_enable  Wrap around Check to be set
     *                                 0=disabled, other = enabled
     * @return  VL53L0X_ERROR_NONE      Success
     * @return  "Other error code"     See ::VL53L0X_Error
     */
    VL53L0X_Error VL53L0X_set_wrap_around_check_enable(VL53L0X_DEV dev,
            uint8_t wrap_around_check_enable);

    /**
     * @brief Gets the VCSEL pulse period.
     *
     * @par Function Description
     * This function retrieves the VCSEL pulse period for the given period type.
     *
     * @note This function Accesses the device
     *
     * @param   dev                      Device Handle
     * @param   vcsel_period_type          VCSEL period identifier (pre-range|final).
     * @param   p_vcsel_pulse_period_pclk        Pointer to VCSEL period value.
     * @return  VL53L0X_ERROR_NONE        Success
     * @return  VL53L0X_ERROR_INVALID_PARAMS  Error VcselPeriodType parameter not
     *                                       supported.
     * @return  "Other error code"           See ::VL53L0X_Error
     */
    VL53L0X_Error VL53L0X_get_vcsel_pulse_period(VL53L0X_DEV dev,
            VL53L0X_VcselPeriod vcsel_period_type, uint8_t *p_vcsel_pulse_period_pclk);

    /**
     * @brief Sets the VCSEL pulse period.
     *
     * @par Function Description
     * This function retrieves the VCSEL pulse period for the given period type.
     *
     * @note This function Accesses the device
     *
     * @param   dev                       Device Handle
     * @param   vcsel_period_type	      VCSEL period identifier (pre-range|final).
     * @param   vcsel_pulse_period          VCSEL period value
     * @return  VL53L0X_ERROR_NONE            Success
     * @return  VL53L0X_ERROR_INVALID_PARAMS  Error VcselPeriodType parameter not
     *                                       supported.
     * @return  "Other error code"           See ::VL53L0X_Error
     */
    VL53L0X_Error VL53L0X_set_vcsel_pulse_period(VL53L0X_DEV dev,
            VL53L0X_VcselPeriod vcsel_period_type, uint8_t vcsel_pulse_period);

    /**
     * @brief Set low and high Interrupt thresholds for a given mode
     * (ranging, ALS, ...) for a given device
     *
     * @par Function Description
     * Set low and high Interrupt thresholds for a given mode (ranging, ALS, ...)
     * for a given device
     *
     * @note This function Access to the device
     *
     * @note DeviceMode is ignored for the current device
     *
     * @param   dev              Device Handle
     * @param   device_mode       Device Mode for which change thresholds
     * @param   threshold_low     Low threshold (mm, lux ..., depending on the mode)
     * @param   threshold_high    High threshold (mm, lux ..., depending on the mode)
     * @return  VL53L0X_ERROR_NONE    Success
     * @return  "Other error code"   See ::VL53L0X_Error
     */
    VL53L0X_Error VL53L0X_set_interrupt_thresholds(VL53L0X_DEV dev,
            VL53L0X_DeviceModes device_mode, FixPoint1616_t threshold_low,
            FixPoint1616_t threshold_high);

    /**
     * @brief  Get high and low Interrupt thresholds for a given mode
     *  (ranging, ALS, ...) for a given device
     *
     * @par Function Description
     * Get high and low Interrupt thresholds for a given mode (ranging, ALS, ...)
     * for a given device
     *
     * @note This function Access to the device
     *
     * @note DeviceMode is ignored for the current device
     *
     * @param   dev              Device Handle
     * @param   device_mode       Device Mode from which read thresholds
     * @param   p_threshold_low    Low threshold (mm, lux ..., depending on the mode)
     * @param   p_threshold_high   High threshold (mm, lux ..., depending on the mode)
     * @return  VL53L0X_ERROR_NONE   Success
     * @return  "Other error code"  See ::VL53L0X_Error
     */
    VL53L0X_Error VL53L0X_get_interrupt_thresholds(VL53L0X_DEV dev,
            VL53L0X_DeviceModes device_mode, FixPoint1616_t *p_threshold_low,
            FixPoint1616_t *p_threshold_high);

    /**
     * @brief Reads the Device information for given Device
     *
     * @note This function Access to the device
     *
     * @param   dev                 Device Handle
     * @param   p_VL53L0X_device_info  Pointer to current device info for a given
     *  Device
     * @return  VL53L0X_ERROR_NONE   Success
     * @return  "Other error code"  See ::VL53L0X_Error
     */
    VL53L0X_Error VL53L0X_get_device_info(VL53L0X_DEV dev,
                                          VL53L0X_DeviceInfo_t *p_VL53L0X_device_info);

    /**
     * @brief Gets the (on/off) state of all sequence steps.
     *
     * @par Function Description
     * This function retrieves the state of all sequence step in the scheduler.
     *
     * @note This function Accesses the device
     *
     * @param   dev                          Device Handle
     * @param   p_scheduler_sequence_steps      Pointer to struct containing result.
     * @return  VL53L0X_ERROR_NONE            Success
     * @return  "Other error code"           See ::VL53L0X_Error
     */
    VL53L0X_Error VL53L0X_get_sequence_step_enables(VL53L0X_DEV dev,
            VL53L0X_SchedulerSequenceSteps_t *p_scheduler_sequence_steps);

    /**
     * @brief Sets the (on/off) state of a requested sequence step.
     *
     * @par Function Description
     * This function enables/disables a requested sequence step.
     *
     * @note This function Accesses the device
     *
     * @param   dev                          Device Handle
     * @param   sequence_step_id	         Sequence step identifier.
     * @param   sequence_step_enabled          Demanded state {0=Off,1=On}
     *                                       is enabled.
     * @return  VL53L0X_ERROR_NONE            Success
     * @return  VL53L0X_ERROR_INVALID_PARAMS  Error SequenceStepId parameter not
     *                                       supported.
     * @return  "Other error code"           See ::VL53L0X_Error
     */
    VL53L0X_Error VL53L0X_set_sequence_step_enable(VL53L0X_DEV dev,
            VL53L0X_SequenceStepId sequence_step_id, uint8_t sequence_step_enabled);

    /**
     * @brief  Gets the fraction enable parameter indicating the resolution of
     * range measurements.
     *
     * @par Function Description
     * Gets the fraction enable state, which translates to the resolution of
     * range measurements as follows :Enabled:=0.25mm resolution,
     * Not Enabled:=1mm resolution.
     *
     * @note This function Accesses the device
     *
     * @param   dev               Device Handle
     * @param   p_enabled           Output Parameter reporting the fraction enable state.
     *
     * @return  VL53L0X_ERROR_NONE                   Success
     * @return  "Other error code"                  See ::VL53L0X_Error
     */
    VL53L0X_Error VL53L0X_get_fraction_enable(VL53L0X_DEV dev, uint8_t *p_enabled);

    /**
     * @brief  Sets the resolution of range measurements.
     * @par Function Description
     * Set resolution of range measurements to either 0.25mm if
     * fraction enabled or 1mm if not enabled.
     *
     * @note This function Accesses the device
     *
     * @param   dev               Device Handle
     * @param   enable            Enable high resolution
     *
     * @return  VL53L0X_ERROR_NONE               Success
     * @return  "Other error code"              See ::VL53L0X_Error
     */
    VL53L0X_Error VL53L0X_set_range_fraction_enable(VL53L0X_DEV dev,
            uint8_t enable);

    /**
    * @brief Return the VL53L0X PAL Implementation Version
    *
    * @note This function doesn't access to the device
    *
    * @param   p_version              Pointer to current PAL Implementation Version
    * @return  VL53L0X_ERROR_NONE     Success
    * @return  "Other error code"    See ::VL53L0X_Error
    */
    VL53L0X_Error VL53L0X_get_version(VL53L0X_Version_t *p_version);

    /**
     * @brief Reads the Product Revision for a for given Device
     * This function can be used to distinguish cut1.0 from cut1.1.
     *
     * @note This function Access to the device
     *
     * @param   dev                 Device Handle
     * @param   p_product_revision_major  Pointer to Product Revision Major
     * for a given Device
     * @param   p_product_revision_minor  Pointer to Product Revision Minor
     * for a given Device
     * @return  VL53L0X_ERROR_NONE      Success
     * @return  "Other error code"  See ::VL53L0X_Error
     */
    VL53L0X_Error VL53L0X_get_product_revision(VL53L0X_DEV dev,
            uint8_t *p_product_revision_major, uint8_t *p_product_revision_minor);

    /**
     * @brief  Retrieve current device parameters
     * @par Function Description
     * Get actual parameters of the device
     * @li Then start ranging operation.
     *
     * @note This function Access to the device
     *
     * @param   dev                   Device Handle
     * @param   p_device_parameters     Pointer to store current device parameters.
     * @return  VL53L0X_ERROR_NONE     Success
     * @return  "Other error code"    See ::VL53L0X_Error
     */
    VL53L0X_Error VL53L0X_get_device_parameters(VL53L0X_DEV dev,
            VL53L0X_DeviceParameters_t *p_device_parameters);

    /**
     * @brief Human readable error string for current PAL error status
     *
     * @note This function doesn't access to the device
     *
     * @param   pal_error_code       The error code as stored on @a VL53L0X_Error
     * @param   p_pal_error_string    The error string corresponding to the
     * PalErrorCode
     * @return  VL53L0X_ERROR_NONE  Success
     * @return  "Other error code" See ::VL53L0X_Error
     */
    VL53L0X_Error VL53L0X_get_pal_error_string(VL53L0X_Error pal_error_code,
            char *p_pal_error_string);

    /**
     * @brief Return the PAL Specification Version used for the current
     * implementation.
     *
     * @note This function doesn't access to the device
     *
     * @param   p_pal_spec_version       Pointer to current PAL Specification Version
     * @return  VL53L0X_ERROR_NONE        Success
     * @return  "Other error code"    See ::VL53L0X_Error
     */
    VL53L0X_Error VL53L0X_get_pal_spec_version(
        VL53L0X_Version_t *p_pal_spec_version);

    /**
     * @brief Reads the internal state of the PAL for a given Device
     *
     * @note This function doesn't access to the device
     *
     * @param   dev                   Device Handle
     * @param   p_pal_state             Pointer to current state of the PAL for a
     * given Device
     * @return  VL53L0X_ERROR_NONE     Success
     * @return  "Other error code"    See ::VL53L0X_Error
     */
    VL53L0X_Error VL53L0X_get_pal_state(VL53L0X_DEV dev,
                                        VL53L0X_State *p_pal_state);

    /**
     * @brief Human readable PAL State string
     *
     * @note This function doesn't access to the device
     *
     * @param   pal_state_code          The State code as stored on @a VL53L0X_State
     * @param   p_pal_state_string       The State string corresponding to the
     * PalStateCode
     * @return  VL53L0X_ERROR_NONE     Success
     * @return  "Other error code"    See ::VL53L0X_Error
     */
    VL53L0X_Error VL53L0X_get_pal_state_string(VL53L0X_State pal_state_code,
            char *p_pal_state_string);

    /*** End High level API ***/

