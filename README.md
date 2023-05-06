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

	BleGamePad *gamepad		  = new BleGamePad("DEVICE NAME");
	BleGamePad::gamepad_t *pad = &(gamepad->buffer()->pad);

	while (true) {
		vTaskDelay(2000 / portTICK_PERIOD_MS);
		pad->buttons++;
		pad->x += 1;
		gamepad->send();
	}
}
```

