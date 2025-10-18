#include "humidity.h"

#include <common_macros.h>
#include <esp_err.h>
#include <esp_log.h>

namespace {

// TODO: remove, once implemented proper measurement.
uint8_t count{0};

// 500ms.
const uint64_t TIMER_INTERVAL_US{500 * 1000};
const char* TAG = "humidity_driver";
void timer_cb_internal(void *arg)
{
    ESP_LOGI(TAG, "esp_timer callback.");
    ++count;
    auto* driver = static_cast<HumidityDriver*>(arg);
    driver->UpdateHumidity();
}

}  // namespace

HumidityDriver::~HumidityDriver() {
    const auto err = esp_timer_stop(timer_);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_timer_start_periodic failed: %d.", err);
    }
}

void HumidityDriver::Init(humidity_update_cb_t cb, uint16_t endpoint_id) {
    cb_ = cb;
    endpoint_id_ = endpoint_id;

    esp_timer_create_args_t args = {
        .callback = timer_cb_internal,
        .arg = this,
    };

    auto err = esp_timer_create(&args, &timer_);
    ABORT_APP_ON_FAILURE(err == ESP_OK, ESP_LOGE(TAG, "esp_timer_create failed, err: %d.", err));

    err = esp_timer_start_periodic(timer_, TIMER_INTERVAL_US);
    ABORT_APP_ON_FAILURE(err == ESP_OK, ESP_LOGE(TAG, "esp_timer_start_periodic failed: %d.", err));
}

void HumidityDriver::UpdateHumidity() {
    cb_(endpoint_id_, count);
}
