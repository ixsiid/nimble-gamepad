#include "gamepad.hpp"

#include "report_map.hpp"

using namespace SimpleNimble;

const char *BleGamePad::tag = "BleGamePad";

struct present_format_t {
	/// Unit (The Unit is a UUID)
	uint16_t unit;
	/// Description
	uint16_t description;
	/// Format
	uint8_t format;
	/// Exponent
	uint8_t exponent;
	/// Name space
	uint8_t name_space;
};

BleGamePad::BleGamePad(const char *device_name, uint8_t count)
    : nimble(SimpleNimblePeripheral::get_instance()),
	 _buffer(new gamepad_u[count]),
	 battery_report(Descriptor{
		.uuid	   = {.u16 = {.u	 = {.type = BLE_UUID_TYPE_16},
						    .value = 0x2904}},
		.flag	   = Dsc_AccessFlag::Read | Dsc_AccessFlag::Write,
		.data_length = sizeof(present_format_t)}),
	 manufacture(Characteristic(0x2a29, 8, Chr_AccessFlag::Read)),
	 battery_level(Characteristic(0x2a19, 1, Chr_AccessFlag::Read | Chr_AccessFlag::Notify, {&battery_report})),
	 hid_info(Characteristic(0x2a4a, 4, Chr_AccessFlag::Read)),
	 hid_control(Characteristic(0x2a4c, 1, Chr_AccessFlag::Write_no_response)),
	 hid_report_map(Characteristic(0x2a4b, sizeof(hid_report_map_data), Chr_AccessFlag::Read)),
	 hid_proto(Characteristic(0x2a4e, 7, Chr_AccessFlag::Read | Chr_AccessFlag::Write_no_response)),
	 hid_pnp(Characteristic(0x2a50, 7, Chr_AccessFlag::Read)),
	 pads(new Descriptor[count]) {

	ESP_LOGI(tag, "set name");
	nimble->set_name(device_name);
	nimble->initialize_services(3);
	ESP_LOGI(tag, "set uuid");

	bool r;

	// device information
	manufacture.write("halzion.net");
	r = nimble->add_service(nullptr, 0x180a, {&manufacture});
	ESP_LOGI(tag, "add device information service: %d", r);

	// battery service
	battery_level.write_u8(32);  // battery level: 32% 適当に定数値いれる
	r = nimble->add_service(nullptr, 0x180f, {&battery_level});
	ESP_LOGI(tag, "add battery service: %d", r);

	// u32, u16でwriteするときはuint8_t[]とは逆順になるので注意
	hid_info.write_u32(0x02000111);  // HID verion Low 0x11, HID version High 0x01, country code 0x00, HID_FLAGS_NORMALLY_CONNECTABLE 0x02
	hid_control.write_u8(0);
	hid_report_map.write(hid_report_map_data, sizeof(hid_report_map_data));
	hid_proto.write_u8(1);								// HID_PROTOCOL_MODE_REPORT
	hid_pnp.write({0x01, 0x02, 0xe5, 0xab, 0xcd, 0x01, 0x10});	// Bluetooth SIG compani: 0xe502, Product Id: 52651 (0xcdab), Product Version 4097 (0x1001)

	// 0x2908 descriptorを変えることで、複数のGamepadを変える
	// HID_RPT_ID_GAMEPAD0_IN 0x01(GamePadのインデックスに応じてインクリメント), HID_REPORT_TYPE_INPUT 0x01
	for (int i = 0; i < count; i++) {
		pads[i].uuid.u16.u.type = BLE_UUID_TYPE_16;
		pads[i].uuid.u16.value  = 0x2908;
		pads[i].flag		    = Dsc_AccessFlag::Read | Dsc_AccessFlag::Write;
		pads[i].data_length	    = 2;
		pads[i].buffer[0]	    = i;
		pads[i].buffer[1]	    = 0x01;
	}

	hid_report_pad = new Characteristic(0x2a4d, sizeof(gamepad_t), Chr_AccessFlag::Read | Chr_AccessFlag::Notify, {&pads[0]});
	hid_report_pad->clear((uint8_t)sizeof(gamepad_t));

	// HID service
	r = nimble->add_service(nullptr, 0x1812, {&hid_info, &hid_control, &hid_report_map, &hid_proto, &hid_pnp, hid_report_pad});
	ESP_LOGI(tag, "add HID service: %d", r);

	ESP_LOG_BUFFER_HEXDUMP(tag, hid_report_map_data, sizeof(hid_report_map_data), esp_log_level_t::ESP_LOG_INFO);

	ESP_LOGI(tag, "start");
	nimble->start();
}

BleGamePad::~BleGamePad() {
	delete[] _buffer;
}

BleGamePad::gamepad_u *BleGamePad::buffer(uint8_t index) { return _buffer + index; };

void BleGamePad::send(uint8_t index) {
	(hid_report_pad + index)->write((_buffer + index)->raw, (uint8_t)sizeof(gamepad_t));
	(hid_report_pad + index)->notify();
}

void BleGamePad::update_battery_level(uint8_t percent) {
	battery_level.write_u8(percent);
	battery_level.notify();
}