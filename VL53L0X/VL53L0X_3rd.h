
    /* api.h functions */

    /**
     * @brief Wait for device booted after chip enable (hardware standby)
     * This function can be run only when VL53L0X_State is VL53L0X_STATE_POWERDOWN.
     *
     * @note This function is not Implemented
     *
     * @param   dev      Device Handle
     * @return  VL53L0X_ERROR_NOT_IMPLEMENTED Not implemented
     *
     */
    VL53L0X_Error VL53L0X_wait_device_booted(VL53L0X_DEV dev);


    VL53L0X_Error sequence_step_enabled(VL53L0X_DEV dev,
                                        VL53L0X_SequenceStepId sequence_step_id, uint8_t sequence_config,
                                        uint8_t *p_sequence_step_enabled);

    VL53L0X_Error VL53L0X_check_and_load_interrupt_settings(VL53L0X_DEV dev,
            uint8_t start_not_stopflag);


    /* api_core.h functions */

    VL53L0X_Error VL53L0X_get_info_from_device(VL53L0X_DEV dev, uint8_t option);

    VL53L0X_Error VL53L0X_device_read_strobe(VL53L0X_DEV dev);

    VL53L0X_Error wrapped_VL53L0X_get_measurement_timing_budget_micro_seconds(VL53L0X_DEV dev,
            uint32_t *p_measurement_timing_budget_micro_seconds);

    VL53L0X_Error wrapped_VL53L0X_get_vcsel_pulse_period(VL53L0X_DEV dev,
            VL53L0X_VcselPeriod vcsel_period_type, uint8_t *p_vcsel_pulse_period_pclk);

    uint8_t VL53L0X_decode_vcsel_period(uint8_t vcsel_period_reg);

    uint32_t VL53L0X_decode_timeout(uint16_t encoded_timeout);

    uint32_t VL53L0X_calc_timeout_us(VL53L0X_DEV dev,
                                     uint16_t timeout_period_mclks,
                                     uint8_t vcsel_period_pclks);

    uint32_t VL53L0X_calc_macro_period_ps(VL53L0X_DEV dev, uint8_t vcsel_period_pclks);

    VL53L0X_Error VL53L0X_measurement_poll_for_completion(VL53L0X_DEV dev);

    VL53L0X_Error VL53L0X_load_tuning_settings(VL53L0X_DEV dev,
            uint8_t *p_tuning_setting_buffer);

    VL53L0X_Error VL53L0X_get_pal_range_status(VL53L0X_DEV dev,
            uint8_t device_range_status,
            FixPoint1616_t signal_rate,
            uint16_t effective_spad_rtn_count,
            VL53L0X_RangingMeasurementData_t *p_ranging_measurement_data,
            uint8_t *p_pal_range_status);
    VL53L0X_Error VL53L0X_calc_sigma_estimate(VL53L0X_DEV dev,
            VL53L0X_RangingMeasurementData_t *p_ranging_measurement_data,
            FixPoint1616_t *p_sigma_estimate,
            uint32_t *p_dmax_mm);
    uint32_t VL53L0X_calc_timeout_mclks(VL53L0X_DEV dev,
                                        uint32_t timeout_period_us,
                                        uint8_t vcsel_period_pclks);
    uint32_t VL53L0X_isqrt(uint32_t num);

    uint32_t VL53L0X_quadrature_sum(uint32_t a, uint32_t b);

    VL53L0X_Error VL53L0X_calc_dmax(
        VL53L0X_DEV dev,
        FixPoint1616_t total_signal_rate_mcps,
        FixPoint1616_t total_corr_signal_rate_mcps,
        FixPoint1616_t pw_mult,
        uint32_t sigma_estimate_p1,
        FixPoint1616_t sigma_estimate_p2,
        uint32_t peak_vcsel_duration_us,
        uint32_t *pd_max_mm);
    VL53L0X_Error wrapped_VL53L0X_set_measurement_timing_budget_micro_seconds(VL53L0X_DEV dev,
            uint32_t measurement_timing_budget_micro_seconds);
    VL53L0X_Error get_sequence_step_timeout(VL53L0X_DEV dev,
                                            VL53L0X_SequenceStepId sequence_step_id,
                                            uint32_t *p_time_out_micro_secs);
    VL53L0X_Error set_sequence_step_timeout(VL53L0X_DEV dev,
                                            VL53L0X_SequenceStepId sequence_step_id,
                                            uint32_t timeout_micro_secs);
    uint16_t VL53L0X_encode_timeout(uint32_t timeout_macro_clks);
    VL53L0X_Error wrapped_VL53L0X_set_vcsel_pulse_period(VL53L0X_DEV dev,
            VL53L0X_VcselPeriod vcsel_period_type, uint8_t vcsel_pulse_period_pclk);
    uint8_t lv53l0x_encode_vcsel_period(uint8_t vcsel_period_pclks);

    /* api_calibration.h functions */
    VL53L0X_Error VL53L0X_apply_offset_adjustment(VL53L0X_DEV dev);
    VL53L0X_Error wrapped_VL53L0X_get_offset_calibration_data_micro_meter(VL53L0X_DEV dev,
            int32_t *p_offset_calibration_data_micro_meter);
    VL53L0X_Error wrapped_VL53L0X_set_offset_calibration_data_micro_meter(VL53L0X_DEV dev,
            int32_t offset_calibration_data_micro_meter);
    VL53L0X_Error wrapped_VL53L0X_perform_ref_spad_management(VL53L0X_DEV dev,
            uint32_t *ref_spad_count,
            uint8_t *is_aperture_spads);
    VL53L0X_Error VL53L0X_perform_ref_calibration(VL53L0X_DEV dev,
            uint8_t *p_vhv_settings, uint8_t *p_phase_cal, uint8_t get_data_enable);
    VL53L0X_Error VL53L0X_perform_vhv_calibration(VL53L0X_DEV dev,
            uint8_t *p_vhv_settings, const uint8_t get_data_enable,
            const uint8_t restore_config);
    VL53L0X_Error VL53L0X_perform_single_ref_calibration(VL53L0X_DEV dev,
            uint8_t vhv_init_byte);
    VL53L0X_Error VL53L0X_ref_calibration_io(VL53L0X_DEV dev, uint8_t read_not_write,
            uint8_t vhv_settings, uint8_t phase_cal,
            uint8_t *p_vhv_settings, uint8_t *p_phase_cal,
            const uint8_t vhv_enable, const uint8_t phase_enable);
    VL53L0X_Error VL53L0X_perform_phase_calibration(VL53L0X_DEV dev,
            uint8_t *p_phase_cal, const uint8_t get_data_enable,
            const uint8_t restore_config);
    VL53L0X_Error enable_ref_spads(VL53L0X_DEV dev,
                                   uint8_t aperture_spads,
                                   uint8_t good_spad_array[],
                                   uint8_t spad_array[],
                                   uint32_t size,
                                   uint32_t start,
                                   uint32_t offset,
                                   uint32_t spad_count,
                                   uint32_t *p_last_spad);
    void get_next_good_spad(uint8_t good_spad_array[], uint32_t size,
                            uint32_t curr, int32_t *p_next);
    uint8_t is_aperture(uint32_t spad_index);
    VL53L0X_Error enable_spad_bit(uint8_t spad_array[], uint32_t size,
                                  uint32_t spad_index);
    VL53L0X_Error set_ref_spad_map(VL53L0X_DEV dev, uint8_t *p_ref_spad_array);
    VL53L0X_Error get_ref_spad_map(VL53L0X_DEV dev, uint8_t *p_ref_spad_array);
    VL53L0X_Error perform_ref_signal_measurement(VL53L0X_DEV dev,
            uint16_t *p_ref_signal_rate);
    VL53L0X_Error wrapped_VL53L0X_set_reference_spads(VL53L0X_DEV dev,
            uint32_t count, uint8_t is_aperture_spads);

    /* api_strings.h functions */
    VL53L0X_Error wrapped_VL53L0X_get_device_info(VL53L0X_DEV dev,
            VL53L0X_DeviceInfo_t *p_VL53L0X_device_info);
    VL53L0X_Error VL53L0X_check_part_used(VL53L0X_DEV dev,
                                          uint8_t *revision,
                                          VL53L0X_DeviceInfo_t *p_VL53L0X_device_info);

    /* Read function of the ID device */
    //   virtual int read_id();
    virtual int read_id(uint8_t *id);

    VL53L0X_Error wait_measurement_data_ready(VL53L0X_DEV dev);

    VL53L0X_Error wait_stop_completed(VL53L0X_DEV dev);

    /* Write and read functions from I2C */
    /**
     * Write single byte register
     * @param   dev       Device Handle
     * @param   index     The register index
     * @param   data      8 bit register data
     * @return  VL53L0X_ERROR_NONE        Success
     * @return  "Other error code"    See ::VL53L0X_Error
     */
    VL53L0X_Error VL53L0X_write_byte(VL53L0X_DEV dev, uint8_t index, uint8_t data);
    /**
     * Write word register
     * @param   dev       Device Handle
     * @param   index     The register index
     * @param   data      16 bit register data
     * @return  VL53L0X_ERROR_NONE        Success
     * @return  "Other error code"    See ::VL53L0X_Error
     */
    VL53L0X_Error VL53L0X_write_word(VL53L0X_DEV dev, uint8_t index, uint16_t data);
    /**
     * Write double word (4 byte) register
     * @param   dev       Device Handle
     * @param   index     The register index
     * @param   data      32 bit register data
     * @return  VL53L0X_ERROR_NONE        Success
     * @return  "Other error code"    See ::VL53L0X_Error
     */
    VL53L0X_Error VL53L0X_write_dword(VL53L0X_DEV dev, uint8_t index, uint32_t data);
    /**
     * Read single byte register
     * @param   dev       Device Handle
     * @param   index     The register index
     * @param   data      pointer to 8 bit data
     * @return  VL53L0X_ERROR_NONE        Success
     * @return  "Other error code"    See ::VL53L0X_Error
     */
    VL53L0X_Error VL53L0X_read_byte(VL53L0X_DEV dev, uint8_t index, uint8_t *p_data);
    /**
     * Read word (2byte) register
     * @param   dev       Device Handle
     * @param   index     The register index
     * @param   data      pointer to 16 bit data
     * @return  VL53L0X_ERROR_NONE        Success
     * @return  "Other error code"    See ::VL53L0X_Error
     */
    VL53L0X_Error VL53L0X_read_word(VL53L0X_DEV dev, uint8_t index, uint16_t *p_data);
    /**
     * Read dword (4byte) register
     * @param   dev       Device Handle
     * @param   index     The register index
     * @param   data      pointer to 32 bit data
     * @return  VL53L0X_ERROR_NONE        Success
     * @return  "Other error code"    See ::VL53L0X_Error
     */
    VL53L0X_Error VL53L0X_read_dword(VL53L0X_DEV dev, uint8_t index, uint32_t *p_data);
    /**
     * Threat safe Update (read/modify/write) single byte register
     *
     * Final_reg = (Initial_reg & and_data) |or_data
     *
     * @param   dev        Device Handle
     * @param   index      The register index
     * @param   and_data    8 bit and data
     * @param   or_data     8 bit or data
     * @return  VL53L0X_ERROR_NONE        Success
     * @return  "Other error code"    See ::VL53L0X_Error
     */
    VL53L0X_Error VL53L0X_update_byte(VL53L0X_DEV dev, uint8_t index, uint8_t and_data, uint8_t or_data);
    /**
     * Writes the supplied byte buffer to the device
     * @param   dev       Device Handle
     * @param   index     The register index
     * @param   p_data     Pointer to uint8_t buffer containing the data to be written
     * @param   count     Number of bytes in the supplied byte buffer
     * @return  VL53L0X_ERROR_NONE        Success
     * @return  "Other error code"    See ::VL53L0X_Error
     */
    VL53L0X_Error VL53L0X_write_multi(VL53L0X_DEV dev, uint8_t index, uint8_t *p_data, uint32_t count);
    /**
     * Reads the requested number of bytes from the device
     * @param   dev       Device Handle
     * @param   index     The register index
     * @param   p_data     Pointer to the uint8_t buffer to store read data
     * @param   count     Number of uint8_t's to read
     * @return  VL53L0X_ERROR_NONE        Success
     * @return  "Other error code"    See ::VL53L0X_Error
     */
    VL53L0X_Error VL53L0X_read_multi(VL53L0X_DEV dev, uint8_t index, uint8_t *p_data, uint32_t count);

    /**
     * @brief  Writes a buffer towards the I2C peripheral device.
     * @param  dev       Device Handle
     * @param  p_data pointer to the byte-array data to send
     * @param  number_of_bytes number of bytes to be written.
     * @retval 0 if ok,
     * @retval -1 if an I2C error has occured
     * @note   On some devices if NumByteToWrite is greater
     *         than one, the RegisterAddr must be masked correctly!
     */
    VL53L0X_Error VL53L0X_i2c_write(uint8_t dev, uint8_t index, uint8_t *p_data, uint16_t number_of_bytes);

    /**
     * @brief  Reads a buffer from the I2C peripheral device.
     * @param  dev       Device Handle
     * @param  p_data pointer to the byte-array to read data in to
     * @param  number_of_bytes number of bytes to be read.
     * @retval 0 if ok,
     * @retval -1 if an I2C error has occured
     * @note   On some devices if NumByteToWrite is greater
     *         than one, the RegisterAddr must be masked correctly!
     */
    VL53L0X_Error VL53L0X_i2c_read(uint8_t dev, uint8_t index, uint8_t *p_data, uint16_t number_of_bytes);

    /**
     * @brief execute delay in all polling API call
     *
     * A typical multi-thread or RTOs implementation is to sleep the task for some 5ms (with 100Hz max rate faster polling is not needed)
     * if nothing specific is need you can define it as an empty/void macro
     * @code
     * #define VL53L0X_PollingDelay(...) (void)0
     * @endcode
     * @param dev       Device Handle
     * @return  VL53L0X_ERROR_NONE        Success
     * @return  "Other error code"    See ::VL53L0X_Error
     */
    VL53L0X_Error VL53L0X_polling_delay(VL53L0X_DEV dev);   /* usually best implemented as a real function */

    int is_present()
    {
        int status;
        uint8_t id = 0;

        status = read_id(&id);
        if (status) {
            VL53L0X_ErrLog("Failed to read ID device. Device not present!\n\r");
        }
        return status;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    //Added functions                                                                                    //
    ///////////////////////////////////////////////////////////////////////////////////////////////////////

    /**
     * @brief  Cycle Power to Device
     *
     * @return status - status 0 = ok, 1 = error
     *
     */
    int32_t VL53L0X_cycle_power(void);

    uint8_t VL53L0X_encode_vcsel_period(uint8_t vcsel_period_pclks);

    VL53L0X_Error wrapped_VL53L0X_get_device_error_string(VL53L0X_DeviceError error_code,
            char *p_device_error_string);

    VL53L0X_Error wrapped_VL53L0X_get_limit_check_info(VL53L0X_DEV dev, uint16_t limit_check_id,
            char *p_limit_check_string);

    VL53L0X_Error wrapped_VL53L0X_get_pal_error_string(VL53L0X_Error pal_error_code,
            char *p_pal_error_string);

    VL53L0X_Error wrapped_VL53L0X_get_pal_state_string(VL53L0X_State pal_state_code,
            char *p_pal_state_string);

    VL53L0X_Error wrapped_VL53L0X_get_range_status_string(uint8_t range_status,
            char *p_range_status_string);

    VL53L0X_Error wrapped_VL53L0X_get_ref_calibration(VL53L0X_DEV dev,
            uint8_t *p_vhv_settings, uint8_t *p_phase_cal);


    VL53L0X_Error count_enabled_spads(uint8_t spad_array[],
                                      uint32_t byte_count, uint32_t max_spads,
                                      uint32_t *p_total_spads_enabled, uint8_t *p_is_aperture);

    VL53L0X_Error wrapped_VL53L0X_get_sequence_steps_info(VL53L0X_SequenceStepId sequence_step_id,
            char *p_sequence_steps_string);


    /**
     * @brief Gets the name of a given sequence step.
     *
     * @par Function Description
     * This function retrieves the name of sequence steps corresponding to
     * SequenceStepId.
     *
     * @note This function doesn't Accesses the device
     *
     * @param   sequence_step_id               Sequence step identifier.
     * @param   p_sequence_steps_string         Pointer to Info string
     *
     * @return  VL53L0X_ERROR_NONE            Success
     * @return  "Other error code"           See ::VL53L0X_Error
     */
    VL53L0X_Error VL53L0X_get_sequence_steps_info(VL53L0X_SequenceStepId sequence_step_id,
            char *p_sequence_steps_string);

    /**
    * @brief Get the frequency of the timer used for ranging results time stamps
    *
    * @param[out] p_timer_freq_hz : pointer for timer frequency
    *
    * @return status : 0 = ok, 1 = error
    *
    */
    int32_t VL53L0X_get_timer_frequency(int32_t *p_timer_freq_hz);

    /**
    * @brief Get the timer value in units of timer_freq_hz (see VL53L0X_get_timestamp_frequency())
    *
    * @param[out] p_timer_count : pointer for timer count value
    *
    * @return status : 0 = ok, 1 = error
    *
    */
    int32_t VL53L0X_get_timer_value(int32_t *p_timer_count);

    /**
    * @brief Configure ranging interrupt reported to system
    *
    * @note This function is not Implemented
    *
    * @param   dev                  Device Handle
    * @param   interrupt_mask         Mask of interrupt to Enable/disable
    *  (0:interrupt disabled or 1: interrupt enabled)
    * @return  VL53L0X_ERROR_NOT_IMPLEMENTED   Not implemented
    */
    VL53L0X_Error VL53L0X_enable_interrupt_mask(VL53L0X_DEV dev,
            uint32_t interrupt_mask);

    /**
     * @brief  Get Dmax Calibration Parameters for a given device
     *
     *
     * @note This function Access to the device
     *
     * @param   dev                     Device Handle
     * @param   p_range_milli_meter        Pointer to Calibration Distance
     * @param   p_signal_rate_rtn_mega_cps   Pointer to Signal rate return
     * @return  VL53L0X_ERROR_NONE       Success
     * @return  "Other error code"      See ::VL53L0X_Error
     */
    VL53L0X_Error VL53L0X_get_dmax_cal_parameters(VL53L0X_DEV dev,
            uint16_t *p_range_milli_meter, FixPoint1616_t *p_signal_rate_rtn_mega_cps);

    /**
    * @brief   Set Dmax Calibration Parameters for a given device
    * When one of the parameter is zero, this function will get parameter
    * from NVM.
    * @note This function doesn't Access to the device
    *
    * @param   dev                    Device Handle
    * @param   range_milli_meter        Calibration Distance
    * @param   signal_rate_rtn_mega_cps   Signal rate return read at CalDistance
    * @return  VL53L0X_ERROR_NONE      Success
    * @return  "Other error code"     See ::VL53L0X_Error
    */
    VL53L0X_Error VL53L0X_get_dmax_cal_parameters(VL53L0X_DEV dev,
            uint16_t range_milli_meter, FixPoint1616_t signal_rate_rtn_mega_cps);

    /**
    * @brief Retrieve the measurements from device for a given setup
    *
    * @par Function Description
    * Get data from last successful Histogram measurement
    * @warning USER should take care about  @a VL53L0X_GetNumberOfROIZones()
    * before get data.
    * PAL will fill a NumberOfROIZones times the corresponding data structure
    * used in the measurement function.
    *
    * @note This function is not Implemented
    *
    * @param   dev                         Device Handle
    * @param   p_histogram_measurement_data   Pointer to the histogram data structure.
    * @return  VL53L0X_ERROR_NOT_IMPLEMENTED   Not implemented
    */
    VL53L0X_Error VL53L0X_get_histogram_measurement_data(VL53L0X_DEV dev,
            VL53L0X_HistogramMeasurementData_t *p_histogram_measurement_data);

    /**
    * @brief  Get current new device mode
    * @par Function Description
    * Get current Histogram mode of a Device
    *
    * @note This function doesn't Access to the device
    *
    * @param   dev                   Device Handle
    * @param   p_histogram_mode        Pointer to current Histogram Mode value
    *                                Valid values are:
    *                                VL53L0X_HISTOGRAMMODE_DISABLED
    *                                VL53L0X_DEVICEMODE_SINGLE_HISTOGRAM
    *                                VL53L0X_HISTOGRAMMODE_REFERENCE_ONLY
    *                                VL53L0X_HISTOGRAMMODE_RETURN_ONLY
    *                                VL53L0X_HISTOGRAMMODE_BOTH
    * @return  VL53L0X_ERROR_NONE     Success
    * @return  "Other error code"    See ::VL53L0X_Error
    */
    VL53L0X_Error VL53L0X_get_histogram_mode(VL53L0X_DEV dev,
            VL53L0X_HistogramModes *p_histogram_mode);

    /**
     * @brief  Set a new Histogram mode
     * @par Function Description
     * Set device to a new Histogram mode
     *
     * @note This function doesn't Access to the device
     *
     * @param   dev                   Device Handle
     * @param   histogram_mode         New device mode to apply
     *                                Valid values are:
     *                                VL53L0X_HISTOGRAMMODE_DISABLED
     *                                VL53L0X_DEVICEMODE_SINGLE_HISTOGRAM
     *                                VL53L0X_HISTOGRAMMODE_REFERENCE_ONLY
     *                                VL53L0X_HISTOGRAMMODE_RETURN_ONLY
     *                                VL53L0X_HISTOGRAMMODE_BOTH
     *
     * @return  VL53L0X_ERROR_NONE                   Success
     * @return  VL53L0X_ERROR_MODE_NOT_SUPPORTED     This error occurs when
     * HistogramMode is not in the supported list
     * @return  "Other error code"    See ::VL53L0X_Error
     */
    VL53L0X_Error VL53L0X_set_histogram_mode(VL53L0X_DEV dev,
            VL53L0X_HistogramModes histogram_mode);

    /**
     * @brief  Return a description string for a given limit check number
     *
     * @par Function Description
     * This function returns a description string for a given limit check number.
     * The limit check is identified with the LimitCheckId.
     *
     * @note This function doesn't Access to the device
     *
     * @param   dev                           Device Handle
     * @param   limit_check_id                  Limit Check ID
     (0<= LimitCheckId < VL53L0X_GetNumberOfLimitCheck() ).
     * @param   p_limit_check_string             Pointer to the
     description string of the given check limit.
     * @return  VL53L0X_ERROR_NONE             Success
     * @return  VL53L0X_ERROR_INVALID_PARAMS   This error is
     returned when LimitCheckId value is out of range.
     * @return  "Other error code"            See ::VL53L0X_Error
     */
    VL53L0X_Error VL53L0X_get_limit_check_info(VL53L0X_DEV dev,
            uint16_t limit_check_id, char *p_limit_check_string);

    /**
     * @brief Get the linearity corrective gain
     *
     * @par Function Description
     * Should only be used after a successful call to @a VL53L0X_DataInit to backup
     * device NVM value
     *
     * @note This function Access to the device
     *
     * @param   dev                                Device Handle
     * @param   p_linearity_corrective_gain           Pointer to the linearity
     * corrective gain in x1000
     * if value is 1000 then no modification is applied.
     * @return  VL53L0X_ERROR_NONE                  Success
     * @return  "Other error code"                 See ::VL53L0X_Error
     */
    VL53L0X_Error VL53L0X_get_linearity_corrective_gain(VL53L0X_DEV dev,
            uint16_t *p_linearity_corrective_gain);

    /**
     * Set the linearity corrective gain
     *
     * @note This function Access to the device
     *
     * @param   dev                                Device Handle
     * @param   linearity_corrective_gain            Linearity corrective
     * gain in x1000
     * if value is 1000 then no modification is applied.
     * @return  VL53L0X_ERROR_NONE                  Success
     * @return  "Other error code"                 See ::VL53L0X_Error
     */
    VL53L0X_Error VL53L0X_set_linearity_corrective_gain(VL53L0X_DEV dev,
            int16_t linearity_corrective_gain);

    /**
     * @brief Get the Maximum number of ROI Zones managed by the Device
     *
     * @par Function Description
     * Get Maximum number of ROI Zones managed by the Device.
     *
     * @note This function doesn't Access to the device
     *
     * @param   dev                    Device Handle
     * @param   p_max_number_of_roi_zones   Pointer to the Maximum Number
     *  of ROI Zones value.
     * @return  VL53L0X_ERROR_NONE      Success
     */
    VL53L0X_Error VL53L0X_get_max_number_of_roi_zones(VL53L0X_DEV dev,
            uint8_t *p_max_number_of_roi_zones);

    /**
     * @brief Retrieve the Reference Signal after a measurements
     *
     * @par Function Description
     * Get Reference Signal from last successful Ranging measurement
     * This function return a valid value after that you call the
     * @a VL53L0X_GetRangingMeasurementData().
     *
     * @note This function Access to the device
     *
     * @param   dev                      Device Handle
     * @param   p_measurement_ref_signal    Pointer to the Ref Signal to fill up.
     * @return  VL53L0X_ERROR_NONE        Success
     * @return  "Other error code"       See ::VL53L0X_Error
     */
    VL53L0X_Error VL53L0X_get_measurement_ref_signal(VL53L0X_DEV dev,
            FixPoint1616_t *p_measurement_ref_signal);

    /**
     * @brief  Get the number of the check limit managed by a given Device
     *
     * @par Function Description
     * This function give the number of the check limit managed by the Device
     *
     * @note This function doesn't Access to the device
     *
     * @param   p_number_of_limit_check           Pointer to the number of check limit.
     * @return  VL53L0X_ERROR_NONE             Success
     * @return  "Other error code"            See ::VL53L0X_Error
     */
    VL53L0X_Error VL53L0X_get_number_of_limit_check(
        uint16_t *p_number_of_limit_check);

    /**
     * @brief Get the number of ROI Zones managed by the Device
     *
     * @par Function Description
     * Get number of ROI Zones managed by the Device
     * USER should take care about  @a VL53L0X_GetNumberOfROIZones()
     * before get data after a perform measurement.
     * PAL will fill a NumberOfROIZones times the corresponding data
     * structure used in the measurement function.
     *
     * @note This function doesn't Access to the device
     *
     * @param   dev                   Device Handle
     * @param   p_number_of_roi_zones     Pointer to the Number of ROI Zones value.
     * @return  VL53L0X_ERROR_NONE     Success
     */
    VL53L0X_Error VL53L0X_get_number_of_roi_zones(VL53L0X_DEV dev,
            uint8_t *p_number_of_roi_zones);

    /**
     * @brief Set the number of ROI Zones to be used for a specific Device
     *
     * @par Function Description
     * Set the number of ROI Zones to be used for a specific Device.
     * The programmed value should be less than the max number of ROI Zones given
     * with @a VL53L0X_GetMaxNumberOfROIZones().
     * This version of API manage only one zone.
     *
     * @param   dev                           Device Handle
     * @param   number_of_roi_zones              Number of ROI Zones to be used for a
     *  specific Device.
     * @return  VL53L0X_ERROR_NONE             Success
     * @return  VL53L0X_ERROR_INVALID_PARAMS   This error is returned if
     * NumberOfROIZones != 1
     */
    VL53L0X_Error VL53L0X_set_number_of_roi_zones(VL53L0X_DEV dev,
            uint8_t number_of_roi_zones);

    /**
     * @brief Gets number of sequence steps managed by the API.
     *
     * @par Function Description
     * This function retrieves the number of sequence steps currently managed
     * by the API
     *
     * @note This function Accesses the device
     *
     * @param   dev                          Device Handle
     * @param   p_number_of_sequence_steps       Out parameter reporting the number of
     *                                       sequence steps.
     * @return  VL53L0X_ERROR_NONE            Success
     * @return  "Other error code"           See ::VL53L0X_Error
     */
    VL53L0X_Error VL53L0X_get_number_of_sequence_steps(VL53L0X_DEV dev,
            uint8_t *p_number_of_sequence_steps);
    /**
     * @brief Get the power mode for a given Device
     *
     * @note This function Access to the device
     *
     * @param   dev                   Device Handle
     * @param   p_power_mode            Pointer to the current value of the power
     * mode. see ::VL53L0X_PowerModes
     *                                Valid values are:
     *                                VL53L0X_POWERMODE_STANDBY_LEVEL1,
     *                                VL53L0X_POWERMODE_IDLE_LEVEL1
     * @return  VL53L0X_ERROR_NONE     Success
     * @return  "Other error code"    See ::VL53L0X_Error
     */
    VL53L0X_Error VL53L0X_get_power_mode(VL53L0X_DEV dev,
                                         VL53L0X_PowerModes *p_power_mode);

    /**
     * @brief Set the power mode for a given Device
     * The power mode can be Standby or Idle. Different level of both Standby and
     * Idle can exists.
     * This function should not be used when device is in Ranging state.
     *
     * @note This function Access to the device
     *
     * @param   dev                   Device Handle
     * @param   power_mode             The value of the power mode to set.
     * see ::VL53L0X_PowerModes
     *                                Valid values are:
     *                                VL53L0X_POWERMODE_STANDBY_LEVEL1,
     *                                VL53L0X_POWERMODE_IDLE_LEVEL1
     * @return  VL53L0X_ERROR_NONE                  Success
     * @return  VL53L0X_ERROR_MODE_NOT_SUPPORTED    This error occurs when PowerMode
     * is not in the supported list
     * @return  "Other error code"    See ::VL53L0X_Error
     */
    VL53L0X_Error VL53L0X_set_power_mode(VL53L0X_DEV dev,
                                         VL53L0X_PowerModes power_mode);

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
     * @param   p_is_aperture_spads              Reports if spads are of type
     *                                       aperture or non-aperture.
     *                                       1:=aperture, 0:=Non-Aperture
     * @return  VL53L0X_ERROR_NONE            Success
     * @return  VL53L0X_ERROR_REF_SPAD_INIT   Error in the in the reference
     *                                       spad configuration.
     * @return  "Other error code"           See ::VL53L0X_Error
     */
    VL53L0X_Error wrapped_VL53L0X_get_reference_spads(VL53L0X_DEV dev,
            uint32_t *p_spad_count, uint8_t *p_is_aperture_spads);

    /**
     * @brief Gets the (on/off) state of a requested sequence step.
     *
     * @par Function Description
     * This function retrieves the state of a requested sequence step, i.e. on/off.
     *
     * @note This function Accesses the device
     *
     * @param   dev                    Device Handle
     * @param   sequence_step_id         Sequence step identifier.
     * @param   p_sequence_step_enabled   Out parameter reporting if the sequence step
     *                                 is enabled {0=Off,1=On}.
     * @return  VL53L0X_ERROR_NONE            Success
     * @return  VL53L0X_ERROR_INVALID_PARAMS  Error SequenceStepId parameter not
     *                                       supported.
     * @return  "Other error code"           See ::VL53L0X_Error
     */
    VL53L0X_Error VL53L0X_get_sequence_step_enable(VL53L0X_DEV dev,
            VL53L0X_SequenceStepId sequence_step_id, uint8_t *p_sequence_step_enabled);


    /**
     * @brief Gets the timeout of a requested sequence step.
     *
     * @par Function Description
     * This function retrieves the timeout of a requested sequence step.
     *
     * @note This function Accesses the device
     *
     * @param   dev                          Device Handle
     * @param   sequence_step_id               Sequence step identifier.
     * @param   p_time_out_milli_secs            Timeout value.
     * @return  VL53L0X_ERROR_NONE            Success
     * @return  VL53L0X_ERROR_INVALID_PARAMS  Error SequenceStepId parameter not
     *                                       supported.
     * @return  "Other error code"           See ::VL53L0X_Error
     */
    VL53L0X_Error VL53L0X_get_sequence_step_timeout(VL53L0X_DEV dev,
            VL53L0X_SequenceStepId sequence_step_id,
            FixPoint1616_t *p_time_out_milli_secs);

    /**
     * @brief Sets the timeout of a requested sequence step.
     *
     * @par Function Description
     * This function sets the timeout of a requested sequence step.
     *
     * @note This function Accesses the device
     *
     * @param   dev                          Device Handle
     * @param   sequence_step_id               Sequence step identifier.
     * @param   time_out_milli_secs             Demanded timeout
     * @return  VL53L0X_ERROR_NONE            Success
     * @return  VL53L0X_ERROR_INVALID_PARAMS  Error SequenceStepId parameter not
     *                                       supported.
     * @return  "Other error code"           See ::VL53L0X_Error
     */
    VL53L0X_Error VL53L0X_set_sequence_step_timeout(VL53L0X_DEV dev,
            VL53L0X_SequenceStepId sequence_step_id, FixPoint1616_t time_out_milli_secs);

    /**
    * @brief  Get the current SPAD Ambient Damper Factor value
    *
    * @par Function Description
    * This function get the SPAD Ambient Damper Factor value
    *
    * @note This function Access to the device
    *
    * @param   dev                           Device Handle
    * @param   p_spad_ambient_damper_factor      Pointer to programmed SPAD Ambient
    * Damper Factor value
    * @return  VL53L0X_ERROR_NONE             Success
    * @return  "Other error code"            See ::VL53L0X_Error
    */
    VL53L0X_Error VL53L0X_get_spad_ambient_damper_factor(VL53L0X_DEV dev,
            uint16_t *p_spad_ambient_damper_factor);
    /**
    * @brief  Set the SPAD Ambient Damper Factor value
    *
    * @par Function Description
    * This function set the SPAD Ambient Damper Factor value
    *
    * @note This function Access to the device
    *
    * @param   dev                           Device Handle
    * @param   spad_ambient_damper_factor       SPAD Ambient Damper Factor value
    * @return  VL53L0X_ERROR_NONE             Success
    * @return  "Other error code"            See ::VL53L0X_Error
    */
    VL53L0X_Error VL53L0X_set_spad_ambient_damper_factor(VL53L0X_DEV dev,
            uint16_t spad_ambient_damper_factor);

    /**
     * @brief  Get the current SPAD Ambient Damper Threshold value
     *
     * @par Function Description
     * This function get the SPAD Ambient Damper Threshold value
     *
     * @note This function Access to the device
     *
     * @param   dev                           Device Handle
     * @param   p_spad_ambient_damper_threshold   Pointer to programmed
     *                                        SPAD Ambient Damper Threshold value
     * @return  VL53L0X_ERROR_NONE             Success
     * @return  "Other error code"            See ::VL53L0X_Error
     */
    VL53L0X_Error VL53L0X_get_spad_ambient_damper_threshold(VL53L0X_DEV dev,
            uint16_t *p_spad_ambient_damper_threshold);

    /**
     * @brief  Set the SPAD Ambient Damper Threshold value
     *
     * @par Function Description
     * This function set the SPAD Ambient Damper Threshold value
     *
     * @note This function Access to the device
     *
     * @param   dev                           Device Handle
     * @param   spad_ambient_damper_threshold    SPAD Ambient Damper Threshold value
     * @return  VL53L0X_ERROR_NONE             Success
     * @return  "Other error code"            See ::VL53L0X_Error
     */
    VL53L0X_Error VL53L0X_set_spad_ambient_damper_threshold(VL53L0X_DEV dev,
            uint16_t spad_ambient_damper_threshold);

    /**
     * @brief Get the maximal distance for actual setup
     * @par Function Description
     * Device must be initialized through @a VL53L0X_SetParameters() prior calling
     * this function.
     *
     * Any range value more than the value returned is to be considered as
     * "no target detected" or
     * "no target in detectable range"\n
     * @warning The maximal distance depends on the setup
     *
     * @note This function is not Implemented
     *
     * @param   dev      Device Handle
     * @param   p_upper_limit_milli_meter   The maximal range limit for actual setup
     * (in millimeter)
     * @return  VL53L0X_ERROR_NOT_IMPLEMENTED        Not implemented
     */
    VL53L0X_Error VL53L0X_get_upper_limit_milli_meter(VL53L0X_DEV dev,
            uint16_t *p_upper_limit_milli_meter);

    /**
    * @brief Get the tuning settings pointer and the internal external switch
    * value.
    *
    * This function is used to get the Tuning settings buffer pointer and the
    * value.
    * of the switch to select either external or internal tuning settings.
    *
    * @note This function Access to the device
    *
    * @param   dev                        Device Handle
    * @param   pp_tuning_setting_buffer      Pointer to tuning settings buffer.
    * @param   p_use_internal_tuning_settings Pointer to store Use internal tuning
    *                                     settings value.
    * @return  VL53L0X_ERROR_NONE          Success
    * @return  "Other error code"         See ::VL53L0X_Error
    */
    VL53L0X_Error VL53L0X_get_tuning_setting_buffer(VL53L0X_DEV dev,
            uint8_t **pp_tuning_setting_buffer, uint8_t *p_use_internal_tuning_settings);

    /**
     * @brief Set the tuning settings pointer
     *
     * This function is used to specify the Tuning settings buffer to be used
     * for a given device. The buffer contains all the necessary data to permit
     * the API to write tuning settings.
     * This function permit to force the usage of either external or internal
     * tuning settings.
     *
     * @note This function Access to the device
     *
     * @param   dev                             Device Handle
     * @param   p_tuning_setting_buffer            Pointer to tuning settings buffer.
     * @param   use_internal_tuning_settings       Use internal tuning settings value.
     * @return  VL53L0X_ERROR_NONE     Success
     * @return  "Other error code"    See ::VL53L0X_Error
     */
    VL53L0X_Error VL53L0X_set_tuning_setting_buffer(VL53L0X_DEV dev,
            uint8_t *p_tuning_setting_buffer, uint8_t use_internal_tuning_settings);

    /**
     * @defgroup VL53L0X_registerAccess_group PAL Register Access Functions
     * @brief    PAL Register Access Functions
     *  @{
     */

    /**
     * Lock comms interface to serialize all commands to a shared I2C interface for a specific device
     * @param   dev       Device Handle
     * @return  VL53L0X_ERROR_NONE        Success
     * @return  "Other error code"    See ::VL53L0X_Error
     */
    VL53L0X_Error VL53L0X_lock_sequence_access(VL53L0X_DEV dev);

    /**
     * Unlock comms interface to serialize all commands to a shared I2C interface for a specific device
     * @param   dev       Device Handle
     * @return  VL53L0X_ERROR_NONE        Success
     * @return  "Other error code"    See ::VL53L0X_Error
     */
    VL53L0X_Error VL53L0X_unlock_sequence_access(VL53L0X_DEV dev);

    /**
     * @brief  Prepare device for operation
     * @par Function Description
     * Update device with provided parameters
     * @li Then start ranging operation.
     *
     * @note This function Access to the device
     *
     * @param   Dev                   Device Handle
     * @param   pDeviceParameters     Pointer to store current device parameters.
     * @return  VL53L0X_ERROR_NONE     Success
     * @return  "Other error code"    See ::VL53L0X_Error
     */
    VL53L0X_Error vl53L0x_set_device_parameters(VL53L0X_DEV Dev,
            const VL53L0X_DeviceParameters_t *pDeviceParameters);

    /**
     * Set Group parameter Hold state
     *
     * @par Function Description
     * Set or remove device internal group parameter hold
     *
     * @note This function is not Implemented
     *
     * @param   dev      Device Handle
     * @param   group_param_hold   Group parameter Hold state to be set (on/off)
     * @return  VL53L0X_ERROR_NOT_IMPLEMENTED        Not implemented
     */
    VL53L0X_Error VL53L0X_set_group_param_hold(VL53L0X_DEV dev,
            uint8_t group_param_hold);


    /**
     * @brief Wait for device ready for a new measurement command.
     * Blocking function.
     *
     * @note This function is not Implemented
     *
     * @param   dev      Device Handle
     * @param   max_loop    Max Number of polling loop (timeout).
     * @return  VL53L0X_ERROR_NOT_IMPLEMENTED   Not implemented
     */
    VL53L0X_Error VL53L0X_wait_device_ready_for_new_measurement(VL53L0X_DEV dev,
            uint32_t max_loop);

    VL53L0X_Error VL53L0X_reverse_bytes(uint8_t *data, uint32_t size);

    int range_meas_int_continuous_mode(void (*fptr)(void));

