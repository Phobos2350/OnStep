// -----------------------------------------------------------------------------------
// Global variables 

#pragma once

// Time keeping --------------------------------------------------------------------------------------------------------------------
long siderealTimer                      = 0;                 // counter to issue steps during tracking
long PecSiderealTimer                   = 0;                 // time since worm wheel zero index for PEC
long guideSiderealTimer                 = 0;                 // counter to issue steps during guiding
boolean dateWasSet                      = false;             // keep track of date/time validity
boolean timeWasSet                      = false;                          
                                                                          
double UT1                              = 0.0;               // the current universal time
double UT1_start                        = 0.0;               // the start of UT1
double JD                               = 0.0;               // and date, used for computing LST
double LMT                              = 0.0;
double timeZone                         = 0.0;
                                                                          
long lst_start                          = 0;                 // marks the start lst when UT1 is set 
volatile long lst                       = 0;                 // local (apparent) sidereal time in 0.01 second ticks,
                                                             // takes 249 days to roll over.
                                                             // 1.00273 wall clock seconds per sidereal second
                                                                          
long siderealInterval                   = 15956313L;                      
long masterSiderealInterval             = siderealInterval;               
                                                             // default = 15956313 ticks per sidereal second, where a tick
                                                             // is 1/16 uS this is stored in EEPROM which is updated/adjusted
                                                             // with the ":T+#" and ":T-#" commands a higher number here means
                                                             // a longer count which slows down the sidereal clock
                                                                          
double HzCf                             = 16000000.0/60.0;   // conversion factor to go to/from Hz for sidereal interval
                                                                          
volatile long SiderealRate;                                  // based on the siderealInterval, time between steps sidereal tracking
volatile long TakeupRate;                                    // takeup rate for synchronizing target and actual positions
                                                                          
long last_loop_micros                   = 0;                 // workload monitoring
long this_loop_time                     = 0;
long loop_time                          = 0;
long worst_loop_time                    = 0;
long average_loop_time                  = 0;

// PPS (GPS) -----------------------------------------------------------------------------------------------------------------------
volatile unsigned long PPSlastMicroS    = 1000000UL;
volatile unsigned long PPSavgMicroS     = 1000000UL;
volatile double PPSrateRatio            = 1.0;
volatile double LastPPSrateRatio        = 1.0;
volatile boolean PPSsynced              = false;

// Tracking and rate control -------------------------------------------------------------------------------------------------------
#if MOUNT_TYPE != ALTAZM
  enum RateCompensation {RC_NONE, RC_REFR_RA, RC_REFR_BOTH, RC_FULL_RA, RC_FULL_BOTH};
  #if TRACK_REFRACTION_RATE_DEFAULT == ON
    RateCompensation rateCompensation = RC_REFR_RA;
  #else
    RateCompensation rateCompensation = RC_NONE;
  #endif
#else
  enum RateCompensation {RC_NONE};
  RateCompensation rateCompensation = RC_NONE;
#endif

long maxRate                            = (double)MaxRate*16.0;
double MaxRateDef                       = (double)MaxRate;

double slewSpeed                        = 0;
volatile long timerRateAxis1            = 0;
volatile long timerRateBacklashAxis1    = 0;
volatile boolean inbacklashAxis1        = false;
boolean faultAxis1                      = false;
volatile long timerRateAxis2            = 0;
volatile long timerRateBacklashAxis2    = 0;
volatile boolean inbacklashAxis2        = false;
boolean faultAxis2                      = false;

#define default_tracking_rate 1
volatile double trackingTimerRateAxis1  = default_tracking_rate;
volatile double trackingTimerRateAxis2  = default_tracking_rate;
volatile double timerRateRatio          = ((double)AXIS1_STEPS_PER_DEGREE/(double)AXIS2_STEPS_PER_DEGREE);
volatile boolean useTimerRateRatio      = (AXIS1_STEPS_PER_DEGREE != AXIS2_STEPS_PER_DEGREE);
#define StepsPerSecondAxis1               ((double)AXIS1_STEPS_PER_DEGREE/240.0)
#define ArcSecPerStepAxis1                (3600.0/AXIS1_STEPS_PER_DEGREE)
#define StepsPerSecondAxis2               ((double)AXIS2_STEPS_PER_DEGREE/240.0)
#define ArcSecPerStepAxis2                (3600.0/AXIS2_STEPS_PER_DEGREE)
#define BreakDistAxis1                    (2L)
#define BreakDistAxis2                    (2L)
long SecondsPerWormRotationAxis1        = ((double)AXIS1_STEPS_PER_WORMROT/StepsPerSecondAxis1);
volatile double StepsForRateChangeAxis1 = (sqrt((double)SLEW_ACCELERATION_DIST*(double)AXIS1_STEPS_PER_DEGREE))*(double)MaxRate*16.0;
volatile double StepsForRateChangeAxis2 = (sqrt((double)SLEW_ACCELERATION_DIST*(double)AXIS2_STEPS_PER_DEGREE))*(double)MaxRate*16.0;

// Basic stepper driver mode setup -------------------------------------------------------------------------------------------------
#if AXIS1_DRIVER_MODEL != OFF
  // Holds translated code for driver microstep setting
  volatile uint8_t _axis1_microstep_code;
  #define AXIS1_DRIVER_MICROSTEP_CODE _axis1_microstep_code
  #if AXIS1_DRIVER_MICROSTEPS_GOTO != OFF
    volatile uint8_t _axis1_microstep_code_goto;
    #define AXIS1_DRIVER_MICROSTEP_CODE_GOTO _axis1_microstep_code_goto
  #endif
  volatile uint8_t _axis2_microstep_code;
  #define AXIS2_DRIVER_MICROSTEP_CODE _axis2_microstep_code
  #if AXIS2_DRIVER_MICROSTEPS_GOTO != OFF
    volatile uint8_t _axis2_microstep_code_goto;
    #define AXIS2_DRIVER_MICROSTEP_CODE_GOTO _axis2_microstep_code_goto
  #endif
  #if AXIS3_DRIVER_MICROSTEPS != OFF
    volatile uint8_t _axis3_microstep_code;
    #define AXIS3_DRIVER_MICROSTEP_CODE _axis3_microstep_code
  #endif
  #if AXIS4_DRIVER_MICROSTEPS != OFF
    volatile uint8_t _axis4_microstep_code;
    #define AXIS4_DRIVER_MICROSTEP_CODE _axis4_microstep_code
  #endif
  #if AXIS5_DRIVER_MICROSTEPS != OFF
    volatile uint8_t _axis5_microstep_code;
    #define AXIS5_DRIVER_MICROSTEP_CODE _axis5_microstep_code
  #endif
#endif

// Location ------------------------------------------------------------------------------------------------------------------------
double latitude                         = 0.0;
double cosLat                           = 1.0;
double sinLat                           = 0.0;
double longitude                        = 0.0;

// fix AXIS1_LIMIT_UNDER_POLE for fork mounts
#if MOUNT_TYPE == FORK
  #undef AXIS1_LIMIT_UNDER_POLE
  #define AXIS1_LIMIT_UNDER_POLE 180.0
#endif

// Coordinates ---------------------------------------------------------------------------------------------------------------------
#if MOUNT_TYPE == GEM
  double homePositionAxis1              = 90.0;
#else
  double homePositionAxis1              = 0.0;
#endif
double homePositionAxis2                = 90.0;
// either 0 or (fabs(latitude))
#define AltAzmDecStartPos (fabs(latitude))

volatile long posAxis1                  = 0;                 // hour angle position in steps
volatile long startAxis1                = 0;                 // hour angle of goto start position in steps
volatile fixed_t targetAxis1;                                // hour angle of goto end   position in steps
volatile byte dirAxis1                  = 1;                 // stepping direction + or -
double origTargetRA                     = 0.0;               // holds the RA for gotos before possible conversion to observed place
double newTargetRA                      = 0.0;               // holds the RA for gotos after conversion to observed place
fixed_t origTargetAxis1;
#if defined(AXIS1_DRIVER_MICROSTEP_CODE) && defined(AXIS1_DRIVER_MICROSTEP_CODE_GOTO)
  volatile long stepAxis1=1;
#else
  #define stepAxis1 1
#endif

volatile long posAxis2                  = 0;                 // declination position in steps
volatile long startAxis2                = 0;                 // declination of goto start position in steps
volatile fixed_t targetAxis2;                                // declination of goto end   position in steps
volatile byte dirAxis2                  = 1;                 // stepping direction + or -
double origTargetDec                    = 0.0;               // holds the Dec for gotos before possible conversion to observed place
double newTargetDec                     = 0.0;               // holds the Dec for gotos after conversion to observed place
long origTargetAxis2                    = 0;
#if defined(AXIS2_DRIVER_MICROSTEP_CODE) && defined(AXIS2_DRIVER_MICROSTEP_CODE_GOTO)
  volatile long stepAxis2=1;
#else
  #define stepAxis2 1
#endif

double newTargetAlt=0.0, newTargetAzm   = 0.0;               // holds the altitude and azmiuth for slews
long   degreesPastMeridianE             = 15;                // east of pier.  How far past the meridian before we do a flip.
long   degreesPastMeridianW             = 15;                // west of pier.  Mount stops tracking when it hits the this limit.
int    minAlt;                                               // the min altitude, in deg, so we don't try to point too low
int    maxAlt;                                               // the max altitude, in deg, keeps telescope away from mount/tripod
bool   autoMeridianFlip                 = false;             // auto meridian flip/continue as tracking hits AXIS1_LIMIT_MERIDIAN_W
                                                                          
double currentAlt                       = 45.0;              // the current altitude
double currentDec                       = 0.0;               // the current declination

// Stepper driver enable/disable and direction -------------------------------------------------------------------------------------

#define defaultDirAxis2EInit              1
#define defaultDirAxis2WInit              0
volatile byte defaultDirAxis2           = defaultDirAxis2EInit;
#define defaultDirAxis1NCPInit            0
#define defaultDirAxis1SCPInit            1
volatile byte defaultDirAxis1           = defaultDirAxis1NCPInit;

// Status --------------------------------------------------------------------------------------------------------------------------
enum Errors {
  ERR_NONE, ERR_MOTOR_FAULT, ERR_ALT_MIN, ERR_LIMIT_SENSE, ERR_DEC, ERR_AZM,
  ERR_UNDER_POLE, ERR_MERIDIAN, ERR_SYNC, ERR_PARK, ERR_GOTO_SYNC, ERR_UNSPECIFIED,
  ERR_ALT_MAX, ERR_GOTO_ERR_NONE, ERR_GOTO_ERR_BELOW_HORIZON, ERR_GOTO_ERR_ABOVE_OVERHEAD,
  ERR_GOTO_ERR_STANDBY, ERR_GOTO_ERR_PARK, ERR_GOTO_ERR_GOTO, ERR_GOTO_ERR_OUTSIDE_LIMITS,
  ERR_GOTO_ERR_HARDWARE_FAULT, ERR_GOTO_ERR_IN_MOTION, ERR_GOTO_ERR_UNSPECIFIED};
Errors lastError = ERR_NONE;
enum GotoErrors {
  GOTO_ERR_NONE, GOTO_ERR_BELOW_HORIZON, GOTO_ERR_ABOVE_OVERHEAD, GOTO_ERR_STANDBY, GOTO_ERR_PARK,
  GOTO_ERR_GOTO, GOTO_ERR_OUTSIDE_LIMITS, GOTO_ERR_HARDWARE_FAULT, GOTO_ERR_IN_MOTION, GOTO_ERR_UNSPECIFIED};

boolean highPrecision = true;

#define TrackingNone                      0
#define TrackingSidereal                  1
#define TrackingMoveTo                    2
volatile byte trackingState             = TrackingNone;
byte abortTrackingState                 = TrackingNone;
volatile byte lastTrackingState         = TrackingNone;
int trackingSyncSeconds                 = 0;
#define StartAbortSlew 1                
byte abortSlew                          = 0;
volatile boolean safetyLimitsOn         = true;
boolean axis1Enabled                    = false;
boolean axis2Enabled                    = false;
                                        
#define MeridianFlipNever                 0
#define MeridianFlipAlign                 1
#define MeridianFlipAlways                2
#if MOUNT_TYPE == GEM
  byte meridianFlip = MeridianFlipAlways;
#endif
#if MOUNT_TYPE == FORK
  byte meridianFlip = MeridianFlipNever;
#endif
#if MOUNT_TYPE == ALTAZM
  byte meridianFlip = MeridianFlipNever;
#endif

byte pierSideControl = PierSideNone;
enum PreferredPierSide {PPS_BEST,PPS_EAST,PPS_WEST};
PreferredPierSide preferredPierSide = PPS_BEST;

#define NotParked                         0
#define Parking                           1
#define Parked                            2
#define ParkFailed                        3
#define ParkUnknown                       4
byte    parkStatus                      = NotParked;
boolean parkSaved                       = false;
boolean atHome                          = true;
boolean homeMount                       = false;

// Command processing --------------------------------------------------------------------------------------------------------------
#define BAUD 9600
// serial speed
unsigned long baudRate[10] = {115200,56700,38400,28800,19200,14400,9600,4800,2400,1200};

// Guide command -------------------------------------------------------------------------------------------------------------------
#define GuideRate1x 2
#ifndef GuideRateDefault
  #define GuideRateDefault 6 // 20x
#endif
#define GuideRateNone                     255
#define RateToDegPerSec                   (1000000.0/(double)AXIS1_STEPS_PER_DEGREE)
#define RateToASPerSec                    (RateToDegPerSec*3600.0)
#define RateToXPerSec                     (RateToASPerSec/15.0)
double  slewRateX                       = (RateToXPerSec/MaxRate)*2.5;
double  accXPerSec                      = (slewRateX/SLEW_ACCELERATION_DIST);
double  guideRates[10]={3.75,7.5,15,30,60,120,300,720,(RateToASPerSec/MaxRate)/2.0,RateToASPerSec/MaxRate};
//                      .25X .5x 1x 2x 4x  8x 20x 48x       half-MaxRate                   MaxRate
//                         0   1  2  3  4   5   6   7                  8                         9

byte currentGuideRate                   = GuideRateDefault;
byte currentPulseGuideRate              = GuideRate1x;
volatile byte activeGuideRate           = GuideRateNone;
                                        
volatile byte guideDirAxis1             = 0;
char          ST4DirAxis1               = 'b';
volatile byte guideDirAxis2             = 0;
char          ST4DirAxis2               = 'b';

volatile double guideTimerRateAxis1     = 0.0;
volatile double guideTimerRateAxis2     = 0.0;
volatile double guideTimerBaseRateAxis1 = 0.0;
volatile double guideTimerBaseRateAxis2 = 0.0;
fixed_t amountGuideAxis1;
fixed_t guideAxis1;
fixed_t amountGuideAxis2;
fixed_t guideAxis2;

// PEC control ---------------------------------------------------------------------------------------------------------------------
#define PECStatusString                   "IpPrR"
#define PECStatusStringAlt                "/,~;^"
#define IgnorePEC                         0
#define ReadyPlayPEC                      1
#define PlayPEC                           2
#define ReadyRecordPEC                    3
#define RecordPEC                         4
byte    pecStatus                       = IgnorePEC;
boolean pecRecorded                     = false;
boolean pecFirstRecord                  = false;
long    lastPecIndex                    = -1;
int     pecBufferSize                   = PEC_BUFFER_SIZE;
long    pecIndex                        = 0;
long    pecIndex1                       = 0;
int     pecAnalogValue                  = 0;
int     pecAutoRecord                   = 0;                 // for writing to PEC table to EEPROM
long    wormSensePos                    = 0;                 // in steps
boolean wormSensedAgain                 = false;             // indicates PEC index was found
int     LastPecPinState                 = PEC_SENSE_STATE;                         
boolean pecBufferStart                  = false;                                   
fixed_t accPecGuideHA;                                       // for PEC, buffers steps to be recorded
volatile double pecTimerRateAxis1 = 0.0;
#if MOUNT_TYPE != ALTAZM
  static byte *pecBuffer;
#endif

// Misc ----------------------------------------------------------------------------------------------------------------------------
#define Rad 57.29577951

// current site index and name
byte currentSite = 0; 
char siteName[16];

// offset corrections simple align
double indexAxis1                       = 0;
long   indexAxis1Steps                  = 0;
double indexAxis2                       = 0;
long   indexAxis2Steps                  = 0;

// tracking and PEC, fractional steps
fixed_t fstepAxis1;
fixed_t fstepAxis2;

// status state
boolean ledOn                           = false;
boolean led2On                          = false;

// sound/buzzer
#if BUZZER_STATE_DEFAULT == ON
  boolean soundEnabled                  = true;
#else                                   
  boolean soundEnabled                  = false;
#endif
volatile int buzzerDuration = 0;

// pause at home on meridian flip
boolean pauseHome                       = false;             // allow pause at home?
boolean waitingHomeContinue             = false;             // set to true to stop pause
boolean waitingHome                     = false;             // true if waiting at home

// reticule control
#if LED_RETICLE >= 0
  int reticuleBrightness=LED_RETICLE;
#endif

// backlash control
volatile int backlashAxis1              = 0;
volatile int backlashAxis2              = 0;
volatile int blAxis1                    = 0;
volatile int blAxis2                    = 0;

// aux pin control
#ifdef Aux0
  byte valueAux0 = 0;
#endif
#ifdef Aux1
  byte valueAux1 = 0;
#endif
#ifdef Aux2
  byte valueAux2 = 0;
#endif
#ifdef Aux3
  byte valueAux3 = 0;
#endif
#ifdef Aux4
  byte valueAux4 = 0;
#endif
#ifdef Aux5
  byte valueAux5 = 0;
#endif
#ifdef Aux6
  byte valueAux6 = 0;
#endif
#ifdef Aux7
  byte valueAux7 = 0;
#endif
#ifdef Aux8
  byte valueAux8 = 0;
#endif
