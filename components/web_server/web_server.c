/**
 * @author Jaya Satish
 *
 *@copyright Copyright (c) 2023
 *Licensed under MIT
 *
 */
/* Simple HTTP Server Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <esp_log.h>
#include <esp_http_server.h>
#include "web_server.h"
#include "router_globals.h"
#include "request_handler.h"
#include "auth_handler.h"
#include <sys/statvfs.h>

static const char *TAG = "HTTPServer";

// declare global variable to store server handle
httpd_handle_t server = NULL;

static httpd_uri_t get_scan = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = get_scan_handler,
    .user_ctx = &auth_info};

static httpd_uri_t get_settings = {
    .uri = "/settings",
    .method = HTTP_GET,
    .handler = get_settings_handler,
    .user_ctx = &auth_info};

static httpd_uri_t get_info = {
    .uri = "/info",
    .method = HTTP_GET,
    .handler = get_info_handler,
    .user_ctx = &auth_info};

static httpd_uri_t get_main_css = {
    .uri = "/main.css",
    .method = HTTP_GET,
    .handler = get_main_css_handler,
    .user_ctx = &auth_info};

static httpd_uri_t get_dark_css = {
    .uri = "/dark.css",
    .method = HTTP_GET,
    .handler = get_dark_css_handler,
    .user_ctx = &auth_info};

static httpd_uri_t get_error_404 = {
    .uri = "/404",
    .method = HTTP_GET,
    .handler = get_error_404_handler,
    .user_ctx = &auth_info};
static httpd_uri_t drive_uri = {
    .uri       = "/drive",
    .method    = HTTP_GET,
    .handler   = drive_get_handler,
    .user_ctx  = NULL;

static httpd_uri_t upload_uri = {
    .uri       = "/upload",
    .method    = HTTP_POST,
    .handler   = upload_post_handler,
    .user_ctx  = NULL;

static httpd_uri_t delete_uri = {
    .uri       = "/delete",
    .method    = HTTP_GET,
    .handler   = delete_get_handler,
    .user_ctx  = NULL;

// javascript
static httpd_uri_t common_js_path = {
    .uri = "/js/*",
    .method = HTTP_GET,
    .handler = common_js_path_handler,
    .user_ctx = &auth_info};

static httpd_uri_t common_data_path = {
    .uri = "/data/*",
    .method = HTTP_GET,
    .handler = common_data_handler,
    .user_ctx = &auth_info};

static httpd_uri_t settings_post_path = {
    .uri = "/data/settingsSave.json",
    .method = HTTP_POST,
    .handler = common_data_handler,
    .user_ctx = &auth_info};

static httpd_uri_t post_ota_update = {
    .uri = "/ota",
    .method = HTTP_POST,
    .handler = post_ota_update_handler,
    .user_ctx = &auth_info};

void get_sd_storage_info(uint64_t *total_mb, uint64_t *free_mb) {
    struct statvfs stat;
    if (statvfs("/sdcard", &stat) == 0) {
        *total_mb = ((uint64_t)stat.f_blocks * stat.f_frsize) / (1024 * 1024);
        *free_mb = ((uint64_t)stat.f_bfree * stat.f_frsize) / (1024 * 1024);
    } else {
        *total_mb = 0;
        *free_mb = 0;
    }
}

esp_err_t drive_get_handler(httpd_req_t *req) {
    uint64_t total_mb = 0, free_mb = 0;
    get_sd_storage_info(&total_mb, &free_mb);
    uint64_t used_mb = total_mb - free_mb;

    DIR *dir = opendir("/sdcard");
    if (!dir) {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "SD no montada");
        return ESP_FAIL;
    }

    httpd_resp_sendstr_chunk(req, "<html><head><meta charset='UTF-8'><style>"
                                  "body{font-family:sans-serif; margin:20px;}"
                                  ".meter{height:20px; width:300px; background:#ddd; border-radius:10px; overflow:hidden;}"
                                  ".fill{height:100%; background:#4CAF50; transition:width 0.5s;}"
                                  "</style><title>Mini Drive</title></head><body>");

    // Título e info de espacio
    httpd_resp_sendstr_chunk(req, "<h2>📁 Archivos en la SD</h2>");
    
    char storage_info[512];
    float percent = (total_mb > 0) ? ((float)used_mb / total_mb) * 100 : 0;
    
    snprintf(storage_info, sizeof(storage_info), 
        "<p>Almacenamiento: %llu MB usados de %llu MB totales (%llu MB libres)</p>"
        "<div class='meter'><div class='fill' style='width:%.1f%%'></div></div><br>", 
        used_mb, total_mb, free_mb, percent);
    httpd_resp_sendstr_chunk(req, storage_info);

    httpd_resp_sendstr_chunk(req, "<ul>");

    // ... (aquí sigue tu bucle readdir actual para listar los archivos) ...
esp_err_t drive_get_handler(httpd_req_t *req) {
    DIR *dir = opendir("/sdcard");
    if (!dir) {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "SD no montada o no encontrada");
        return ESP_FAIL;
    }

    // Cabecera HTML
    httpd_resp_sendstr_chunk(req, "<html><head><meta charset='UTF-8'><title>Mini Drive</title></head><body>");
    httpd_resp_sendstr_chunk(req, "<h2>📁 Archivos en la SD</h2><ul>");

    struct dirent *entry;
    char line[512];

    // Listar archivos
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) { // Solo archivos regulares
            snprintf(line, sizeof(line), 
                "<li>"
                "<a href='/download?file=%s'>%s</a> "
                "<a href='/delete?file=%s' style='color:red; margin-left:15px;' onclick=\"return confirm('¿Borrar?')\">[X]</a>"
                "</li>", 
                entry->d_name, entry->d_name, entry->d_name);
            httpd_resp_sendstr_chunk(req, line);
        }
    }
    closedir(dir);

    // Formulario de subida
    httpd_resp_sendstr_chunk(req, "</ul><hr><h3>Subir nuevo archivo</h3>");
    httpd_resp_sendstr_chunk(req, "<form action='/upload' method='post' enctype='multipart/form-data'>");
    httpd_resp_sendstr_chunk(req, "<input type='file' name='f'><input type='submit' value='Subir'>");
    httpd_resp_sendstr_chunk(req, "</form></body></html>");
    
    httpd_resp_sendstr_chunk(req, NULL); // Finalizar respuesta
    return ESP_OK;
}

esp_err_t upload_post_handler(httpd_req_t *req) {
    char filepath[128];
    // Recuperamos el nombre del archivo desde el header (o un query param)
    // Para simplificar, lo llamaremos "upload.bin" si no viene nombre, 
    // pero lo ideal es sacarlo del header 'filename'
    snprintf(filepath, sizeof(filepath), "/sdcard/uploaded_file.dat");

    FILE *fd = fopen(filepath, "w");
    if (!fd) {
        ESP_LOGE("DRIVE", "Fallo al crear el archivo en la SD");
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Error al crear archivo");
        return ESP_FAIL;
    }

    char *buf = malloc(4096); // Buffer de 4KB para la transferencia
    int received;
    int remaining = req->content_len;

    while (remaining > 0) {
        if ((received = httpd_req_recv(req, buf, MIN(remaining, 4096))) <= 0) {
            if (received == HTTPD_SOCK_ERR_TIMEOUT) continue;
            fclose(fd);
            free(buf);
            return ESP_FAIL;
        }
        fwrite(buf, 1, received, fd);
        remaining -= received;
    }

    fclose(fd);
    free(buf);
    httpd_resp_set_status(req, "303 See Other"); // Redirigir de vuelta al drive
    httpd_resp_set_header(req, "Location", "/drive");
    httpd_resp_sendstr(req, "Archivo subido con éxito");
    return ESP_OK;
}
esp_err_t delete_get_handler(httpd_req_t *req) {
    char filepath[128];
    char query[128];
    
    // Obtener el nombre del archivo de la URL (ej: /delete?file=foto.jpg)
    if (httpd_req_get_url_query_str(req, query, sizeof(query)) == ESP_OK) {
        char filename[64];
        if (httpd_query_key_value(query, "file", filename, sizeof(filename)) == ESP_OK) {
            snprintf(filepath, sizeof(filepath), "/sdcard/%s", filename);
            
            ESP_LOGI("DRIVE", "Borrando archivo: %s", filepath);
            if (unlink(filepath) == 0) {
                // Redirigir al drive después de borrar
                httpd_resp_set_status(req, "303 See Other");
                httpd_resp_set_header(req, "Location", "/drive");
                httpd_resp_sendstr(req, "Archivo borrado");
                return ESP_OK;
            }
        }
    }

    httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "No se pudo borrar el archivo");
    return ESP_FAIL;
}

//-----------------------------------------------------------------------------
esp_err_t http_404_error_handler(httpd_req_t *req)
{
    httpd_resp_set_status(req, "302 Temporary Redirect");
    httpd_resp_set_hdr(req, "Location", "/404");
    httpd_resp_send(req, "Redirect to 404 page", HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

static httpd_uri_t error_404_handler = {
    .uri = "/*",
    .method = HTTP_GET,
    .handler = http_404_error_handler,
    .user_ctx = NULL};

//-----------------------------------------------------------------------------
httpd_handle_t start_webserver(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    // config.task_priority = 6,
    // config.max_resp_headers = 1024;
    config.max_uri_handlers = 15;
    config.uri_match_fn = httpd_uri_match_wildcard;
    config.lru_purge_enable = true;

    // config.stack_size = 10000;

    if (httpd_start(&server, &config) != ESP_OK)
    {
        ESP_LOGE(TAG, "Error starting server!");
        return NULL;
    }
    ESP_LOGI(TAG, "Registering URI handlers");
    httpd_register_uri_handler(server, &get_scan);
    httpd_register_uri_handler(server, &get_info);
    httpd_register_uri_handler(server, &get_settings);
    httpd_register_uri_handler(server, &get_main_css);
    httpd_register_uri_handler(server, &get_dark_css);
    httpd_register_uri_handler(server, &common_js_path);
    httpd_register_uri_handler(server, &common_data_path);
    httpd_register_uri_handler(server, &settings_post_path);
    httpd_register_uri_handler(server, &post_ota_update);
    httpd_register_uri_handler(server, &get_error_404);
    httpd_register_uri_handler(server, &drive_uri);
   httpd_register_uri_handler(server, &download_uri);
   httpd_register_uri_handler(server, &delete_uri);
    httpd_register_uri_handler(server, &error_404_handler);
    return server;
}

//-----------------------------------------------------------------------------
void stop_web_server(void)
{
    if (server != NULL)
    { 
        httpd_stop(server);
        server = NULL; 
        ESP_LOGI(TAG, "Http server stoped");
    }
}

//-----------------------------------------------------------------------------
void toggle_webserver(void)
{
    IsWebServerEnable = !IsWebServerEnable;
    if (IsWebServerEnable)
    {
        server = start_webserver(); 
    }
    else
    {
        stop_web_server(); 
    }
}

//-----------------------------------------------------------------------------
