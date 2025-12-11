# 🌾 Monitoramento Ambiental 
 
**Curso**: Residência tecnológica em sistemas embarcados  
**Autor**: Jose Rodolfo Madeiro de Lima 

---

Este repositório contém o desenvolvimento da **Monitoramneto Ambiental**,  e inteligente projetada para monitorar as condições climáticas,onde propõe o desenvolvimento de um sistema IoT (Internet das Coisas) de monitoramento ambiental utilizando a placa BitDogLab, que integra o microcontrolador RP2040 e conectividade Wi-Fi. O sistema fará a leitura de 
temperatura, umidade,pressão atmosférica e luminosidade através dos sensores AHT10, BMP280, BH1750, exibindo os dados em tempo real em uma interface web.  

O projeto se concentra no monitoramento em tempo real de parâmetros críticos, como **temperatura, umidade, exposição à luz e pressão atmosférica**. 

---

## 🛠️ Visão Geral do Projeto  

### Descrição do Problema  
Ambientes de trabalho, laboratórios e áreas de cultivo frequentemente carecem 
de monitoramento contínuo e automatizado das condições ambientais. O uso de 
medições manuais é ineficiente e impede a análise em tempo real de variáveis 
críticas. 
Dessa forma, o problema central é a ausência de um sistema IoT simples, confiável 
e conectado, capaz de coletar, processar e disponibilizar dados ambientais em 
tempo real via rede Wi-Fi, além de gerar alertas automáticos quando parâmetros 
saírem de faixas aceitáveis. 

### ⚙️ Objetivo da Solução  
Desenvolver uma estação de monitoramento ambiental e inteligente que:  
- Monitore **temperatura, umidade, luz e pressão** em tempo real.  
- Emita na **tela OLED** os dados coletados dos sensores.  
- Armazene no **cartão SD** todos os dados coletados pelos sensores durante o ano para futura análise.  
- Envie por **Wi-Fi** todos os dados coletados pelos sensores para o site ThingSpeak, onde são gerados gráficos.   

---

## ✅ Requisitos Funcionais (RF)  

| Código | Requisitos Funcionais |  
|--------|------------------------|  
| RF01   | Coletar os Dados de Temperatura. |  
| RF02   | Coletar os Dados de Pressão. |  
| RF03   | Coletar os Dados de Umidade. |  
| RF04   | Coletar os Dados de Luminosidade. |  
| RF05   | Exibir dados dos sensores, e da rede Wi-Fi em uma tela OLED. |  
| RF06   | Armazenar os dados coletados no Cartão SD em intervalos de tempo. |  
| RF07   | Mostrar o status de armazenamento, se gravou ou não. |  
| RF08   | Ao pressionar o botão A, a tela deve avançar para tela seguinte. Ao pressionar o botão B, a tela deve voltar sempre para a Tela 1 (status). |  
| RF09   | Exibir mensagens de alerta na tela OLED quando condições críticas forem detectadas. |  
| RF10   | Enviar os dados por Wi-Fi para o site ThingSpeak. |   
| RF11  | Definir o intervalo de leitura dos sensores e o modo de gravação dos dados no cartão SD. |  

---

## 📦 Lista de Materiais  

| Item | Quantidade | Descrição |
|------|------------|-----------|
| Placa BitDogLab com Raspberry Pi Pico W | 1 | Microcontrolador com periféricos integrados (OLED, botões, Wi-Fi) |
| Sensor de Temperatura e Pressão BMP280 | 1 | Sensor externo conectado |
| Sensor de Umidade e Temperatura AHT10 | 1 | Sensor externo conectado |
| Sensor de Luminosidade BH1750 | 1 | Sensor externo conectado |
| Placa para SD Card SPI | 1 | Módulo externo conectado via conector IDC direto |
| Cabos customizados XH I2C | 1 | Para conexão dos sensores externos à BitDogLab |
| Botão A | 1 | Avançar as telas do display OLED |
| Botão B | 1 | Voltar a tela de status no display OLED |

---

## 📊 Estrutura do Projeto  

### Principais Características  
✅ **Monitoramento em Tempo Real**: Sensores monitoram temperatura, umidade, luz e pressão atmosférica.  
✅ **Tela de Informações**: Tela OLED com dados dos sensores, da Wi-Fi, do cartão SD, dos Alertas.  
✅ **Registro de Dados**: Armazena informações em cartão SD ou transmite via Wi-Fi.    

### Fluxo de Trabalho do Sistema  
1. **Sensores** coletam dados ambientais.  
2. **BitDogLab (RP2040)** processa dados e salva no cartão SD.  
3. **Display OLED** exibe dados coletados.  
4. **Conectividade Wi-Fi** Manda os dados para o site ThingSpeak, para visualização de gráficos.  
---


## 📂 Estrutura do Projeto  
```  
├── src/
│ └── main.c # Programa principal
├── drivers/
│ ├── ssd1306.c # Driver OLED SSD1306
│ ├── ssd1306.h
│ ├── ssd1306_i2c.c
│ ├── ssd1306_i2c.h
│ └── ssd1306_font.h
├── hal/
│ ├── AHT10.c # Leitura AHT10
│ ├── BH1750.c # Leitura BH1750
│ ├── BMP280.c # Leitura BMP280
│ ├── buttons.c # Botoes A e B
│ ├── i2c_setup.c # Configura porta i2c
│ ├── thingspeak.c # Configura a conexão com site
│ └── display.c # Funções do display OLED
├── include/
│ ├── AHT10.h
│ ├── BH1750.h
│ ├── BMP280.h
│ ├── buttons.h
│ ├── credentials.h
│ ├── thingspeak.h
│ ├── i2c_setup.h
│ ├── FreeRTOSConfig.h
│ ├── lwipopts.h
│ └── display.h
├── lib/
│ ├── hw_config.h
│ ├── sd_card.c
│ └── sd_card.h
├── no-OS-FatFS-SD-SPI-RPi-Pico/
├── CMakeLists.txt
├── .gitignore
├── pico_sdk_import.cmake
└── README.md
```  
---


## ⚙️ Como Compilar ##

Antes de compilar, é necessário clonar os repositórios de dependências na raiz do projeto:


`git clone https://github.com/FreeRTOS/FreeRTOS-Kernel.git`

`git clone https://github.com/carlk3/no-OS-FatFS-SD-SPI-RPi-Pico`


---

## 📄 Licença  
Este projeto está licenciado sob a [MIT License](LICENSE).  

---
