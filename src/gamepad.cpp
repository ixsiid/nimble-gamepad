#include "gamepad.hpp"

#include "report_map.hpp"

using namespace SimpleNimble;

const char *BleGamePad::tag = "BleGamePad";

// model, serial, firm, soft, hard は必須ではない
static SimpleNimbleCharacteristicBuffer *model, *serial, *firm, *soft, *hard, *level, *manu;

static SimpleNimbleCharacteristicBuffer *hid_info, *hid_control, *hid_report_map, *hid_proto, *hid_report_pad0, *hid_pnp;

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

const uint8_t HID_PROTOCOL_MODE_REPORT = 1;

BleGamePad::BleGamePad(const char *device_name)
    : nimble(SimpleNimblePeripheral::get_instance()),
	 _buffer({}) {
	ESP_LOGI(tag, "set name");
	nimble->set_name(device_name);
	nimble->initialize_services(2);
	ESP_LOGI(tag, "set uuid");

	bool r;

	// NimBLEではNotifyフラグが建てられたCharacteristicに0x2902は自動的に作られる
	battery_report.uuid.u16.u.type = BLE_UUID_TYPE_16;
	battery_report.uuid.u16.value	 = 0x2904;
	battery_report.flag			 = Dsc_AccessFlag::Read | Dsc_AccessFlag::Write;
	battery_report.data_length	 = sizeof(present_format_t);
	present_format_t *pft		 = (present_format_t *)battery_report.buffer;
	// battery level: 32%
	level = new SimpleNimbleCharacteristicBuffer(0x2a19, 1,
										Chr_AccessFlag::Read | Chr_AccessFlag::Notify,
										{&battery_report});
	level->write_u8(32);

	// model revision
	model = new SimpleNimbleCharacteristicBuffer(0x2a24, 8, Chr_AccessFlag::Read);
	model->write("1.0.0");
	// serial number
	serial = new SimpleNimbleCharacteristicBuffer(0x2a25, 8, Chr_AccessFlag::Read);
	serial->write("12345");
	// firmware version
	firm = new SimpleNimbleCharacteristicBuffer(0x2a26, 8, Chr_AccessFlag::Read);
	firm->write("0.1.0");
	// hardware version
	hard = new SimpleNimbleCharacteristicBuffer(0x2a27, 8, Chr_AccessFlag::Read);
	hard->write("0.2.1");
	// software version
	soft = new SimpleNimbleCharacteristicBuffer(0x2a28, 8, Chr_AccessFlag::Read);
	soft->write("0.3.2");
	// manufacture
	manu = new SimpleNimbleCharacteristicBuffer(0x2a29, 8, Chr_AccessFlag::Read);
	manu->write("ixsiid");

	// device information
	// r = nimble->add_service(nullptr, 0x180a, {model, serial, firm, hard, soft, manu});
	// ESP_LOGI(tag, "add device information service: %d", r);

	// battery service
	r = nimble->add_service(nullptr, 0x180f, {level});
	ESP_LOGI(tag, "add battery service: %d", r);

	// u32, u16でwriteするときはuint8_t[]とは逆順になるので注意
	hid_info = new SimpleNimbleCharacteristicBuffer(0x2a4a, 4, Chr_AccessFlag::Read);
	// HID verion Low 0x07, HID version High 0x07, country code 0x00, HID_FLAGS_NORMALLY_CONNECTABLE 0x02
	hid_info->write_u32(0x02000707);

	hid_control = new SimpleNimbleCharacteristicBuffer(0x2a4c, 1, Chr_AccessFlag::Write_no_response);
	hid_control->write_u8(0);

	hid_report_map = new SimpleNimbleCharacteristicBuffer(0x2a4b, sizeof(hid_report_map_data), Chr_AccessFlag::Read);
	hid_report_map->write(hid_report_map_data, sizeof(hid_report_map_data));

	hid_proto = new SimpleNimbleCharacteristicBuffer(0x2a4e, 1, Chr_AccessFlag::Read | Chr_AccessFlag::Write);
	hid_proto->write_u8(HID_PROTOCOL_MODE_REPORT);

	hid_report_pad0 = new SimpleNimbleCharacteristicBuffer(0x2a4d, 2, Chr_AccessFlag::Read | Chr_AccessFlag::Notify);
	// HID_RPT_ID_GAMEPAD0_IN 0x01(数に応じてインクリメント), HID_REPORT_TYPE_INPUT 0x01
	hid_report_pad0->write_u16(0x0101);

	hid_pnp = new SimpleNimbleCharacteristicBuffer(0x2a50, 2, Chr_AccessFlag::Read);
	// HID_RPT_ID_FEATURE 0x00, HID_REPORT_TYPE_FEATURE 0x03
	hid_pnp->write_u16(0x0300);

	// HID service
	r = nimble->add_service(nullptr, 0x1812, {hid_info, hid_control, hid_report_map, hid_proto, hid_report_pad0, hid_pnp});
	ESP_LOGI(tag, "add HID service: %d", r);

	ESP_LOGI(tag, "start");
	nimble->start();
};

BleGamePad::gamepad_u *BleGamePad::buffer() { return &_buffer; };

void BleGamePad::send(){
    //	nimble->debug();
};