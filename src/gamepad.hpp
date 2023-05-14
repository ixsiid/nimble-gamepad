#pragma once

#include <stdint.h>
#include <simple_nimble_peripheral.hpp>

#include "uuid.hpp"

class BleGamePad {
    private:
	static const char *tag;

	SimpleNimblePeripheral *nimble;
	uint8_t *_buffer;

	~BleGamePad();

	// NimBLEではNotifyフラグが建てられたCharacteristicにDescripter: 0x2902は自動的に作られるため0x2904だけ作る
	Descriptor battery_report;
	Characteristic manufacture, battery_level;
	Characteristic hid_info, hid_control, hid_report_map, hid_proto, hid_pnp;

	Descriptor *pads;
	Characteristic **hid_report_pads;

	Service device_info, battery_info, hid;

	const BleHid::report_t * reports;

    public:
	BleGamePad(const char *device_name, const BleHid::report_t * reports, size_t report_count, size_t total_report_size, size_t total_report_map_size);
	uint8_t *buffer(uint8_t index = 0);
	void send(uint8_t index = 0);
	void update_battery_level(uint8_t percent);
};
