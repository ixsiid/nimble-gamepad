#pragma once

#include <stdint.h>

namespace ReportMapGenerator {
	static const size_t report_id_index = 7;
	static const uint8_t hid_report_map_template[] = {
		0x05, 0x01,        /* USAGE_PAGE (Generic Desktop)       */                       \
		0x09, 0x05,        /* USAGE (Gamepad)                    */                       \
		0xa1, 0x01,        /* COLLECTION (Application)           */                       \
		0x85, 0xff,        /*   REPORT_ID (first report id is 1) */                       \
		/* ---------- Buttons (1 to 16)  -------------------------------------- */        \
		0x05, 0x09,        /*   USAGE_PAGE (Button)              */                       \
		0x19, 0x01,        /*   USAGE_MINIMUM (Button 1)         */                       \
		0x29, 0x10,        /*   USAGE_MAXIMUM (Button 16)        */                       \
		0x15, 0x00,        /*   LOGICAL_MINIMUM (0)              */                       \
		0x25, 0x01,        /*   LOGICAL_MAXIMUM (1)              */                       \
		0x75, 0x01,        /*   REPORT_SIZE (1)                  */                       \
		0x95, 0x10,        /*   REPORT_COUNT (16)                */                       \
		0x81, 0x02,        /*   INPUT (Data, Variable, Absolute) ;16 bits */              \
		/* ---------- X/Y/Z position    -------------------------------------- */         \
		0x05, 0x01,	   /*   USAGE_PAGE (Generic Desktop)     */                        \
		0x09, 0x01,        /*   USAGE (Pointer)                  */                       \
		0x16, 0x01, 0x80,  /*   LOGICAL_MINIMUM (-32767)         */                       \
		0x26, 0xff, 0x7f,  /*   LOGICAL_MAXIMUM (32767)          */                       \
		0x75, 0x10,	   /*   REPORT_SIZE (16)                 */                        \
		0x95, 0x07,	   /*   REPORT_COUNT (7)                 */                        \
		0xa1, 0x00,	   /*   COLLECTION (Physical)            */                        \
		0x09, 0x30,	   /*     USAGE (X)                      */                        \
		0x09, 0x31,	   /*     USAGE (Y)                      */                        \
		0x09, 0x32,	   /*     USAGE (Z)                      */                        \
		0x09, 0x33,	   /*     USAGE (rX)                     */                        \
		0x09, 0x34,	   /*     USAGE (rY)                     */                        \
		0x09, 0x35,	   /*     USAGE (rZ)                     */                        \
		0x09, 0x36,	   /*     Usage (Slider := Qw)           */                        \
		0x81, 0x02,	   /*     INPUT (Data, Variable, Absolute) ;14 bytes (2 * axis) */ \
		0xc0,	             /*   END_COLLECTION (~Physical)       */                   \
		/* ----------- END -----------------------------------------------------------*/  \
		0xc0,	             /* END_COLLECTION   (~Application)    */
	};

	static size_t length = 0;
	static uint8_t * buffer = nullptr;
	static size_t generate(size_t count) {
		if (buffer) return 0;

		size_t l = sizeof(hid_report_map_template);

		buffer = new uint8_t[l * count];
		for(int i=0; i<count; i++) {
			memcpy(buffer + i * l, hid_report_map_template, l);
			buffer[i * l + report_id_index] = i + 1;
		}

		return length = l * count;
	};
}