#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>

static const struct gpio_dt_spec led3 = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);
static const struct gpio_dt_spec y0   = GPIO_DT_SPEC_GET(DT_ALIAS(led1), gpios);
static const struct gpio_dt_spec y1   = GPIO_DT_SPEC_GET(DT_ALIAS(led2), gpios);

struct phase {
    const char *name;
    int y0;
    int y1;
    int ms;
};

/* Mảng lưu 3 pha đèn giao thông */
static const struct phase traffic_phases[] = {
    { .name = "RED",   .y0 = 1, .y1 = 0, .ms = 7000 },
    { .name = "GREEN", .y0 = 0, .y1 = 1, .ms = 5000 },
    { .name = "AMBER", .y0 = 1, .y1 = 1, .ms = 2000 },
};

/* Luồng 1: Đèn LED3 báo hệ thống đang chạy (Alive indicator) */
void thread_led_blinky(void *p1, void *p2, void *p3)
{
    if (!gpio_is_ready_dt(&led3)) return;
    gpio_pin_configure_dt(&led3, GPIO_OUTPUT_INACTIVE);

    while (1) {
        gpio_pin_set_dt(&led3, 1);
        k_msleep(1000);
        gpio_pin_set_dt(&led3, 0);
        k_msleep(1000);
    }
}

/* Luồng 2: Điều khiển chu trình đèn giao thông */
void thread_traffic_light(void *p1, void *p2, void *p3)
{
    if (!gpio_is_ready_dt(&y0) || !gpio_is_ready_dt(&y1)) return;
    gpio_pin_configure_dt(&y0, GPIO_OUTPUT_INACTIVE);
    gpio_pin_configure_dt(&y1, GPIO_OUTPUT_INACTIVE);

    while (1) {
        /* Duyệt qua từng pha đèn trong mảng */
        for (int i = 0; i < 3; i++) {
            const struct phase *p = &traffic_phases[i];

            gpio_pin_set_dt(&y0, p->y0);
            gpio_pin_set_dt(&y1, p->y1);

            printk("[LIGHT] %s - %d s\n", p->name, p->ms / 1000);

            k_msleep(p->ms);
        }
    }
}

K_THREAD_DEFINE(t_alive,   1024, thread_led_blinky,    NULL, NULL, NULL, 7, 0, 0);
K_THREAD_DEFINE(t_traffic, 1024, thread_traffic_light, NULL, NULL, NULL, 7, 0, 0);

int main(void)
{
    return 0;
}