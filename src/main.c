#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>

struct blink_cfg {
    struct gpio_dt_spec led;
    int on_ms;
    int off_ms;
};

/* Cấu hình riêng cho từng đèn */
static const struct blink_cfg cfg_led3 = {
    .led = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios),
    .on_ms = 2000,
    .off_ms = 2000
};

static const struct blink_cfg cfg_y0 = {
    .led = GPIO_DT_SPEC_GET(DT_ALIAS(led1), gpios),
    .on_ms = 500,
    .off_ms = 500
};

static const struct blink_cfg cfg_y1 = {
    .led = GPIO_DT_SPEC_GET(DT_ALIAS(led2), gpios),
    .on_ms = 3000,
    .off_ms = 1000
};

/*
 * GIẢI THÍCH (THEO YÊU CẦU ĐỀ BÀI):
 * Ba hàm luồng riêng biệt là không cần thiết vì toàn bộ logic điều khiển của 3 đèn
 * là hoàn toàn giống nhau (bật đèn -> ngủ on_ms -> tắt đèn -> ngủ off_ms).
 * Bằng cách truyền con trỏ cấu hình qua tham số p1, ta tái sử dụng được mã nguồn,
 * giúp tiết kiệm bộ nhớ Flash của vi điều khiển và code dễ bảo trì hơn.
 */
void thread_blink(void *p1, void *p2, void *p3)
{
    const struct blink_cfg *cfg = (const struct blink_cfg *)p1;

    if (!gpio_is_ready_dt(&cfg->led)) return;
    gpio_pin_configure_dt(&cfg->led, GPIO_OUTPUT_INACTIVE);

    while (1) {
        gpio_pin_set_dt(&cfg->led, 1);
        k_msleep(cfg->on_ms);

        gpio_pin_set_dt(&cfg->led, 0);
        k_msleep(cfg->off_ms);
    }
}

/* 3 luồng cùng dùng chung 1 hàm thread_blink nhưng truyền 3 biến cấu hình khác nhau */
K_THREAD_DEFINE(t_led3, 1024, thread_blink, (void *)&cfg_led3, NULL, NULL, 7, 0, 0);
K_THREAD_DEFINE(t_y0,   1024, thread_blink, (void *)&cfg_y0,   NULL, NULL, 7, 0, 0);
K_THREAD_DEFINE(t_y1,   1024, thread_blink, (void *)&cfg_y1,   NULL, NULL, 7, 0, 0);

int main(void)
{
    return 0;
}