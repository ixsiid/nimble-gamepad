#pragma once

#include <stdint.h>
#include <simple_nimble_peripheral.hpp>

class BleGamePad {
    public:
#pragma pack(1)
	struct gamepad_t {
		uint8_t buttons;
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
	gamepad_u _buffer;

    public:
	BleGamePad(const char *device_name);
	gamepad_u *buffer();
	void send();
};
