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

#include <simple_nimble_peripheral.hpp>

#include "../../src/sample.hpp"

#define tag "BleGamepad"

extern "C" {
void app_main();
}

static ble_uuid_any_t gatt_svr_svc_uuid = {
    .u128 = {
	   .u = {
		  .type = BLE_UUID_TYPE_128,
	   },
	   .value = {0x2d, 0x71, 0xa2, 0x59, 0xb4, 0x58, 0xc8, 0x12, 0x99, 0x99, 0x43, 0x95, 0x12, 0x2f, 0x46, 0x59},
    },
};
static ble_uuid_any_t gatt_svr_chr_uuid1 = {
    .u128 = {
	   .u = {
		  .type = BLE_UUID_TYPE_128,
	   },
	   .value = {0x00, 0x00, 0x00, 0x00, 0x11, 0x11, 0x11, 0x11, 0x22, 0x22, 0x22, 0x22, 0x33, 0x33, 0x33, 0x33},
    },
};
static ble_uuid_any_t gatt_svr_chr_uuid2 = {
    .u128 = {
	   .u = {
		  .type = BLE_UUID_TYPE_128,
	   },
	   .value = {0xff, 0xff, 0xff, 0xff, 0x11, 0x11, 0x11, 0x11, 0x22, 0x22, 0x22, 0x22, 0x33, 0x33, 0x33, 0x33},
    },
};

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

	SimpleNimblePeripheral *ph = SimpleNimblePeripheral::get_instance();
	ESP_LOGI(tag, "set name");
	ph->set_name("name");
	ph->initialize_services(1);
	ESP_LOGI(tag, "set uuid");

	std::initializer_list<SimpleNimbleCharacteristicBuffer *> charas = {
	    new SimpleNimbleCharacteristicBuffer(
		   gatt_svr_chr_uuid1, 16,
		   SimpleNimbleCharacteristicBuffer::Flag::Read | SimpleNimbleCharacteristicBuffer::Flag::Write),
	    new SimpleNimbleCharacteristicBuffer(
		   gatt_svr_chr_uuid2, 16,
		   SimpleNimbleCharacteristicBuffer::Flag::Read | SimpleNimbleCharacteristicBuffer::Flag::Notify),
	};

	ph->add_service(nullptr, gatt_svr_svc_uuid, charas);

	ESP_LOGI(tag, "start");
	ph->start();

	
	Sample * s = new Sample();
	ESP_LOGI(tag, "%d", s->count);

	while (true) {
		vTaskDelay(1000);
		ESP_LOGI(tag, "Idle");
	}

}
