#include "sd_card.h"
#include "pico/stdlib.h"
#include "string.h"
#include "sd_card.h"
#include "hw_config.h"
#include "ff.h"
#include "hardware/rtc.h"
#include "pico/util/datetime.h"
#include <stdio.h>

// Variável estática para manter o estado do cartão SD.
// O "static" a torna visível apenas neste arquivo.
static sd_card_t *sd_card_instance;

// Inicializa o cartão SD.
FRESULT sd_card_init() {
    sd_card_instance = sd_get_by_num(0);
    return f_mount(&sd_card_instance->fatfs, sd_card_instance->pcName, 1);
}

FRESULT sd_card_read_and_print_file(const char* filename) {
    FIL file;
    FRESULT fr;
    char buffer[256];
    UINT bytes_read;

    // Abre o arquivo em modo de leitura
    fr = f_open(&file, filename, FA_READ);
    if (fr != FR_OK) {
        printf("Erro ao abrir o arquivo para leitura: %d\n", fr);
        return fr;
    }

    printf("\n--- Conteudo do arquivo %s ---\n", filename);

    // Lê o arquivo em blocos e imprime no terminal
    do {
        fr = f_read(&file, buffer, sizeof(buffer) - 1, &bytes_read);
        if (fr != FR_OK) {
            printf("Erro na leitura do arquivo: %d\n", fr);
            break;
        }
        buffer[bytes_read] = '\0'; // Adiciona terminador nulo para imprimir a string
        printf("%s", buffer);
    } while (bytes_read > 0);

    // Fecha o arquivo
    f_close(&file);

    printf("--- Fim do arquivo ---\n");
    return fr;
}


// Adiciona uma linha de dados a um arquivo CSV.
FRESULT sd_card_append_to_csv(const char* filename, const char* data) {
    FIL file;
    FRESULT fr;
    FILINFO fno;
    
    // Verifica se o arquivo existe.
    fr = f_stat(filename, &fno);
    
    // Se o arquivo não existir, cria-o e escreve o cabeçalho.
    if (fr == FR_NO_FILE) {
        fr = f_open(&file, filename, FA_WRITE | FA_CREATE_ALWAYS);
        if (fr == FR_OK) {
            const char* header = "timestamp,vrms,irms,v_pu,p_instant\n";
            UINT bytes_written;
            f_write(&file, header, strlen(header), &bytes_written);
            f_close(&file);
        }
    }

    // Abre o arquivo em modo de anexação (append) para adicionar os dados.
    fr = f_open(&file, filename, FA_WRITE | FA_OPEN_APPEND);
    if (fr != FR_OK) {
        return fr;
    }
    
    // Escreve os dados no arquivo.
    UINT bytes_written;
    fr = f_write(&file, data, strlen(data), &bytes_written);
    f_close(&file);
    return fr;
}

// Obtém e formata a data e hora do RTC.
void sd_card_get_formatted_timestamp(char* buffer, size_t size) {
    datetime_t now;
    rtc_get_datetime(&now);
    sprintf(buffer, "%04d-%02d-%02dT%02d:%02d:%02d",
            now.year, now.month, now.day, now.hour, now.min, now.sec);
}