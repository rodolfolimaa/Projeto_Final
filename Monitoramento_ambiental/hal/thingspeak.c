/**
 * @file thingspeak.c
 * @brief Envio de leituras ao ThingSpeak usando TCP (lwIP) com DNS.
 */

#include "include/thingspeak.h"
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include "pico/time.h"
#include "lwip/ip_addr.h"
#include "lwip/tcp.h"
#include "lwip/dns.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "credentials.h"

#define THINGSPEAK_PORT           80
#define THINGSPEAK_TCP_TIMEOUT_MS 7000U

typedef struct
{
    struct tcp_pcb *pcb;
    SemaphoreHandle_t sem_done;
    err_t last_err;
    int sent;
    int finished;
} ts_http_ctx_t;

static inline uint32_t uptime_s(void)
{
    return (uint32_t)(to_ms_since_boot(get_absolute_time()) / 1000u);
}

// TCP Callbacks
static err_t thingspeak_tcp_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
    ts_http_ctx_t *ctx = (ts_http_ctx_t *)arg;
    ctx->last_err = err;

    if (p == NULL) {
        ctx->finished = 1;
        tcp_close(tpcb);
        ctx->pcb = NULL;
        xSemaphoreGive(ctx->sem_done);
        return ERR_OK;
    }

    tcp_recved(tpcb, p->len);
    pbuf_free(p);
    return ERR_OK;
}

static void thingspeak_tcp_err(void *arg, err_t err)
{
    ts_http_ctx_t *ctx = (ts_http_ctx_t *)arg;
    ctx->last_err = err;
    ctx->finished = 1;
    if (ctx->sem_done) xSemaphoreGive(ctx->sem_done);
}

static err_t thingspeak_tcp_sent(void *arg, struct tcp_pcb *tpcb, u16_t len)
{
    (void)arg; (void)tpcb; (void)len;
    return ERR_OK;
}

static err_t thingspeak_tcp_connected(void *arg, struct tcp_pcb *tpcb, err_t err)
{
    ts_http_ctx_t *ctx = (ts_http_ctx_t *)arg;
    ctx->last_err = err;
    if (err != ERR_OK) {
        ctx->finished = 1;
        if (ctx->sem_done) xSemaphoreGive(ctx->sem_done);
    }
    return err;
}

// Função para enviar dados
void thingspeak_send(const char *api_key, uint8_t num_fields, ...)
{
    if (!api_key || num_fields == 0 || num_fields > 8) {
        printf("ThingSpeak: parâmetros inválidos\n");
        return;
    }

    char qs[256] = {0};
    size_t pos = snprintf(qs, sizeof(qs), "api_key=%s", api_key);

    va_list ap;
    va_start(ap, num_fields);

    for (int i = 1; i <= num_fields && pos < sizeof(qs); i++) {
        double val = va_arg(ap, double);
        if (isnan(val) || isinf(val)) val = 0.0;
        pos += snprintf(qs + pos, sizeof(qs) - pos, "&field%d=%.2f", i, val);
    }

    va_end(ap);

    char req[512];
    int nreq = snprintf(req, sizeof(req),
                        "GET /update?%s HTTP/1.1\r\n"
                        "Host: %s\r\n"
                        "User-Agent: pico-w/rawtcp\r\n"
                        "Connection: close\r\n\r\n",
                        qs, THINGSPEAK_HOST);

    if (nreq <= 0 || nreq >= (int)sizeof(req)) {
        printf("ThingSpeak: requisição muito grande\n");
        return;
    }

    ip_addr_t ip;
    err_t err = dns_gethostbyname(THINGSPEAK_HOST, &ip, NULL, NULL);
    if (err == ERR_INPROGRESS) {
        // Espera resolver DNS
        for (int i = 0; i < 20; i++) {
            vTaskDelay(pdMS_TO_TICKS(200));
            if (dns_gethostbyname(THINGSPEAK_HOST, &ip, NULL, NULL) == ERR_OK) break;
        }
    }
    if (err != ERR_OK) {
        printf("ThingSpeak: falha ao resolver DNS\n");
        return;
    }

    ts_http_ctx_t ctx = {0};
    ctx.pcb = tcp_new_ip_type(IPADDR_TYPE_V4);
    if (!ctx.pcb) { printf("ThingSpeak: tcp_new falhou\n"); return; }

    ctx.sem_done = xSemaphoreCreateBinary();
    if (!ctx.sem_done) { printf("ThingSpeak: semáforo falhou\n"); tcp_abort(ctx.pcb); return; }

    tcp_arg(ctx.pcb, &ctx);
    tcp_err(ctx.pcb, thingspeak_tcp_err);
    tcp_recv(ctx.pcb, thingspeak_tcp_recv);
    tcp_sent(ctx.pcb, thingspeak_tcp_sent);

    err = tcp_connect(ctx.pcb, &ip, THINGSPEAK_PORT, thingspeak_tcp_connected);
    if (err != ERR_OK) { printf("ThingSpeak: tcp_connect err=%d\n", err); vSemaphoreDelete(ctx.sem_done); tcp_abort(ctx.pcb); return; }

    vTaskDelay(pdMS_TO_TICKS(30));

    if (!ctx.sent && ctx.pcb) {
        err = tcp_write(ctx.pcb, req, (u16_t)nreq, TCP_WRITE_FLAG_COPY);
        if (err == ERR_OK) { tcp_output(ctx.pcb); ctx.sent = 1; }
        else { printf("ThingSpeak: tcp_write err=%d\n", err); tcp_abort(ctx.pcb); vSemaphoreDelete(ctx.sem_done); return; }
    }

    if (xSemaphoreTake(ctx.sem_done, pdMS_TO_TICKS(THINGSPEAK_TCP_TIMEOUT_MS)) != pdTRUE) {
        printf("ThingSpeak: timeout aguardando resposta\n");
        if (ctx.pcb) tcp_abort(ctx.pcb);
        vSemaphoreDelete(ctx.sem_done);
        return;
    }

    if (ctx.pcb) {
        tcp_arg(ctx.pcb, NULL);
        tcp_recv(ctx.pcb, NULL);
        tcp_err(ctx.pcb, NULL);
        tcp_sent(ctx.pcb, NULL);
        tcp_close(ctx.pcb);
    }

    vSemaphoreDelete(ctx.sem_done);

    printf("ThingSpeak: dados enviados (%d bytes)\n", nreq);
}
