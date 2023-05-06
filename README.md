# Simple Nimble Gamepad

ESPをHID Gamepadとして認識させます。NimBLEスタックを使います。
デフォルトでは、8ボタン7軸デバイスとして認識し、カスタムするときはソースコードから改変する必要があります。

手軽に使えること、複数のGamepadを使えることをコンセプトにしています。

これは、もともと一つのESPに複数のIMUをぶら下げ、
IMUの値から計算したLocation {x, y, z}, Quaternion {i, j, k, w}を7軸のデータとしてホストに送信するために作ったからです。

動作確認済み
platform
- esp-idf v4.4

board
- ATOMS3

```cpp
#include <esp_log.h>
#include <nvs_flash.h>

#include <gamepad.hpp>

const char *tag = "GamepadSample";

extern "C" { void app_main(); }

void app_main(void) {
	esp_err_t ret;
	ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK(ret);

	const size_t device_count = 3;
	BleGamePad *gamepad = new BleGamePad("DEVICE NAME", device_count);

	BleGamePad::gamepad_t *pad[device_count] = {
		&(gamepad->buffer(0)->pad),
		&(gamepad->buffer(1)->pad),
		&(gamepad->buffer(2)->pad)
	};

	while (true) {
		vTaskDelay(2000 / portTICK_PERIOD_MS);
		pad[0]->buttons++;
		pad[1]->x += 1;
		pad[2]->slider = 2900;
		gamepad->send(0);
		gamepad->send(1);
		gamepad->send(2);
	}
}
```

