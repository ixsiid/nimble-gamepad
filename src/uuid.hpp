#pragma once

#include <host/ble_uuid.h>

namespace BleHid {
constexpr ble_uuid16_t Uuid_2904 = {
    .u	 = {.type = BLE_UUID_TYPE_16},
    .value = 0x2904,
};

constexpr ble_uuid16_t Uuid_2908 = {
    .u	 = {.type = BLE_UUID_TYPE_16},
    .value = 0x2908,
};
}