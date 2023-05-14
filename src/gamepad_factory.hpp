#pragma once

#include <esp_log.h>
#include "uuid.hpp"
#include "gamepad.hpp"

namespace BleHid {
enum class Axis : unsigned int {
	None	  = 0,
	X	  = 1 << 0,
	Y	  = 1 << 1,
	Z	  = 1 << 2,
	rX	  = 1 << 3,
	rY	  = 1 << 4,
	rZ	  = 1 << 5,
	Slider = 1 << 6,
};

class Factory {
    private:
	uint8_t buffer[256];
	uint8_t i;

	uint8_t button;
	uint8_t dummy_button;
	Axis axis;
	const char *name;
	uint8_t pad_index;

	report_t reports[16];
	size_t report_count;
	size_t total_report_size;

	Factory();

    public:
	static Factory begin();
	Factory &set_name(const char *device_name);
	Factory &add_button(uint8_t count);
	Factory &add_axis(Axis axis);
	Factory &add_pad();
	Factory &dupulicate_pad(uint8_t count);
	BleGamePad *start();
};

constexpr Axis operator|(Axis l, Axis r) {
	return static_cast<Axis>(static_cast<int>(l) | static_cast<int>(r));
}

constexpr Axis operator&(Axis l, Axis r) {
	return static_cast<Axis>(static_cast<int>(l) & static_cast<int>(r));
}

unsigned int axis_count(Axis a) {
	unsigned count = 0;
	unsigned int b = static_cast<unsigned int>(a);
	for (int i = 0; i < 32; i++) {
		if (b & 1) count++;
		b >>= 1;
	}
	return count;
}

Factory::Factory() : i(),
				 button(),
				 dummy_button(),
				 axis(),
				 name(),
				 pad_index(1),
				 reports(),
				 report_count(0),
				 total_report_size(0) {}

Factory &Factory::set_name(const char *device_name) {
	name = device_name;
	return *this;
}

Factory Factory::begin() {
	return Factory();
}

Factory &Factory::add_button(uint8_t count) {
	button	   = count;
	dummy_button = -button & 0b111;
	return *this;
}

Factory &Factory::add_axis(Axis axis_flag) {
	axis = axis_flag;
	return *this;
}

Factory &Factory::add_pad() { return dupulicate_pad(1); }
Factory &Factory::dupulicate_pad(uint8_t count) {
	for (int n = 0; n < count; n++) {
		reports[report_count].report_map  = buffer;
		reports[report_count].report_size = 0;

		buffer[i++] = 0x05, buffer[i++] = 0x01;		  /* USAGE_PAGE (Generic Desktop)       */
		buffer[i++] = 0x09, buffer[i++] = 0x05;		  /* USAGE (Gamepad)                    */
		buffer[i++] = 0xa1, buffer[i++] = 0x01;		  /* COLLECTION (Application)           */
		buffer[i++] = 0x85, buffer[i++] = pad_index++; /*   REPORT_ID (first report id is 1) */

		if (button > 0) {
			/* ---------- Buttons (1 to N)  -------------------------------------- */
			buffer[i++] = 0x05, buffer[i++] = 0x09;	  /*   USAGE_PAGE (Button)              */
			buffer[i++] = 0x19, buffer[i++] = 0x01;	  /*   USAGE_MINIMUM (Button 1)         */
			buffer[i++] = 0x29, buffer[i++] = button; /*   USAGE_MAXIMUM (Button N)         */
			buffer[i++] = 0x15, buffer[i++] = 0x00;	  /*   LOGICAL_MINIMUM (0)              */
			buffer[i++] = 0x25, buffer[i++] = 0x01;	  /*   LOGICAL_MAXIMUM (1)              */
			buffer[i++] = 0x75, buffer[i++] = 0x01;	  /*   REPORT_SIZE (1)                  */
			buffer[i++] = 0x95, buffer[i++] = button; /*   REPORT_COUNT (N)                 */
			buffer[i++] = 0x81, buffer[i++] = 0x02;	  /*   INPUT (Data, Variable, Absolute) */
			reports[report_count].report_size += 1 * button;
		}
		if (dummy_button > 0) {
			buffer[i++] = 0x75, buffer[i++] = 0x01;		   /*   REPORT_SIZE (1)                  */
			buffer[i++] = 0x95, buffer[i++] = dummy_button; /*   REPORT_COUNT (N)                 */
			buffer[i++] = 0x81, buffer[i++] = 0x03;		   /*   INPUT (Const, Variable, Absolute) */
			reports[report_count].report_size += 1 * dummy_button;
		}

		if (static_cast<bool>(axis)) {
			/* ---------- X/Y/Z position    -------------------------------------- */
			buffer[i++] = 0x05, buffer[i++] = 0x01;					/*   USAGE_PAGE (Generic Desktop)     */
			buffer[i++] = 0x09, buffer[i++] = 0x01;					/*   USAGE (Pointer)                  */
			buffer[i++] = 0x16, buffer[i++] = 0x01, buffer[i++] = 0x80; /*   LOGICAL_MINIMUM (-32767)         */
			buffer[i++] = 0x26, buffer[i++] = 0xff, buffer[i++] = 0x7f; /*   LOGICAL_MAXIMUM (32767)          */
			buffer[i++] = 0x75, buffer[i++] = 0x10;					/*   REPORT_SIZE (16)                 */
			buffer[i++] = 0x95, buffer[i++] = axis_count(axis);		/*   REPORT_COUNT (N)                 */
			buffer[i++] = 0xa1, buffer[i++] = 0x00;					/*   COLLECTION (Physical)            */
			reports[report_count].report_size += 0x10 * axis_count(axis);
			if (static_cast<bool>(axis & Axis::X))
				buffer[i++] = 0x09, buffer[i++] = 0x30; /*     USAGE (X)                      */
			if (static_cast<bool>(axis & Axis::Y))
				buffer[i++] = 0x09, buffer[i++] = 0x31; /*     USAGE (Y)                      */
			if (static_cast<bool>(axis & Axis::Z))
				buffer[i++] = 0x09, buffer[i++] = 0x32; /*     USAGE (Z)                      */
			if (static_cast<bool>(axis & Axis::rX))
				buffer[i++] = 0x09, buffer[i++] = 0x33; /*     USAGE (rX)                     */
			if (static_cast<bool>(axis & Axis::rY))
				buffer[i++] = 0x09, buffer[i++] = 0x34; /*     USAGE (rY)                     */
			if (static_cast<bool>(axis & Axis::rZ))
				buffer[i++] = 0x09, buffer[i++] = 0x35; /*     USAGE (rZ)                     */
			if (static_cast<bool>(axis & Axis::Slider))
				buffer[i++] = 0x09, buffer[i++] = 0x36; /*     Usage (Slider)           */
			buffer[i++] = 0x81, buffer[i++] = 0x02;		/*     INPUT (Data, Variable, Absolute)  */
			buffer[i++] = 0xc0;						/*   END_COLLECTION (~Physical)       */
		}
		/* ----------- END -----------------------------------------------------------*/
		buffer[i++] = 0xc0; /* END_COLLECTION   (~Application)    */

		reports[report_count].report_size /= 8;	 // convert bit to byte
		total_report_size += reports[report_count].report_size;
		report_count++;
	};

	button	   = 0;
	dummy_button = 0;
	axis		   = Axis::None;
	return *this;
}

BleGamePad *Factory::start() {
	ESP_LOG_BUFFER_HEXDUMP("BUFFER", buffer, i, esp_log_level_t::ESP_LOG_INFO);
	// 	BleGamePad *p = new BleGamePad(name, 1);
	BleGamePad *p = new BleGamePad(name, reports, report_count, total_report_size, i);
	// 色々カスタムする
	return p;
}
}  // namespace BleHid
