#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "nvs_flash.h"
#include "esp_http_server.h"
#include "driver/gpio.h"
#include "mdns.h"

#define RELAY_GPIO 12
#define LED_GPIO   14

#define SHORT_PRESS_MS 500
#define LONG_PRESS_MS  5000

static const char *TAG = "esp32_webswitch";

static const char *html_page =
    "<!DOCTYPE html><html><head>"
    "<meta charset=\"UTF-8\">"
    "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">"
    "<style>"
    "body{font-family:Arial;text-align:center;background:#f0f0f0;margin-top:40px}"
    "button{width:80%%;max-width:300px;padding:20px;font-size:20px;border:none;border-radius:10px;color:#fff;margin:10px;}"
    ".on{background:#4CAF50;}.off{background:#f44336;}button:active{opacity:0.6;}"
    "</style></head>"
    "<body><h2>ESP32 PowerControl</h2>"
    "<button class=\"on\" onclick=\"location.href='/short'\">🟢 PowerOn</button><br/>"
    "<button class=\"off\" onclick=\"location.href='/long'\">🔴 PowerDown</button>"
    "</body></html>";

static esp_err_t root_get_handler(httpd_req_t *req) {
    httpd_resp_send(req, html_page, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

static void press_button(int duration_ms) {
    gpio_set_level(RELAY_GPIO, 0);
    gpio_set_level(LED_GPIO, 1);
    vTaskDelay(pdMS_TO_TICKS(duration_ms));
    gpio_set_level(RELAY_GPIO, 1);
    gpio_set_level(LED_GPIO, 0);
}

static esp_err_t short_press_handler(httpd_req_t *req) {
    press_button(SHORT_PRESS_MS);
    httpd_resp_set_status(req, "303 See Other");
    httpd_resp_set_hdr(req, "Location", "/");
    httpd_resp_send(req, NULL, 0);
    return ESP_OK;
}

static esp_err_t long_press_handler(httpd_req_t *req) {
    press_button(LONG_PRESS_MS);
    httpd_resp_set_status(req, "303 See Other");
    httpd_resp_set_hdr(req, "Location", "/");
    httpd_resp_send(req, NULL, 0);
    return ESP_OK;
}

static httpd_handle_t start_webserver(void) {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;

    if (httpd_start(&server, &config) == ESP_OK) {
        httpd_uri_t root = {
            .uri = "/", .method = HTTP_GET, .handler = root_get_handler, .user_ctx = NULL
        };
        httpd_uri_t short_press = {
            .uri = "/short", .method = HTTP_GET, .handler = short_press_handler, .user_ctx = NULL
        };
        httpd_uri_t long_press = {
            .uri = "/long", .method = HTTP_GET, .handler = long_press_handler, .user_ctx = NULL
        };

        httpd_register_uri_handler(server, &root);
        httpd_register_uri_handler(server, &short_press);
        httpd_register_uri_handler(server, &long_press);
    }
    return server;
}

static void wifi_init_sta(void) {
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = "HUAWEI-2.4G-3bus",
            .password = "34Rfvcde",
            .threshold.authmode = WIFI_AUTH_WPA2_PSK
        },
    };

    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    esp_wifi_start();

    ESP_LOGI(TAG, "Connecting to WiFi...");
    esp_wifi_connect();
}

static void mdns_init_service(void) {
    ESP_ERROR_CHECK(mdns_init());
    ESP_ERROR_CHECK(mdns_hostname_set("esp32"));
    ESP_ERROR_CHECK(mdns_instance_name_set("ESP32 Web Switch"));
    mdns_service_add("ESP Web", "_http", "_tcp", 80, NULL, 0);
    ESP_LOGI(TAG, "mDNS started: http://esp32.local");
}

void app_main(void) {
    nvs_flash_init();
    wifi_init_sta();

    gpio_reset_pin(RELAY_GPIO);
    gpio_set_direction(RELAY_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_level(RELAY_GPIO, 1);

    gpio_reset_pin(LED_GPIO);
    gpio_set_direction(LED_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_level(LED_GPIO, 0);

    vTaskDelay(pdMS_TO_TICKS(5000));
    mdns_init_service();
    start_webserver();
    ESP_LOGI(TAG, "Web server started");
}