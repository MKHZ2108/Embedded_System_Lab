#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>

static const struct gpio_dt_spec led3 = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);

void thread_led_blinky(void *p1, void *p2, void *p3)
{
    if (!gpio_is_ready_dt(&led3)) {
        return;
    }
    gpio_pin_configure_dt(&led3, GPIO_OUTPUT_INACTIVE);

    while (1) {
        gpio_pin_set_dt(&led3, 1); /* BẬT LED3 */
        k_msleep(1000);            /* Giữ sáng 1 giây */

        gpio_pin_set_dt(&led3, 0); /* TẮT LED3 */
        k_msleep(2000);            /* Giữ tắt 2 giây */
    }
}

/* Khai báo luồng: độ ưu tiên 7, không trễ */
K_THREAD_DEFINE(tid_led_blinky, 1024, thread_led_blinky, NULL, NULL, NULL, 7, 0, 0);

int main(void)
{
    return 0;
}