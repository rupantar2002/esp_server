#include "server.h"
#include <esp_http_server.h>
#include <esp_log.h>
#include <esp_err.h>
#include <mdns.h>

#define MIN(a, b) a > b ? b : a

static const char *TAG = "server";

extern const uint8_t INDEX_HTML_START[] asm("_binary_index_html_start");
extern const uint8_t INDEX_HTML_STOP[] asm("_binary_index_html_end");

extern const uint8_t APP_JS_START[] asm("_binary_app_js_start");
extern const uint8_t APP_JS_STOP[] asm("_binary_app_js_end");

extern const uint8_t CONFIG_JSON_START[] asm("_binary_config_json_start");
extern const uint8_t CONFIG_JSON_STOP[] asm("_binary_config_json_end");

static httpd_handle_t gServer = NULL;

/* ========== URI =========== */
static httpd_uri_t gUriGetIndexHtml = {0};
static httpd_uri_t gUriPostIndexHtml = {0};
static httpd_uri_t gUriAppJs = {0};
static httpd_uri_t gUriConfigJson = {0};

/* ===== URI HANDLERS ======= */
static esp_err_t GetReqHandler_index_html(httpd_req_t *req);
static esp_err_t PostReqHandler_index_html(httpd_req_t *req);
static esp_err_t GetReqHandler_app_js(httpd_req_t *req);
static esp_err_t GetReqHandler_config_json(httpd_req_t *req);

void server_Init()
{
    gUriGetIndexHtml.handler = GetReqHandler_index_html;
    gUriGetIndexHtml.method = HTTP_GET;
    gUriGetIndexHtml.uri = "/";
    // gUriGetIndexHtml.user_ctx = NULL;

    gUriPostIndexHtml.handler = PostReqHandler_index_html;
    gUriPostIndexHtml.method = HTTP_POST;
    gUriPostIndexHtml.uri = "/";
    // gUriPostIndexHtml.user_ctx = NULL;

    gUriAppJs.handler = GetReqHandler_app_js;
    gUriAppJs.method = HTTP_GET;
    gUriAppJs.uri = "/app.js";
    // gUriAppJs.user_ctx = NULL;

    gUriConfigJson.handler = GetReqHandler_config_json;
    gUriConfigJson.method = HTTP_GET;
    gUriConfigJson.uri = "/config.json";
}

void server_Start()
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    if (httpd_start(&gServer, &config) == ESP_OK)
    {
        /* Register URI handlers */
        httpd_register_uri_handler(gServer, &gUriGetIndexHtml);
        httpd_register_uri_handler(gServer, &gUriPostIndexHtml);
        httpd_register_uri_handler(gServer, &gUriAppJs);
        httpd_register_uri_handler(gServer, &gUriConfigJson);

        /*=========MDNS========= */

        mdns_init();
        mdns_hostname_set("myserver-esp");
        // mdns_instance_name_set(MDNS_INSTANCE);

        // mdns_txt_item_t serviceTxtData[] = {
        //     {"board", "esp32"},
        //     {"path", "/"}};

        // ESP_ERROR_CHECK(mdns_service_add("ESP32-WebServer", "_http", "_tcp", 80, serviceTxtData,
        //                                  sizeof(serviceTxtData) / sizeof(serviceTxtData[0])));

        /*====================== */

        ESP_LOGI(TAG, "server Started\n");
        ESP_LOGI(TAG, "port->%d\n", config.server_port);
    }
    else
    {
        ESP_LOGE(TAG, "failed to start server\r\n");
    }
}

void server_StartSocket(int port)
{
}

void server_StopSocket(int port)
{
}

void server_Stop()
{
    if (gServer)
    {
        httpd_stop(gServer);
    }
}

/* Our URI handler function to be called during POST /uri request */
// esp_err_t post_handler(httpd_req_t *req)
// {
//     /* Destination buffer for content of HTTP POST request.
//      * httpd_req_recv() accepts char* only, but content could
//      * as well be any binary data (needs type casting).
//      * In case of string data, null termination will be absent, and
//      * content length would give length of string */
//     char content[100];

//     /* Truncate if content length larger than the buffer */
//     size_t recv_size = MIN(req->content_len, sizeof(content));

//     int ret = httpd_req_recv(req, content, recv_size);
//     if (ret <= 0)
//     { /* 0 return value indicates connection closed */
//         /* Check if timeout occurred */
//         if (ret == HTTPD_SOCK_ERR_TIMEOUT)
//         {
//             /* In case of timeout one can choose to retry calling
//              * httpd_req_recv(), but to keep it simple, here we
//              * respond with an HTTP 408 (Request Timeout) error */
//             httpd_resp_send_408(req);
//         }
//         /* In case of error, returning ESP_FAIL will
//          * ensure that the underlying socket is closed */
//         return ESP_FAIL;
//     }

//     /* Send a simple response */
//     const char resp[] = "URI POST Response";
//     httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
//     return ESP_OK;
// }

// /* URI handler structure for GET /uri */
// httpd_uri_t uri_get = {
//     .uri = "/uri",
//     .method = HTTP_GET,
//     .handler = get_handler,
//     .user_ctx = NULL};

// /* URI handler structure for POST /uri */
// httpd_uri_t uri_post = {
//     .uri = "/uri",
//     .method = HTTP_POST,
//     .handler = post_handler,
//     .user_ctx = NULL};

// /* Function for starting the webserver */
// httpd_handle_t start_webserver(void)
// {
//     /* Generate default configuration */
//     httpd_config_t config = HTTPD_DEFAULT_CONFIG();

//     /* Empty handle to esp_http_server */
//     httpd_handle_t server = NULL;

//     /* Start the httpd server */
//     if (httpd_start(&server, &config) == ESP_OK)
//     {
//         /* Register URI handlers */
//         httpd_register_uri_handler(server, &uri_get);
//         httpd_register_uri_handler(server, &uri_post);
//     }
//     /* If server failed to start, handle will be NULL */
//     return server;
// }

// /* Function for stopping the webserver */
// void stop_webserver(httpd_handle_t server)
// {
//     if (server)
//     {
//         /* Stop the httpd server */
//         httpd_stop(server);
//     }
// }

/* ====================== HANDLER DEFINITIONS ========================= */

esp_err_t GetReqHandler_index_html(httpd_req_t *req)
{
    ESP_LOGW(TAG, "page1 get request\r\n");
    httpd_resp_send(req, (const char *)INDEX_HTML_START, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

esp_err_t PostReqHandler_index_html(httpd_req_t *req)
{
    ESP_LOGI(TAG, "page1 post request\r\n");

    static char content[1024];
    memset(content, 0, 1024);
    uint32_t recv_Size = MIN(req->content_len, sizeof(content));
    int ret = httpd_req_recv(req, content, recv_Size);

    if (ret <= 0)
    {
        if (ret == HTTPD_SOCK_ERR_TIMEOUT)
        {
            httpd_resp_send_408(req);
        }
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "conetnt->%s", content);
    char buff[100] = {0};
    httpd_req_get_hdr_value_str(req, "Content-Type", buff, sizeof(buff));
    ESP_LOGI(TAG, "HEADER->Content-Type->%s", buff);
    httpd_req_get_hdr_value_str(req, "Content-Length", buff, sizeof(buff));
    ESP_LOGI(TAG, "HEADER->Content-Length->%s\r\n", buff);
    httpd_resp_send(req, content, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

static esp_err_t GetReqHandler_app_js(httpd_req_t *req)
{
    ESP_LOGW(TAG, "app.js get Request\r\n");
    return httpd_resp_send(req, (const char *)APP_JS_START, HTTPD_RESP_USE_STRLEN);
}

static esp_err_t GetReqHandler_config_json(httpd_req_t *req)
{
    ESP_LOGW(TAG, "config.json get Request\r\n");
    return httpd_resp_send(req, (const char *)CONFIG_JSON_START, HTTPD_RESP_USE_STRLEN);
}