#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>

static const struct gpio_dt_spec led3 = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);
static const struct gpio_dt_spec y0   = GPIO_DT_SPEC_GET(DT_ALIAS(led1), gpios);


static volatile bool led_state = false;

/* Luồng 1: Điều khiển LED3 và cập nhật biến dùng chung */
void thread_led_blinky(void *p1, void *p2, void *p3)
{
    if (!gpio_is_ready_dt(&led3)) return;
    gpio_pin_configure_dt(&led3, GPIO_OUTPUT_INACTIVE);

    while (1) {
        led_state = true;
        gpio_pin_set_dt(&led3, 1);
        k_msleep(2000); /* Sáng 2 giây */

        led_state = false;
        gpio_pin_set_dt(&led3, 0);
        k_msleep(2000); /* Tắt 2 giây */
    }
}

/* Luồng 2: Bám theo trạng thái của LED3, không tự đếm thời gian 2 giây */
void thread_y0_follow(void *p1, void *p2, void *p3)
{
    if (!gpio_is_ready_dt(&y0)) return;
    gpio_pin_configure_dt(&y0, GPIO_OUTPUT_INACTIVE);

    while (1) {
        /* Thăm dò biến led_state và cập nhật đèn Y0 */
        gpio_pin_set_dt(&y0, led_state ? 1 : 0);
        k_msleep(10); /* Quét mỗi 10ms */
    }
}

K_THREAD_DEFINE(t_blinky, 1024, thread_led_blinky, NULL, NULL, NULL, 7, 0, 0);
K_THREAD_DEFINE(t_follow, 1024, thread_y0_follow, NULL, NULL, NULL, 7, 0, 0);

int main(void)
{
    return 0;
}