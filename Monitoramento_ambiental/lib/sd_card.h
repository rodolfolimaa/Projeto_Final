#ifndef SD_CARD_H
#define SD_CARD_H

#include "ff.h"
#include "pico/types.h"

// Inicializa a comunicação com o cartão SD.
FRESULT sd_card_init();

// Grava uma string em um arquivo CSV, adicionando uma nova linha.
FRESULT sd_card_append_to_csv(const char* filename, const char* data);

// Obtém e formata a hora atual do RTC em uma string.
void sd_card_get_formatted_timestamp(char* buffer, size_t size);

// Lê o conteúdo de um arquivo e o imprime no terminal.
FRESULT sd_card_read_and_print_file(const char* filename);

#endif