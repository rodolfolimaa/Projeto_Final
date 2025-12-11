#include "include/display.h"

// Buffer global do display
static uint8_t ssd[ssd1306_width * ssd1306_n_pages];

void screem_init() {
    i2c_init(i2c1, ssd1306_i2c_clock * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    ssd1306_init();
}

// Mensagem simples sem quebra de linha nem espaçamento
void screem_mensage(const char* msg) {
    // Zera o buffer
    memset(ssd, 0, sizeof(ssd));

    // Desenha a string no topo da tela
    ssd1306_draw_string(ssd, 0, 0, (char*)msg);

    // Prepara área de renderização
    struct render_area area = {
        .start_column = 0,
        .end_column = ssd1306_width - 1,
        .start_page = 0,
        .end_page = ssd1306_n_pages - 1
    };
    calculate_render_area_buffer_length(&area);

    // Envia para o display
    render_on_display(ssd, &area);

    ssd1306_init();
}

// Mensagem com quebra de linha
void screem_mensage_multiline(const char* msg) {
    memset(ssd, 0, sizeof(ssd));

    const int char_width = 6;   // Largura média de um caractere (fonte de 6x8)
    const int char_height = 8;  // Altura da fonte
    const int screen_width = ssd1306_width;        // Ex: 128
    const int screen_height = ssd1306_n_pages * 8; // Ex: 64

    int max_chars_per_line = screen_width / char_width;
    int max_lines = screen_height / char_height;

    char line_buf[32]; // linha temporária (suporte até 32 caracteres)
    int line = 0;

    const char* ptr = msg;
    while (*ptr && line < max_lines) {
        // Copia uma linha
        int i = 0;
        while (i < max_chars_per_line && *ptr && *ptr != '\n') {
            line_buf[i++] = *ptr++;
        }
        line_buf[i] = '\0';

        // Se caractere atual for '\n', pula ele
        if (*ptr == '\n') ptr++;

        // Desenha a linha no buffer
        ssd1306_draw_string(ssd, 0, line * char_height, line_buf);
        line++;
    }

    // Renderiza tudo
    struct render_area area = {
        .start_column = 0,
        .end_column = ssd1306_width - 1,
        .start_page = 0,
        .end_page = ssd1306_n_pages - 1
    };
    calculate_render_area_buffer_length(&area);
    render_on_display(ssd, &area);
}

// // Mensagem centralizada
void screem_centered_message(const char* msg) {
    memset(ssd, 0, sizeof(ssd));

    const int char_width = 6;
    const int char_height = 8;

    int msg_len = strlen(msg);
    int text_width = msg_len * char_width;
    int x = (ssd1306_width - text_width) / 2;
    int y = (ssd1306_n_pages * 8 - char_height) / 2;

    ssd1306_draw_string(ssd, x, y, (char*)msg);

    struct render_area area = {
        .start_column = 0,
        .end_column = ssd1306_width - 1,
        .start_page = 0,
        .end_page = ssd1306_n_pages - 1
    };
    calculate_render_area_buffer_length(&area);
    render_on_display(ssd, &area);
}


void screem_message_4lines(const char* l1, const char* l2, const char* l3, const char* l4) {
    memset(ssd, 0, sizeof(ssd));

    ssd1306_draw_string(ssd, 0, 0, (char*)l1);
    ssd1306_draw_string(ssd, 0, 16, (char*)l2);
    ssd1306_draw_string(ssd, 0, 32, (char*)l3);
    ssd1306_draw_string(ssd, 0, 48, (char*)l4);

    struct render_area area = {
        .start_column = 0,
        .end_column = ssd1306_width - 1,
        .start_page = 0,
        .end_page = ssd1306_n_pages - 1
    };
    calculate_render_area_buffer_length(&area);
    render_on_display(ssd, &area);
}