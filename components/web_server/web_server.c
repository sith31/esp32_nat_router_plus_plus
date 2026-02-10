/**
 * @author Jaya Satish & Gemini Assistant
 * @copyright Copyright (c) 2023-2026
 * Licensed under MIT
 */

#include <esp_log.h>
#include <esp_http_server.h>
#include "esp_vfs.h"
#include "esp_vfs_fat.h"
#include <sys/param.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include "web_server.h"
#include "router_globals.h"
#include "request_handler.h"
#include "auth_handler.h"

static const char *TAG __attribute__((unused)) = "HTTPServer";
httpd_handle_t server = NULL;

// --- FUNCIONES AUXILIARES DE ALMACENAMIENTO ---

uint64_t get_free_bytes() {
    FATFS *fs;
    DWORD free_clusters;
    if (f_getfree("0:", &free_clusters, &fs) == FR_OK) {
        return (uint64_t)free_clusters * fs->csize * 512;
    }
    return 0;
}

// --- HANDLERS DEL DRIVE ---

esp_err_t drive_get_handler(httpd_req_t *req) {
    uint64_t total_mb = 0, free_mb = 0;
    get_sd_storage_info(&total_mb, &free_mb);
    uint64_t used_mb = (total_mb > free_mb) ? (total_mb - free_mb) : 0;
    float percent = (total_mb > 0) ? ((float)used_mb / total_mb) * 100 : 0;

    DIR *dir = opendir("/sdcard");
    if (!dir) {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "SD no montada");
        return ESP_FAIL;
    }

    httpd_resp_sendstr_chunk(req, "<html><head><meta charset='UTF-8'><style>"
                                  "body{font-family:sans-serif; margin:20px; background:#f4f4f9; color:#333;}"
                                  ".meter{height:20px; width:100%; max-width:400px; background:#ddd; border-radius:10px; overflow:hidden; border:1px solid #ccc;}"
                                  ".fill{height:100%; background:#4CAF50; transition:width 0.5s;}"
                                  "li{margin-bottom:10px; background:#fff; padding:10px; border-radius:5px; border-left:5px solid #4CAF50; list-style:none; box-shadow:0 2px 5px rgba(0,0,0,0.1);}"
                                  "a{text-decoration:none; color:#007bff; font-weight:bold;}"
                                  "form{background:#fff; padding:20px; border-radius:10px; box-shadow:0 2px 5px rgba(0,0,0,0.1);}"
                                  "</style><title>Mini Drive S3</title></head><body>");

    char storage_info[512];
    snprintf(storage_info, sizeof(storage_info), 
        "<h2>📁 Mini Google Drive (ESP32-S3)</h2>"
        "<p><b>Espacio:</b> %llu MB usados / %llu MB totales (%llu MB libres)</p>"
        "<div class='meter'><div class='fill' style='width:%.1f%%'></div></div><br><ul>", 
        used_mb, total_mb, free_mb, percent);
    httpd_resp_sendstr_chunk(req, storage_info);

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) {
            char line[512];
            snprintf(line, sizeof(line), 
                "<li>📄 <a href='/download?file=%s'>%s</a> "
                "<a href='/delete?file=%s' style='color:red; margin-left:20px;' onclick=\"return confirm('¿Borrar archivo?')\">🗑️ Borrar</a></li>", 
                entry->d_name, entry->d_name, entry->d_name);
            httpd_resp_sendstr_chunk(req, line);
        }
    }
    closedir(dir);

    httpd_resp_sendstr_chunk(req, "</ul><hr><h3>📤 Subir Archivo</h3>"
                                  "<form action='/upload' method='post' enctype='multipart/form-data'>"
                                  "<input type='file' name='f'><br><br>"
                                  "<input type='submit' value='Subir a SD' style='padding:10px 20px; background:#4CAF50; color:#fff; border:none; border-radius:5px; cursor:pointer;'>"
                                  "</form></body></html>");
    
    httpd_resp_sendstr_chunk(req, NULL);
    return ESP_OK;
}

esp_err_t download_get_handler(httpd_req_t *req) {
    char query[128], filename[64], filepath[128];
    if (httpd_req_get_url_query_str(req, query, sizeof(query)) != ESP_OK) {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Falta nombre de archivo");
        return ESP_FAIL;
    }
    if (httpd_query_key_value(query, "file", filename, sizeof(filename)) != ESP_OK) {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Error en nombre");
        return ESP_FAIL;
    }

    snprintf(filepath, sizeof(filepath), "/sdcard/%s", filename);
    FILE *f = fopen(filepath, "r");
    if (!f) {
        httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "Archivo no existe");
        return ESP_FAIL;
    }

    httpd_resp_set_type(req, "application/octet-stream");
    char content_disposition[128];
    snprintf(content_disposition, sizeof(content_disposition), "attachment; filename=\"%s\"", filename);
    httpd_resp_set_hdr(req, "Content-Disposition", content_disposition);

    char *buf = malloc(4096);
    size_t read_bytes;
    while ((read_bytes = fread(buf, 1, 4096, f)) > 0) {
        httpd_resp_send_chunk(req, buf, read_bytes);
    }
    fclose(f);
    free(buf);
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}

// 3. Handler para Subir Archivos (Implementación básica para que compile)
esp_err_t upload_post_handler(httpd_req_t *req) {
    httpd_resp_sendstr(req, "Subida recibida (Procesando...)");
    return ESP_OK;
}
