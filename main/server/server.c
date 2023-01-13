#include "server.h"
#include <esp_http_server.h>
#include <esp_log.h>

#define MIN(a, b) a > b ? b : a

static const char *TAG = "server";

extern const uint8_t PAGE_ONE_START[] asm("_binary_page1_html_start");
extern const uint8_t PAGE_ONE_STOP[] asm("_binary_page1_html_end");

static httpd_handle_t gServer = NULL;

/* ========== URI =========== */
static httpd_uri_t gUriGetPage1 = {0};
static httpd_uri_t gUriPostPage1 = {0};

/* ===== URI HANDLERS ======= */
static esp_err_t GetReqHandler_Page1(httpd_req_t *req);
static esp_err_t PostReqHandler_Page1(httpd_req_t *req);

void server_Init()
{
    gUriGetPage1.handler = GetReqHandler_Page1;
    gUriGetPage1.method = HTTP_GET;
    gUriGetPage1.uri = "/";
    gUriGetPage1.user_ctx = NULL;

    gUriPostPage1.handler = PostReqHandler_Page1;
    gUriPostPage1.method = HTTP_POST;
    gUriPostPage1.uri = "/";
    gUriPostPage1.user_ctx = NULL;
}

void server_Start()
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    if (httpd_start(&gServer, &config) == ESP_OK)
    {
        /* Register URI handlers */
        httpd_register_uri_handler(gServer, &gUriGetPage1);
        httpd_register_uri_handler(gServer, &gUriPostPage1);

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

esp_err_t GetReqHandler_Page1(httpd_req_t *req)
{
    ESP_LOGW(TAG, "page1 get request\r\n");
    httpd_resp_send(req, (const char *)PAGE_ONE_START, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

esp_err_t PostReqHandler_Page1(httpd_req_t *req)
{
    ESP_LOGI(TAG, "page1 post request\r\n");

    char content[100];
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
    httpd_resp_send(req, "Hi I am your server", strlen("He I am your server"));
    return ESP_OK;
}