#include "gamepad.hpp"

#include "report_map.hpp"

using namespace SimpleNimble;

const char *BleGamePad::tag = "BleGamePad";

// model, serial, firm, soft, hard は必須ではない
static Characteristic *model, *serial, *firm, *soft, *hard, *level, *manu;

static Characteristic *hid_info, *hid_control, *hid_report_map, *hid_proto, *hid_report_pad0, *hid_pnp;

static Descriptor pad0;

static Descriptor battery_report;
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

uint8_t b = 34;

BleGamePad::BleGamePad(const char *device_name)
    : nimble(SimpleNimblePeripheral::get_instance()),
	 _buffer({}) {
	ESP_LOGI(tag, "set name");
	nimble->set_name(device_name);
	nimble->initialize_services(3);
	ESP_LOGI(tag, "set uuid");

	bool r;

	// NimBLEではNotifyフラグが建てられたCharacteristicに0x2902は自動的に作られる
	battery_report.uuid.u16.u.type = BLE_UUID_TYPE_16;
	battery_report.uuid.u16.value	 = 0x2904;
	battery_report.flag			 = Dsc_AccessFlag::Read | Dsc_AccessFlag::Write;
	battery_report.data_length	 = sizeof(present_format_t);
	present_format_t *pft		 = (present_format_t *)battery_report.buffer;
	// battery level: 32%
	level = new Characteristic(0x2a19, 1,
						  Chr_AccessFlag::Read | Chr_AccessFlag::Notify,
						  {&battery_report});
	level->write_u8(32);

	// model revision
	model = new Characteristic(0x2a24, 8, Chr_AccessFlag::Read);
	model->write("1.0.0");
	// serial number
	serial = new Characteristic(0x2a25, 8, Chr_AccessFlag::Read);
	serial->write("12345");
	// firmware version
	firm = new Characteristic(0x2a26, 8, Chr_AccessFlag::Read);
	firm->write("0.1.0");
	// hardware version
	hard = new Characteristic(0x2a27, 8, Chr_AccessFlag::Read);
	hard->write("0.2.1");
	// software version
	soft = new Characteristic(0x2a28, 8, Chr_AccessFlag::Read);
	soft->write("0.3.2");
	// manufacture
	manu = new Characteristic(0x2a29, 8, Chr_AccessFlag::Read);
	manu->write("ixsiid");

	// device information
	r = nimble->add_service(nullptr, 0x180a, {model, serial, firm, hard, soft, manu});
	// ESP_LOGI(tag, "add device information service: %d", r);

	// battery service
	r = nimble->add_service(nullptr, 0x180f, {level});
	ESP_LOGI(tag, "add battery service: %d", r);

	// u32, u16でwriteするときはuint8_t[]とは逆順になるので注意
	hid_info = new Characteristic(0x2a4a, 4, Chr_AccessFlag::Read);
	// HID verion Low 0x11, HID version High 0x01, country code 0x00, HID_FLAGS_NORMALLY_CONNECTABLE 0x02
	hid_info->write_u32(0x02000111);

	hid_control = new Characteristic(0x2a4c, 1, Chr_AccessFlag::Write_no_response);
	hid_control->write_u8(0);

	hid_report_map = new Characteristic(0x2a4b, sizeof(hid_report_map_data), Chr_AccessFlag::Read);
	hid_report_map->write(hid_report_map_data, sizeof(hid_report_map_data));

	hid_proto = new Characteristic(0x2a4e, 7, Chr_AccessFlag::Read | Chr_AccessFlag::Write_no_response);
	// HID_PROTOCOL_MODE_REPORT
	hid_proto->write_u8(1);

	// 0x2908 descriptorを変えることで、複数のGamepadを変える
	// HID_RPT_ID_GAMEPAD0_IN 0x01(GamePadのインデックスに応じてインクリメント), HID_REPORT_TYPE_INPUT 0x01
	pad0.uuid.u16.u.type = BLE_UUID_TYPE_16;
	pad0.uuid.u16.value	 = 0x2908;
	pad0.flag			 = Dsc_AccessFlag::Read | Dsc_AccessFlag::Write;
	pad0.data_length	 = 2;
	pad0.buffer[0]		 = 0x01;
	pad0.buffer[1]		 = 0x01;

	hid_report_pad0	= new Characteristic(0x2a4d, sizeof(gamepad_t), Chr_AccessFlag::Read | Chr_AccessFlag::Notify, {&pad0});
	_buffer.pad.slider	= 0x1234;
	_buffer.pad.x		= 0x5432;
	_buffer.pad.buttons = 0b10100101;
	hid_report_pad0->write(_buffer.raw, (uint8_t)sizeof(gamepad_t));

	hid_pnp = new Characteristic(0x2a50, 7, Chr_AccessFlag::Read);
	// Bluetooth SIG compani: 0xe502, Product Id: 52651 (0xcdab), Product Version 4097 (0x1001)
	hid_pnp->write({0x01, 0x02, 0xe5, 0xab, 0xcd, 0x01, 0x10});

	// HID service
	r = nimble->add_service(nullptr, 0x1812, {hid_info, hid_control, hid_report_map, hid_proto, hid_pnp, hid_report_pad0});
	ESP_LOGI(tag, "add HID service: %d", r);

	ESP_LOG_BUFFER_HEXDUMP(tag, hid_report_map_data, sizeof(hid_report_map_data), esp_log_level_t::ESP_LOG_INFO);

	ESP_LOGI(tag, "start");
	nimble->start();
};

BleGamePad::gamepad_u *BleGamePad::buffer() { return &_buffer; };

void BleGamePad::send() {
	hid_report_pad0->write(_buffer.raw, (uint8_t)sizeof(gamepad_t));
	hid_report_pad0->notify();

	// level->write_u8(++b);
	// level->notify();
};