// RAPS128 stepper driver validate

#if AXIS1_DRIVER_MODEL == RAPS128
  #if AXIS1_DRIVER_MICROSTEPS != 1 && AXIS1_DRIVER_MICROSTEPS != 2 && AXIS1_DRIVER_MICROSTEPS != 4 && AXIS1_DRIVER_MICROSTEPS != 8 && AXIS1_DRIVER_MICROSTEPS != 16 && AXIS1_DRIVER_MICROSTEPS != 32 && AXIS1_DRIVER_MICROSTEPS != 64 && AXIS1_DRIVER_MICROSTEPS != 128
    #error "Configuration (Config.h): AXIS1_DRIVER_MICROSTEPS; RAPS128 invalid micro-step mode, use: 128,64,32,16,8,4,2,or 1"
  #endif
  #if AXIS1_DRIVER_MICROSTEPS_GOTO != OFF
    #if AXIS1_DRIVER_MICROSTEPS_GOTO != 1 && AXIS1_DRIVER_MICROSTEPS_GOTO != 2 && AXIS1_DRIVER_MICROSTEPS_GOTO != 4 && AXIS1_DRIVER_MICROSTEPS_GOTO != 8 && AXIS1_DRIVER_MICROSTEPS_GOTO != 16 && AXIS1_DRIVER_MICROSTEPS_GOTO != 32 && AXIS1_DRIVER_MICROSTEPS_GOTO != 64 && AXIS1_DRIVER_MICROSTEPS_GOTO != 128
      #error "Configuration (Config.h): AXIS1_DRIVER_MICROSTEPS_GOTO; RAPS128 invalid micro-step mode, use: 128,64,32,16,8,4,2,1,or OFF"
    #endif
  #endif
  #if HAL_PULSE_WIDTH < RAPS128_PULSE_WIDTH
    #error "Configuration (Config.h): STEP_WAVE_FORM PULSE; Pulse width is below the RAPS128 stepper driver specifications."
  #endif
#endif

#if AXIS2_DRIVER_MODEL == RAPS128
  #if AXIS2_DRIVER_MICROSTEPS != 1 && AXIS2_DRIVER_MICROSTEPS != 2 && AXIS2_DRIVER_MICROSTEPS != 4 && AXIS2_DRIVER_MICROSTEPS != 8 && AXIS2_DRIVER_MICROSTEPS != 16 && AXIS2_DRIVER_MICROSTEPS != 32 && AXIS2_DRIVER_MICROSTEPS != 64 && AXIS2_DRIVER_MICROSTEPS != 128
    #error "Configuration (Config.h): AXIS2_DRIVER_MICROSTEPS; RAPS128 invalid micro-step mode, use: 128,64,32,16,8,4,2,or 1"
  #endif
  #if AXIS2_DRIVER_MICROSTEPS_GOTO != OFF
    #if AXIS2_DRIVER_MICROSTEPS_GOTO != 1 && AXIS2_DRIVER_MICROSTEPS_GOTO != 2 && AXIS2_DRIVER_MICROSTEPS_GOTO != 4 && AXIS2_DRIVER_MICROSTEPS_GOTO != 8 && AXIS2_DRIVER_MICROSTEPS_GOTO != 16 && AXIS2_DRIVER_MICROSTEPS_GOTO != 32 && AXIS2_DRIVER_MICROSTEPS_GOTO != 64 && AXIS2_DRIVER_MICROSTEPS_GOTO != 128
      #error "Configuration (Config.h): AXIS2_DRIVER_MICROSTEPS_GOTO; RAPS128 invalid micro-step mode, use: 128,64,32,16,8,4,2,1,or OFF"
    #endif
  #endif
  #if HAL_PULSE_WIDTH < RAPS128_PULSE_WIDTH
    #error "Configuration (Config.h): STEP_WAVE_FORM PULSE; Pulse width is below the RAPS128 stepper driver specifications."
  #endif
#endif
