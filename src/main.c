#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>

static const struct gpio_dt_spec led3 = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);
static const struct gpio_dt_spec y0   = GPIO_DT_SPEC_GET(DT_ALIAS(led1), gpios);
static const struct gpio_dt_spec y1   = GPIO_DT_SPEC_GET(DT_ALIAS(led2), gpios);

/* Luồng 1: LED3 chớp 1.5s ON / 0.5s OFF */
void thread_led_blinky(void *p1, void *p2, void *p3)
{
    if (!gpio_is_ready_dt(&led3)) return;
    gpio_pin_configure_dt(&led3, GPIO_OUTPUT_INACTIVE);

    while (1) {
        gpio_pin_set_dt(&led3, 1);
        k_msleep(1500);
        gpio_pin_set_dt(&led3, 0);
        k_msleep(500);
    }
}

/* Luồng 2: Chu kỳ 3 trạng thái của Y0 và Y1 */
void thread_output_sequence(void *p1, void *p2, void *p3)
{
    if (!gpio_is_ready_dt(&y0) || !gpio_is_ready_dt(&y1)) return;
    gpio_pin_configure_dt(&y0, GPIO_OUTPUT_INACTIVE);
    gpio_pin_configure_dt(&y1, GPIO_OUTPUT_INACTIVE);

    while (1) {
        /* Trạng thái 1: Chỉ Y0 bật trong 1s */
        gpio_pin_set_dt(&y0, 1);
        gpio_pin_set_dt(&y1, 0);
        k_msleep(1000);

        /* Trạng thái 2: Chỉ Y1 bật trong 2s */
        gpio_pin_set_dt(&y0, 0);
        gpio_pin_set_dt(&y1, 1);
        k_msleep(2000);

        /* Trạng thái 3: Cả Y0 và Y1 cùng bật trong 2s */
        gpio_pin_set_dt(&y0, 1);
        gpio_pin_set_dt(&y1, 1);
        k_msleep(2000);
    }
}

K_THREAD_DEFINE(t_led, 1024, thread_led_blinky, NULL, NULL, NULL, 7, 0, 0);
K_THREAD_DEFINE(t_seq, 1024, thread_output_sequence, NULL, NULL, NULL, 7, 0, 0);

int main(void)
{
    return 0;
}