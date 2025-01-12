
#include "SmartController.h"
#include "LX200.h"
#include "Catalog.h"

unsigned long display_blank_time = DISPLAY_BLANK_TIME;
unsigned long display_dim_time = DISPLAY_DIM_TIME;

#ifdef ESP32
  void timerAlarmsEnable() { SerialST4.paused(false); }
  void timerAlarmsDisable() { SerialST4.paused(true); }
  #include "NV_EEPROM_ESP.h"
#else
#include "NV_EEPROM.h"
#endif

#include "Initialize.h"

#define MY_BORDER_SIZE 1
#define icon_width 16
#define icon_height 16

#define onstep_logo_width 128
#define onstep_logo_height 68

static unsigned char align1_bits[] U8X8_PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x1c, 0x00, 0x1c, 0x00, 0x00, 0x00, 0xc0, 0x03, 0xc0, 0x00, 0x40, 0x01, 0x50, 0x02, 0x18, 0x04, 0x10, 0x08, 0x10, 0x10, 0x10, 0x20, 0x00, 0x00, 0x00, 0x00 };

static unsigned char align2_bits[] U8X8_PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x1c, 0x00, 0x1c, 0x00, 0x00, 0x00, 0xc0, 0x03, 0xc0, 0x00, 0x40, 0x01, 0x48, 0x02, 0x14, 0x04, 0x10, 0x08, 0x08, 0x10, 0x04, 0x20, 0x1c, 0x00, 0x00, 0x00 };

static unsigned char align3_bits[] U8X8_PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x1c, 0x00, 0x1c, 0x00, 0x00, 0x00, 0xc0, 0x03, 0xc0, 0x00, 0x40, 0x01, 0x40, 0x02, 0x1c, 0x04, 0x10, 0x08, 0x18, 0x10, 0x10, 0x20, 0x1c, 0x00, 0x00, 0x00 };

static unsigned char align4_bits[] U8X8_PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x1c, 0x00, 0x1c, 0x00, 0x00, 0x00, 0xc0, 0x03, 0xc0, 0x00, 0x40, 0x01, 0x40, 0x02, 0x14, 0x04, 0x14, 0x08, 0x1c, 0x10, 0x10, 0x20, 0x10, 0x00, 0x00, 0x00 };

static unsigned char align5_bits[] U8X8_PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x1c, 0x00, 0x1c, 0x00, 0x00, 0x00, 0xc0, 0x03, 0xc0, 0x00, 0x40, 0x01, 0x40, 0x02, 0x1c, 0x04, 0x04, 0x08, 0x1c, 0x10, 0x10, 0x20, 0x1c, 0x00, 0x00, 0x00 };

static unsigned char align6_bits[] U8X8_PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x1c, 0x00, 0x1c, 0x00, 0x00, 0x00, 0xc0, 0x03, 0xc0, 0x00, 0x40, 0x01, 0x40, 0x02, 0x1c, 0x04, 0x04, 0x08, 0x1c, 0x10, 0x14, 0x20, 0x1c, 0x00, 0x00, 0x00 };

static unsigned char align7_bits[] U8X8_PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x1c, 0x00, 0x1c, 0x00, 0x00, 0x00, 0xc0, 0x03, 0xc0, 0x00, 0x40, 0x01, 0x40, 0x02, 0x1c, 0x04, 0x10, 0x08, 0x08, 0x10, 0x08, 0x20, 0x04, 0x00, 0x00, 0x00 };

static unsigned char align8_bits[] U8X8_PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x1c, 0x00, 0x1c, 0x00, 0x00, 0x00, 0xc0, 0x03, 0xc0, 0x00, 0x40, 0x01, 0x40, 0x02, 0x1c, 0x04, 0x14, 0x08, 0x1c, 0x10, 0x14, 0x20, 0x1c, 0x00, 0x00, 0x00 };

static unsigned char align9_bits[] U8X8_PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x1c, 0x00, 0x1c, 0x00, 0x00, 0x00, 0xc0, 0x03, 0xc0, 0x00, 0x40, 0x01, 0x40, 0x02, 0x1c, 0x04, 0x14, 0x08, 0x1c, 0x10, 0x10, 0x20, 0x10, 0x00, 0x00, 0x00 };

static unsigned char home_bits[] U8X8_PROGMEM = {
  0x00, 0x02, 0x00, 0x07, 0x80, 0x0f, 0xc0, 0x1f, 0x80, 0x3f, 0x00, 0x7f, 0x00, 0x7e, 0x00, 0x7f, 0x80, 0xfb, 0xc0, 0xc1, 0xe0, 0x01, 0xbc, 0x49, 0x9e, 0x49, 0x9e, 0x79, 0x8c, 0x49, 0x80, 0x49 };

static unsigned char parked_bits[] U8X8_PROGMEM = {
  0x00, 0x00, 0xfe, 0x7f, 0x02, 0x40, 0x02, 0x40, 0xe2, 0x43, 0x62, 0x46, 0x62, 0x46, 0x62, 0x46, 0xe2, 0x43, 0x62, 0x40, 0x62, 0x40, 0x62, 0x40, 0x62, 0x40, 0x02, 0x40, 0xfe, 0x7f, 0x00, 0x00 };

static unsigned char parking_bits[] U8X8_PROGMEM = {
  0xff, 0xff, 0x01, 0x80, 0x01, 0x80, 0xf9, 0x80, 0x99, 0x81, 0x99, 0x81, 0x99, 0x81, 0xf9, 0x80, 0x19, 0x80, 0x99, 0x84, 0x99, 0x8d, 0x99, 0x9f, 0x81, 0x8d, 0x81, 0x84, 0x01, 0x80, 0xff, 0xff };

static unsigned char parkingFailed_bits[] U8X8_PROGMEM = {
  0xff, 0xff, 0x01, 0x80, 0x01, 0x80, 0xf9, 0x90, 0x99, 0x91, 0x99, 0x91, 0x99, 0x91, 0xf9, 0x90, 0x19, 0x90, 0xd9, 0x93, 0x59, 0x90, 0xd9, 0x91, 0x41, 0x80, 0x41, 0x90, 0x01, 0x80, 0xff, 0xff };

static unsigned char guiding_bits[] U8X8_PROGMEM = {
  0x00, 0x00, 0x80, 0x01, 0x80, 0x01, 0xc0, 0x03, 0x20, 0x04, 0x10, 0x08, 0x08, 0x10, 0x8e, 0x71, 0x8e, 0x71, 0x08, 0x10, 0x10, 0x08, 0x20, 0x04, 0xc0, 0x03, 0x80, 0x01, 0x80, 0x01, 0x00, 0x00 };

static unsigned char no_tracking_bits[] U8X8_PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x38, 0x1c, 0x7c, 0x3e, 0x7c, 0x3e, 0x7c, 0x3e, 0x7c, 0x3e, 0x7c, 0x3e, 0x7c, 0x3e, 0x7c, 0x3e, 0x7c, 0x3e, 0x7c, 0x3e, 0x7c, 0x3e, 0x7c, 0x3e, 0x38, 0x1c, 0x00, 0x00 };

//static unsigned char tracking_bits[] U8X8_PROGMEM = {
//  0x00, 0x00, 0x02, 0x00, 0x06, 0x00, 0x0e, 0x00, 0x1e, 0x00, 0x3e, 0x00, 0x7e, 0x00, 0xfe, 0x38, 0xfe, 0x44, 0x7e, 0x44, 0x3e, 0x20, 0x1e, 0x10, 0x0e, 0x10, 0x06, 0x00, 0x02, 0x10, 0x00, 0x00 };

static unsigned char tracking_S_bits[] U8X8_PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x06, 0x00, 0x0e, 0x00, 0x1e, 0x00, 0x3e, 0x00, 0x7e, 0x00, 0xfe, 0x38, 0x7e, 0x04, 0x3e, 0x04, 0x1e, 0x18, 0x0e, 0x20, 0x06, 0x20, 0x02, 0x1c, 0x00, 0x00 };

static unsigned char tracking_sid_bits[] U8X8_PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x06, 0x00, 0x0E, 0x00, 0x1E, 0x00, 0x3E, 0x00, 0x7E, 0x08, 0xFE, 0x08, 0x7E, 0x7F, 0x3E, 0x3E, 0x1E, 0x1C, 0x0E, 0x3E, 0x06, 0x22, 0x02, 0x00, 0x00, 0x00 };

static unsigned char tracking_sid_r_bits[] U8X8_PROGMEM = {
  0x00, 0x00, 0x80, 0x03, 0x82, 0x04, 0x86, 0x03, 0x8e, 0x04, 0x9e, 0x04, 0x3e, 0x00, 0x7e, 0x08, 0xfe, 0x08, 0x7e, 0x7f, 0x3e, 0x3e, 0x1e, 0x1c, 0x0e, 0x3e, 0x06, 0x22, 0x02, 0x00, 0x00, 0x00 };

static unsigned char tracking_sid_rd_bits[] U8X8_PROGMEM = {
  0x00, 0x00, 0x80, 0x33, 0x82, 0x54, 0x86, 0x53, 0x8e, 0x54, 0x9e, 0x34, 0x3e, 0x00, 0x7e, 0x08, 0xfe, 0x08, 0x7e, 0x7f, 0x3e, 0x3e, 0x1e, 0x1c, 0x0e, 0x3e, 0x06, 0x22, 0x02, 0x00, 0x00, 0x00 };
 
static unsigned char tracking_sid_f_bits[] U8X8_PROGMEM = {
  0x00, 0x00, 0x80, 0x03, 0x82, 0x00, 0x86, 0x03, 0x8e, 0x00, 0x9e, 0x00, 0x3e, 0x00, 0x7e, 0x08, 0xfe, 0x08, 0x7e, 0x7f, 0x3e, 0x3e, 0x1e, 0x1c, 0x0e, 0x3e, 0x06, 0x22, 0x02, 0x00, 0x00, 0x00 };
 
static unsigned char tracking_sid_fd_bits[] U8X8_PROGMEM = {
  0x00, 0x00, 0x80, 0x33, 0x82, 0x50, 0x86, 0x53, 0x8e, 0x50, 0x9e, 0x30, 0x3e, 0x00, 0x7e, 0x08, 0xfe, 0x08, 0x7e, 0x7f, 0x3e, 0x3e, 0x1e, 0x1c, 0x0e, 0x3e, 0x06, 0x22, 0x02, 0x00, 0x00, 0x00 };

static unsigned char tracking_sol_bits[] U8X8_PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x06, 0x00, 0x0E, 0x00, 0x1E, 0x00, 0x3E, 0x00, 0x7E, 0x00, 0xFE, 0x1C, 0x7E, 0x22, 0x3E, 0x41, 0x1E, 0x49, 0x0E, 0x41, 0x06, 0x22, 0x02, 0x1C, 0x00, 0x00 };

static unsigned char tracking_lun_bits[] U8X8_PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x06, 0x00, 0x0E, 0x00, 0x1E, 0x00, 0x3E, 0x00, 0x7E, 0x38, 0xFE, 0x1C, 0x7E, 0x06, 0x3E, 0x06, 0x1E, 0x06, 0x0E, 0x06, 0x06, 0x1C, 0x02, 0x38, 0x00, 0x00 };  

static unsigned char sleewing_bits[] U8X8_PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x02, 0x01, 0x06, 0x03, 0x0e, 0x07, 0x1e, 0x0f, 0x3e, 0x1f, 0x7e, 0x3f, 0xfe, 0x7f, 0x7e, 0x3f, 0x3e, 0x1f, 0x1e, 0x0f, 0x0e, 0x07, 0x06, 0x03, 0x02, 0x01, 0x00, 0x00 };
  
static unsigned char pec_play_bits[] U8X8_PROGMEM = {
  0x00, 0x00, 0x80, 0x07, 0x80, 0x08, 0x80, 0x08, 0x80, 0x08, 0x80, 0x08, 0x80, 0x07, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0xb8, 0x40, 0x44, 0xa2, 0x83, 0x9d, 0x00, 0x00, 0x00, 0x00 };

static unsigned char pec_record_bits[] U8X8_PROGMEM = {
  0x00, 0x00, 0x80, 0x07, 0x80, 0x08, 0x80, 0x08, 0x80, 0x08, 0x80, 0x08, 0x80, 0x07, 0x80, 0x00, 0x90, 0x04, 0xa0, 0x02, 0xc0, 0x01, 0xb8, 0x40, 0x44, 0xa2, 0x83, 0x9d, 0x00, 0x00, 0x00, 0x00 };

static unsigned char pec_wait_bits[] U8X8_PROGMEM = {
  0x00, 0x00, 0x80, 0x07, 0x80, 0x08, 0x80, 0x08, 0x80, 0x08, 0x98, 0x68, 0x98, 0x67, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0xb8, 0x40, 0x44, 0xa2, 0x83, 0x9d, 0x00, 0x00, 0x00, 0x00 };

static unsigned char W_bits[] U8X8_PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0xe0, 0x03, 0x10, 0x04, 0x08, 0x08, 0x24, 0x12, 0x22, 0x22, 0x22, 0x22, 0xa2, 0x22, 0xa2, 0x22, 0x42, 0x21, 0x44, 0x11, 0x08, 0x08, 0x10, 0x04, 0xe0, 0x03, 0x00, 0x00 };

static unsigned char E_bits[] U8X8_PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0xe0, 0x03, 0x10, 0x04, 0x08, 0x08, 0xe4, 0x13, 0x22, 0x20, 0x22, 0x20, 0xe2, 0x21, 0x22, 0x20, 0x22, 0x20, 0xe4, 0x13, 0x08, 0x08, 0x10, 0x04, 0xe0, 0x03, 0x00, 0x00 };

static unsigned char ErrMf_bits[] U8X8_PROGMEM = {
  0xff, 0xff, 0x00, 0x80, 0x0e, 0xb0, 0x02, 0xb0, 0x66, 0xb3, 0x22, 0xb1, 0x2e, 0xb1, 0x00, 0xb0, 0x22, 0xb0, 0x36, 0xb0, 0xaa, 0xb3, 0xa2, 0xb0, 0xa2, 0x81, 0xa2, 0xb0, 0xa2, 0xb0, 0x00, 0x80 };

static unsigned char ErrAlt_bits[] U8X8_PROGMEM = {
  0xff, 0xff, 0x00, 0x80, 0x0e, 0xb0, 0x02, 0xb0, 0x66, 0xb3, 0x22, 0xb1, 0x2e, 0xb1, 0x00, 0xb0, 0x1e, 0xb0, 0x06, 0xb0, 0x4a, 0xb0, 0x90, 0xb4, 0x20, 0x85, 0x00, 0xb6, 0x00, 0xb7, 0x00, 0x80 };

static unsigned char ErrLs_bits[] U8X8_PROGMEM = {
  0xff, 0xff, 0x00, 0x80, 0x0e, 0xb0, 0x02, 0xb0, 0x66, 0xb3, 0x22, 0xb1, 0x2e, 0xb1, 0x00, 0xb0, 0x00, 0xb2, 0x40, 0xb2, 0x80, 0xb2, 0xfe, 0xb3, 0x80, 0x82, 0x40, 0xb2, 0x00, 0xb2, 0x00, 0x80 };

static unsigned char ErrDe_bits[] U8X8_PROGMEM = {
  0xff, 0xff, 0x00, 0x80, 0x0e, 0xb0, 0x02, 0xb0, 0x66, 0xb3, 0x22, 0xb1, 0x2e, 0xb1, 0x00, 0xb0, 0x1e, 0xb0, 0x22, 0xb0, 0xa2, 0xb3, 0xa2, 0xb2, 0xa2, 0x83, 0xa2, 0xb0, 0x9e, 0xb3, 0x00, 0x80 };

static unsigned char ErrAz_bits[] U8X8_PROGMEM = {
  0xff, 0xff, 0x00, 0x80, 0x0e, 0xb0, 0x02, 0xb0, 0x66, 0xb3, 0x22, 0xb1, 0x2e, 0xb1, 0x00, 0xb0, 0x08, 0xb0, 0x14, 0xb0, 0xa2, 0xb3, 0x22, 0xb2, 0x3e, 0x81, 0xa2, 0xb0, 0xa2, 0xb3, 0x00, 0x80 };

static unsigned char ErrUp_bits[] U8X8_PROGMEM = {
  0xff, 0xff, 0x00, 0x80, 0x0e, 0xb0, 0x02, 0xb0, 0x66, 0xb3, 0x22, 0xb1, 0x2e, 0xb1, 0x00, 0xb0, 0x22, 0xb0, 0x22, 0xb0, 0xa2, 0xb3, 0xa2, 0xb2, 0xa2, 0x83, 0xa2, 0xb0, 0x9c, 0xb0, 0x00, 0x80 };

static unsigned char ErrMe_bits[] U8X8_PROGMEM = {
  0xff, 0xff, 0x00, 0x80, 0x0e, 0xb0, 0x02, 0xb0, 0x66, 0xb3, 0x22, 0xb1, 0x2e, 0xb1, 0x00, 0xb0, 0x22, 0xb0, 0x36, 0xb0, 0xaa, 0xb3, 0xa2, 0xb2, 0xa2, 0x83, 0xa2, 0xb0, 0xa2, 0xb3, 0x00, 0x80 };

static const unsigned char onstep_logo_bits[] U8X8_PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x80, 0x23, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0xC4, 0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFE, 0x7F, 0x00, 
  0x00, 0x00, 0x00, 0xF8, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0xFE, 0xFF, 0x07, 0x00, 0x00, 0x00, 0xFE, 0x3F, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0xFE, 0xFF, 0x0F, 0x00, 0x00, 0x00, 0xFF, 
  0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x7F, 0xF3, 0x0F, 
  0x00, 0x00, 0x80, 0x1F, 0xFC, 0xC0, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0xE0, 0x1F, 0xC3, 0x0F, 0x00, 0x00, 0xC0, 0x07, 0xF0, 0xC1, 0x03, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0xE0, 0x07, 0x83, 0x1F, 0x00, 0x00, 0xC0, 0x03, 
  0xE0, 0xC1, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x07, 0x03, 0x7F, 
  0x00, 0x00, 0xC0, 0x03, 0xE0, 0xC1, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0xE0, 0x03, 0x03, 0x7E, 0x1C, 0x3F, 0xC0, 0x03, 0xC0, 0xF1, 0x1F, 0xF0, 
  0x0F, 0xF0, 0xFC, 0x01, 0xF0, 0x03, 0x03, 0x3E, 0xFC, 0xFF, 0xC0, 0x03, 
  0x00, 0xF0, 0x1F, 0xFC, 0x3F, 0xF0, 0xFF, 0x03, 0xF8, 0x01, 0x03, 0x3E, 
  0xFC, 0xFF, 0xC1, 0x0F, 0x00, 0xF0, 0x1F, 0xFE, 0x7F, 0xF0, 0xFF, 0x07, 
  0xF8, 0x01, 0x03, 0x3C, 0xFC, 0xE0, 0x81, 0xFF, 0x00, 0xC0, 0x03, 0x1E, 
  0x78, 0xF0, 0x83, 0x0F, 0xF0, 0x01, 0x03, 0x7C, 0x7C, 0xE0, 0x01, 0xFF, 
  0x07, 0xC0, 0x03, 0x0F, 0xF0, 0xF0, 0x01, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
  0xFF, 0xFF, 0xFF, 0xFF, 0xF0, 0x01, 0x03, 0x7C, 0x3C, 0xC0, 0x03, 0x00, 
  0xFF, 0xC1, 0x83, 0x07, 0xF0, 0xF1, 0x00, 0x1E, 0xF8, 0x01, 0x03, 0x3C, 
  0x3C, 0xC0, 0x01, 0x00, 0xF8, 0xC1, 0x83, 0xFF, 0xFF, 0xF1, 0x00, 0x1E, 
  0xF8, 0x01, 0x03, 0x3E, 0x3C, 0xC0, 0x03, 0x00, 0xE0, 0xC3, 0x83, 0xFF, 
  0xFF, 0xF1, 0x00, 0x1E, 0xF0, 0x03, 0x03, 0x3E, 0x3C, 0xC0, 0xE3, 0x00, 
  0xC0, 0xC3, 0x83, 0xFF, 0xFF, 0xF0, 0x00, 0x1E, 0xE0, 0x03, 0x03, 0x7E, 
  0x3C, 0xC0, 0xE3, 0x01, 0xC0, 0xC3, 0x83, 0x07, 0x00, 0xF0, 0x00, 0x1E, 
  0xC0, 0x07, 0x03, 0x7F, 0x3C, 0xC0, 0xE3, 0x01, 0xC0, 0xC3, 0x83, 0x07, 
  0x00, 0xF0, 0x00, 0x1E, 0xE0, 0x07, 0x83, 0x1F, 0x3C, 0xC0, 0xC3, 0x03, 
  0xC0, 0xC3, 0x83, 0x0F, 0xE0, 0xF0, 0x00, 0x0E, 0xE0, 0x0F, 0xC3, 0x0F, 
  0x3C, 0xC0, 0xC3, 0x07, 0xE0, 0xC1, 0x03, 0x0F, 0xF0, 0xF0, 0x00, 0x0F, 
  0xC0, 0x3F, 0xF3, 0x07, 0x3C, 0xC0, 0x83, 0x1F, 0xF8, 0x81, 0x07, 0x1F, 
  0xF8, 0xF0, 0x81, 0x0F, 0x00, 0xFE, 0xFF, 0x0F, 0x3C, 0xC0, 0x83, 0xFF, 
  0xFF, 0x80, 0x3F, 0xFE, 0x7F, 0xF0, 0xFF, 0x07, 0x00, 0xFE, 0xFF, 0x07, 
  0x3C, 0xC0, 0x03, 0xFE, 0x7F, 0x80, 0x3F, 0xFC, 0x3F, 0xF0, 0xFF, 0x03, 
  0x00, 0xFE, 0x7F, 0x00, 0x3C, 0xC0, 0x01, 0xFC, 0x1F, 0x00, 0x3F, 0xF0, 
  0x1F, 0xF0, 0xFE, 0x01, 0x00, 0xC4, 0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x80, 0x23, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x00, 0x00, 
  0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x00, 0x00, 
  0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 
  0x76, 0x18, 0xF0, 0x01, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x80, 0x12, 0x11, 0x50, 0x01, 0x00, 0x83, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0xBB, 0x6B, 0x20, 0xE6, 
  0x8D, 0xDA, 0x97, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 
  0x2B, 0x59, 0x20, 0x55, 0x4B, 0x89, 0x5C, 0x02, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0xC0, 0x26, 0x49, 0xA0, 0x3B, 0xC9, 0x91, 0x24, 0x02, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x92, 0x24, 0x90, 0x88, 
  0x24, 0x55, 0x22, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 
  0xB2, 0x65, 0xB8, 0xBB, 0xBC, 0xCF, 0xE2, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, };

void gethms(const long& v, uint8_t& v1, uint8_t& v2, uint8_t& v3)
{
  v3 = v % 60;
  v2 = (v / 60) % 60;
  v1 = v / 3600;
}

/*
void getdms(const long& v, short& v1, uint8_t& v2, uint8_t& v3)
{
  v3 = abs(v) % 60;
  v2 = (abs(v) / 60) % 60;
  v1 = v / 3600;
}
*/

void secondsToFloat(const long& v, float& f)
{
  f = (double)v / 3600.0;
}

void getDegrees(const long& v, short& v1, uint8_t& v2, uint8_t& v3)
{
  v3 = abs(v) % 60;
  v2 = (abs(v) / 60) % 60;
  v1 = v / 3600;
}

void longRa2Ra(long Ra, int& h, int& m, int& s)
{
  h = Ra / 30;
  m = (Ra - h * 30) / 60;
  s = (Ra / 30) % 60;
}

void longDec2Dec(long Dec, int& deg, int& min)
{
  deg = Dec / 60;
  min = Dec % 60;
}

void SmartHandController::setup(const char version[], const int pin[7],const bool active[7], const int SerialBaud, const OLED model)
{
  // get "EEPROM" ready
  nv.init();
  // initialize if necessary
  initInitNvValues();
  // read the saved values
  initReadNvValues();
   
  if (strlen(version)<=19) strcpy(_version,version);
  
  telInfo.lastState = 0;
  buttonPad.setup( pin, active);
#ifdef AUX_ST4_ON
  auxST4.setup();
#endif

#ifdef UTILITY_LIGHT
  #ifdef ESP32
    ledcSetup(0, 5000, 8);
    ledcAttachPin(UTILITY_LIGHT_PIN, 0);
    ledcWrite(0, UTILITY_LIGHT);
  #else
    pinMode(UTILITY_LIGHT_PIN, OUTPUT);
    analogWrite(UTILITY_LIGHT_PIN, UTILITY_LIGHT);
  #endif
#endif

  //choose a 128x64 display supported by U8G2lib (if not listed below there are many many others in u8g2 library example Sketches)
  //U8G2_SH1106_128X64_NONAME_1_HW_I2C display(U8G2_R0);
  //U8G2_SSD1306_128X64_NONAME_F_SW_I2C display(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);   // All Boards without Reset of the Display

  if (model == OLED_SH1106)
    display = new U8G2_EXT_SH1106_128X64_NONAME_1_HW_I2C(U8G2_R0);
  else if (model == OLED_SSD1306)
    display = new U8G2_EXT_SSD1306_128X64_NONAME_F_HW_I2C(U8G2_R0);
  display->begin();
  display->setContrast(maxContrast);
  display->setFont(u8g2_font_helvR10_tf);

#ifdef DEBUG_ON
  DebugSer.begin(9600);
  delay(1000);
#endif

  // establish comms and clear the channel
  Ser.begin(SerialBaud);
  
  // display the splash screen
  drawIntro();
  tickButtons();

  for (int i = 0; i < 3; i++)
  {
    Ser.print(":#");
    delay(400);
    Ser.flush();
    delay(100);
  }

  DisplayMessage("Establishing", "Connection", 1000);
  
  // OnStep coordinate mode for getting and setting RA/Dec
  // 0 = OBSERVED_PLACE (same as not supported)
  // 1 = TOPOCENTRIC (does refraction)
  // 2 = ASTROMETRIC_J2000 (does refraction and precession/nutation)
  char s[20]="";

  int thisTry=0;
again:
  delay(4000);
  if (GetLX200(":GXEE#", s) == LX200VALUEGET) {
    if (s[0]=='0') {
      telescopeCoordinates=OBSERVED_PLACE; 
      DisplayMessage("Connection", "Warning!", 1000);
      DisplayMessage("Coordinates", "Observed Place.", 2000);
  } else 
    if (s[0]=='1') {
      telescopeCoordinates=TOPOCENTRIC; 
      DisplayMessage("Connection", "Ok!", 1000);
    } else 
    if (s[0]=='2') {
      telescopeCoordinates=ASTROMETRIC_J2000;
      DisplayMessage("Connection", "Warning!", 1000);
      DisplayMessage("Coordinates", "J2000 Mode?", 2000);
    }
  } else {
    if (++thisTry <= 4) goto again;
    telescopeCoordinates=OBSERVED_PLACE;
    DisplayMessage("Connection", "Failed!", 1000);
  }
}

void SmartHandController::tickButtons()
{
  buttonPad.tickButtons();
#ifdef AUX_ST4_ON
  auxST4.tickButtons();
#endif
}

void SmartHandController::update()
{
  // keep "EEPROM" subsystem awake
#ifndef DISABLE_EEPROM_COMMIT_ON
  nv.poll();
#endif
  
  tickButtons();
  unsigned long top = millis();

  // sleep and wake up display
  if (buttonPad.anyPressed()) {
    if (sleepDisplay) { display->setContrast(maxContrast); display->sleepOff(); sleepDisplay = false; lowContrast = false; buttonPad.clearAllPressed(); time_last_action = millis(); }
    if (lowContrast)  { display->setContrast(maxContrast); lowContrast = false; time_last_action = top; }
  } else if (sleepDisplay) return;
  if (display_blank_time && top - time_last_action > display_blank_time) { display->sleepOn(); sleepDisplay = true; return; }
  if (display_dim_time && top - time_last_action > display_dim_time && !lowContrast) { display->setContrast(0); lowContrast = true; return; }

  // power cycle reqd message
  if (powerCylceRequired) { display->setFont(u8g2_font_helvR12_tf); DisplayMessage("REBOOT", "DEVICE", 1000); return; }
  
  if (telInfo.align == Telescope::ALI_SELECT_STAR_1 || telInfo.align == Telescope::ALI_SELECT_STAR_2 || telInfo.align == Telescope::ALI_SELECT_STAR_3 || 
      telInfo.align == Telescope::ALI_SELECT_STAR_4 || telInfo.align == Telescope::ALI_SELECT_STAR_5 || telInfo.align == Telescope::ALI_SELECT_STAR_6 ||
      telInfo.align == Telescope::ALI_SELECT_STAR_7 || telInfo.align == Telescope::ALI_SELECT_STAR_8 || telInfo.align == Telescope::ALI_SELECT_STAR_9) {
    char message[10]="Star# ?";
    if (telInfo.align == Telescope::ALI_SELECT_STAR_1) strcpy(message,"Star #1");
    if (telInfo.align == Telescope::ALI_SELECT_STAR_2) strcpy(message,"Star #2");
    if (telInfo.align == Telescope::ALI_SELECT_STAR_3) strcpy(message,"Star #3");
    if (telInfo.align == Telescope::ALI_SELECT_STAR_4) strcpy(message,"Star #4");
    if (telInfo.align == Telescope::ALI_SELECT_STAR_5) strcpy(message,"Star #5");
    if (telInfo.align == Telescope::ALI_SELECT_STAR_6) strcpy(message,"Star #6");
    if (telInfo.align == Telescope::ALI_SELECT_STAR_7) strcpy(message,"Star #7");
    if (telInfo.align == Telescope::ALI_SELECT_STAR_8) strcpy(message,"Star #8");
    if (telInfo.align == Telescope::ALI_SELECT_STAR_9) strcpy(message,"Star #9");
    DisplayLongMessage("Select a Star", "From following list", "", message, -1);

    // bring up the list of stars so user can goto the alignment star
    if (!SelectStarAlign()) { DisplayMessage("Alignment", "Aborted", -1); telInfo.align = Telescope::ALI_OFF; return; }
    
    // mark this as the next alignment star 
    telInfo.align = static_cast<Telescope::AlignState>(telInfo.align + 1);
  } else
  if (top - lastpageupdate > BACKGROUND_CMD_RATE/2) updateMainDisplay(page);
  
  if (telInfo.connected == false) DisplayMessage("Hand controler", "not connected", -1);

  // is a goto happening?
  if (telInfo.connected && (telInfo.getTrackingState() == Telescope::TRK_SLEWING || telInfo.getParkState() == Telescope::PRK_PARKING))
  {
    if (buttonPad.nsewPressed()) {
      Ser.print(":Q#"); Ser.flush();
      if (telInfo.align != Telescope::ALI_OFF) telInfo.align = static_cast<Telescope::AlignState>(telInfo.align - 1); // try another align star?
      time_last_action = millis();
      display->sleepOff();
      buttonPad.clearAllPressed();
      DisplayMessage("Slew to target", "canceled!", 1000);
      return;
    }
  } else

  // -------------------------------------------------------------------------------------------------------------------
  // handle the guiding buttons
  {
    buttonCommand = false;
#ifdef AUX_ST4_ON
    if (!moveEast  && (buttonPad.e.isDown() || auxST4.e.isDown())) { moveEast = true;   Ser.write(ccMe); buttonCommand=true; } else
    if (moveEast   && (buttonPad.e.isUp()   && auxST4.e.isUp()))   { moveEast = false;  Ser.write(ccQe); buttonCommand=true; buttonPad.e.clearPress(); auxST4.e.clearPress(); }
    if (!moveWest  && (buttonPad.w.isDown() || auxST4.w.isDown())) { moveWest = true;   Ser.write(ccMw); buttonCommand=true; } else
    if (moveWest   && (buttonPad.w.isUp()   && auxST4.w.isUp()))   { moveWest = false;  Ser.write(ccQw); buttonCommand=true; buttonPad.w.clearPress(); auxST4.w.clearPress(); }
    if (!moveNorth && (buttonPad.n.isDown() || auxST4.n.isDown())) { moveNorth = true;  Ser.write(ccMn); buttonCommand=true; } else
    if (moveNorth  && (buttonPad.n.isUp()   && auxST4.n.isUp()))   { moveNorth = false; Ser.write(ccQn); buttonCommand=true; buttonPad.n.clearPress(); auxST4.n.clearPress(); }
    if (!moveSouth && (buttonPad.s.isDown() || auxST4.s.isDown())) { moveSouth = true;  Ser.write(ccMs); buttonCommand=true; } else
    if (moveSouth  && (buttonPad.s.isUp()   && auxST4.s.isUp()))   { moveSouth = false; Ser.write(ccQs); buttonCommand=true; buttonPad.s.clearPress(); auxST4.s.clearPress(); }
#else
    if (!moveEast  && (buttonPad.e.isDown())) { moveEast = true;   Ser.write(ccMe); buttonCommand=true; } else
    if (moveEast   && (buttonPad.e.isUp()  )) { moveEast = false;  Ser.write(ccQe); buttonCommand=true; buttonPad.e.clearPress(); }
    if (!moveWest  && (buttonPad.w.isDown())) { moveWest = true;   Ser.write(ccMw); buttonCommand=true; } else
    if (moveWest   && (buttonPad.w.isUp()  )) { moveWest = false;  Ser.write(ccQw); buttonCommand=true; buttonPad.w.clearPress(); }
    if (!moveNorth && (buttonPad.n.isDown())) { moveNorth = true;  Ser.write(ccMn); buttonCommand=true; } else
    if (moveNorth  && (buttonPad.n.isUp()  )) { moveNorth = false; Ser.write(ccQn); buttonCommand=true; buttonPad.n.clearPress(); }
    if (!moveSouth && (buttonPad.s.isDown())) { moveSouth = true;  Ser.write(ccMs); buttonCommand=true; } else
    if (moveSouth  && (buttonPad.s.isUp()  )) { moveSouth = false; Ser.write(ccQs); buttonCommand=true; buttonPad.s.clearPress(); }
#endif
    if (buttonCommand) { time_last_action = millis(); return; }
  }

  // -------------------------------------------------------------------------------------------------------------------
  // handle the feature buttons
  char cmd[32];
  static bool focOut=false;
  static bool focIn=false;
  static bool rotCw=false;
  static bool rotCcw=false;
  buttonCommand=false;
  if (telInfo.align != Telescope::ALI_OFF) featureKeyMode = 1;
  switch (featureKeyMode) {
    case 1:   // guide rate
      if (buttonPad.F.wasPressed()) { activeGuideRate--; strcpy(briefMessage,"Guide Slower"); buttonCommand=true; } else
      if (buttonPad.f.wasPressed()) { activeGuideRate++; strcpy(briefMessage,"Guide Faster"); buttonCommand=true; }
      if (buttonCommand) {
        if (activeGuideRate<1)  activeGuideRate=1;
        if (activeGuideRate>10) activeGuideRate=10;
        char cmd[5]= ":Rn#"; cmd[2] = '0' + activeGuideRate - 1;
        DisplayMessageLX200(SetLX200(cmd));
      }
    break;
    case 2:   // util. light
#ifdef UTILITY_LIGHT
      if (buttonPad.F.wasPressed()) { current_selection_utility_light--; strcpy(briefMessage,"Util Dimmer"); buttonCommand=true; } else
      if (buttonPad.f.wasPressed()) { current_selection_utility_light++; strcpy(briefMessage,"Util Brighter"); buttonCommand=true; }
      if (buttonCommand) {
        if (current_selection_utility_light<1) current_selection_utility_light=1;
        if (current_selection_utility_light>6) current_selection_utility_light=6;
        int i; switch(current_selection_utility_light) { case 1: i=0; break; case 2: i=15; break; case 3: i=31; break; case 4: i=63; break; case 5: i=127; break; case 6: i=255; break; default: i=127; break; }
#ifdef ESP32
        ledcWrite(0, i);
#else
        analogWrite(UTILITY_LIGHT_PIN, i);
#endif
      }
#endif
    break;
    case 3:  // reticule
      if (buttonPad.F.wasPressed()) { Ser.print(":B-#"); strcpy(briefMessage,"Reticle Dimmer"); } else
      if (buttonPad.f.wasPressed()) { Ser.print(":B+#"); strcpy(briefMessage,"Reticle Brighter"); }
    break;
    case 4: case 5:  // focuser1/2
      if (featureKeyMode==4) strcpy(cmd,":FA1#"); else strcpy(cmd,":FA2#"); 
           if (!focOut && buttonPad.F.isDown()) { focOut = true;  strcat(cmd,":FS#:F+#"); SetLX200(cmd); strcpy(briefMessage,"Focus Out"); buttonCommand=true; }
      else if ( focOut && buttonPad.F.isUp())   { focOut = false; Ser.print(":FQ#"); buttonCommand=true; buttonPad.F.clearPress(); }
      else if (!focIn  && buttonPad.f.isDown()) { focIn = true;   strcat(cmd,":FS#:F-#"); SetLX200(cmd); strcpy(briefMessage,"Focus In"); buttonCommand=true; }
      else if ( focIn  && buttonPad.f.isUp())   { focIn = false;  Ser.print(":FQ#"); buttonCommand=true; buttonPad.f.clearPress(); }
#ifndef FOCUSER_ACCELERATE_DISABLE_ON
      // acceleration control
      else if ((focOut && buttonPad.F.isDown() && (buttonPad.F.timeDown()>5000))) { Ser.print(":FF#:F+#"); strcpy(briefMessage,"Focus Fast"); }
      else if ((focIn  && buttonPad.f.isDown() && (buttonPad.f.timeDown()>5000))) { Ser.print(":FF#:F-#"); strcpy(briefMessage,"Focus Fast"); }
#endif
    break;
    case 6:  // rotator
           if (!rotCcw && buttonPad.F.isDown()) { rotCcw = true;  Ser.print(":r2#:rc#:r<#"); strcpy(briefMessage,"Rotate Ccw"); buttonCommand=true; }
      else if ( rotCcw && buttonPad.F.isUp())   { rotCcw = false; Ser.print(":rQ#"); buttonCommand=true; buttonPad.F.clearPress(); }
      else if (!rotCw  && buttonPad.f.isDown()) { rotCw = true;   Ser.print(":r2#:rc#:r>#"); strcpy(briefMessage,"Rotate Cw"); buttonCommand=true; }
      else if ( rotCw  && buttonPad.f.isUp())   { rotCw = false;  Ser.print(":rQ#"); buttonCommand=true; buttonPad.f.clearPress(); }
      // acceleration control
      else if ((rotCcw && buttonPad.F.isDown() && (buttonPad.F.timeDown()>5000))) { Ser.print(":r4#:rc#:r<#"); strcpy(briefMessage,"Rotate Fast"); }
      else if (( rotCw && buttonPad.f.isDown() && (buttonPad.f.timeDown()>5000))) { Ser.print(":r4#:rc#:r>#"); strcpy(briefMessage,"Rotate Fast"); }
    break;
  }
  if (buttonCommand) { time_last_action = millis(); return; }

  // -------------------------------------------------------------------------------------------------------------------
  // handle shift button features
  if (buttonPad.shift.isDown()) {
    // a long press brings up the main menu
    if ((buttonPad.shift.timeDown()>1000) && telInfo.align == Telescope::ALI_OFF) { menuMain(); time_last_action = millis(); } // bring up the menus
  } else {
    // wait long enough that a double press can happen before picking up other press events
    if (buttonPad.shift.timeUp()>250) {
      if (buttonPad.shift.wasDoublePressed()) { 
        if (telInfo.align == Telescope::ALI_OFF) {
          // display feature key menu OR...
          menuFeatureKey();
        } else {
          // ...if aligning, go back and select a different star
          if ((telInfo.align == Telescope::ALI_RECENTER_1 || telInfo.align == Telescope::ALI_RECENTER_2 || telInfo.align == Telescope::ALI_RECENTER_3 ||
             telInfo.align == Telescope::ALI_RECENTER_4 || telInfo.align == Telescope::ALI_RECENTER_5 || telInfo.align == Telescope::ALI_RECENTER_6 ||
             telInfo.align == Telescope::ALI_RECENTER_7 || telInfo.align == Telescope::ALI_RECENTER_8 || telInfo.align == Telescope::ALI_RECENTER_9)) {
            telInfo.align = static_cast<Telescope::AlignState>(telInfo.align - 2);
          }
        }
        time_last_action = millis();
      } else
      if (buttonPad.shift.wasPressed()) {
        if (telInfo.align == Telescope::ALI_OFF) {
          // cycles through disp of Eq, Hor, Time, Ambient OR...
          page++;
  #ifdef AMBIENT_CONDITIONS_ON
          if (page > 3) page = 0;
  #else
          if (page > 2) page = 0;
  #endif
        } else {
          // ...if aligning, accept the align star
          if ((telInfo.align == Telescope::ALI_RECENTER_1 || telInfo.align == Telescope::ALI_RECENTER_2 || telInfo.align == Telescope::ALI_RECENTER_3 ||
             telInfo.align == Telescope::ALI_RECENTER_4 || telInfo.align == Telescope::ALI_RECENTER_5 || telInfo.align == Telescope::ALI_RECENTER_6 ||
             telInfo.align == Telescope::ALI_RECENTER_7 || telInfo.align == Telescope::ALI_RECENTER_8 || telInfo.align == Telescope::ALI_RECENTER_9)) {
            if (telInfo.addStar()) { if (telInfo.align == Telescope::ALI_OFF) DisplayMessage("Alignment", "Success!", 2000); else DisplayMessage("Add Star", "Success!", 2000); } else DisplayMessage("Add Star", "Failed!", -1);
          }
        }
        time_last_action = millis();
      }
    }
  }
}

void SmartHandController::updateMainDisplay( u8g2_uint_t page)
{
  u8g2_t *u8g2 = display->getU8g2();
  display->setFont(u8g2_font_helvR12_tf);
  u8g2_uint_t line_height = u8g2_GetAscent(u8g2) - u8g2_GetDescent(u8g2) + MY_BORDER_SIZE;

  // get the status
  telInfo.connected = true;
  telInfo.updateSeq++;
  telInfo.updateTel();
  if (telInfo.connected == false) return;

  // detect align mode
  if (telInfo.hasTelStatus && telInfo.align != Telescope::ALI_OFF)
  {
    telInfo.updateTel(true); // really make sure we have the status
    Telescope::TrackState curT = telInfo.getTrackingState();
    if (curT != Telescope::TRK_SLEWING && 
       (telInfo.align == Telescope::ALI_SLEW_STAR_1 || telInfo.align == Telescope::ALI_SLEW_STAR_2 || telInfo.align == Telescope::ALI_SLEW_STAR_3 || 
        telInfo.align == Telescope::ALI_SLEW_STAR_4 || telInfo.align == Telescope::ALI_SLEW_STAR_5 || telInfo.align == Telescope::ALI_SLEW_STAR_6 ||
        telInfo.align == Telescope::ALI_SLEW_STAR_7 || telInfo.align == Telescope::ALI_SLEW_STAR_8 || telInfo.align == Telescope::ALI_SLEW_STAR_9)
       ) telInfo.align = static_cast<Telescope::AlignState>(telInfo.align + 1);
    page = 4;
  }

  // update status info.
  if (page == 0)
    telInfo.updateRaDec();
  else
    if (page == 1)
      telInfo.updateAzAlt();
    else
      if (page == 2)
        telInfo.updateTime();
 
  // the graphics loop
  u8g2_FirstPage(u8g2);
  do
  {
    u8g2_uint_t x = u8g2_GetDisplayWidth(u8g2);

    // OnStep status
    if (telInfo.hasTelStatus) { 

      // update guide rate (if available)
      if (telInfo.getGuideRate()>=0) {
        char string_Speed[][8] = {"¼x","½x","1x","2x","4x","8x","20x","48x","½Mx","Max"};
        char string_PSpeed[][6] = {" ¼x"," ½x"," 1x"};
        int gr=telInfo.getGuideRate(); activeGuideRate=gr+1;
        int pgr=telInfo.getPulseGuideRate();
        if ((pgr!=gr) && (pgr>=0) && (pgr<3)) strcat(string_Speed[gr],string_PSpeed[pgr]); 
        if ((gr>=0) && (gr<=9)) {
          display->setFont(u8g2_font_helvR10_tf);
          u8g2_DrawUTF8(u8g2, 0, icon_height, string_Speed[gr]);
          display->setFont(u8g2_font_helvR12_tf);
        }
      }

      Telescope::ParkState curP = telInfo.getParkState();
      Telescope::TrackState curT = telInfo.getTrackingState();
      Telescope::TrackRate curTR = telInfo.getTrackingRate();
    
      if (curP == Telescope::PRK_PARKED)  { display->drawXBMP(x - icon_width, 0, icon_width, icon_height, parked_bits); x -= icon_width + 1; } else
      if (curP == Telescope::PRK_PARKING) { display->drawXBMP(x - icon_width, 0, icon_width, icon_height, parking_bits); x -= icon_width + 1; } else
      if (telInfo.atHome())               { display->drawXBMP(x - icon_width, 0, icon_width, icon_height, home_bits); x -= icon_width + 1;  } else 
      {
        if (curT == Telescope::TRK_SLEWING) { display->drawXBMP(x - icon_width, 0, icon_width, icon_height, sleewing_bits); x -= icon_width + 1; } else
        if (curT == Telescope::TRK_ON)      
        {
          if (curTR == Telescope::TR_SIDEREAL) {
            Telescope::RateCompensation rc = telInfo.getRateCompensation();
            if (Telescope::RC_NONE      == rc) { display->drawXBMP(x - icon_width, 0, icon_width, icon_height, tracking_sid_bits);    x -= icon_width + 1; } else
            if (Telescope::RC_REFR_RA   == rc) { display->drawXBMP(x - icon_width, 0, icon_width, icon_height, tracking_sid_r_bits);  x -= icon_width + 1; } else
            if (Telescope::RC_REFR_BOTH == rc) { display->drawXBMP(x - icon_width, 0, icon_width, icon_height, tracking_sid_rd_bits); x -= icon_width + 1; } else
            if (Telescope::RC_FULL_RA   == rc) { display->drawXBMP(x - icon_width, 0, icon_width, icon_height, tracking_sid_f_bits);  x -= icon_width + 1; } else
            if (Telescope::RC_FULL_BOTH == rc) { display->drawXBMP(x - icon_width, 0, icon_width, icon_height, tracking_sid_fd_bits); x -= icon_width + 1; }
          } else
          if (curTR == Telescope::TR_LUNAR)  { display->drawXBMP(x - icon_width, 0, icon_width, icon_height, tracking_lun_bits); x -= icon_width + 1; } else
          if (curTR == Telescope::TR_SOLAR)  { display->drawXBMP(x - icon_width, 0, icon_width, icon_height, tracking_sol_bits); x -= icon_width + 1; } else
                                             { display->drawXBMP(x - icon_width, 0, icon_width, icon_height, tracking_S_bits); x -= icon_width + 1; }
         } else
        if (curT == Telescope::TRK_OFF) { display->drawXBMP(x - icon_width, 0, icon_width, icon_height, no_tracking_bits); x -= icon_width + 1; }

        if (curP == Telescope::PRK_FAILED) { display->drawXBMP(x - icon_width, 0, icon_width, icon_height, parkingFailed_bits); x -= icon_width + 1; }

        Telescope::PierState CurP = telInfo.getPierState();
        if (CurP == Telescope::PIER_E) { display->drawXBMP(x - icon_width, 0, icon_width, icon_height, E_bits); x -= icon_width + 1; } else 
        if (CurP == Telescope::PIER_W) { display->drawXBMP(x - icon_width, 0, icon_width, icon_height, W_bits); x -= icon_width + 1; }

        if (telInfo.align != Telescope::ALI_OFF) {
          if (telInfo.aliMode == Telescope::ALIM_ONE)   { display->drawXBMP(x - icon_width, 0, icon_width, icon_height, align1_bits); x -= icon_width + 1; } else 
          if (telInfo.aliMode == Telescope::ALIM_TWO)   { display->drawXBMP(x - icon_width, 0, icon_width, icon_height, align2_bits); x -= icon_width + 1; } else
          if (telInfo.aliMode == Telescope::ALIM_THREE) { display->drawXBMP(x - icon_width, 0, icon_width, icon_height, align3_bits); x -= icon_width + 1; } else
          if (telInfo.aliMode == Telescope::ALIM_FOUR)  { display->drawXBMP(x - icon_width, 0, icon_width, icon_height, align4_bits); x -= icon_width + 1; } else
          if (telInfo.aliMode == Telescope::ALIM_FIVE)  { display->drawXBMP(x - icon_width, 0, icon_width, icon_height, align5_bits); x -= icon_width + 1; } else
          if (telInfo.aliMode == Telescope::ALIM_SIX)   { display->drawXBMP(x - icon_width, 0, icon_width, icon_height, align6_bits); x -= icon_width + 1; } else
          if (telInfo.aliMode == Telescope::ALIM_SEVEN) { display->drawXBMP(x - icon_width, 0, icon_width, icon_height, align7_bits); x -= icon_width + 1; } else
          if (telInfo.aliMode == Telescope::ALIM_EIGHT) { display->drawXBMP(x - icon_width, 0, icon_width, icon_height, align8_bits); x -= icon_width + 1; } else
          if (telInfo.aliMode == Telescope::ALIM_NINE ) { display->drawXBMP(x - icon_width, 0, icon_width, icon_height, align9_bits); x -= icon_width + 1; }
        }

        if (telInfo.isPecPlaying())   { display->drawXBMP(x - icon_width, 0, icon_width, icon_height, pec_play_bits);   x -= icon_width + 1; } else
        if (telInfo.isPecRecording()) { display->drawXBMP(x - icon_width, 0, icon_width, icon_height, pec_record_bits); x -= icon_width + 1; } else
        if (telInfo.isPecWaiting())   { display->drawXBMP(x - icon_width, 0, icon_width, icon_height, pec_wait_bits);   x -= icon_width + 1; }

        if (telInfo.isGuiding()) { display->drawXBMP(x - icon_width, 0, icon_width, icon_height, guiding_bits); x -= icon_width + 1; }
      }

      if (!telInfo.isGuiding()) {
        switch (telInfo.getError()) {
          case Telescope::ERR_MOTOR_FAULT: display->drawXBMP(x - icon_width, 0, icon_width, icon_height, ErrMf_bits); x -= icon_width + 1; break; // motor fault
          case Telescope::ERR_ALT_MIN:     display->drawXBMP(x - icon_width, 0, icon_width, icon_height, ErrAlt_bits);x -= icon_width + 1; break; // above below horizon
          case Telescope::ERR_LIMIT_SENSE: display->drawXBMP(x - icon_width, 0, icon_width, icon_height, ErrLs_bits); x -= icon_width + 1; break; // physical limit switch triggered
          case Telescope::ERR_DEC:         display->drawXBMP(x - icon_width, 0, icon_width, icon_height, ErrDe_bits); x -= icon_width + 1; break; // past the rarely used Dec limit
          case Telescope::ERR_AZM:         display->drawXBMP(x - icon_width, 0, icon_width, icon_height, ErrAz_bits); x -= icon_width + 1; break; // for AltAz mounts, past limit in Az
          case Telescope::ERR_UNDER_POLE:  display->drawXBMP(x - icon_width, 0, icon_width, icon_height, ErrUp_bits); x -= icon_width + 1; break; // for Eq mounts, past limit in HA
          case Telescope::ERR_MERIDIAN:    display->drawXBMP(x - icon_width, 0, icon_width, icon_height, ErrMe_bits); x -= icon_width + 1; break; // for Eq mounts, past meridian limit
          case Telescope::ERR_ALT_MAX:     display->drawXBMP(x - icon_width, 0, icon_width, icon_height, ErrAlt_bits);x -= icon_width + 1; break; // above overhead
          default: break;
        }
      }
    }

    // show brief message, simply place the message in the briefMessage string and it'll show for one second
    static char lastMessage[20]="";
    if ((strlen(briefMessage)!=0) || (strlen(lastMessage)!=0)) {
      static unsigned long startTime=0;
      if (strlen(briefMessage)!=0) { startTime=millis(); strcpy(lastMessage,briefMessage); strcpy(briefMessage,""); }

      x = u8g2_GetDisplayWidth(u8g2);  u8g2_uint_t y = 36;  u8g2_uint_t x1 = u8g2_GetStrWidth(u8g2,lastMessage);
      u8g2_DrawUTF8(u8g2, (x/2)-(x1/2), y+8, lastMessage);

      if (millis()-startTime>1000) { startTime=0; strcpy(lastMessage,""); }
    } else

    // show equatorial coordinates
    if (page == 0)
    {
      if (telInfo.hasInfoRa && telInfo.hasInfoDec) {
        char rs[20]; strcpy(rs,telInfo.TempRa); int l=strlen(rs); if (l>1) rs[l-1]=0;
        u8g2_uint_t x = u8g2_GetDisplayWidth(u8g2)-u8g2_GetUTF8Width(u8g2,"00000000");
        u8g2_uint_t y = 36;

        u8g2_DrawUTF8(u8g2, 0, y, "RA");
        display->DrawFwNumeric(x, y, rs);

        char ds[20]; strcpy(ds,telInfo.TempDec); l=strlen(ds); if (l>1) ds[l-1]=0; if (l>8) { ds[3]='\xb0'; ds[6]='\''; }
        x = u8g2_GetDisplayWidth(u8g2)-u8g2_GetUTF8Width(u8g2,"000000000");
        y += line_height + 4;
        u8g2_DrawUTF8(u8g2, 0, y, "Dec"); 
        display->DrawFwNumeric(x, y, ds);
      }
    } else

    // show horizon coordinates
    if (page == 1) {
      if (telInfo.hasInfoAz && telInfo.hasInfoAlt)
      {
        char zs[20]; strcpy(zs,telInfo.TempAz); int l=strlen(zs); if (l>1) zs[l-1]=0; if (l>8) { zs[3]='\xb0'; zs[6]='\''; }
        x = u8g2_GetDisplayWidth(u8g2)-u8g2_GetUTF8Width(u8g2,"000000000");
        u8g2_uint_t y = 36; 
        u8g2_DrawUTF8(u8g2, 0, y, "Az");
        display->DrawFwNumeric(x,y,zs);

        char as[20]; strcpy(as,telInfo.TempAlt); l=strlen(as); if (l>1) as[l-1]=0; if (l>8) { as[3]='\xb0'; as[6]='\''; }
        y += line_height + 4; 
        u8g2_DrawUTF8(u8g2, 0, y, "Alt");
        display->DrawFwNumeric(x,y,as);
      }
    } else
    
    // show time
    if (page == 2) {
      if (telInfo.hasInfoUTC && telInfo.hasInfoSidereal)
      {
        char us[20]; strcpy(us,telInfo.TempUniversalTime); int l=strlen(us); if (l>1) us[l-1]=0;
        x = u8g2_GetDisplayWidth(u8g2)-u8g2_GetUTF8Width(u8g2,"00000000");
        u8g2_uint_t y = 36;
        display->setFont(u8g2_font_helvR10_tf); u8g2_DrawUTF8(u8g2, 0, y, "UT"); display->setFont(u8g2_font_helvR12_tf);
        display->DrawFwNumeric(x,y,us);

        char ss[20]; strcpy(ss,telInfo.TempSidereal); l=strlen(ss); if (l>1) ss[l-1]=0;
        y += line_height + 4;
        u8g2_DrawUTF8(u8g2, 0, y, "LST");
        display->DrawFwNumeric(x,y,ss);
      }
    } else

    if (page == 3) {
      // T24.6 P997mb
      // H46% DP13.7C
      display->setFont(u8g2_font_helvR10_tf);

      double T,P,H,DP;
      if (telInfo.getT(T) && telInfo.getP(P) && telInfo.getH(H) && telInfo.getDP(DP)) {
        char temp[20],line[20];
        u8g2_uint_t y = 36;
        u8g2_uint_t dx = u8g2_GetDisplayWidth(u8g2);

        dtostrf(T,3,1,temp);
        sprintf(line,"T%s\xb0%s",temp,"C");
        display->DrawFwNumeric(0,y,line);

        sprintf(line,"P%dmb",(int)round(P));
        display->DrawFwNumeric(dx-display->GetFwNumericWidth(line),y,line);

        y += line_height + 4;
        sprintf(line,"H%d%%",(int)round(H));
        display->DrawFwNumeric(0,y,line);

        dtostrf(DP,3,1,temp);
        sprintf(line,"DP%s\xb0%s",temp,"C");
        display->DrawFwNumeric(dx-display->GetFwNumericWidth(line),y,line);
      }
      
      display->setFont(u8g2_font_helvR12_tf);
    } else

    // show align status
    if (page == 4) {
      u8g2_uint_t y = 36;

      char txt[20];
      if ((telInfo.align - 1) % 3 == 1) sprintf(txt, "Slewing to Star %u", (telInfo.align - 1) / 3 + 1); else
      if ((telInfo.align - 1) % 3 == 2) sprintf(txt, "Recenter Star %u", (telInfo.align - 1) / 3 + 1);
      u8g2_DrawUTF8(u8g2, 0, y, txt);

      y += line_height + 4;
      u8g2_SetFont(u8g2, u8g2_font_unifont_t_greek);
      u8g2_DrawGlyph(u8g2, 0, y, 945 + cat_mgr.bayerFlam());

      const uint8_t* myfont = u8g2->font; u8g2_SetFont(u8g2, myfont);
      u8g2_DrawUTF8(u8g2, 16, y, cat_mgr.constellationStr());
    }
    
  } while (u8g2_NextPage(u8g2));
  lastpageupdate = millis();
}

void SmartHandController::drawIntro()
{
  display->firstPage();
  do {
    display->drawXBMP(0, 0, onstep_logo_width, onstep_logo_height, onstep_logo_bits);
  } while (display->nextPage());
  delay(1000);
}

#include "MenuMain.h"

// misc.

bool SmartHandController::SelectStarAlign()
{
  cat_mgr.setLat(telInfo.getLat()); cat_mgr.setLstT0(telInfo.getLstT0());
  cat_mgr.select(0);

  cat_mgr.filtersClear();
  cat_mgr.filterAdd(FM_ALIGN_ALL_SKY);

  cat_mgr.setIndex(0);
  if (cat_mgr.isInitialized()) {
    if (display->UserInterfaceCatalog(&buttonPad, "Select Star")) {
      bool ok = DisplayMessageLX200(SyncSelectedStarLX200(cat_mgr.getIndex()),false);
      return ok;
    }
  }
  return false;
}

void SmartHandController::DisplayMessage(const char* txt1, const char* txt2, int duration)
{
  uint8_t x;
  uint8_t y = 40;
  display->firstPage();
  do {
    if (txt2 != NULL)
    {
      y = 50;
      x = (display->getDisplayWidth() - display->getStrWidth(txt2)) / 2;
      display->drawStr(x, y, txt2);
      y = 25;
    }
    x = (display->getDisplayWidth() - display->getStrWidth(txt1)) / 2;
    display->drawStr(x, y, txt1);
  } while (display->nextPage());
  if (duration >= 0) delay(duration); else { buttonPad.waitForPress(); buttonPad.clearAllPressed(); }
}

void SmartHandController::DisplayLongMessage(const char* txt1, const char* txt2, const char* txt3, const char* txt4, int duration)
{
  display->setFont(u8g2_font_helvR10_tf);
  uint8_t h = 15;
  uint8_t x = 0;
  uint8_t y = h;
  display->firstPage();
  do {

    y = h;
    x = (display->getDisplayWidth() - display->getStrWidth(txt1)) / 2;
    display->drawStr(x, y, txt1);
    y += h;
    if (txt2 != NULL)
    {
      x = 0;
      display->drawStr(x, y, txt2);
    }
    else
    {
      y -= 7;
    }
    y += 15;
    if (txt3 != NULL)
    {
      x = 0;
      display->drawStr(x, y, txt3);
    }

    y += 15;
    if (txt4 != NULL)
    {
      x = 0;
      display->drawStr(x, y, txt4);
    }
  } while (display->nextPage());
  if (duration >= 0) delay(duration); else { buttonPad.waitForPress(); buttonPad.clearAllPressed(); }

  display->setFont(u8g2_font_helvR12_tf);
}
  
bool SmartHandController::DisplayMessageLX200(LX200RETURN val, bool silentOk)
{
  char text1[20] = "";
  char text2[20] = "";
  int time = -1;
  if (val < LX200OK)
  {
         if (val == LX200NOTOK)                    { sprintf(text1, "LX200 Command"); sprintf(text2, "has failed!");    }
    else if (val == LX200SETVALUEFAILED)           { sprintf(text1, "Set Value");     sprintf(text2, "has failed!");    }
    else if (val == LX200GETVALUEFAILED)           { sprintf(text1, "Get Value");     sprintf(text2, "has failed!");    }
    else if (val == LX200SETTARGETFAILED)          { sprintf(text1, "Set Target");    sprintf(text2, "has failed!");    }
    else if (val == LX200NOOBJECTSELECTED)         { sprintf(text1, "No Object");     sprintf(text2, "Selected!");      }
    else if (val == LX200_GOTO_ERR_BELOW_HORIZON)  { sprintf(text1, "Target is");     sprintf(text2, "Below Horizon!"); }
    else if (val == LX200_GOTO_ERR_ABOVE_OVERHEAD) { sprintf(text1, "Target is");     sprintf(text2, "Above Limit!");   }
    else if (val == LX200_GOTO_ERR_STANDBY)        { sprintf(text1, "Telescope");     sprintf(text2, "in standby!");    }
    else if (val == LX200_GOTO_ERR_PARK)           { sprintf(text1, "Telescope");     sprintf(text2, "is Parked!");     }
    else if (val == LX200_GOTO_ERR_GOTO)           { sprintf(text1, "Goto already");  sprintf(text2, "in progress!");   }
    else if (val == LX200_GOTO_ERR_OUTSIDE_LIMITS) { sprintf(text1, "Target");        sprintf(text2, "outside lmts!");  }
    else if (val == LX200_GOTO_ERR_HARDWARE_FAULT) { sprintf(text1, "Telescope");     sprintf(text2, "h/w fault!");     }
    else if (val == LX200_GOTO_ERR_IN_MOTION)      { sprintf(text1, "Telescope");     sprintf(text2, "in motion!");     }
    else if (val == LX200_GOTO_ERR_UNSPECIFIED)    { sprintf(text1, "Goto unknown");  sprintf(text2, "error!");         }
    else { sprintf(text1, "Error"); sprintf(text2, "-1"); }
    DisplayMessage(text1, text2, -1);
  }
  else if (!silentOk)
  {
    time = 1000;
         if (val == LX200OK)            { sprintf(text1, "LX200 Command"); sprintf(text2, "Done!");   }
    else if (val == LX200VALUESET)      { sprintf(text1, "Value");         sprintf(text2, "Set!");    }
    else if (val == LX200VALUEGET)      { sprintf(text1, "Value");         sprintf(text2, "Get!");    }
    else if (val == LX200SYNCED)        { sprintf(text1, "Telescope");     sprintf(text2, "Synced!"); }
    else if (val == LX200_GOTO_GOINGTO) { sprintf(text1, "Slew to");       sprintf(text2, "Target");  }
    DisplayMessage(text1, text2, time);
  }
  return isOk(val);
}
