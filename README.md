# ESP32 Radio Player v1.1

Projekt radia internetowego wraz z odtwarzaczem plików audio z karty SD. Jest to ukończona wersja projektu, która obsługuje również sterowanie za pomocą pilota IR w standardzie NEC. Głównym modułem jest ESP32-S3-WROOM-1-N16R8, który wraz z modułem DAC typu PCM5102A tworzy zasadniczą część projektu.

Jako wyświetlacz zastosowałem OLED 2,42" 128x64px biały sterowany z magaistrali I2C, obudowa jest produkcji GAINTA G748, a do zasilania użyłem popularnego modułu dla płytek stykowych MB102. Całość jest sterowana za pomocą czterech przełączników monostabilnych NO (dwa PS11BBK i dwa PS11BBU), dwóch enkoderów obrotowych z przyciskiem oraz pilota IR 38kHz w standarzie NEC.

Opis jak ustawić połączenie WiFi przy pierwszym uruchomieniu znajduje się [tutaj](https://github.com/tzapu/WiFiManager). W skrócie, na telefonie należy odszukać sieć "ESP32 internet radio" i się do niej podłączyć. Następnie w przeglądarce internetowej na telefonie należy wpisać adres 192.168.4.1 i wyszukać sieć WiFi, z którą będzie się normalnie potem łączył ESP32, wpisać dla niej hasło i zapisać dane. Po restarcie zasilania moduł ESP32 podłączy się do zapisanej sieci.

## Użyte biblioteki
Projekt korzysta z następujących bibliotek:
- [Arduino](https://www.arduino.cc/) – Standardowy nagłówek Arduino, który dostarcza podstawowe funkcje i definicje.
- [Audio](https://github.com/espressif/arduino-esp32/tree/master/libraries/Audio) – Biblioteka do obsługi funkcji związanych z dźwiękiem i audio.
- [SPI](https://www.arduino.cc/en/Reference/SPI) – Biblioteka do obsługi komunikacji SPI.
- [SD](https://www.arduino.cc/en/Reference/SD) – Biblioteka do obsługi kart SD.
- [FS](https://www.arduino.cc/en/Reference/FS) – Biblioteka do obsługi systemu plików.
- [U8g2lib](https://github.com/olikraus/u8g2) – Biblioteka do obsługi wyświetlaczy OLED.
- [ezButton](https://github.com/ArduinoGetStarted/ezButton) – Biblioteka do obsługi enkoderów z przyciskiem.
- [HTTPClient](https://www.arduino.cc/en/Reference/HTTPClient) – Biblioteka do wykonywania żądań HTTP.
- [EEPROM](https://www.arduino.cc/en/Reference/EEPROM) – Biblioteka do obsługi pamięci EEPROM, przechowywanie danych w pamięci nieulotnej.
- [Ticker](https://www.arduino.cc/reference/en/libraries/ticker/) – Mechanizm tickera do odświeżania timera co 1s.
- [WiFiManager](https://github.com/tzapu/WiFiManager) – Biblioteka do zarządzania konfiguracją sieci WiFi.
- [Time](https://www.arduino.cc/en/Reference/Time) – Biblioteka do obsługi funkcji związanych z czasem.
- [Wire](https://www.arduino.cc/en/Reference/Wire) – Biblioteka do obsługi magistrali I2C.

## Definicje pinów

### Piny dla I2C wyświetlacza OLED
- **SDA_PIN**: Pin 8 - Linie danych dla magistrali I2C.
- **SCL_PIN**: Pin 9 - Linie zegarowe dla magistrali I2C.

### Piny dla SPI czytnika kart SD
- **SD_CS**: Pin 47 - Pin CS (Chip Select) do komunikacji z kartą SD.
- **SD_MOSI**: Pin 48 - Pin MOSI (Master Out Slave In) dla interfejsu SPI.
- **SD_MISO**: Pin 0 - Pin MISO (Master In Slave Out) dla interfejsu SPI.
- **SD_SCLK**: Pin 45 - Pin SCK (Serial Clock) dla interfejsu SPI.

### Piny dla I2S modułu DAC (PCM5102A)
- **I2S_DOUT**: Pin 13 - Połączenie z pinem DIN na module DAC.
- **I2S_BCLK**: Pin 12 - Połączenie z pinem BCK na module DAC.
- **I2S_LRC**: Pin 14 - Połączenie z pinem LCK na module DAC.

### Piny dla enkoderów obrotowych
- **CLK_PIN1**: Pin 6 - Połączenie z CLK na prawym enkoderze.
- **DT_PIN1**: Pin 5 - Połączenie z DT na prawym enkoderze.
- **SW_PIN1**: Pin 4 - Połączenie z przyciskiem (SW) na prawym enkoderze.
- **CLK_PIN2**: Pin 10 - Połączenie z CLK na lewym enkoderze.
- **DT_PIN2**: Pin 11 - Połączenie z DT na lewym enkoderze.
- **SW_PIN2**: Pin 1 - Połączenie z przyciskiem (SW) na lewym enkoderze.

### Definicje pinów przycisków
- **BUTTON_S1**: Pin 17 - Przycisk S1.
- **BUTTON_S2**: Pin 18 - Przycisk S2.
- **BUTTON_S3**: Pin 15 - Przycisk S3.
- **BUTTON_S4**: Pin 16 - Przycisk S4.

### Inne definicje
- **MAX_STATIONS**: 99 - Maksymalna liczba stacji radiowych, które mogą być przechowywane w jednym banku.
- **STATION_NAME_LENGTH**: 42 - Długość nazwy stacji radiowej do wyświetlenia w pierwszej linii ekranu.
- **MAX_DIRECTORIES**: 128 - Maksymalna liczba katalogów w systemie plików.
- **MAX_FILES**: 128 - Maksymalna liczba plików w katalogu.

## Adresy URL do plików z listą stacji radiowych
Projekt wykorzystuje poniższe adresy URL do pobierania listy stacji radiowych:
- **[STATIONS_URL](https://raw.githubusercontent.com/sarunia/ESP32_stream/main/radio_v2_bank_01)**
- **[STATIONS_URL1](https://raw.githubusercontent.com/sarunia/ESP32_stream/main/radio_v2_bank_02)**
- **[STATIONS_URL2](https://raw.githubusercontent.com/sarunia/ESP32_stream/main/radio_v2_bank_03)**
- **[STATIONS_URL3](https://raw.githubusercontent.com/sarunia/ESP32_stream/main/radio_v2_bank_04)**
- **[STATIONS_URL4](https://raw.githubusercontent.com/sarunia/ESP32_stream/main/radio_v2_bank_05)**
- **[STATIONS_URL5](https://raw.githubusercontent.com/sarunia/ESP32_stream/main/radio_v2_bank_06)**
- **[STATIONS_URL6](https://raw.githubusercontent.com/sarunia/ESP32_stream/main/radio_v2_bank_07)**
- **[STATIONS_URL7](https://raw.githubusercontent.com/sarunia/ESP32_stream/main/radio_v2_bank_08)**
- **[STATIONS_URL8](https://raw.githubusercontent.com/sarunia/ESP32_stream/main/radio_v2_bank_09)**
- **[STATIONS_URL9](https://raw.githubusercontent.com/sarunia/ESP32_stream/main/radio_v2_bank_10)**
- **[STATIONS_URL10](https://raw.githubusercontent.com/sarunia/ESP32_stream/main/radio_v2_bank_11)**
- **[STATIONS_URL11](https://raw.githubusercontent.com/sarunia/ESP32_stream/main/radio_v2_bank_12)**
- **[STATIONS_URL12](https://raw.githubusercontent.com/sarunia/ESP32_stream/main/radio_v2_bank_13)**
- **[STATIONS_URL13](https://raw.githubusercontent.com/sarunia/ESP32_stream/main/radio_v2_bank_14)**
- **[STATIONS_URL14](https://raw.githubusercontent.com/sarunia/ESP32_stream/main/radio_v2_bank_15)**
- **[STATIONS_URL15](https://raw.githubusercontent.com/sarunia/ESP32_stream/main/radio_v2_bank_16)**

