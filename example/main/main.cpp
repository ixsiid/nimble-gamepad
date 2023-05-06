/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#include <esp_log.h>
#include <nvs_flash.h>

#include "../../src/gamepad.hpp"

#define tag "GamepadSample"

extern "C" {
void app_main();
}

void app_main(void) {
	ESP_LOGI(tag, "Start");
	/* Initialize NVS — it is used to store PHY calibration data */
	esp_err_t ret;
	ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK(ret);

	BleGamePad *gamepad		  = new BleGamePad("AtomS3");
	BleGamePad::gamepad_t *pad = &(gamepad->buffer()->pad);

	uint8_t b = 15;
	while (true) {
		vTaskDelay(2000 / portTICK_PERIOD_MS);
		ESP_LOGI(tag, "Idle");
		pad->buttons++;
		gamepad->send();

		gamepad->update_battery_level(++b);
	}
}
