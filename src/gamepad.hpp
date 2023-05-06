#pragma once

#include <stdint.h>
#include <vector>
#include <simple_nimble_peripheral.hpp>

class BleGamePad {
    public:
#pragma pack(1)
	struct gamepad_t {
		uint16_t buttons;
		uint16_t x, y, z;
		uint16_t rx, ry, rz;
		uint16_t slider;
	};
#pragma pack()
	union gamepad_u {
		uint8_t raw[sizeof(gamepad_t)];
		gamepad_t pad;
	};

    private:
	static const char *tag;

	SimpleNimblePeripheral *nimble;
	gamepad_u *_buffer;

	~BleGamePad();

	// NimBLEではNotifyフラグが建てられたCharacteristicにDescripter: 0x2902は自動的に作られるため0x2904だけ作る
	Descriptor battery_report;
	Characteristic manufacture, battery_level;
	Characteristic hid_info, hid_control, hid_report_map, hid_proto, hid_pnp;

	std::vector<Descriptor> pads;
	std::vector<Characteristic> hid_report_pads;

	// Descriptor UUIDは多用するため、共用する
	static const ble_uuid16_t _2904, _2908;
	static const ble_uuid_t *uuid2904, *uuid2908;

    public:
	BleGamePad(const char *device_name, uint8_t gamepad_count = 1);
	gamepad_u *buffer(uint8_t index = 0);
	void send(uint8_t index = 0);
	void update_battery_level(uint8_t percent);
};
