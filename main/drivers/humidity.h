#include <esp_timer.h>

using humidity_update_cb_t = void(*)(uint16_t, float);

class HumidityDriver {
  public:
    HumidityDriver() = default;
    ~HumidityDriver();
    void Init(humidity_update_cb_t cb, uint16_t endpoint_id);
    void UpdateHumidity();
  private:
    esp_timer_handle_t timer_;
    humidity_update_cb_t cb_;
    uint16_t endpoint_id_;
};
