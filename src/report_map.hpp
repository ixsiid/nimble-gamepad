#include <stdint.h>

#define GAMEPAD_REPORT_MAP(n) \
    0x05, 0x01,        /* USAGE_PAGE (Generic Desktop)         */                            \
    0x09, 0x05,        /* USAGE (Gamepad)                      */                            \
    0xa1, 0x01,        /* COLLECTION (Application)             */                            \
    0x09, 0x01,        /*   USAGE (Pointer)                    */                            \
    0xa1, 0x00,        /*   COLLECTION (Physical)              */                            \
    0x85,  n+1,        /*     REPORT_ID (first report id is 1) */                            \
    /* ---------- Buttons (1 to 8)  -------------------------------------- */                \
    0x05, 0x09,        /*     USAGE_PAGE (Button)              */                            \
    0x19, 0x01,        /*     USAGE_MINIMUM (Button 1)         */                            \
    0x29, 0x08,        /*     USAGE_MAXIMUM (Button 8)         */                            \
    0x15, 0x00,        /*     LOGICAL_MINIMUM (0)              */                            \
    0x25, 0x01,        /*     LOGICAL_MAXIMUM (1)              */                            \
    0x75, 0x01,        /*     REPORT_SIZE (1)                  */                            \
    0x95, 0x08,        /*     REPORT_COUNT (8)                 */                            \
    0x81, 0x02,        /*     INPUT (Data, Variable, Absolute) ;8 bits */                    \
    /* ---------- X/Y/Z position    -------------------------------------- */                \
    0x05, 0x01,	   /*     USAGE_PAGE (Generic Desktop)     */                            \
    0x09, 0x30,	   /*     USAGE (X)                        */                            \
    0x09, 0x31,	   /*     USAGE (Y)                        */                            \
    0x09, 0x32,	   /*     USAGE (Z)                        */                            \
    0x16, 0x01, 0x80,  /*     LOGICAL_MINIMUM (-32767)         */                            \
    0x26, 0xff, 0x7f,  /*     LOGICAL_MAXIMUM (32767)          */                            \
    0x75, 0x10,	   /*     REPORT_SIZE (16)                 */                            \
    0x95, 0x03,	   /*     REPORT_COUNT (3)                 */                            \
    0x81, 0x02,	   /*     INPUT (Data, Variable, Absolute) ;6 bytes (X,Y,Z) */           \
    /* ---------- rX/rY/rZ rotation ---------------------------------------*/                \
    0x05, 0x01,	   /*     USAGE_PAGE (Generic Desktop)     */                            \
    0x09, 0x33,	   /*     USAGE (rX)                       */                            \
    0x09, 0x34,	   /*     USAGE (rY)                       */                            \
    0x09, 0x35,	   /*     USAGE (rZ)                       */                            \
    0x09, 0x36,	   /*     Usage (Slider := Qw)             */                            \
    0x16, 0x01, 0x80,  /*     LOGICAL_MINIMUM (-32767)         */                            \
    0x26, 0xff, 0x7f,  /*     LOGICAL_MAXIMUM (32767)          */                            \
    0x75, 0x10,	   /*     REPORT_SIZE (16)                 */                            \
    0x95, 0x04,	   /*     REPORT_COUNT (3)                 */                            \
    0x81, 0x02,	   /*     INPUT (Data, Variable, Absolute) ;8 bytes rX, rY, rZ, slider */\
    /* ----------- END -----------------------------------------------------------*/         \
    0xc0,	             /*   END_COLLECTION                     */                            \
    0xc0,	             /* END_COLLECTION                       */


static const uint8_t hid_report_map_data[] = {
	GAMEPAD_REPORT_MAP(0)
};