#include "Arduino.h"              // Standardowy nagłówek Arduino, który dostarcza podstawowe funkcje i definicje
#include "Audio.h"                // Biblioteka do obsługi funkcji związanych z dźwiękiem i audio
#include "SPI.h"                  // Biblioteka do obsługi komunikacji SPI
#include "SD.h"                   // Biblioteka do obsługi kart SD
#include "FS.h"                   // Biblioteka do obsługi systemu plików
#include <U8g2lib.h>              // Biblioteka do obsługi wyświetlaczy
#include <ezButton.h>             // Biblioteka do obsługi enkodera z przyciskiem
#include <HTTPClient.h>           // Biblioteka do wykonywania żądań HTTP, umożliwia komunikację z serwerami przez protokół HTTP
#include <EEPROM.h>               // Biblioteka do obsługi pamięci EEPROM, przechowywanie danych w pamięci nieulotnej
#include <Ticker.h>               // Mechanizm tickera do odświeżania timera 1s, pomocny do cyklicznych akcji w pętli głównej
#include <WiFiManager.h>          // Biblioteka do zarządzania konfiguracją sieci WiFi, opis jak ustawić połączenie WiFi przy pierwszym uruchomieniu jest opisany tu: https://github.com/tzapu/WiFiManager
#include <Time.h>                 // Biblioteka do obsługi funkcji związanych z czasem, np. odczytu daty i godziny
#include <Wire.h>

// Definicje pinów dla I2C wyświetlacza OLED 
#define SDA_PIN 8
#define SCL_PIN 9

// Definicje pinów dla SPI czytnika kart SD
#define SD_CS         47          // Pin CS (Chip Select) do komunikacji z kartą SD
#define SD_MOSI       48          // Pin MOSI (Master Out Slave In) dla interfejsu SPI
#define SD_MISO       0           // Pin MISO (Master In Slave Out) dla interfejsu SPI
#define SD_SCLK       45          // Pin SCK (Serial Clock) dla interfejsu SPI

// Definicje pinów dla I2S modułu DAC z PCM5102A
#define I2S_DOUT      13          // Podłączenie do pinu DIN na module DAC z PCM5102A
#define I2S_BCLK      12          // Podłączenie po pinu BCK na module DAC z PCM5102A
#define I2S_LRC       14          // Podłączenie do pinu LCK na module DAC z PCM5102A

#define CLK_PIN1 6                // Podłączenie z pinu 6 do CLK na enkoderze prawym
#define DT_PIN1  5                // Podłączenie z pinu 5 do DT na enkoderze prawym
#define SW_PIN1  4                // Podłączenie z pinu 4 do SW na enkoderze prawym (przycisk)
#define CLK_PIN2 10               // Podłączenie z pinu 10 do CLK na enkoderze
#define DT_PIN2  11               // Podłączenie z pinu 11 do DT na enkoderze lewym
#define SW_PIN2  1                // Podłączenie z pinu 1 do SW na enkoderze lewym (przycisk)
#define MAX_STATIONS 99           // Maksymalna liczba stacji radiowych, które mogą być przechowywane w jednym banku
#define STATION_NAME_LENGTH 42    // Nazwa stacji wraz z bankiem i numerem stacji do wyświetlenia w pierwszej linii na ekranie
#define MAX_DIRECTORIES 128       // Maksymalna liczba katalogów
#define MAX_FILES 128             // Maksymalna liczba plików w katalogu
#define STATIONS_URL    "https://raw.githubusercontent.com/sarunia/ESP32_stream/main/radio_v2_bank_01"      // Adres URL do pliku z listą stacji radiowych
#define STATIONS_URL1   "https://raw.githubusercontent.com/sarunia/ESP32_stream/main/radio_v2_bank_02"      // Adres URL do pliku z listą stacji radiowych
#define STATIONS_URL2   "https://raw.githubusercontent.com/sarunia/ESP32_stream/main/radio_v2_bank_03"      // Adres URL do pliku z listą stacji radiowych
#define STATIONS_URL3   "https://raw.githubusercontent.com/sarunia/ESP32_stream/main/radio_v2_bank_04"      // Adres URL do pliku z listą stacji radiowych
#define STATIONS_URL4   "https://raw.githubusercontent.com/sarunia/ESP32_stream/main/radio_v2_bank_05"      // Adres URL do pliku z listą stacji radiowych
#define STATIONS_URL5   "https://raw.githubusercontent.com/sarunia/ESP32_stream/main/radio_v2_bank_06"      // Adres URL do pliku z listą stacji radiowych
#define STATIONS_URL6   "https://raw.githubusercontent.com/sarunia/ESP32_stream/main/radio_v2_bank_07"      // Adres URL do pliku z listą stacji radiowych
#define STATIONS_URL7   "https://raw.githubusercontent.com/sarunia/ESP32_stream/main/radio_v2_bank_08"      // Adres URL do pliku z listą stacji radiowych
#define STATIONS_URL8   "https://raw.githubusercontent.com/sarunia/ESP32_stream/main/radio_v2_bank_09"      // Adres URL do pliku z listą stacji radiowych
#define STATIONS_URL9   "https://raw.githubusercontent.com/sarunia/ESP32_stream/main/radio_v2_bank_10"      // Adres URL do pliku z listą stacji radiowych
#define STATIONS_URL10  "https://raw.githubusercontent.com/sarunia/ESP32_stream/main/radio_v2_bank_11"      // Adres URL do pliku z listą stacji radiowych
#define STATIONS_URL11  "https://raw.githubusercontent.com/sarunia/ESP32_stream/main/radio_v2_bank_12"      // Adres URL do pliku z listą stacji radiowych
#define STATIONS_URL12  "https://raw.githubusercontent.com/sarunia/ESP32_stream/main/radio_v2_bank_13"      // Adres URL do pliku z listą stacji radiowych
#define STATIONS_URL13  "https://raw.githubusercontent.com/sarunia/ESP32_stream/main/radio_v2_bank_14"      // Adres URL do pliku z listą stacji radiowych
#define STATIONS_URL14  "https://raw.githubusercontent.com/sarunia/ESP32_stream/main/radio_v2_bank_15"      // Adres URL do pliku z listą stacji radiowych
#define STATIONS_URL15  "https://raw.githubusercontent.com/sarunia/ESP32_stream/main/radio_v2_bank_16"      // Adres URL do pliku z listą stacji radiowych

int currentSelection = 0;         // Numer aktualnego wyboru na ekranie OLED
int firstVisibleLine = 0;         // Numer pierwszej widocznej linii na ekranie OLED
int station_nr;                   // Numer aktualnie wybranej stacji radiowej z listy
int previous_station_nr = 0;      // Numer stacji radiowej przechowywanej w buforze do przywrócenia na ekran po bezczynności
int bank_nr;                      // Numer aktualnie wybranego banku stacji z listy
int previous_bank_nr = 0;         // Numer aktualnie wybranego banku stacji z listy do przywrócenia na ekran po bezczynności
int CLK_state1;                   // Aktualny stan CLK enkodera prawego
int prev_CLK_state1;              // Poprzedni stan CLK enkodera prawego    
int CLK_state2;                   // Aktualny stan CLK enkodera lewego
int prev_CLK_state2;              // Poprzedni stan CLK enkodera lewego          
int stationsCount = 0;            // Aktualna liczba przechowywanych stacji w tablicy
int folderCount = 0;              // Licznik folderów na karcie SD
int filesCount = 0;               // Licznik plików w danym folderze na karcie SD
int fileIndex = 0;                // Numer aktualnie wybranego pliku audio ze wskazanego folderu
int previous_fileIndex = 0;       // Numer aktualnie wybranego pliku do przywrócenia na ekran po bezczynności
int folderIndex = 0;              // Numer aktualnie wybranego folderu podczas przełączenia do odtwarzania z karty SD
int previous_folderIndex = 0;     // Numer aktualnie wybranego folderu do przywrócenia na ekran po bezczynności
int volumeValue = 12;             // Wartość głośności, domyślnie ustawiona na 12
int volumeArray[100];             // Wartości głośności dla 100 stacji w każdym banku
int cycle = 0;                    // Numer cyklu do danych pogodowych wyświetlanych w trzech rzutach co 10 sekund
int maxVisibleLines = 4;          // Maksymalna liczba widocznych linii na ekranie OLED

bool encoderButton1 = false;      // Flaga określająca, czy przycisk enkodera 1 został wciśnięty
bool encoderButton2 = false;      // Flaga określająca, czy przycisk enkodera 2 został wciśnięty
bool fileEnd = false;             // Flaga sygnalizująca koniec odtwarzania pliku audio
bool displayActive = false;       // Flaga określająca, czy wyświetlacz jest aktywny
bool isPlaying = false;           // Flaga określająca, czy obecnie trwa odtwarzanie
bool mp3 = false;                 // Flaga określająca, czy aktualny plik audio jest w formacie MP3
bool flac = false;                // Flaga określająca, czy aktualny plik audio jest w formacie FLAC
bool aac = false;                 // Flaga określająca, czy aktualny plik audio jest w formacie AAC
bool vorbis = false;              // Flaga określająca, czy aktualny plik audio jest w formacie VORBIS
bool id3tag = false;              // Flaga określająca, czy plik audio posiada dane ID3
bool timeDisplay = true;          // Flaga określająca kiedy pokazać czas na wyświetlaczu, domyślnie od razu po starcie
bool listedStations = false;      // Flaga określająca, czy na ekranie jest pokazana lista stacji do wyboru
bool menuEnable = false;          // Flaga określająca, czy na ekranie można wyświetlić menu
bool bitratePresent = false;      // Flaga określająca, czy na serial terminalu pojawiła się informacja o bitrate - jako ostatnia dana spływajaca z info
bool playNextFile = false;        // Flaga określająca przejście do kolejnego odtwarzanego pliku audio
bool playPreviousFile = false;    // Flaga określająca przejście do poprzednio odtwarzanego pliku audio
bool bankChange = false;          // Flaga określająca włączenie menu wyboru banku ze stacjami radiowymi
bool folderSelection = false;     // Flaga określająca wyświetlanie listy folderów z karty SD
bool fileSelection = false;       // Flaga określająca wyświetlanie listy plików z aktualnego folderu

// Definicje flag do obsługi z pilota zdalnego sterowania z protokołu NEC 38kHz
bool IRrightArrow = false;        // Flaga określająca użycie zdalnego sterowania z pilota IR - kierunek w prawo
bool IRleftArrow = false;         // Flaga określająca użycie zdalnego sterowania z pilota IR - kierunek w lewo
bool IRupArrow = false;           // Flaga określająca użycie zdalnego sterowania z pilota IR - kierunek w górę
bool IRdownArrow = false;         // Flaga określająca użycie zdalnego sterowania z pilota IR - kierunek w dół
bool IRmenuButton = false;        // Flaga określająca użycie zdalnego sterowania z pilota IR - przycisk "MODE"
bool IRhomeButton = false;        // Flaga określająca użycie zdalnego sterowania z pilota IR - przycisk "HOME"
bool IRokButton = false;          // Flaga określająca użycie zdalnego sterowania z pilota IR - przycisk środkowy "OK"
bool IRvolumeUp = false;          // Flaga określająca użycie zdalnego sterowania z pilota IR - przycisk VOL+
bool IRvolumeDown = false;        // Flaga określająca użycie zdalnego sterowania z pilota IR - przycisk VOL-
bool IRbankUp = false;            // Flaga określająca użycie zdalnego sterowania z pilota IR - przycisk FAV+
bool IRbankDown = false;          // Flaga określająca użycie zdalnego sterowania z pilota IR - przycisk FAV-

unsigned long debounceDelay = 300;        // Czas trwania debouncingu w milisekundach
unsigned long displayTimeout = 6000;      // Czas wyświetlania komunikatu na ekranie w milisekundach
unsigned long displayStartTime = 0;       // Czas rozpoczęcia wyświetlania komunikatu
unsigned long seconds = 0;                // Licznik sekund timera
unsigned char *psramData;                 // Wskaźnik do przechowywania danych stacji w pamięci PSRAM
unsigned int PSRAM_lenght = MAX_STATIONS * (STATION_NAME_LENGTH) + MAX_STATIONS; // Deklaracja długości pamięci PSRAM
unsigned long lastCheckTime = 0;          // Zmienna do śledzenia ostatniego czasu wyświetlenia komunikatu

String directories[MAX_DIRECTORIES];      // Tablica do przechowywania nazw folderów
String files[MAX_FILES];                  // Tablica do przechowywania nazw plików
String currentDirectory = "/";            // Ścieżka bieżącego katalogu
String stationName;                       // Nazwa aktualnie wybranej stacji radiowej
String stationInfo;                       // Dodatkowe dane info stacji radiowej
String bitrateString;                     // Zmienna przechowująca informację o bitrate
String sampleRateString;                  // Zmienna przechowująca informację o sample rate
String bitsPerSampleString;               // Zmienna przechowująca informację o liczbie bitów na próbkę
String artistString;                      // Zmienna przechowująca informację o wykonawcy
String titleString;                       // Zmienna przechowująca informację o tytule utworu
String fileNameString;                    // Zmienna przechowująca informację o nazwie pliku
String folderNameString;                  // Zmienna przechowująca informację o nazwie folderu

File myFile; // Uchwyt pliku

// Inicjalizacja sprzętowego I2C dla wyświetlacza OLED
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

ezButton button1(SW_PIN1);                // Utworzenie obiektu przycisku z enkodera 1 ezButton, podłączonego do pinu 4
ezButton button2(SW_PIN2);                // Utworzenie obiektu przycisku z enkodera 1 ezButton, podłączonego do pinu 1
Audio audio;                              // Obiekt do obsługi funkcji związanych z dźwiękiem i audio
AudioBuffer audioBuffer;
Ticker timer1;                            // Timer do updateTimer co 1s
WiFiClient client;                        // Obiekt do obsługi połączenia WiFi dla klienta HTTP

char stations[MAX_STATIONS][STATION_NAME_LENGTH];   // Tablica przechowująca nazwy stacji wraz z bankiem i numerem stacji

const char* ntpServer = "pool.ntp.org";      // Adres serwera NTP używany do synchronizacji czasu
const long  gmtOffset_sec = 3600;            // Przesunięcie czasu UTC w sekundach
const int   daylightOffset_sec = 3600;       // Przesunięcie czasu letniego w sekundach, dla Polski to 1 godzina

enum MenuOption
{
  PLAY_FILES,          // Odtwarzacz plików
  INTERNET_RADIO,      // Radio internetowe
};
MenuOption currentOption = INTERNET_RADIO;  // Aktualnie wybrana opcja menu (domyślnie radio internetowe)


/*===============    Definicja portu i deklaracje zmiennych do obsługi odbiornika IR    =============*/
int recv_pin = 15;                          // Pin odbiornika IR
int bit_count = 0;                          // Licznik bitów w odebranym kodzie

volatile bool pulse_ready = false;          // Flaga sygnału gotowości
volatile bool pulse_ready9ms = false;       // Flaga sygnału gotowości
volatile bool pulse_ready_low = false;      // Flaga sygnału gotowości
bool data_start_detected = false;           // Flaga dla sygnału wstępnego

unsigned long pulse_start = 0;              // Czas początkowy impulsu
unsigned long pulse_end = 0;                // Czas końcowy impulsu
unsigned long pulse_duration = 0;           // Czas trwania impulsu
unsigned long pulse_duration_9ms = 0;       // Tylko do analizy - Czas trwania impulsu startowego 9ms 
unsigned long pulse_duration_4_5ms = 0;     // Tylko do analizy - Czas trwania impulsu startowego 4,5ms
unsigned long pulse_duration_560us = 0;     // Tylko do analizy - Czas trwania impulsu logicznego 0 (1,12ms - 0,56ms)
unsigned long pulse_duration_1690us = 0;    // Tylko do analizy - Czas trwania impulsu logicznej 1 (2,25ms - 0,56ms)
unsigned long pulse_start_low = 0;          // Czas początkowy impulsu
unsigned long pulse_end_low = 0;            // Czas końcowy impulsu
unsigned long pulse_duration_low = 0;       // Czas trwania impulsu
unsigned long runTime1 = 0;                 // Czas T1 służący do obliczenia czasu trwania impulsu sygnału IR
unsigned long runTime2 = 0;                 // Czas T2 służący do obliczenia czasu trwania impulsu sygnału IR
unsigned long ir_code = 0;                  // Zmienna do przechowywania kodu IR

const int LEAD_HIGH = 9000;         // 9 ms sygnał wysoki (początkowy)
const int LEAD_LOW = 4600;          // 4,5 ms sygnał niski (początkowy)
const int TOLERANCE = 160;          // Tolerancja (w mikrosekundach)
const int HIGH_THRESHOLD = 1690;    // Sygnał "1"
const int LOW_THRESHOLD = 600;      // Sygnał "0"


// ========= Indywidualne przypisanie przycisków, czyli adresu z komendą pilota w standardzie NEC ============ //
#define rcCmdVolumeUp     0x0028   // Przycisk VOL+
#define rcCmdVolumeDown   0x0024   // Przycisk VOL-
#define rcCmdArrowRight   0x0026   // Przycisk w prawo - następna stacja / następny plik, od razu uruchamiane przejście
#define rcCmdArrowLeft    0x0027   // Przycisk w lewo - poprzednia stacja / poprzedni plik, od razu uruchamiane przejście
#define rcCmdArrowUp      0x0030   // Przycisk w górę - lista stacji / lista plików - krok do góry na przewijanej liście
#define rcCmdArrowDown    0x0022   // Przycisk w dół - lista stacji / lista plikow - krok w dół na przewijanej liście
#define rcCmdOk           0x0025   // Przycisk OK - zatwierdzenie wybranej stacji / folderu / pliku z przewijanej listy 
#define rcCmdMode         0x0020   // Przycisk MODE - przełączanie radio internetowe / odtwarzacz plików
#define rcCmdHome         0x0023   // Przycisk HOME - uruchomienie menu systemowego ********do zrobienia w przyszłości
#define rcCmdMute         0x0029   // Przycisk MUTE - wyciszenie ********do zrobienia w przyszłości
#define rcCmdKey0         0x0012   // Przycisk "0"
#define rcCmdKey1         0x0015   // Przycisk "1"
#define rcCmdKey2         0x0014   // Przycisk "2"
#define rcCmdKey3         0x0008   // Przycisk "3"
#define rcCmdKey4         0x0011   // Przycisk "4"
#define rcCmdKey5         0x0010   // Przycisk "5"
#define rcCmdKey6         0x0009   // Przycisk "6"
#define rcCmdKey7         0x0007   // Przycisk "7"
#define rcCmdKey8         0x0006   // Przycisk "8"
#define rcCmdKey9         0x0005   // Przycisk "9"
#define rcCmdBankUp       0x0018   // Przycisk FAV+ - lista banków / lista folderów - krok w dół na przewijanej liście
#define rcCmdBankDown     0x0019   // Przycisk FAV- lista banków / lista folderów - krok do góry na przewijanej liście


// Funkcja obsługująca przerwanie (reakcja na zmianę stanu pinu)
void IRAM_ATTR pulseISR()
{
  runTime1 = esp_timer_get_time();
  if (digitalRead(recv_pin) == HIGH)
  {
    pulse_start = micros();  // Zapis początku impulsu
  }
  else
  {
    pulse_end = micros();    // Zapis końca impulsu
    pulse_ready = true;
  }

  if (digitalRead(recv_pin) == LOW)
  {
    pulse_start_low = micros();  // Zapis początku impulsu
  }
  else
  {
    pulse_end_low = micros();    // Zapis końca impulsu
    pulse_ready_low = true;
  }

  
  // ------------------------------ ANALIZA PULSÓW ----------------------------- //
  if (pulse_ready_low) // sprawdzamy czy jest stan niski przez 9ms - start ramki
  {
    pulse_duration_low = pulse_end_low - pulse_start_low;
  
    if (pulse_duration_low > (LEAD_HIGH - TOLERANCE) && pulse_duration_low < (LEAD_HIGH + TOLERANCE))
    {
      pulse_duration_9ms = pulse_duration_low; // przypisz czas trwania pulsu Low do zmiennej puls 9ms
      pulse_ready9ms = true; // flaga poprawnego wykrycia pulsu 9ms w granicach tolerancji
    }

  }
  // Sprawdzenie, czy impuls jest gotowy do analizy
  if ((pulse_ready== true) && (pulse_ready9ms = true))
  {
    pulse_ready = false;
    pulse_ready9ms = false; // kasujemy flagę wykrycia pulsu 9ms

    // Obliczenie czasu trwania impulsu
    pulse_duration = pulse_end - pulse_start;
    //Serial.println(pulse_duration); odczyt dlugosci pulsow z pilota - debug
    if (!data_start_detected)
    {
    
      // Oczekiwanie na sygnał 4,5 ms wysoki
      if (pulse_duration > (LEAD_HIGH - TOLERANCE) && pulse_duration < (LEAD_HIGH + TOLERANCE))
      {
       
        // Początek sygnału: 9 ms niski
        //Serial.println("Otrzymano początek sygnału (9 ms niski).");
        //pulse_duration_9ms = pulse_duration; 
      }
      else if (pulse_duration > (LEAD_LOW - TOLERANCE) && pulse_duration < (LEAD_LOW + TOLERANCE))
      {
        
        pulse_duration_4_5ms = pulse_duration;
        // Początek sygnału: 4,5 ms wysoki
        // Serial.println("Otrzymano początek sygnału (4,5 ms wysoki).");
        data_start_detected = true;  // Ustawienie flagi po wykryciu sygnału wstępnego
        bit_count = 0;               // Reset bit_count przed odebraniem danych
        ir_code = 0;                 // Reset kodu IR przed odebraniem danych
      }
    }
    else
    {
      // Sygnały dla bajtów (adresu ADDR, IADDR, komendy CMD, ICMD) zaczynają się po wstępnym sygnale
      if (pulse_duration > (HIGH_THRESHOLD - TOLERANCE) && pulse_duration < (HIGH_THRESHOLD + TOLERANCE))
      {
        ir_code = (ir_code << 1) | 1;  // Dodanie "1" do kodu IR
        bit_count++;
        pulse_duration_1690us = pulse_duration;
      }
      else if (pulse_duration > (LOW_THRESHOLD - TOLERANCE) && pulse_duration < (LOW_THRESHOLD + TOLERANCE))
      {
        ir_code = (ir_code << 1) | 0;  // Dodanie "0" do kodu IR
        bit_count++;
        pulse_duration_560us = pulse_duration;
      }

      // Sprawdzenie, czy otrzymano pełny 32-bitowy kod IR
      if (bit_count == 32)
      {
        // Rozbicie kodu na 4 bajty
        uint8_t ADDR = (ir_code >> 24) & 0xFF;  // Pierwszy bajt
        uint8_t IADDR = (ir_code >> 16) & 0xFF; // Drugi bajt (inwersja adresu)
        uint8_t CMD = (ir_code >> 8) & 0xFF;    // Trzeci bajt (komenda)
        uint8_t ICMD = ir_code & 0xFF;          // Czwarty bajt (inwersja komendy)

        // Sprawdzenie poprawności (inwersja) bajtów adresu i komendy
        if ((ADDR ^ IADDR) == 0xFF && (CMD ^ ICMD) == 0xFF)
        {
          data_start_detected = false;
        }
        else
        {
          ir_code = 0; 
          data_start_detected = false;       
        }

      }
    }
  }
  runTime2 = esp_timer_get_time();
}

// Odwrócenie kolejności bitów z otrzymanego ciągu z nadajnika IR
uint32_t reverse_bits(uint32_t inval, int bits)
{
  if ( bits > 0 )
  {
    bits--;
    return reverse_bits(inval >> 1, bits) | ((inval & 1) << bits);
  }
  return 0;
}
  
// Funkcja przypisująca odpowiednie flagi do użytych przyciskow z pilota zdalnego sterowania
void processIRCode()
{
  if (bit_count == 32)  // Jeśli poskładany pełny ciąg 4 bajtów
  {
    if (ir_code != 0)
    {
      detachInterrupt(recv_pin);
      Serial.print("Kod NEC OK: ");
      Serial.print(ir_code, HEX);
      ir_code = reverse_bits(ir_code, 32);   // Rotacja bitów zmiana z LSB-MSB na MSB-LSB
      Serial.print("  MSB-LSB: ");
      Serial.print(ir_code, HEX);

      uint8_t CMD = (ir_code >> 16) & 0xFF;  // Drugi bajt (inwersja adresu)
      uint8_t ADDR = ir_code & 0xFF;         // Czwarty bajt (inwersja komendy)

      Serial.print("  ADR:");
      Serial.print(ADDR, HEX);
      Serial.print(" CMD:");
      Serial.println(CMD, HEX);
      ir_code = ADDR << 8 | CMD;             // Łączymy ADDR i CMD w jedną zmienną 0xDDRCMD

      Serial.print("Czasy trwania impulsów:  9ms:");
      Serial.print(pulse_duration_9ms);
      Serial.print("  4.5ms:");
      Serial.print(pulse_duration_4_5ms);
      Serial.print("  1690us:");
      Serial.print(pulse_duration_1690us);
      Serial.print("  560us:");
      Serial.println(pulse_duration_560us);

      attachInterrupt(digitalPinToInterrupt(recv_pin), pulseISR, CHANGE);

      Serial.print("Kontrola stosu: ");

      // Pobranie minimalnej liczby wolnych słów stosu (każde słowo to 4 bajty)
      uint32_t stackSizeInBytes = uxTaskGetStackHighWaterMark(NULL) * 4;

      // Sprawdzenie, czy liczba bajtów jest większa niż 1024, aby przeliczyć na kilobajty z dokładnością do 2 miejsc po przecinku
      if (stackSizeInBytes > 1024)
      {
        float stackSizeInKB = stackSizeInBytes / 1024.0;  // Przeliczenie na kilobajty (float dla precyzji)
        Serial.print(stackSizeInKB, 2);  // Wydruk z dokładnością do 2 miejsc po przecinku
        Serial.println(" KB");
      }
      else
      {
        Serial.print(stackSizeInBytes);  // Wydruk w bajtach
        Serial.println(" bajtów");
      }


      // Rozpoznawanie przycisków na podstawie kodu
      if (ir_code == rcCmdArrowRight)        // Przycisk w prawo
      { 
          IRrightArrow = true;
      } 
      else if (ir_code == rcCmdArrowLeft)    // Przycisk w lewo
      {  
          IRleftArrow = true;
      }
      else if (ir_code == rcCmdArrowUp)      // Przycisk w górę
      {  
          IRupArrow = true;
      }
      else if (ir_code == rcCmdArrowDown)    // Przycisk w dół
      {  
          IRdownArrow = true;
      }
      else if (ir_code == rcCmdMode)         // Przycisk MODE
      {  
          IRmenuButton = true;
      }
      else if (ir_code == rcCmdHome)         // Przycisk MOME
      {  
          IRhomeButton = true;
      }
      else if (ir_code == rcCmdOk)           // Przycisk OK
      {  
          IRokButton = true;
      }
      else if (ir_code == rcCmdVolumeUp)     // Przycisk VOL+
      {  
          IRvolumeUp = true;
      }
      else if (ir_code == rcCmdVolumeDown)   // Przycisk VOL-
      {  
          IRvolumeDown = true;
      }
      else if (ir_code == rcCmdBankUp)       // Przycisk FAV+
      {  
          IRbankUp = true;
      }
      else if (ir_code == rcCmdBankDown)     // Przycisk FAV-
      {  
          IRbankDown = true;
      }
      else
      {
          Serial.println("Inny przycisk");
      }

      ir_code = 0;
      bit_count = 0;
    }
  }
}


// Tablica z dodanymi polskimi znakami diakrytycznymi
const uint8_t spleen6x12PL[2954] U8G2_FONT_SECTION("spleen6x12PL") =
  "\340\1\3\2\3\4\1\3\4\6\14\0\375\10\376\11\377\1\225\3]\13f \7\346\361\363\237\0!\12"
  "\346\361#i\357`\316\0\42\14\346\361\3I\226dI\316/\0#\21\346\361\303I\64HI\226dI"
  "\64HIN\6$\22\346q\205CRK\302\61\311\222,I\206\60\247\0%\15\346\361cQK\32\246"
  "I\324\316\2&\17\346\361#Z\324f\213\22-Zr\42\0'\11\346\361#i\235\237\0(\13\346\361"
  "ia\332s\254\303\0)\12\346\361\310\325\36\63\235\2*\15\346\361S\243L\32&-\312\31\1+\13"
  "\346\361\223\323l\320\322\234\31,\12\346\361\363)\15s\22\0-\11\346\361s\32t\236\0.\10\346\361"
  "\363K\316\0/\15\346q\246a\32\246a\32\246\71\15\60\21\346\361\3S\226DJ\213\224dI\26\355"
  "d\0\61\12\346\361#\241\332\343N\6\62\16\346\361\3S\226\226\246\64\35t*\0\63\16\346\361\3S"
  "\226fr\232d\321N\6\64\14\346q\247\245\236\6\61\315\311\0\65\16\346q\17J\232\16qZ\31r"
  "\62\0\66\20\346\361\3S\232\16Q\226dI\26\355d\0\67\13\346q\17J\226\206\325v\6\70\20\346"
  "\361\3S\226d\321\224%Y\222E;\31\71\17\346\361\3S\226dI\26\15ii'\3:\11\346\361"
  "\263\346L\71\3;\13\346\361\263\346\264\64\314I\0<\12\346\361cak\334N\5=\13\346\361\263\15"
  ":\60\350\334\0>\12\346\361\3qk\330\316\2\77\14\346\361\3S\226\206\325\34\314\31@\21\346\361\3"
  "S\226dI\262$K\262\304CN\5A\22\346\361\3S\226dI\226\14J\226dI\226S\1B\22"
  "\346q\17Q\226d\311\20eI\226d\311\220\223\1C\14\346\361\3C\222\366<\344T\0D\22\346q"
  "\17Q\226dI\226dI\226d\311\220\223\1E\16\346\361\3C\222\246C\224\226\207\234\12F\15\346\361"
  "\3C\222\246C\224\266\63\1G\21\346\361\3C\222V\226,\311\222,\32r*\0H\22\346qgI"
  "\226d\311\240dI\226dI\226S\1I\12\346\361\3c\332\343N\6J\12\346\361\3c\332\233\316\2"
  "K\21\346qgI\226D\321\26\325\222,\311r*\0L\12\346q\247}\36r*\0M\20\346qg"
  "\211eP\272%Y\222%YN\5N\20\346qg\211\224HI\77)\221\222\345T\0O\21\346\361\3"
  "S\226dI\226dI\226d\321N\6P\17\346q\17Q\226dI\226\14QZg\2Q\22\346\361\3"
  "S\226dI\226dI\226d\321\252\303\0R\22\346q\17Q\226dI\226\14Q\226dI\226S\1S"
  "\16\346\361\3C\222\306sZ\31r\62\0T\11\346q\17Z\332w\6U\22\346qgI\226dI\226"
  "dI\226d\321\220S\1V\20\346qgI\226dI\226dI\26m;\31W\21\346qgI\226d"
  "I\226\264\14\212%\313\251\0X\21\346qgI\26%a%\312\222,\311r*\0Y\20\346qgI"
  "\226dI\26\15ie\310\311\0Z\14\346q\17j\330\65\35t*\0[\13\346\361\14Q\332\257C\16"
  "\3\134\15\346q\244q\32\247q\32\247\71\14]\12\346\361\14i\177\32r\30^\12\346\361#a\22e"
  "\71\77_\11\346\361\363\353\240\303\0`\11\346\361\3q\235_\0a\16\346\361S\347hH\262$\213\206"
  "\234\12b\20\346q\247\351\20eI\226dI\226\14\71\31c\14\346\361S\207$m\36r*\0d\21"
  "\346\361ci\64$Y\222%Y\222ECN\5e\17\346\361S\207$K\262dP\342!\247\2f\14"
  "\346\361#S\32\16Y\332\316\2g\21\346\361S\207$K\262$K\262hN\206\34\1h\20\346q\247"
  "\351\20eI\226dI\226d\71\25i\13\346\361#\71\246v\325\311\0j\13\346\361C\71\230\366\246S"
  "\0k\16\346q\247\245J&&YT\313\251\0l\12\346\361\3i\237u\62\0m\15\346\361\23\207("
  "\351\337\222,\247\2n\20\346\361\23\207(K\262$K\262$\313\251\0o\16\346\361S\247,\311\222,"
  "\311\242\235\14p\21\346\361\23\207(K\262$K\262d\210\322*\0q\20\346\361S\207$K\262$K"
  "\262hH[\0r\14\346\361S\207$K\322v&\0s\15\346\361S\207$\236\323d\310\311\0t\13"
  "\346\361\3i\70\246\315:\31u\20\346\361\23\263$K\262$K\262h\310\251\0v\16\346\361\23\263$"
  "K\262$\213\222\60gw\17\346\361\23\263$KZ\6\305\222\345T\0x\16\346\361\23\263$\213\266)"
  "K\262\234\12y\22\346\361\23\263$K\262$K\262hH\223!G\0z\14\346\361\23\7\65l\34t"
  "*\0{\14\346\361iiM\224\323\262\16\3|\10\346q\245\375;\5}\14\346\361\310iY\324\322\232"
  "N\1~\12\346\361s\213\222D\347\10\177\7\346\361\363\237\0\200\6\341\311\243\0\201\6\341\311\243\0\202"
  "\6\341\311\243\0\203\6\341\311\243\0\204\6\341\311\243\0\205\6\341\311\243\0\206\6\341\311\243\0\207\6\341"
  "\311\243\0\210\6\341\311\243\0\211\6\341\311\243\0\212\6\341\311\243\0\213\6\341\311\243\0\214\16\346\361e"
  "C\222\306sZ\31r\62\0\215\6\341\311\243\0\216\6\341\311\243\0\217\14\346qe\203T\354\232\16:"
  "\25\220\6\341\311\243\0\221\6\341\311\243\0\222\6\341\311\243\0\223\6\341\311\243\0\224\6\341\311\243\0\225"
  "\6\341\311\243\0\226\6\341\311\243\0\227\16\346\361eC\222\306sZ\31r\62\0\230\6\341\311\243\0\231"
  "\6\341\311\243\0\232\6\341\311\243\0\233\6\341\311\243\0\234\16\346\361\205\71\66$\361\234&CN\6\235"
  "\6\341\311\243\0\236\6\341\311\243\0\237\15\346\361\205\71\64\250a\343\240S\1\240\7\346\361\363\237\0\241"
  "\23\346\361\3S\226dI\226\14J\226dI\26\306\71\0\242\21\346\361\23\302!\251%Y\222%\341\220"
  "\345\24\0\243\14\346q\247-\231\230\306CN\5\244\22\346\361\3S\226dI\226\14J\226dI\26\346"
  "\4\245\22\346\361\3S\226dI\226\14J\226dI\26\346\4\246\16\346\361eC\222\306sZ\31r\62"
  "\0\247\17\346\361#Z\224\245Z\324\233\232E\231\4\250\11\346\361\3I\316\237\1\251\21\346\361\3C\22"
  "J\211\22)\221bL\206\234\12\252\15\346\361#r\66\325vd\310\31\1\253\17\346\361\223\243$J\242"
  "\266(\213r\42\0\254\14\346qe\203T\354\232\16:\25\255\10\346\361s\333y\3\256\21\346\361\3C"
  "\22*\226d\261$c\62\344T\0\257\14\346qe\203\32vM\7\235\12\260\12\346\361#Z\324\246\363"
  "\11\261\20\346\361S\347hH\262$\213\206\64\314\21\0\262\14\346\361#Z\224\206\305!\347\6\263\13\346"
  "\361\3i\252\251\315:\31\264\11\346\361Ca\235\337\0\265\14\346\361\23\243\376i\251\346 \0\266\16\346"
  "\361\205\71\66$\361\234&CN\6\267\10\346\361s\314y\4\270\11\346\361\363\207\64\14\1\271\20\346\361"
  "S\347hH\262$\213\206\64\314\21\0\272\15\346\361#Z\324\233\16\15\71#\0\273\17\346\361\23\243,"
  "\312\242\226(\211r\62\0\274\15\346\361\205\71\64\250a\343\240S\1\275\17\346\361\204j-\211\302\26\245"
  "\24\26\207\0\276\21\346\361hQ\30'\222\64\206ZR\33\302\64\1\277\15\346\361#\71\64\250a\343\240"
  "S\1\300\21\346\361\304\341\224%Y\62(Y\222%YN\5\301\21\346\361\205\341\224%Y\62(Y\222"
  "%YN\5\302\22\346q\205I\66eI\226\14J\226dI\226S\1\303\23\346\361DI\242MY\222"
  "%\203\222%Y\222\345T\0\304\20\346\361S\347hH\262$\213\206\64\314\21\0\305\16\346\361eC\222"
  "\306sZ\31r\62\0\306\14\346\361eC\222\366<\344T\0\307\15\346\361\3C\222\366<di\30\2"
  "\310\17\346\361\304\341\220\244\351\20\245\361\220S\1\311\17\346\361\205\341\220\244\351\20\245\361\220S\1\312\20"
  "\346\361\3C\222\246C\224\226\207\64\314\21\0\313\17\346\361\324\241!I\323!J\343!\247\2\314\13\346"
  "\361\304\341\230v\334\311\0\315\13\346\361\205\341\230v\334\311\0\316\14\346q\205I\66\246\35w\62\0\317"
  "\13\346\361\324\241\61\355\270\223\1\320\15\346\361\3[\324\262D}\332\311\0\321\20\346\361EIV\221\22"
  ")\351'%\322\251\0\322\20\346\361\304\341\224%Y\222%Y\222E;\31\323\20\346\361\205\341\224%Y"
  "\222%Y\222E;\31\324\21\346q\205I\66eI\226dI\226d\321N\6\325\22\346\361DI\242M"
  "Y\222%Y\222%Y\264\223\1\326\21\346\361\324\241)K\262$K\262$\213v\62\0\327\14\346\361S"
  "\243L\324\242\234\33\0\330\20\346qFS\226DJ_\244$\213\246\234\6\331\21\346\361\304Y%K\262"
  "$K\262$\213\206\234\12\332\21\346\361\205Y%K\262$K\262$\213\206\234\12\333\23\346q\205I\224"
  "%Y\222%Y\222%Y\64\344T\0\334\22\346\361\324\221,\311\222,\311\222,\311\242!\247\2\335\17"
  "\346\361\205Y%K\262hH+CN\6\336\21\346\361\243\351\20eI\226dI\226\14QN\3\337\17"
  "\346\361\3Z\324%\213j\211\224$:\31\340\20\346q\305\71\64GC\222%Y\64\344T\0\341\20\346"
  "\361\205\71\66GC\222%Y\64\344T\0\342\11\346\361Ca\235\337\0\343\21\346\361DI\242Cs\64"
  "$Y\222ECN\5\344\20\346\361\3I\16\315\321\220dI\26\15\71\25\345\20\346q\205I\30\316\321"
  "\220dI\26\15\71\25\346\15\346\361Ca\70$i\363\220S\1\347\15\346\361S\207$m\36\262\64\14"
  "\1\350\20\346q\305\71\64$Y\222%\203\22\17\71\25\351\20\346\361\205\71\66$Y\222%\203\22\17\71"
  "\25\352\20\346\361S\207$K\262dP\342!\254C\0\353\21\346\361\3I\16\15I\226d\311\240\304C"
  "N\5\354\13\346q\305\71\244v\325\311\0\355\13\346\361\205\71\246v\325\311\0\356\14\346q\205I\16\251"
  "]u\62\0\357\14\346\361\3I\16\251]u\62\0\360\21\346q$a%\234\262$K\262$\213v\62"
  "\0\361\21\346\361\205\71\64DY\222%Y\222%YN\5\362\20\346q\305\71\64eI\226dI\26\355"
  "d\0\363\20\346\361\205\71\66eI\226dI\26\355d\0\364\20\346q\205I\16MY\222%Y\222E"
  ";\31\365\21\346\361c\222\222HI\226dI\66\15\221N\4\366\20\346\361\3I\16MY\222%Y\222"
  "E;\31\367\13\346\361\223sh\320\241\234\31\370\17\346\361\223\242)RZ\244$\213\246\234\6\371\21\346"
  "q\305\71\222%Y\222%Y\222ECN\5\372\21\346\361\205\71\224%Y\222%Y\222ECN\5\373"
  "\22\346q\205I\216dI\226dI\226d\321\220S\1\374\22\346\361\3I\216dI\226dI\226d\321"
  "\220S\1\375\23\346\361\205\71\224%Y\222%Y\222ECZ\31\42\0\376\22\346q\247\351\20eI\226"
  "dI\226\14Q\232\203\0\377\23\346\361\3I\216dI\226dI\226d\321\220V\206\10\0\0\0\4\377"
  "\377\0";

// Funkcja sprawdza, czy plik jest plikiem audio na podstawie jego rozszerzenia
bool isAudioFile(const char *fileNameString)
{
  // Znajdź ostatnie wystąpienie kropki w nazwie pliku
  const char *ext = strrchr(fileNameString, '.');
  
  // Jeśli nie znaleziono kropki lub nie ma rozszerzenia, zwróć false
  if (!ext)
  {
    return false;
  }

  // Sprawdź rozszerzenie, ignorując wielkość liter
  return (strcasecmp(ext, ".mp3") == 0 ||  // MP3: popularny format stratny, szeroko stosowany
          strcasecmp(ext, ".wav") == 0 ||  // WAV: nieskompresowany format bezstratny, często używany w nagraniach
          strcasecmp(ext, ".flac") == 0 || // FLAC: bezstratna kompresja, wysoka jakość dźwięku
          strcasecmp(ext, ".aac") == 0 ||  // AAC: lepsza kompresja niż MP3, używany w iTunes
          strcasecmp(ext, ".wma") == 0 ||  // WMA: format stratny stworzony przez Microsoft
          strcasecmp(ext, ".ogg") == 0 ||  // OGG: otwarty format stratny, często używany z kodekiem Vorbis
          strcasecmp(ext, ".m4a") == 0 ||  // M4A: używany przez Apple, podobny do AAC
          strcasecmp(ext, ".aiff") == 0 || // AIFF: nieskompresowany format bezstratny, używany głównie na komputerach Apple
          strcasecmp(ext, ".alac") == 0);  // ALAC: bezstratny format od Apple, podobny do FLAC
}


// Funkcja do obsługi przycisków enkoderów, odpowiedzialna za debouncing i wykrywanie długiego naciśnięcia
void handleButtons()  
{
  static unsigned long buttonPressTime1 = 0;  // Zmienna do przechowywania czasu naciśnięcia przycisku enkodera 1
  static bool isButton1Pressed = false;       // Flaga do śledzenia, czy przycisk enkodera 1 jest wciśnięty
  static bool action1Taken = false;           // Flaga do śledzenia, czy akcja dla enkodera 1 została wykonana

  static unsigned long buttonPressTime2 = 0;  // Zmienna do przechowywania czasu naciśnięcia przycisku enkodera 2
  static bool isButton2Pressed = false;       // Flaga do śledzenia, czy przycisk enkodera 2 jest wciśnięty
  static bool action2Taken = false;           // Flaga do śledzenia, czy akcja dla enkodera 2 została wykonana
  
  static unsigned long lastPressTime = 0;     // Zmienna do kontrolowania debouncingu (ostatni czas naciśnięcia)
  const unsigned long debounceDelay = 50;     // Opóźnienie debouncingu

  // ===== Obsługa przycisku enkodera 1 =====
  int reading1 = digitalRead(SW_PIN1);

  // Debouncing dla przycisku enkodera 1
  if (reading1 == LOW)  // Przycisk jest wciśnięty (stan niski)
  {
    if (millis() - lastPressTime > debounceDelay)
    {
      lastPressTime = millis();  // Aktualizujemy czas ostatniego naciśnięcia

      // Sprawdzamy, czy przycisk był wciśnięty przez 3 sekundy
      if (!isButton1Pressed)
      {
        buttonPressTime1 = millis();  // Ustawiamy czas naciśnięcia
        isButton1Pressed = true;      // Ustawiamy flagę, że przycisk jest wciśnięty
        action1Taken = false;         // Resetujemy flagę akcji dla enkodera 1
      }

      // Jeśli przycisk jest wciśnięty przez co najmniej 3 sekundy i akcja jeszcze nie była wykonana
      if (millis() - buttonPressTime1 >= 3000 && !action1Taken)
      {
        timeDisplay = false;
        displayMenu();
        menuEnable = true;
        displayActive = true;
        displayStartTime = millis();

        Serial.println("Wyświetlenie menu po przytrzymaniu przycisku enkodera 1");

        // Ustawiamy flagę, że akcja została wykonana
        action1Taken = true;
      }
    }
  }
  else
  {
    isButton1Pressed = false;  // Resetujemy flagę naciśnięcia przycisku enkodera 1
    action1Taken = false;      // Resetujemy flagę akcji dla enkodera 1
  }

  // ===== Obsługa przycisku enkodera 2 =====
  int reading2 = digitalRead(SW_PIN2);

  // Debouncing dla przycisku enkodera 2
  if (reading2 == LOW)  // Przycisk jest wciśnięty (stan niski)
  {
    if (millis() - lastPressTime > debounceDelay)
    {
      lastPressTime = millis();  // Aktualizujemy czas ostatniego naciśnięcia

      // Sprawdzamy, czy przycisk był wciśnięty przez 3 sekundy
      if (!isButton2Pressed)
      {
        buttonPressTime2 = millis();  // Ustawiamy czas naciśnięcia
        isButton2Pressed = true;      // Ustawiamy flagę, że przycisk jest wciśnięty
        action2Taken = false;         // Resetujemy flagę akcji dla enkodera 2
      }

      // Jeśli przycisk jest wciśnięty przez co najmniej 3 sekundy i akcja jeszcze nie była wykonana
      if (millis() - buttonPressTime2 >= 3000 && !action2Taken)
      {
        timeDisplay = false;
        bankChange = true;           // Ustawienie listy banków do przewijania i wyboru

        Serial.println("Wyświetlenie listy banków");
        u8g2.clearBuffer();	
        u8g2.setFont(spleen6x12PL);
        u8g2.drawStr(20, 40, "WYBIERZ  BANK");
        u8g2.sendBuffer();

        // Ustawiamy flagę, że akcja została wykonana
        action2Taken = true;
      }
    }
  }
  else
  {
    isButton2Pressed = false;  // Resetujemy flagę naciśnięcia przycisku enkodera 2
    action2Taken = false;      // Resetujemy flagę akcji dla enkodera 2
  }
}



//Funkcja odpowiedzialna za zapisywanie informacji o stacji do pamięci EEPROM.
void saveStationToEEPROM(const char* station)
{   
  // Sprawdź, czy istnieje jeszcze miejsce na kolejną stację w pamięci EEPROM.
  if (stationsCount < MAX_STATIONS)
  {
    int length = strlen(station);

    // Sprawdź, czy długość linku nie przekracza ustalonego maksimum.
    if (length <= STATION_NAME_LENGTH)
    {
      // Zapisz długość linku jako pierwszy bajt.
      EEPROM.write(stationsCount * (STATION_NAME_LENGTH + 1), length);

      // Zapisz link jako kolejne bajty w pamięci EEPROM.
      for (int i = 0; i < length; i++)
      {
        EEPROM.write(stationsCount * (STATION_NAME_LENGTH + 1) + 1 + i, station[i]);
      }

      // Potwierdź zapis do pamięci EEPROM - czasowe wyłączenie na testy
      //EEPROM.commit();

      // Wydrukuj informację o zapisanej stacji na Serialu.
      Serial.println(String(stationsCount + 1) + "   " + String(station)); // Drukowanie na serialu od nr 1 jak w banku na serwerze

      // Zwiększ licznik zapisanych stacji.
      stationsCount++;
    } 
    else
    {
      // Informacja o błędzie w przypadku zbyt długiego linku do stacji.
      Serial.println("Błąd: Link do stacji jest zbyt długi");
    }
  }
  else
  {
    // Informacja o błędzie w przypadku osiągnięcia maksymalnej liczby stacji.
    Serial.println("Błąd: Osiągnięto maksymalną liczbę zapisanych stacji");
  }
}

// Funkcja odpowiedzialna za zmianę aktualnie wybranej stacji radiowej.
void changeStation()
{
  mp3 = flac = aac = vorbis = false;
  bitratePresent = false;

  stationInfo.remove(0);  // Usunięcie wszystkich znaków z obiektu stationInfo

  // Tworzymy nazwę pliku banku
  String fileNameWithBank = String("/bank") + (bank_nr < 10 ? "0" : "") + String(bank_nr) + ".txt";

  // Sprawdzamy, czy plik istnieje
  if (!SD.exists(fileNameWithBank))
  {
    Serial.println("Błąd: Plik banku nie istnieje.");
    return;
  }

  // Otwieramy plik w trybie do odczytu
  File bankFile = SD.open(fileNameWithBank, FILE_READ);
  if (!bankFile)
  {
    Serial.println("Błąd: Nie można otworzyć pliku banku.");
    return;
  }

  // Przechodzimy do odpowiedniego wiersza pliku
  int currentLine = 0;
  String stationUrl = "";
  
  while (bankFile.available())
  {
    String line = bankFile.readStringUntil('\n');
    currentLine++;

    if (currentLine == station_nr)
    {
      // Wyciągnij pierwsze 42 znaki i przypisz do stationName
      stationName = line.substring(0, 42);  // Skopiuj pierwsze 42 znaki z linii
      Serial.print("Nazwa stacji: ");
      Serial.println(stationName);

      // Znajdź część URL w linii
      int urlStart = line.indexOf("http");  // Szukamy miejsca, gdzie zaczyna się URL
      if (urlStart != -1)
      {
        stationUrl = line.substring(urlStart);  // Wyciągamy URL od "http"
        stationUrl.trim();  // Usuwamy białe znaki na początku i końcu
      }
      break;
    }
  }

  bankFile.close();  // Zamykamy plik po odczycie

  // Sprawdzamy, czy znaleziono stację
  if (stationUrl.isEmpty())
  {
    Serial.println("Błąd: Nie znaleziono stacji dla podanego numeru.");
    Serial.print("Numer stacji:");
    Serial.println(station_nr);
    Serial.print("Numer banku:");
    Serial.println(bank_nr);
    return;
  }

  // Weryfikacja, czy w linku znajduje się "http" lub "https"
  if (stationUrl.startsWith("http://") || stationUrl.startsWith("https://")) 
  {
    // Wydrukuj nazwę stacji i link na serialu
    Serial.print("Aktualnie wybrana stacja: ");
    Serial.println(station_nr);
    Serial.print("Link do stacji: ");
    Serial.println(stationUrl);

    // Połącz z daną stacją
    audio.connecttohost(stationUrl.c_str());
    // Wczytujemy ustawienia głośności dla banku
    loadVolumeSettings(station_nr, bank_nr);
    previous_station_nr = station_nr;
    previous_bank_nr = bank_nr;
    saveStationOnSD();
  } 
  else 
  {
    Serial.println("Błąd: link stacji nie zawiera 'http' lub 'https'");
    Serial.println("Odczytany URL: " + stationUrl);
  }
}

// Funkcja do pobierania listy stacji radiowych z serwera i zapisania ich w wybranym banku na karcie SD
void fetchStationsFromServer()
{
  previous_bank_nr = bank_nr;
  
  // Utwórz obiekt klienta HTTP
  HTTPClient http;

  // URL stacji dla danego banku
  String url;

  // Wybierz URL na podstawie bank_nr za pomocą switch
  switch (bank_nr)
  {
    case 1:
      url = STATIONS_URL;
      break;
    case 2:
      url = STATIONS_URL1;
      break;
    case 3:
      url = STATIONS_URL2;
      break;
    case 4:
      url = STATIONS_URL3;
      break;
    case 5:
      url = STATIONS_URL4;
      break;
    case 6:
      url = STATIONS_URL5;
      break;
    case 7:
      url = STATIONS_URL6;
      break;
    case 8:
      url = STATIONS_URL7;
      break;
    case 9:
      url = STATIONS_URL8;
      break;
    case 10:
      url = STATIONS_URL9;
      break;
    case 11:
      url = STATIONS_URL10;
      break;
    case 12:
      url = STATIONS_URL11;
      break;
    case 13:
      url = STATIONS_URL12;
      break;
    case 14:
      url = STATIONS_URL13;
      break;
    case 15:
      url = STATIONS_URL14;
      break;
    case 16:
      url = STATIONS_URL15;
      break;
    default:
      Serial.println("Nieprawidłowy numer banku");
      return;
  }

  // Tworzenie nazwy pliku dla danego banku
  String fileNameWithBank = String("/bank") + (bank_nr < 10 ? "0" : "") + String(bank_nr) + ".txt";
  
  // Sprawdzenie, czy plik istnieje
  if (SD.exists(fileNameWithBank))
  {
    Serial.println("Plik banku " + fileNameWithBank + " już istnieje.");
  }
  else
  {
    // Próba utworzenia pliku, jeśli nie istnieje
    File bankFile = SD.open(fileNameWithBank, FILE_WRITE);
    
    if (bankFile)
    {
      Serial.println("Utworzono plik banku: " + fileNameWithBank);
      bankFile.close();  // Zamykanie pliku po utworzeniu
    }
    else
    {
      Serial.println("Błąd: Nie można utworzyć pliku banku: " + fileNameWithBank);
      return;  // Przerwij dalsze działanie, jeśli nie udało się utworzyć pliku
    }
  }

  // Inicjalizuj żądanie HTTP do podanego adresu URL
  http.begin(url);

  // Wykonaj żądanie GET i zapisz kod odpowiedzi HTTP
  int httpCode = http.GET();

  // Wydrukuj dodatkowe informacje diagnostyczne
  Serial.print("Kod odpowiedzi HTTP: ");
  Serial.println(httpCode);

  // Sprawdź, czy żądanie było udane (HTTP_CODE_OK)
  if (httpCode == HTTP_CODE_OK)
  {
    // Pobierz zawartość odpowiedzi HTTP w postaci tekstu
    String payload = http.getString();
    //Serial.println("Stacje pobrane z serwera:");
    //Serial.println(payload);  // Wyświetlenie pobranych danych (payload)

    // Otwórz plik w trybie zapisu, aby zapisać payload
    File bankFile = SD.open(fileNameWithBank, FILE_WRITE);
    if (bankFile)
    {
      bankFile.println(payload);  // Zapisz dane do pliku
      bankFile.close();  // Zamknij plik po zapisaniu
      Serial.println("Dane zapisane do pliku: " + fileNameWithBank);
    }
    else
    {
      Serial.println("Błąd: Nie można otworzyć pliku do zapisu: " + fileNameWithBank);
    }

    // Zapisz każdą niepustą stację do pamięci EEPROM z indeksem
    int startIndex = 0;
    int endIndex;
    stationsCount = 0;

    // Przeszukuj otrzymaną zawartość w poszukiwaniu nowych linii
    while ((endIndex = payload.indexOf('\n', startIndex)) != -1 && stationsCount < MAX_STATIONS)
    {
      // Wyodrębnij pojedynczą stację z otrzymanego tekstu
      String station = payload.substring(startIndex, endIndex);
      
      // Sprawdź, czy stacja nie jest pusta, a następnie przetwórz i zapisz
      if (!station.isEmpty())
      {
        // Zapisz stację do pliku na karcie SD
        sanitizeAndSaveStation(station.c_str());
      }
      
      // Przesuń indeks początkowy do kolejnej linii
      startIndex = endIndex + 1;
    }
  }
  else
  {
    // W przypadku nieudanego żądania wydrukuj informację o błędzie z kodem HTTP
    Serial.printf("Błąd podczas pobierania stacji. Kod HTTP: %d\n", httpCode);
  }

  // Zakończ połączenie HTTP
  http.end();
}

// Funkcja przetwarza i zapisuje stację do pamięci EEPROM
void sanitizeAndSaveStation(const char* station)
{
  // Bufor na przetworzoną stację - o jeden znak dłuższy niż maksymalna długość linku
  char sanitizedStation[STATION_NAME_LENGTH + 1];
  
  // Indeks pomocniczy dla przetwarzania
  int j = 0;

  // Przeglądaj każdy znak stacji i sprawdź czy jest to drukowalny znak ASCII
  for (int i = 0; i < STATION_NAME_LENGTH && station[i] != '\0'; i++)
  {
    // Sprawdź, czy znak jest drukowalnym znakiem ASCII
    if (isprint(station[i]))
    {
      // Jeśli tak, dodaj do przetworzonej stacji
      sanitizedStation[j++] = station[i];
    }
  }

  // Dodaj znak końca ciągu do przetworzonej stacji
  sanitizedStation[j] = '\0';

  // Zapisz przetworzoną stację do pamięci EEPROM
  saveStationToEEPROM(sanitizedStation);
}

void audio_info(const char *info)
{
  Serial.print("info        ");
  Serial.println(info);
  // Znajdź pozycję "BitRate:" w tekście
  int bitrateIndex = String(info).indexOf("BitRate:");
  if (bitrateIndex != -1)
  {
    if (bitratePresent == false)
    {
      // Przytnij tekst od pozycji "BitRate:" do końca linii
      bitrateString = String(info).substring(bitrateIndex + 8, String(info).indexOf('\n', bitrateIndex));
    }
    if (currentOption == PLAY_FILES)
    {
      displayPlayer();
    }
    if (currentOption == INTERNET_RADIO)
    {
      displayRadio();
    }
  }

  // Znajdź pozycję "SampleRate:" w tekście
  int sampleRateIndex = String(info).indexOf("SampleRate:");
  if (sampleRateIndex != -1)
  {
    // Przytnij tekst od pozycji "SampleRate:" do końca linii
    sampleRateString = String(info).substring(sampleRateIndex + 11, String(info).indexOf('\n', sampleRateIndex));
  }

  // Znajdź pozycję "BitsPerSample:" w tekście
  int bitsPerSampleIndex = String(info).indexOf("BitsPerSample:");
  if (bitsPerSampleIndex != -1)
  {
    // Przytnij tekst od pozycji "BitsPerSample:" do końca linii
    bitsPerSampleString = String(info).substring(bitsPerSampleIndex + 15, String(info).indexOf('\n', bitsPerSampleIndex));
  }

  // Znajdź pozycję "skip metadata" w tekście
  int metadata = String(info).indexOf("skip metadata");
  if (metadata != -1)
  {
    id3tag = false;
    Serial.println("Brak ID3 - nazwa pliku: " + fileNameString);
    if (fileNameString.length() > 84)
    {
      fileNameString = String(fileNameString).substring(0, 84); // Przytnij string do 84 znaków, aby zmieścić w 2 liniach z dalszym podziałem na pełne wyrazy
    }
  }

  if (String(info).indexOf("MP3Decoder") != -1)
  {
    mp3 = true;
    flac = false;
    aac = false;
    vorbis = false;
  }

  if (String(info).indexOf("FLACDecoder") != -1)
  {
    flac = true;
    mp3 = false;
    aac = false;
    vorbis = false;
  }

  if (String(info).indexOf("AACDecoder") != -1)
  {
    aac = true;
    flac = false;
    mp3 = false;
    vorbis = false;
  }

  if (String(info).indexOf("VORBISDecoder") != -1)
  {
    vorbis = true;
    aac = false;
    flac = false;
    mp3 = false;
  }
}

void audio_id3data(const char *info)
{
  Serial.print("id3data     ");
  Serial.println(info);

  // Znajdź pozycję w tekście
  int artistIndex1 = String(info).indexOf("Artist: ");
  int artistIndex2 = String(info).indexOf("ARTIST=");
  int artistIndex3 = String(info).indexOf("Artist=");

  if (artistIndex1 != -1)
  {
    // Przytnij tekst od pozycji "Artist:" do końca linii
    artistString = String(info).substring(artistIndex1 + 8, String(info).indexOf('\n', artistIndex1));
    Serial.println("Znalazłem artystę: " + artistString);
    id3tag = true;
  }
  if (artistIndex2 != -1)
  {
    // Przytnij tekst od pozycji "ARTIST=" do końca linii
    artistString = String(info).substring(artistIndex2 + 7, String(info).indexOf('\n', artistIndex2));
    Serial.println("Znalazłem artystę: " + artistString);
    id3tag = true;
  }
  if (artistIndex3 != -1)
  {
    // Przytnij tekst od pozycji "Artist=" do końca linii
    artistString = String(info).substring(artistIndex3 + 7, String(info).indexOf('\n', artistIndex3));
    Serial.println("Znalazłem artystę: " + artistString);
    id3tag = true;
  }

  // Znajdź pozycję w tekście
  int titleIndex1 = String(info).indexOf("Title: ");
  int titleIndex2 = String(info).indexOf("TITLE=");
  int titleIndex3 = String(info).indexOf("Title=");
  
  if (titleIndex1 != -1)
  {
    // Przytnij tekst od pozycji "Title: " do końca linii
    titleString = String(info).substring(titleIndex1 + 7, String(info).indexOf('\n', titleIndex1));
    Serial.println("Znalazłem tytuł: " + titleString);
    id3tag = true;
  }
  if (titleIndex2 != -1)
  {
    // Przytnij tekst od pozycji "TITLE=" do końca linii
    titleString = String(info).substring(titleIndex2 + 6, String(info).indexOf('\n', titleIndex2));
    Serial.println("Znalazłem tytuł: " + titleString);
    id3tag = true;
  }
  if (titleIndex3 != -1)
  {
    // Przytnij tekst od pozycji "Title=" do końca linii
    titleString = String(info).substring(titleIndex3 + 6, String(info).indexOf('\n', titleIndex3));
    Serial.println("Znalazłem tytuł: " + titleString);
    id3tag = true;
  }
}

void audio_bitrate(const char *info)
{
  Serial.print("bitrate     ");
  Serial.println(info);
  bitrateString = String(info);
  bitratePresent = true;
}

void audio_eof_mp3(const char *info)
{
  fileEnd = true;
  Serial.print("eof_mp3     ");
  Serial.println(info);
}

void audio_showstation(const char *info)
{
  Serial.print("station     ");
  Serial.println(info);
}

void audio_showstreamtitle(const char *info)
{
  Serial.print("streamtitle ");
  Serial.println(info);
  stationInfo = String(info);
  if (currentOption == INTERNET_RADIO)
  {
    displayRadio();
  }
}

void audio_commercial(const char *info)
{
  Serial.print("commercial  ");
  Serial.println(info);
}
void audio_icyurl(const char *info)
{
  Serial.print("icyurl      ");
  Serial.println(info);
}
void audio_lasthost(const char *info)
{
  Serial.print("lasthost    ");
  Serial.println(info);
}
void audio_eof_speech(const char *info)
{
  Serial.print("eof_speech  ");
  Serial.println(info);
}

void displayMenu()
{
  timeDisplay = false;
  menuEnable = true;
  u8g2.clearBuffer();
  u8g2.setFont(spleen6x12PL);
  u8g2.drawStr(65, 20, "MENU");

  switch (currentOption)
  {
    case PLAY_FILES:
      u8g2.drawStr(0, 40, ">> Odtwarzacz plikow");
      u8g2.drawStr(0, 60, "   Radio internetowe");
      break;
    case INTERNET_RADIO:
      u8g2.drawStr(0, 40, "   Odtwarzacz plikow");
      u8g2.drawStr(0, 60, ">> Radio internetowe");
      break;
  }
  u8g2.sendBuffer();
}


// Funkcja do porównywania ciągów uwzględniająca liczby
int compareStringsWithNumbers(const String &a, const String &b)
{
  int i = 0, j = 0;
  
  while (i < a.length() && j < b.length())
  {
    // Wyciągnij kolejne znaki
    char charA = a[i];
    char charB = b[j];
    
    // Sprawdź, czy mamy do czynienia z liczbą w obu ciągach
    if (isdigit(charA) && isdigit(charB))
    {
      // Wyciągnij pełne liczby
      String numA, numB;
      while (i < a.length() && isdigit(a[i]))
      {
        numA += a[i++];
      }
      while (j < b.length() && isdigit(b[j]))
      {
        numB += b[j++];
      }

      // Porównaj liczby jako liczby (nie tekstowo)
      int intA = numA.toInt();
      int intB = numB.toInt();
      
      if (intA != intB)
      {
        return intA - intB;
      }
    } 
    else
    {
      // Porównaj inne znaki
      if (charA != charB)
      {
        return charA - charB;
      }
      i++;
      j++;
    }
  }
  
  // Jeżeli wszystko jest równe, porównaj długości
  return a.length() - b.length();
}

void printDirectoriesAndSavePaths(File dir, int numTabs, String currentPath)
{
  folderCount = 0;

  // Przejrzyj wszystkie pliki w katalogu
  while (true)
  {
    File entry = dir.openNextFile();
    
    if (!entry)
    {
      break; // Koniec plików
    }

    if (entry.isDirectory())
    {
      // Utwórz pełną ścieżkę
      String path = currentPath + "/" + entry.name();
      
      // Sprawdź, czy katalog to nie "System Volume Information"
      if (path != "/System Volume Information")
      {
        directories[folderCount] = path; // Zapisz ścieżkę do tablicy
        folderCount++; // Zwiększ licznik katalogów
      }
    }

    entry.close();
  }

  // Sortowanie katalogów za pomocą funkcji porównującej
  for (int i = 0; i < folderCount - 1; i++)
  {
    for (int j = i + 1; j < folderCount; j++)
    {
      if (compareStringsWithNumbers(directories[i], directories[j]) > 0)
      {
        String temp = directories[i];
        directories[i] = directories[j];
        directories[j] = temp;
      }
    }
  }

  // Wydrukuj na serial terminalu alfabetycznie posortowane katalogi
  for (int i = 0; i < folderCount; i++)
  {
    Serial.print(i + 1); // Drukuje alfabetyczny numer katalogu
    Serial.print(": ");
    Serial.println(directories[i].substring(1)); // Drukuje ścieżkę bez pierwszego znaku
  }

  // Wyświetl na ekranie, jeśli to nie System Volume Information
  for (int i = 0; i < folderCount; i++)
  {
    String fullPath = directories[i];
  }
}



// Funkcja do wylistowania katalogów z karty 
void listDirectories(const char *dirname)
{
  File root = SD.open(dirname);
  if (!root)
  {
    Serial.println("Błąd otwarcia katalogu!");
    Serial.println(dirname);
    return;
  }
  printDirectoriesAndSavePaths(root, 0, ""); // Początkowo pełna ścieżka jest pusta
  Serial.println("Wylistowano katalogi z karty SD");
  root.close();
  scrollDown();
  displayFolders();
}

// Funkcja do przewijania w górę
void scrollUp()
{
  if (currentSelection > 0)
  {
    currentSelection--;
    if (currentSelection < firstVisibleLine)
    {
      firstVisibleLine = currentSelection;
    }
  }
  else
  {
    // Jeśli osiągnięto wartość 0, przejdź do najwyższej wartości
    currentSelection = maxSelection(); 
    firstVisibleLine = currentSelection - maxVisibleLines + 1; // Ustaw pierwszą widoczną linię na najwyższą
  }
  
  Serial.print("Scroll Up: CurrentSelection = ");
  Serial.println(currentSelection);
  Serial.print("Scroll Down: firstVisibleLine = ");
  Serial.println(firstVisibleLine);
}

// Funkcja do przewijania w dół
void scrollDown()
{
  if (currentSelection < maxSelection())
  {
    currentSelection++;
    if (currentSelection >= firstVisibleLine + maxVisibleLines)
    {
      firstVisibleLine++;
    }
  }
  else
  {
    // Jeśli osiągnięto maksymalną wartość, przejdź do najmniejszej (0)
    currentSelection = 0;
    firstVisibleLine = 0; // Przywróć do pierwszej widocznej linii
  }

  Serial.print("Scroll Down: CurrentSelection = ");
  Serial.println(currentSelection);
  Serial.print("Scroll Down: firstVisibleLine = ");
  Serial.println(firstVisibleLine);
}

// Funkcja zwracająca maksymalny możliwy wybór w zależności od opcji
int maxSelection()
{
  if (currentOption == INTERNET_RADIO)
  {
    return stationsCount - 1;  // Zwraca maksymalny wybór stacji radiowych
  }
  else if (currentOption == PLAY_FILES)
  {
    if (folderSelection == true)
    {
      return folderCount - 1;  // Zwraca maksymalny wybór folderów
    }
    else if (fileSelection == true)
    {
      return filesCount - 1;  // Zwraca maksymalny wybór plików w bieżącym folderze
    }
  }
  return 0; // Zwraca 0, jeśli żaden warunek nie jest spełniony
}


// Funkcja do odtwarzania plików audio z wybranej lokalizacji z karty SD
void playFromSelectedFolder()
{
  fileSelection = false;
  folderSelection = false;
  folderNameString = directories[folderIndex];
  Serial.println("Odtwarzanie plików z wybranego folderu: " + folderNameString);

  // Otwórz folder
  File root = SD.open(folderNameString);
  if (!root)
  {
    Serial.println("Błąd otwarcia katalogu!");
    return;
  }

  filesCount = 0;
  fileIndex = 0;

  // Zliczanie plików audio w folderze
  while (File entry = root.openNextFile())
  {
    fileNameString = entry.name();
    Serial.print("Plik: ");
    Serial.print(fileNameString);
    
    // Pobieranie rozmiaru pliku w bajtach i przeliczanie na megabajty
    float fileSizeMB = entry.size() / 1024.0 / 1024.0;
    
    // Wydrukowanie rozmiaru pliku w MB z dokładnością do 0,1
    Serial.print(" | Rozmiar: ");
    Serial.print(fileSizeMB, 1); // 1 oznacza jedno miejsce po przecinku
    Serial.println(" MB");

    // Sprawdzanie, czy plik jest plikiem audio
    if (isAudioFile(fileNameString.c_str()))
    {
      // Zapisujemy pełną ścieżkę do pliku w tablicy `files[]`
      files[filesCount] = folderNameString + "/" + fileNameString;
      filesCount++;  // Zwiększ licznik plików audio
    }
    else
    {
      // Jeśli plik nie jest plikiem audio, wydrukuj pełną nazwę pliku wraz z rozszerzeniem
      Serial.print("Pominięty plik: ");
      Serial.println(fileNameString);  // Drukowanie pełnej nazwy pliku z rozszerzeniem
    }

    entry.close(); // Zamykaj każdy plik natychmiast po zakończeniu przetwarzania
  }
  root.rewindDirectory(); // Przewiń katalog na początek

  bool playNextFolder = false;  // Flaga kontrolująca przejście do kolejnego folderu

  // Odtwarzanie plików
  while (fileIndex <= filesCount && !playNextFolder)
  {
    File entry = root.openNextFile();
    if (!entry)
    {
      break;  // Koniec plików w folderze
    }

    fileNameString = entry.name();

    // Pomijaj pliki, które nie są w formacie audio
    if (!isAudioFile(fileNameString.c_str()))
    {
      Serial.println("Pominięto plik: " + fileNameString);
      entry.close(); // Zamknij pominięty plik
      continue;
    }

    Serial.print("Odtwarzanie pliku: ");
    Serial.print(fileIndex + 1);   // Liczymy od 1, nie od 0 na serialu
    Serial.print("/");
    Serial.print(filesCount); // Liczba plików
    Serial.print(" - ");
    Serial.println(fileNameString);

    // Pełna ścieżka do pliku
    String fullPath = folderNameString + "/" + fileNameString;
    Serial.println(fullPath);

    // Odtwarzaj pierwszy plik z wybranego katalogu
    audio.connecttoFS(SD, fullPath.c_str());
    seconds = 0;
    isPlaying = true;
    previous_fileIndex = fileIndex;
    previous_folderIndex = folderIndex;

    entry.close();  // Zamykaj plik po odczytaniu

    // Oczekuj na zakończenie odtwarzania
    while (isPlaying)
    {
      audio.loop(); 
      button1.loop();
      button2.loop();
      processIRCode();
      vTaskDelay(1); 

      // Jeśli wybrany następny plik lub jeśli aktualnie odtwarzany plik skończył sie samoczynnie
      if ((playNextFile == true) || (IRrightArrow == true) || (fileEnd == true))
      {
        IRrightArrow = false;
        playNextFile = false;
        fileEnd = false;
        audio.stopSong();
        fileIndex++;
        if (fileIndex >= filesCount)  // Upewniamy się, że indeks nie przekroczy liczby plików
        {
          Serial.println("To jest ostatni plik w folderze");
          folderIndex++; // Przejście do nowego folderu
          playFromSelectedFolder();
        }
        else
        {
          playFile();
        }
        previous_fileIndex = fileIndex;
      }

      // Jeśli wybrany poprzedni plik
      if ((playPreviousFile == true) || (IRleftArrow == true))
      {
        IRleftArrow = false;
        playPreviousFile = false;
        audio.stopSong();
        fileIndex--;
        if (fileIndex < 0)
        {
          Serial.println("To jest pierwszy plik w folderze");
          fileIndex = 0;
        }
        playFile();
        previous_fileIndex = fileIndex;
      }

      if (button2.isPressed()) // Użycie przycisku enkodera nr 2
      {
        audio.stopSong();
        playNextFolder = true;
        id3tag = false;
        String text = "  ŁADOWANIE PLIKÓW Z WYBRANEGO FOLDERU... ";
        processText(text);  // Podstawienie polskich znaków diakrytycznych
        u8g2.clearBuffer();
        u8g2.setFont(spleen6x12PL);
        u8g2.setCursor(0, 10);
        u8g2.print(text);
        u8g2.sendBuffer();
        break;
      }

      if (button1.isPressed() || (IRmenuButton == true)) // Użycie przycisku enkodera nr 1 lub naciśnięcie HOME na pilocie powoduje przerwanie odtwarzania i wyjście do menu
      {
        IRmenuButton = false;
        audio.stopSong();
        encoderButton1 = true;
        break;
      }

      if (IRbankDown == true)  // Przycisk FAV- na pilocie do przewijania listy folderów z karty SD
      {
        IRbankDown = false;
        fileSelection = false;
        folderSelection = true;
        timeDisplay = false;
        displayActive = true;
        displayStartTime = millis();
        scrollUpFolders();
        folderIndex = currentSelection; // Zaktualizuj indeks folderu
      }

      if (IRbankUp == true)  // Przycisk FAV+ na pilocie do przewijania listy folderów z karty SD
      {
        IRbankUp = false;
        fileSelection = false;
        folderSelection = true;
        timeDisplay = false;
        displayActive = true;
        displayStartTime = millis();
        scrollDownFolders();
        folderIndex = currentSelection; // Zaktualizuj indeks folderu
      }

      if (IRdownArrow == true)  // Dolny przycisk kierunkowy w pilocie do przewijania listy plików z aktualnego folderu
      {
        IRdownArrow = false;
        fileSelection = true;
        folderSelection = false;
        timeDisplay = false;
        displayActive = true;
        displayStartTime = millis();
        scrollDownFiles();
        fileIndex = currentSelection; // Zaktualizuj indeks pliku
      }

      if (IRupArrow == true)  // Górny przycisk kierunkowy w pilocie do przewijania listy plików z aktualnego folderu
      {
        IRupArrow = false;
        fileSelection = true;
        folderSelection = false;
        timeDisplay = false;
        displayActive = true;
        displayStartTime = millis();
        scrollUpFiles();
        fileIndex = currentSelection; // Zaktualizuj indeks pliku
      }

      if ((IRokButton == true) && (folderSelection == true))  // Zatwierdzenie startu odtwarzania z wybranego folderu
      {
        currentSelection = 0;
        firstVisibleLine = 0;
        IRokButton = false;
        folderSelection = false;
        audio.stopSong();
        playNextFolder = true;
        id3tag = false;
        String text = "  ŁADOWANIE PLIKÓW Z WYBRANEGO FOLDERU... ";
        processText(text);  // Podstawienie polskich znaków diakrytycznych
        u8g2.clearBuffer();
        u8g2.setFont(spleen6x12PL);
        u8g2.setCursor(0, 10);
        u8g2.print(text);
        u8g2.sendBuffer();
        break;
      }

      if ((fileSelection == true) && (IRokButton == true))  // Zatwierdzenie startu odtwarzania wybranego pliku z przewijanej listy plików
      {
        IRokButton = false;
        fileSelection = false;
        isPlaying = false;
        audio.stopSong();
        
        fileIndex = currentSelection;
        previous_fileIndex = fileIndex;

        // Sprawdź, czy indeks jest poprawny
        if (fileIndex >= 0 && fileIndex < filesCount)
        {
          playFile();
        }
      }

      handleEncoder1Rotation();  // Obsługa kółka enkodera nr 1
      handleEncoder2Rotation();  // Obsługa kółka enkodera nr 2
      backDisplayPlayer();       // Obsługa bezczynności, przywrócenie wyświetlania danych audio
      volumeSetFromRemote();     // Obsługa regulacji głośności z pilota zdalnego sterowania
    }

    // Jeśli encoderButton1 aktywowany, wyjdź z pętli
    if (encoderButton1)
    {
      encoderButton1 = false;
      displayMenu();
      break;
    }

    // Sprawdź, czy zakończono odtwarzanie plików w folderze
    if (fileIndex > filesCount)
    {
      Serial.println("To był ostatni plik w folderze, przechodzę do kolejnego folderu");
      playNextFolder = true;
      folderIndex++;
      fileIndex = currentSelection + 1;
    }
  }

  // Przejdź do kolejnego folderu, jeśli ustawiono flagę
  if (playNextFolder == true)
  {
    if (folderIndex < folderCount)  // Upewnij się, że folderIndex nie przekroczy dostępnych folderów
    {
      playFromSelectedFolder();  // Wywołanie funkcji tylko raz
    }
    else
    {
      Serial.println("To był ostatni folder.");
    }
  }

  // Po zakończeniu zamknij katalog
  root.close();
}

void playFile()
{
  // Pobierz ścieżkę pliku z tablicy
  String fullPath = files[fileIndex];

  // Odtwórz tylko w przypadku, gdy to jest plik audio
  if (isAudioFile(fullPath.c_str()))
  {
    audio.connecttoFS(SD, fullPath.c_str());
    seconds = 0;
    isPlaying = true;
    Serial.print("Odtwarzanie pliku: ");
    Serial.print(fileIndex + 1);  // Liczymy od 1, nie od 0 na serialu
    Serial.print("/");
    Serial.print(filesCount); // Łączna liczba plików w folderze
    Serial.print(" - ");
    Serial.println(fullPath); // Pełna ścieżka pliku

    // Usunięcie folderu ze ścieżki pliku (zostaje tylko nazwa pliku)
    int lastSlashIndex = fullPath.lastIndexOf('/');
    if (lastSlashIndex != -1)
    {
      fileNameString = fullPath.substring(lastSlashIndex + 1);  // Wycinanie nazwy pliku po ostatnim ukośniku
    }
  }
}

// Wyświetlanie przewijalnej listy plików z podświetleniem wybranego pliku
void displayFiles()
{ 
  String text = "ODTWARZACZ PLIKÓW - LISTA PLIKÓW";
  processText(text);  // Zamiana polskich znaków
  u8g2.clearBuffer();
  u8g2.setFont(spleen6x12PL);
  u8g2.setCursor(0, 10);
  u8g2.print(text + " " + (fileIndex + 1) + "/" + filesCount);
  u8g2.setCursor(0, 21);
  u8g2.print(currentDirectory);  // Wyświetlenie bieżącego katalogu

  int displayRow = 1;  // Numer wiersza wyświetlania, zaczynając od drugiego (pierwszy to nagłówek)

  // Wyświetlanie plików zaczynając od pierwszej widocznej linii
  for (int i = firstVisibleLine; i < min(firstVisibleLine + 4, filesCount); i++)
  {
    String fileNameDisplay = files[i];  // files[] to tablica z nazwami plików

    // Usunięcie folderu ze ścieżki pliku (zostaje tylko nazwa pliku)
    int lastSlashIndex = fileNameDisplay.lastIndexOf('/');
    if (lastSlashIndex != -1)
    {
      fileNameDisplay = fileNameDisplay.substring(lastSlashIndex + 1);  // Wycinanie nazwy pliku po ostatnim ukośniku
    }

    // Podświetlenie zaznaczonego pliku
    if (i == currentSelection)
    {
      u8g2.setDrawColor(1);  // Białe tło
      u8g2.drawBox(0, displayRow * 13 - 2, 256, 13);  // Narysuj prostokąt jako tło dla zaznaczonego pliku
      u8g2.setDrawColor(0);  // Czarny kolor tekstu
    }
    else
    {
      u8g2.setDrawColor(1);  // Biały kolor tekstu na czarnym tle
    }

    u8g2.drawStr(0, displayRow * 13 + 8, fileNameDisplay.c_str());  // Wyświetlanie nazwy pliku bez ścieżki

    // Przesunięcie do kolejnego wiersza
    displayRow++;
  }

  // Przywróć domyślny kolor rysowania (biały tekst na czarnym tle)
  u8g2.setDrawColor(1);
  u8g2.sendBuffer();
}


// Obsługa wyświetlacza dla odtwarzanego strumienia radia internetowego
void displayRadio()
{
  u8g2.clearBuffer();	
  u8g2.setFont(spleen6x12PL);
  u8g2.drawStr(0, 10, stationName.c_str());

  // Parametry do obługi wyświetlania w 3 kolejnych wierszach z podzialem do pełnych wyrazów
  const int maxLineLength = 21;  // Maksymalna długość jednej linii w znakach
  String currentLine = "";  // Bieżąca linia
  int yPosition = 21;  // Początkowa pozycja Y

  // Podziel tekst na wyrazy
  String word;
  int wordStart = 0;
  processText(stationInfo);  // Podstawienie polskich znaków diakrytycznych

  for (int i = 0; i <= stationInfo.length(); i++)
  {
    // Sprawdź, czy dotarliśmy do końca słowa lub do końca tekstu
    if (i == stationInfo.length() || stationInfo.charAt(i) == ' ')
    {
      // Pobierz słowo
      String word = stationInfo.substring(wordStart, i);
      wordStart = i + 1;

      // Sprawdź, czy dodanie słowa do bieżącej linii nie przekroczy maxLineLength
      if (currentLine.length() + word.length() <= maxLineLength)
      {
        // Dodaj słowo do bieżącej linii
        if (currentLine.length() > 0)
        {
          currentLine += " ";  // Dodaj spację między słowami
        }
        currentLine += word;
      }
      else
      {
        // Jeśli słowo nie pasuje, wyświetl bieżącą linię i przejdź do nowej linii
        u8g2.drawStr(0, yPosition, currentLine.c_str());
        yPosition += 10;  // Przesunięcie w dół dla kolejnej linii

        // Zresetuj bieżącą linię i dodaj nowe słowo
        currentLine = word;
      }
    }
  }

  // Wyświetl ostatnią linię, jeśli coś zostało
  if (currentLine.length() > 0)
  {
    u8g2.drawStr(0, yPosition, currentLine.c_str());
  }

  u8g2.sendBuffer();
  // Ponowne wysłanie buforu zawartości ekranu, eliminowanie przypadkowych krzaczków
  if (millis() - lastCheckTime >= 100)
  {
    u8g2.sendBuffer();
    lastCheckTime = millis(); // Zaktualizuj czas ostatniego sprawdzenia
  }
}

// Obsługa wyświetlacza dla odtwarzanego pliku z karty SD
void displayPlayer()
{
  if (id3tag == true)
  {
    timeDisplay = true;
    u8g2.clearBuffer();
    u8g2.setFont(spleen6x12PL);
    u8g2.setCursor(0, 10);
    u8g2.print("PLIK ");
    u8g2.print(previous_fileIndex + 1); // Liczymy od 1, nie od 0 na wyświetlaczu
    u8g2.print("/");
    u8g2.print(filesCount);
    u8g2.print(" FOLD ");
    u8g2.print(previous_folderIndex + 1); // Liczymy od 1, nie od 0 na wyświetlaczu
    u8g2.print("/");
    u8g2.print(folderCount);

    if (artistString.length() > 33)
    {
      artistString = artistString.substring(0, 33); // Ogranicz długość tekstu do 33 znaków
    }

    // Pomocnicza pętla w celu wyłapania bajtów titleString na serial terminalu
    /*Serial.println("Bajty RAW DATA artysty:");
    for (int i = 0; i < artistString.length(); i++)
    {
      Serial.print("0x");
      if (artistString[i] < 0x10)
      {
        Serial.print("0"); // Dodaj zero przed pojedynczymi cyframi w formacie hex
      }
      Serial.print(artistString[i], HEX); // Drukowanie znaku jako wartość hex
      Serial.print(" "); // Dodanie spacji po każdym bajcie
    }
    Serial.println();*/

    u8g2.setCursor(0, 21);
    u8g2.print("Artysta: ");
    processText(artistString);  // Podstawienie polskich znaków diakrytycznych
    u8g2.print(artistString);

    if (titleString.length() > 35)
    {
      titleString = titleString.substring(0, 35); // Ogranicz długość tekstu do 35 znaków
    }

    // Pomocnicza pętla w celu wyłapania bajtów titleString na serial terminalu
    /*Serial.println("Bajty RAW DATA tytułu:");
    for (int i = 0; i < titleString.length(); i++)
    {
      Serial.print("0x");
      if (titleString[i] < 0x10)
      {
        Serial.print("0"); // Dodaj zero przed pojedynczymi cyframi w formacie hex
      }
      Serial.print(titleString[i], HEX); // Drukowanie znaku jako wartość hex
      Serial.print(" "); // Dodanie spacji po każdym bajcie
    }
    Serial.println();*/

    u8g2.setCursor(0, 31);
    String text = "Tytuł: ";
    processText(text);  // Podstawienie polskich znaków diakrytycznych
    u8g2.print(text);
    processText(titleString);  // Podstawienie polskich znaków diakrytycznych
    u8g2.print(titleString);

    if (folderNameString.startsWith("/"))
    {
      folderNameString = folderNameString.substring(1); // Usuń pierwszy ukośnik
    }

    u8g2.setCursor(0, 41);
    u8g2.print("Folder: ");
    String folder = folderNameString;
    processText(folder);  // Podstawienie polskich znaków diakrytycznych
    u8g2.print(folder);
    u8g2.sendBuffer();
    Serial.println("Tagi ID3 artysty, tytułu i folderu gotowe do wyświetlenia");
  }
  else
  {
    // Maksymalna długość wiersza (21 znaków)
    int maxLineLength = 21;
    timeDisplay = true;
    u8g2.clearBuffer();
    u8g2.setFont(spleen6x12PL);
    u8g2.setCursor(0, 10);
    u8g2.print("PLIK ");
    u8g2.print(previous_fileIndex + 1);  // Liczymy od 1, nie od 0 na wyświetlaczu
    u8g2.print("/");
    u8g2.print(filesCount);
    u8g2.print(" FOLD ");
    u8g2.print(previous_folderIndex + 1);  // Liczymy od 1, nie od 0 na wyświetlaczu
    u8g2.print("/");
    u8g2.print(folderCount);
    u8g2.drawStr(0, 21, "Brak danych ID3 utworu, nazwa pliku:");
    processText(fileNameString);  // Podstawienie polskich znaków diakrytycznych

    // Jeśli długość nazwy pliku przekracza 42 znaki na wiersz
    if (fileNameString.length() > maxLineLength)
    {
      // Pierwszy wiersz - pierwsze 42 znaki
      String firstLine = fileNameString.substring(0, maxLineLength);
      // Drugi wiersz - pozostałe znaki
      String secondLine = fileNameString.substring(maxLineLength);
      u8g2.setCursor(0, 31);
      u8g2.print(firstLine);
      u8g2.setCursor(0, 41);
      u8g2.print(secondLine);
    }
    else
    {
      // Jeśli nazwa pliku mieści się w jednym wierszu
      u8g2.setCursor(0, 31);
      u8g2.print(fileNameString);
    }
    u8g2.sendBuffer();
    Serial.println("Brak prawidłowych tagów ID3 do wyświetlenia");
  }
}

// Funkcja przywracająca wyświetlanie danych o utworze po przekroczeniu czasu bezczynności podczas odtwarzania plików audio z karty SD
void backDisplayPlayer()
{
  if (displayActive && (millis() - displayStartTime >= displayTimeout))
  {
    folderIndex = previous_folderIndex;
    fileIndex = previous_fileIndex;
    currentSelection = fileIndex;
    firstVisibleLine = currentSelection;
    displayPlayer();
    displayActive = false;
    timeDisplay = true;
    fileSelection = false;
    folderSelection = false;
  }
}

// Obsługa kółka enkodera 1 podczas dzialania odtwarzacza plików
void handleEncoder1Rotation()
{
  CLK_state1 = digitalRead(CLK_PIN1);
  if (CLK_state1 != prev_CLK_state1 && CLK_state1 == HIGH)
  {
    timeDisplay = false;
    displayActive = true;
    displayStartTime = millis();
    if (digitalRead(DT_PIN1) == HIGH)
    {
      volumeValue--;
      if (volumeValue < 3)
      {
        volumeValue = 3;
      }
    }
    else
    {
      volumeValue++;
      if (volumeValue > 18)
      {
        volumeValue = 18;
      }
    }
    Serial.print("Wartość głośności: ");
    Serial.println(volumeValue);
    audio.setVolume(volumeValue); // dopuszczalny zakres 0...21

    String volumeValueStr = String(volumeValue);  // Zamiana liczby VOLUME na ciąg znaków
    u8g2.clearBuffer();
    u8g2.setFont(spleen6x12PL);
    u8g2.drawStr(32, 25, "VOLUME SET");
    u8g2.drawStr(58, 50, volumeValueStr.c_str());
    u8g2.sendBuffer();

  }
  prev_CLK_state1 = CLK_state1;
}

// Obsługa kółka enkodera 2 podczas dzialania odtwarzacza plików
void handleEncoder2Rotation() 
{
  CLK_state2 = digitalRead(CLK_PIN2);
  if (CLK_state2 != prev_CLK_state2 && CLK_state2 == HIGH) 
  {
    folderSelection = true;
    folderIndex = currentSelection; // Zaktualizuj indeks folderu
    timeDisplay = false;
    if (digitalRead(DT_PIN2) == HIGH) 
    {
      scrollUpFolders();
    } 
    else 
    {
      scrollDownFolders();
    }

    displayActive = true;
    displayStartTime = millis();
  }
  prev_CLK_state2 = CLK_state2;
}

// Przewijanie listy folderów w górę
void scrollUpFolders()
{
  folderIndex--;
  if (folderIndex < 0)
  {
      folderIndex = folderCount;
  }
  Serial.print("Numer folderu do tyłu: ");
  Serial.println(folderIndex);

  scrollUp();
  displayFolders();
}

// Przewijanie listy folderów w dół
void scrollDownFolders()
{
  folderIndex++;
  if (folderIndex > (folderCount - 1))
  {
    folderIndex = 1;
  }
  Serial.print("Numer folderu do przodu: ");
  Serial.println(folderIndex);

  scrollDown();
  displayFolders();
}

// Przewijanie listy plików w górę
void scrollUpFiles()
{
  fileIndex--;
  if (fileIndex < 0)
  {
      fileIndex = filesCount;
  }
  Serial.print("Numer pliku do tyłu: ");
  Serial.println(fileIndex);

  scrollUp();
  displayFiles();
}

// Przewijanie listy plików w dół
void scrollDownFiles()
{
  fileIndex++;
  if (fileIndex > filesCount)
  {
    fileIndex = 0;
  }
  Serial.print("Numer pliku do przodu: ");
  Serial.println(fileIndex);

  scrollDown();
  displayFiles();
}

// Funkcja do wyświetlania folderów na ekranie OLED z uwzględnieniem zaznaczenia
void displayFolders()
{
  String text = "ODTWARZACZ PLIKÓW - LISTA KATALOGÓW";
  processText(text);  // Podstawienie polskich znaków diakrytycznych
  u8g2.clearBuffer();
  u8g2.setFont(spleen6x12PL);
  u8g2.setCursor(0, 10);
  u8g2.print(text + " " + (folderIndex + 1) + "/" + folderCount);
  u8g2.setCursor(0, 21);
  u8g2.print(currentDirectory);  // Wyświetl bieżący katalog

  int displayRow = 1;  // Zmienna dla numeru wiersza, zaczynając od drugiego (pierwszy to nagłówek)

  // Wyświetlanie katalogów zaczynając od pierwszej widocznej linii
  for (int i = firstVisibleLine; i < min(firstVisibleLine + 4, folderCount); i++)
  {
    String fullPath = directories[i];

    // Pomijaj "System Volume Information"
    if (fullPath != "/System Volume Information")
    {
      // Sprawdź, czy ścieżka zaczyna się od aktualnego katalogu
      if (fullPath.startsWith(currentDirectory))
      {
        // Ogranicz długość do 42 znaków
        String displayedPath = fullPath.substring(currentDirectory.length(), currentDirectory.length() + 42);

        // Podświetlenie zaznaczonego katalogu
        if (i == currentSelection)
        {
          u8g2.setDrawColor(1);  // Biały kolor tła
          u8g2.drawBox(0, displayRow * 13 - 2, 256, 13);  // Narysuj prostokąt jako tło dla zaznaczonego folderu
          u8g2.setDrawColor(0);  // Czarny kolor tekstu
        }
        else
        {
          u8g2.setDrawColor(1);  // Biały kolor tekstu na czarnym tle
        }

        // Wyświetl ścieżkę
        u8g2.drawStr(0, displayRow * 13 + 8 ,String(displayedPath).c_str());

        // Przesuń się do kolejnego wiersza
        displayRow++;
      }
    }
  }
    // Przywróć domyślne ustawienia koloru rysowania (biały tekst na czarnym tle)
  u8g2.setDrawColor(1);  // Biały kolor rysowania
  u8g2.sendBuffer();
}

// Funkcja do wyświetlania listy stacji radiowych z opcją wyboru poprzez zaznaczanie w negatywie
void displayStations()
{
  u8g2.clearBuffer();  // Wyczyść bufor przed rysowaniem, aby przygotować ekran do nowej 
  u8g2.setFont(spleen6x12PL);
  u8g2.setCursor(0, 10);  // Ustaw pozycję kursora (x=60, y=10) dla nagłówka
  u8g2.print("   STACJE RADIOWE:   ");  // Wyświetl nagłówek "STACJE RADIOWE"
  u8g2.print(String(station_nr) + " / " + String(stationsCount));  // Dodaj numer aktualnej stacji i licznik wszystkich stacji
  
  int displayRow = 1;  // Zmienna dla numeru wiersza, zaczynając od drugiego (pierwszy to nagłówek)

  // Wyświetlanie stacji, zaczynając od drugiej linii (y=21)
  for (int i = firstVisibleLine; i < min(firstVisibleLine + maxVisibleLines, stationsCount); i++)
  {
    char station[STATION_NAME_LENGTH + 1];  // Tablica na nazwę stacji o maksymalnej długości zdefiniowanej przez STATION_NAME_LENGTH
    memset(station, 0, sizeof(station));  // Wyczyszczenie tablicy zerami przed zapisaniem danych
    
    // Odczytaj długość nazwy stacji z EEPROM dla bieżącego indeksu stacji
    int length = EEPROM.read(i * (STATION_NAME_LENGTH + 1));

    // Odczytaj nazwę stacji z EEPROM jako ciąg bajtów, maksymalnie do STATION_NAME_LENGTH
    for (int j = 0; j < min(length, STATION_NAME_LENGTH); j++)
    {
      station[j] = EEPROM.read(i * (STATION_NAME_LENGTH + 1) + 1 + j);  // Odczytaj znak po znaku nazwę stacji
    }

    // Sprawdź, czy bieżąca stacja to ta, która jest aktualnie zaznaczona
    if (i == currentSelection)
    {
      u8g2.setDrawColor(1);  // Ustaw biały kolor rysowania
      u8g2.drawBox(0, displayRow * 13 - 2, 256, 13);  // Narysuj prostokąt jako tło dla zaznaczonej stacji (x=0, szerokość 256, wysokość 10)
      u8g2.setDrawColor(0);  // Zmień kolor rysowania na czarny dla tekstu zaznaczonej stacji
    }
    else
    {
      u8g2.setDrawColor(1);  // Dla niezaznaczonych stacji ustaw zwykły biały kolor tekstu
    }

    // Wyświetl nazwę stacji, ustawiając kursor na odpowiedniej pozycji
    u8g2.drawStr(0, displayRow * 13 + 8 , String(station).c_str());

    // Przejdź do następnej linii (następny wiersz na ekranie)
    displayRow++;
  }

  // Przywróć domyślne ustawienia koloru rysowania (biały tekst na czarnym tle)
  u8g2.setDrawColor(1);  // Biały kolor rysowania
  u8g2.sendBuffer();  // Wyślij zawartość bufora do ekranu OLED, aby wyświetlić zmiany
}

// Funkcja wywoływana co sekundę przez timer do aktualizacji czasu oraz parametrów audio na wyświetlaczu
void updateTimer()  
{
  u8g2.setDrawColor(1);
  u8g2.setFont(spleen6x12PL);
  u8g2.drawStr(0, 63, "                     ");

  // Zwiększ licznik sekund
  seconds++;

  // Wyświetl aktualny czas w sekundach
  // Konwertuj sekundy na minutę i sekundy
  unsigned int minutes = seconds / 60;
  unsigned int remainingSeconds = seconds % 60;

  if ((timeDisplay == true) && audio.isRunning())
  {
    if (mp3 == true)
    {
      u8g2.drawStr(50, 63, "MP3");
    }
    if (flac == true)
    {
      u8g2.drawStr(500, 63, "FLAC");
    }
    if (aac == true)
    {
      u8g2.drawStr(50, 63, "AAC");
    }
    if (vorbis == true)
    {
      u8g2.drawStr(50, 63, "VORB");
    }
    
    if (currentOption == PLAY_FILES)
    {
      // Formatuj czas jako "mm:ss"
      char timeString[10];
      snprintf(timeString, sizeof(timeString), "%02um:%02us", minutes, remainingSeconds);

      // Pobierz bitrate
      bitrateString = audio.getBitRate(true);
      //Serial.print("Bitrate pliku: ");
      //Serial.println(bitrateString + " b/s");
      
      String displayString = sampleRateString.substring(1) + "Hz " + bitsPerSampleString + "bit " + bitrateString;
      u8g2.drawStr(0, 52, displayString.c_str());
      u8g2.drawStr(82, 63, timeString);
      u8g2.sendBuffer();
    }

    if ((currentOption == INTERNET_RADIO) && (timeDisplay == true) && (audio.isRunning() == true))
    {
      // Struktura przechowująca informacje o czasie
      struct tm timeinfo;

      // Sprawdź, czy udało się pobrać czas z lokalnego zegara czasu rzeczywistego
      if (!getLocalTime(&timeinfo))
      {
        Serial.println("Nie udało się uzyskać czasu");
        return; // Zakończ funkcję, gdy nie udało się uzyskać czasu
      }

      // Konwertuj godzinę, minutę i sekundę na stringi w formacie "HH:MM:SS"
      char timeString[9]; // Bufor przechowujący czas w formie tekstowej
      snprintf(timeString, sizeof(timeString), "%02d:%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);

      // Pobierz bitrate
      //bitrateString = audio.getBitRate(false);
      //Serial.print("Bitrate stacji: ");
      //Serial.println(bitrateString + " b/s");

      String displayString = sampleRateString.substring(1) + "Hz " + bitsPerSampleString + "bit " + bitrateString;
      u8g2.drawStr(0, 52, displayString.c_str());
      u8g2.drawStr(78, 63, timeString);
      u8g2.sendBuffer();

    }
  }

  // Sprawdzanie czy Internet Radio jest włączone, ale nie ma strumienia audio
  if ((currentOption == INTERNET_RADIO) && (timeDisplay == true) && (audio.isRunning() == false))
  {
    // Struktura przechowująca informacje o czasie
    struct tm timeinfo;

    // Sprawdź, czy udało się pobrać czas z lokalnego zegara czasu rzeczywistego
    if (!getLocalTime(&timeinfo))
    {
      Serial.println("Nie udało się uzyskać czasu");
      return; // Zakończ funkcję, gdy nie udało się uzyskać czasu
    }

    // Konwertuj godzinę, minutę i sekundę na stringi w formacie "HH:MM:SS"
    char timeString[9]; // Bufor przechowujący czas w formie tekstowej
    snprintf(timeString, sizeof(timeString), "%02d:%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);

    // Pulsujący co 1 sekundę napis o braku strumienia audio z radia
    if (millis() - lastCheckTime >= 1000)
    {
      u8g2.drawStr(0, 51, ".... Brak strumienia audio ! ....");
      lastCheckTime = millis(); // Zaktualizuj czas ostatniego sprawdzenia
    }
    u8g2.drawStr(78, 63, timeString);
    u8g2.sendBuffer();
  }
}


void saveStationOnSD()
{
  // Sprawdź, czy plik station_nr.txt istnieje
  if (SD.exists("/station_nr.txt"))
  {
    Serial.println("Plik station_nr.txt już istnieje.");

    // Otwórz plik do zapisu i nadpisz aktualną wartość station_nr
    myFile = SD.open("/station_nr.txt", FILE_WRITE);
    if (myFile)
    {
      myFile.println(station_nr);
      myFile.close();  // Zamknięcie pliku po zapisie
      Serial.println("Aktualizacja station_nr.txt na karcie SD.");
    }
    else
    {
      Serial.println("Błąd podczas otwierania pliku station_nr.txt.");
    }
  }
  else
  {
    Serial.println("Plik station_nr.txt nie istnieje. Tworzenie...");

    // Utwórz plik i zapisz w nim aktualną wartość station_nr
    myFile = SD.open("/station_nr.txt", FILE_WRITE);
    if (myFile)
    {
      myFile.println(station_nr);
      myFile.close();  // Zamknięcie pliku po zapisie
      Serial.println("Utworzono i zapisano station_nr.txt na karcie SD.");
    }
    else
    {
      Serial.println("Błąd podczas tworzenia pliku station_nr.txt.");
    }
  }

  // Sprawdź, czy plik bank_nr.txt istnieje
  if (SD.exists("/bank_nr.txt"))
  {
    Serial.println("Plik bank_nr.txt już istnieje.");

    // Otwórz plik do zapisu i nadpisz aktualną wartość bank_nr
    myFile = SD.open("/bank_nr.txt", FILE_WRITE);
    if (myFile)
    {
      myFile.println(bank_nr);
      myFile.close();  // Zamknięcie pliku po zapisie
      Serial.println("Aktualizacja bank_nr.txt na karcie SD.");
    }
    else
    {
      Serial.println("Błąd podczas otwierania pliku bank_nr.txt.");
    }
  }
  else
  {
    Serial.println("Plik bank_nr.txt nie istnieje. Tworzenie...");

    // Utwórz plik i zapisz w nim aktualną wartość bank_nr
    myFile = SD.open("/bank_nr.txt", FILE_WRITE);
    if (myFile)
    {
      myFile.println(bank_nr);
      myFile.close();  // Zamknięcie pliku po zapisie
      Serial.println("Utworzono i zapisano bank_nr.txt na karcie SD.");
    }
    else
    {
      Serial.println("Błąd podczas tworzenia pliku bank_nr.txt.");
    }
  }
}


// Funkcja do odczytywania numeru stacji i numeru banku z karty SD
void readStationFromSD()
{
  // Sprawdź, czy karta SD jest dostępna
  if (!SD.begin(47))
  {
    Serial.println("Nie można znaleźć karty SD. Ustawiam domyślne wartości.");
    station_nr = 9; // Domyślny numer stacji gdy brak karty SD
    bank_nr = 1;  // Domyślny numer banku gdy brak karty SD
    return;
  }

  // Sprawdź, czy plik station_nr.txt istnieje
  if (SD.exists("/station_nr.txt"))
  {
    myFile = SD.open("/station_nr.txt");
    if (myFile)
    {
      station_nr = myFile.parseInt();
      myFile.close();
      Serial.print("Wczytano station_nr z karty SD: ");
      Serial.println(station_nr);
    }
    else
    {
      Serial.println("Błąd podczas otwierania pliku station_nr.txt.");
    }
  }
  else
  {
    Serial.println("Plik station_nr.txt nie istnieje.");
  }

  // Sprawdź, czy plik bank_nr.txt istnieje
  if (SD.exists("/bank_nr.txt"))
  {
    myFile = SD.open("/bank_nr.txt");
    if (myFile)
    {
      bank_nr = myFile.parseInt();
      myFile.close();
      Serial.print("Wczytano bank_nr z karty SD: ");
      Serial.println(bank_nr);
    }
    else
    {
      Serial.println("Błąd podczas otwierania pliku bank_nr.txt.");
    }
  }
  else
  {
    Serial.println("Plik bank_nr.txt nie istnieje.");
  }
}


// Funkcja przetwarza tekst, wstawiając polskie znaki diakrytyczne
void processText(String &text)
{
  for (int i = 0; i < text.length(); i++)
  {
    switch (text[i])
    {
      case (char)0xC2:
        switch (text[i+1])
        {
          case (char)0xB3: text.setCharAt(i, 0xB3); break; // Zamiana na "ł"
          case (char)0x9C: text.setCharAt(i, 0x9C); break; // Zamiana na "ś"
          case (char)0x8C: text.setCharAt(i, 0x8C); break; // Zamiana na "Ś"
          case (char)0xB9: text.setCharAt(i, 0xB9); break; // Zamiana na "ą"
          case (char)0x9B: text.setCharAt(i, 0xEA); break; // Zamiana na "ę"
          case (char)0xBF: text.setCharAt(i, 0xBF); break; // Zamiana na "ż"
          case (char)0x9F: text.setCharAt(i, 0x9F); break; // Zamiana na "ź"
        }
        text.remove(i+1, 1);
        break;
      case (char)0xC3:
        switch (text[i+1])
        {
          case (char)0xB1: text.setCharAt(i, 0xF1); break; // Zamiana na "ń"
          case (char)0xB3: text.setCharAt(i, 0xF3); break; // Zamiana na "ó"
          case (char)0xBA: text.setCharAt(i, 0x9F); break; // Zamiana na "ź"
          case (char)0xBB: text.setCharAt(i, 0xAF); break; // Zamiana na "Ż"
          case (char)0x93: text.setCharAt(i, 0xD3); break; // Zamiana na "Ó"
        }
        text.remove(i+1, 1);
        break;
      case (char)0xC4:
        switch (text[i+1])
        {
          case (char)0x85: text.setCharAt(i, 0xB9); break; // Zamiana na "ą"
          case (char)0x99: text.setCharAt(i, 0xEA); break; // Zamiana na "ę"
          case (char)0x87: text.setCharAt(i, 0xE6); break; // Zamiana na "ć"
          case (char)0x84: text.setCharAt(i, 0xA5); break; // Zamiana na "Ą"
          case (char)0x98: text.setCharAt(i, 0xCA); break; // Zamiana na "Ę"
          case (char)0x86: text.setCharAt(i, 0xC6); break; // Zamiana na "Ć"
        }
        text.remove(i+1, 1);
        break;
      case (char)0xC5:
        switch (text[i+1])
        {
          case (char)0x82: text.setCharAt(i, 0xB3); break; // Zamiana na "ł"
          case (char)0x84: text.setCharAt(i, 0xF1); break; // Zamiana na "ń"
          case (char)0x9B: text.setCharAt(i, 0x9C); break; // Zamiana na "ś"
          case (char)0xBB: text.setCharAt(i, 0xAF); break; // Zamiana na "Ż"
          case (char)0xBC: text.setCharAt(i, 0xBF); break; // Zamiana na "ż"
          case (char)0x83: text.setCharAt(i, 0xD1); break; // Zamiana na "Ń"
          case (char)0x9A: text.setCharAt(i, 0x97); break; // Zamiana na "Ś"
          case (char)0x81: text.setCharAt(i, 0xA3); break; // Zamiana na "Ł"
          case (char)0xB9: text.setCharAt(i, 0xAC); break; // Zamiana na "Ź"
        }
        text.remove(i+1, 1);
        break;
    }
  }
}


// Funkcja wyświetlająca numer banku na pełnym ekranie
void displayBank()
{
  String bankNrStr = String(bank_nr);  // Zamiana liczby banku na ciąg znaków

  u8g2.clearBuffer();
  u8g2.setFont(spleen6x12PL);
  u8g2.drawStr(32, 25, "NR  BANKU ");

  // Przekazujemy bankNrStr jako ciąg znaków do wyświetlacza
  u8g2.drawStr(58, 40, bankNrStr.c_str());  // Wyświetlenie numeru banku
  u8g2.sendBuffer();
}

// Funkcja do ustawienia głośności na żądaną wartość
void volumeSet()
{
  timeDisplay = false;  // Wyłączanie wyświetlania czasu
  displayActive = true;  // Ustawienie flagi aktywności wyświetlacza
  displayStartTime = millis();  // Zapisanie czasu rozpoczęcia wyświetlania
  
  // Sprawdzenie, czy volumeValue mieści się w zakresie 0-21
  if (volumeValue < 0) volumeValue = 0;
  if (volumeValue > 21) volumeValue = 21;

  Serial.print("Wartość głośności: ");
  Serial.println(volumeValue);
  
  // Ustawienie głośności
  audio.setVolume(volumeValue); // dopuszczalny zakres 0...21
  
  // Wyświetlanie głośności na ekranie
  String volumeValueStr = String(volumeValue);  // Zamiana liczby VOLUME na ciąg znaków
  u8g2.clearBuffer();
  u8g2.setFont(spleen6x12PL);
  u8g2.drawStr(32, 25, "VOLUME SET");
  u8g2.drawStr(58, 50, volumeValueStr.c_str());
  u8g2.sendBuffer();
  saveVolumeSettings(station_nr, volumeValue, bank_nr);
}


// Obsługa regulacji głośności z pilota zdalnego sterowania
void volumeSetFromRemote()
{
  if (IRvolumeUp == true)  // Przycisk VOL+ w pilocie
  {
    IRvolumeUp = false;
    volumeValue++;
    if (volumeValue > 18)
    {
      volumeValue = 18;
    }
    volumeSet();
  }

  if (IRvolumeDown == true)  // Przycisk VOL- w pilocie
  {
    IRvolumeDown = false;
    volumeValue--;
    if (volumeValue < 3)
    {
      volumeValue = 3;
    }
    volumeSet();
  }
}


// Zapisywanie na karcie SD wartości głośności dla wybranej stacji z aktualnego banku
void saveVolumeSettings(int station, int volume, int bank)
{
  // Sprawdzenie czy stacja mieści się w zakresie
  if (station < 0 || station >= 100)
  {
    Serial.println("Błąd: Indeks stacji poza zakresem.");
    return;
  }

  // Generowanie nazwy pliku na podstawie numeru banku (np. /volume_bank01.txt)
  String fileNameString = "/volume_bank";
  if (bank < 10)
  {
    fileNameString += "0";  // Dodajemy '0', jeśli numer banku jest mniejszy niż 10
  }
  fileNameString += String(bank) + ".txt";

  // Otwieramy plik do zapisu
  File file = SD.open(fileNameString, FILE_WRITE);
  if (file)
  {
    // Aktualizujemy tylko wartość dla danej stacji (station_nr - 1, aby użyć właściwego indeksu tablicy)
    volumeArray[station - 1] = volume;  // Zapisywanie głośności dla wybranej stacji

    // Zapisujemy całą tablicę głośności do pliku
    for (int i = 0; i < 100; i++)
    {
      file.print(volumeArray[i]);
      if (i < 99)
      {
        file.print(", ");
      }
      else
      {
        file.println(volumeArray[i]);
      }
    }

    file.close();
    Serial.println("Ustawienia głośności zostały zapisane.");

    // Drukowanie tablicy po zapisie
    Serial.println("Tablica głośności po zapisie:");
    for (int i = 0; i < 100; i++)
    {
      Serial.print(volumeArray[i]);
      Serial.print(", ");
    }
    Serial.println();
  }
  else
  {
    Serial.println("Błąd otwierania pliku do zapisu.");
  }
}


// Ładowanie zapisanych na karcie SD wartości głośności dla wybranej stacji z aktualnego banku
void loadVolumeSettings(int station, int bank)
{
  // Sprawdzamy, czy indeks stacji jest w zakresie
  if (station < 1 || station > 100)
  { 
    Serial.println("Błąd: Indeks stacji poza zakresem.");
    return;
  }

  // Generowanie nazwy pliku na podstawie numeru banku (np. /volume_bank01.txt)
  String fileNameString = "/volume_bank";
  if (bank < 10)
  {
    fileNameString += "0";  // Dodajemy '0', jeśli numer banku jest mniejszy niż 10
  }
  fileNameString += String(bank) + ".txt";

  // Sprawdzamy, czy plik z ustawieniami głośności istnieje
  if (SD.exists(fileNameString))
  {
    Serial.println("Plik głośności istnieje, próbuję otworzyć...");

    // Otwieramy plik do odczytu
    File file = SD.open(fileNameString, FILE_READ);
    if (file)
    {
      Serial.println("Ustawienia głośności wczytane.");

      int i = 0;
      while (file.available())
      {
        String line = file.readStringUntil(',');  // Czytamy do przecinka
        int volume = line.toInt();  // Przekształcamy string na int

        if (i < 100)
        {
          // Sprawdzenie aby zmieścić się w min / max głośności
          if (volume < 0) volume = 0;
          if (volume > 21) volume = 21;
          volumeArray[i] = volume;  // Zapisujemy odczytane wartości do tablicy
        }
        i++;
      }
      file.close();

      // Ustawiamy głośność dla wybranej stacji
      volumeValue = volumeArray[station - 1];
      Serial.print("Głośność dla stacji ");
      Serial.print(station);
      Serial.print(" została ustawiona na: ");
      Serial.println(volumeValue);
      // Ustawienie głośności
      audio.setVolume(volumeValue); // dopuszczalny zakres 0...21
    }
    else
    {
      Serial.println("Błąd otwierania pliku do odczytu.");
    }
  }
  else
  {
    Serial.println("Plik głośności nie istnieje.");
    
    // Tworzymy nowy plik z domyślnymi wartościami 12 dla wszystkich stacji
    Serial.println("Tworzenie nowego pliku z wartościami 12 dla wszystkich stacji...");

    File file = SD.open(fileNameString, FILE_WRITE);
    if (file)
    {
      // Ustawiamy wszystkie elementy tablicy na 12
      for (int i = 0; i < 100; i++)
      {
        volumeArray[i] = 12;  // Domyślna wartość głośności
        file.print(volumeArray[i]);
        if (i < 99)
        {
          file.print(", ");
        }
      }
      file.close();
      Serial.println("Plik z wartościami głośności 12 został zapisany.");

      // Ustawiamy głośność dla wybranej stacji
      volumeValue = volumeArray[station - 1];  // Pobieramy głośność dla wybranej stacji
      Serial.print("Głośność dla stacji ");
      Serial.print(station);
      Serial.print(" została ustawiona na: ");
      Serial.println(volumeValue);
      // Ustawienie głośności
      audio.setVolume(volumeValue); // dopuszczalny zakres 0...21
    }
    else
    {
      Serial.println("Błąd podczas tworzenia pliku.");
    }
  }

  // Drukowanie tablicy po odczycie
  Serial.println("Tablica głośności po odczycie:");
  for (int i = 0; i < 100; i++)
  {
    Serial.print(volumeArray[i]);
    Serial.print(", ");
  }
  Serial.println();
}


// Inicjalizacja karty SD wraz z pierwszyn utworzeniem wymaganych plików w głównym katalogu karty, jesli pliki już istnieją funkcja sprawdza ich obecność
void SDinit()
{
  if (!SD.begin(SD_CS))
  {
    Serial.println("Błąd inicjalizacji karty SD!");
    return;
  }
  Serial.println("Karta SD zainicjalizowana pomyślnie.");

  // Sprawdzanie pojemności i zajęctości karty SD
  unsigned long totalSpace = SD.cardSize() / (1024 * 1024);  // Całkowita pojemność karty w MB
  unsigned long usedSpace = SD.usedBytes() / (1024 * 1024);   // Użyta przestrzeń w MB
  unsigned long freeSpace = totalSpace - usedSpace;  // Wolna przestrzeń w MB

  Serial.print("Całkowita pojemność karty SD: ");
  Serial.print(totalSpace);
  Serial.println(" MB");

  Serial.print("Użyte miejsce: ");
  Serial.print(usedSpace);
  Serial.println(" MB");

  Serial.print("Wolne miejsce: ");
  Serial.print(freeSpace);
  Serial.println(" MB");

  // Sprawdzenie i ewentualne utworzenie plików
  if (!SD.exists("/station_nr.txt"))
  {
    // Plik station_nr.txt nie istnieje, tworzymy go i zapisujemy wartość 9
    File stationFile = SD.open("/station_nr.txt", FILE_WRITE);
    if (stationFile)
    {
      stationFile.println("9");
      stationFile.close();
      Serial.println("Plik station_nr.txt został utworzony.");
    }
    else
    {
      Serial.println("Błąd podczas tworzenia pliku station_nr.txt!");
    }
  }
  else
  {
    Serial.println("Plik station_nr.txt już istnieje.");
  }

  if (!SD.exists("/bank_nr.txt"))
  {
    // Plik bank_nr.txt nie istnieje, tworzymy go i zapisujemy wartość 1
    File bankFile = SD.open("/bank_nr.txt", FILE_WRITE);
    if (bankFile)
    {
      bankFile.println("1");
      bankFile.close();
      Serial.println("Plik bank_nr.txt został utworzony.");
    }
    else
    {
      Serial.println("Błąd podczas tworzenia pliku bank_nr.txt!");
    }
  }
  else
  {
    Serial.println("Plik bank_nr.txt już istnieje.");
  }
}


void setup()
{
  // Ustaw pin CS dla karty SD jako wyjście i ustaw go na wysoki stan
  pinMode(SD_CS, OUTPUT);
  digitalWrite(SD_CS, HIGH);

  // Konfiguruj piny enkodera jako wejścia
  pinMode(CLK_PIN1, INPUT);
  pinMode(DT_PIN1, INPUT);
  pinMode(CLK_PIN2, INPUT);
  pinMode(DT_PIN2, INPUT);

    // Inicjalizacja przycisków enkoderów jako wejścia
  pinMode(SW_PIN1, INPUT_PULLUP);
  pinMode(SW_PIN2, INPUT_PULLUP);

  pinMode(recv_pin, INPUT); // Ustawienie pinu odbiornika IR jako wejście

  // Przerwanie na zmianę stanu pinu (odczyt impulsu)
  attachInterrupt(digitalPinToInterrupt(recv_pin), pulseISR, CHANGE);

  // Odczytaj początkowy stan pinu CLK enkodera
  prev_CLK_state1 = digitalRead(CLK_PIN1);
  prev_CLK_state2 = digitalRead(CLK_PIN2);

  audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT); // Konfiguruj pinout dla interfejsu I2S audio
  audio.setVolume(volumeValue); // Ustaw głośność na podstawie wartości zmiennej volumeValue w zakresie 0...21

  // Inicjalizacja SPI dla karty SD
  SPI.begin(SD_SCLK, SD_MISO, SD_MOSI, SD_CS);

  // SPI.setFrequency(1000000);
  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.setClock(400000);  // Ustawienie prędkości I2C

  // Inicjalizuj komunikację szeregową
  Serial.begin(115200);

  // Rezerwuje pamięć PSRAM dla bufora o określonej długości
  // 'ps_malloc' alokuje pamięć w PSRAM (jeśli dostępna) zamiast standardowej RAM
  // 'PSRAM_lenght' to liczba bajtów, jaką chcemy zaalokować
  psramData = (unsigned char *)ps_malloc(PSRAM_lenght * sizeof(unsigned char));

  // Sprawdzamy, czy pamięć PSRAM została poprawnie zainicjowana
  if (psramInit())
  {
    Serial.println("Pamięć PSRAM zainicjowana poprawnie");
    Serial.print("Dostepna pamięć PSRAM:");
    Serial.println(ESP.getPsramSize());
    Serial.print("Wolna pamięć PSRAM:");
    Serial.println(ESP.getFreePsram());
  }
  else
  {
    Serial.println("Błąd pamięci PSRAM");
  }
  
  // Inicjalizacja karty SD wraz z pierwszyn utworzeniem wymaganych plików w głównym katalogu karty, jesli pliki już istnieją funkcja sprawdza ich obecność
  SDinit();

  audioBuffer.changeMaxBlockSize(16384);  // Wywołanie metody na obiekcie audioBuffer // is default 1600 for mp3 and aac, set 16384 for FLAC 

  Serial.print("Numer seryjny ESP:");
  Serial.println(ESP.getEfuseMac()); // Wyświetla unikalny adres MAC ESP32
  
  // Inicjalizuj pamięć EEPROM z odpowiednim rozmiarem
  EEPROM.begin(MAX_STATIONS * STATION_NAME_LENGTH); // 100 * 42

  // Inicjalizuj wyświetlacz i odczekaj 250 milisekund na włączenie
  u8g2.begin();
  
  u8g2.clearBuffer();	
  u8g2.setFont(spleen6x12PL);
  u8g2.drawStr(0, 30, "   INTERNET RADIO    ");
  u8g2.sendBuffer();	

  button2.setDebounceTime(50);  // Ustawienie czasu debouncingu dla przycisku enkodera 2



  // Inicjalizacja WiFiManagera
  WiFiManager wifiManager;

  // Odczytaj numer banku i numer stacji z karty SD
  readStationFromSD();
  previous_bank_nr = bank_nr; // Wyrównanie numerów banku przy starcie

  // Rozpoczęcie konfiguracji Wi-Fi i połączenie z siecią
  if (wifiManager.autoConnect("ESP Internet Radio"))
  {
    Serial.println("Połączono z siecią WiFi");
    u8g2.clearBuffer();	
    u8g2.setFont(spleen6x12PL);
    u8g2.drawStr(0, 30, "   WI-FI CONNECTED   ");
    u8g2.sendBuffer();
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    timer1.attach(1, updateTimer);   // Ustaw timer, aby wywoływał funkcję updateTimer co sekundę
    fetchStationsFromServer();
    changeStation();
  }
  else
  {
    Serial.println("Brak połączenia z siecią WiFi");
    u8g2.clearBuffer();	
    u8g2.setFont(spleen6x12PL);
    u8g2.drawStr(0, 30, " WI-FI NOT CONNECTED ");
    u8g2.sendBuffer();
  }
}

void loop()
{
  audio.loop();            // Wykonuje główną pętlę dla obiektu audio (np. odtwarzanie dźwięku, obsługa audio)
  button1.loop();          // Wykonuje pętlę dla obiektu button1 (sprawdza stan przycisku z enkodera 1)
  button2.loop();          // Wykonuje pętlę dla obiektu button2 (sprawdza stan przycisku z enkodera 2)
  handleButtons();         // Wywołuje funkcję obsługującą przyciski i wykonuje odpowiednie akcje (np. zmiana opcji, wejście do menu)
  processIRCode();         // Funkcja przypisująca odpowiednie flagi do użytych przyciskow z pilota zdalnego sterowania
  volumeSetFromRemote();   // Obsługa regulacji głośności z pilota zdalnego sterowania
  vTaskDelay(1);           // Krótkie opóźnienie, oddaje czas procesora innym zadaniom

  CLK_state1 = digitalRead(CLK_PIN1);  // Odczytanie aktualnego stanu pinu CLK enkodera 1
  if (CLK_state1 != prev_CLK_state1 && CLK_state1 == HIGH)  // Sprawdzenie, czy stan CLK zmienił się na wysoki
  {
    timeDisplay = false;
    displayActive = true;
    displayStartTime = millis();
    if (menuEnable == true)  // Przewijanie menu prawym enkoderem
    {
      int DT_state1 = digitalRead(DT_PIN1);

      // Obsługa przewijania menu enkoderem
      if (DT_state1 == HIGH)
      {
        // Obrót w jedną stronę - inkrementacja
        currentOption = static_cast<MenuOption>((static_cast<int>(currentOption) + 1) % 2);  // Cykl pomiędzy 0 a 1
      }
      else
      {
        // Obrót w drugą stronę - dekrementacja
        currentOption = static_cast<MenuOption>((static_cast<int>(currentOption) - 1 + 2) % 2);  // Cykl pomiędzy 0 a 1
      }

      // Wyświetl menu po zmianie opcji
      displayMenu();
    }

    else  // Regulacja głośności
    {
      if (digitalRead(DT_PIN1) == HIGH)
      {
        volumeValue--;
        if (volumeValue < 3)
        {
          volumeValue = 3;
        }
      } 
      else
      {
        volumeValue++;
        if (volumeValue > 18)
        {
          volumeValue = 18;
        }
      }
      volumeSet();
    }
  }
  prev_CLK_state1 = CLK_state1;

  CLK_state2 = digitalRead(CLK_PIN2);  // Odczytanie aktualnego stanu pinu CLK enkodera 2
  if (CLK_state2 != prev_CLK_state2 && CLK_state2 == HIGH)  // Sprawdzenie, czy stan CLK zmienił się na wysoki
  {
    timeDisplay = false;  // Wyłączanie wyświetlania czasu
    displayActive = true;  // Ustawienie flagi aktywności wyświetlacza
    displayStartTime = millis();  // Zapisanie czasu rozpoczęcia wyświetlania

    if ((currentOption == INTERNET_RADIO) && (bankChange == false))  // Przewijanie listy stacji radiowych
    {
      station_nr = currentSelection + 1;
      if (digitalRead(DT_PIN2) == HIGH)  // Obracanie w lewo
      {
        station_nr--;
        if (station_nr < 1)
        {
          station_nr = stationsCount;
        }
        Serial.print("Numer stacji do tyłu: ");
        Serial.println(station_nr);
        scrollUp();  // Wywołanie funkcji przewijania w górę
      }
      else  // Obracanie w prawo
      {
        station_nr++;
        if (station_nr > stationsCount)
        {
          station_nr = 1;
        }
        Serial.print("Numer stacji do przodu: ");
        Serial.println(station_nr);
        scrollDown();  // Wywołanie funkcji przewijania w dół
      }
      displayStations();  // Aktualizacja wyświetlacza z listą stacji
    }

    if (bankChange == true)  // Przewijanie listy banków stacji radiowych
    {
      if (digitalRead(DT_PIN2) == HIGH)  // Obracanie w lewo
      {
        bank_nr--;
        if (bank_nr < 1)
        {
          bank_nr = 16;
        }
      } 
      else  // Obracanie w prawo
      {
        bank_nr++;
        if (bank_nr > 16)
        {
          bank_nr = 1;
        }
      }
      displayBank();
    }
  }
  prev_CLK_state2 = CLK_state2;  // Zapisanie aktualnego stanu CLK jako poprzedni stan

  // Przywracanie poprzedniej zawartości ekranu po 6 sekundach
  if (displayActive && (millis() - displayStartTime >= displayTimeout))   
  {
    displayTimeout = 6000;
    displayActive = false;
    timeDisplay = true;
    listedStations = false;
    menuEnable = false;
    bankChange = false;
    currentOption = INTERNET_RADIO;
    bank_nr = previous_bank_nr;
    displayRadio();
  }
  
  if ((currentOption == PLAY_FILES) && (menuEnable == true) && (button1.isPressed() || IRokButton == true))
  {
    IRokButton = false;
    menuEnable = false;
    if (!SD.begin(SD_CS))
    {
      Serial.println("Błąd inicjalizacji karty SD!");
      return;
    }
    folderIndex = 0;
    currentSelection = 0;
    firstVisibleLine = 1;
    String text = "  ŁADOWANIE FOLDERÓW Z KARTY SD, CZEKAJ... ";
    processText(text);  // Podstawienie polskich znaków diakrytycznych
    u8g2.clearBuffer();
    u8g2.setFont(spleen6x12PL);
    u8g2.setCursor(0, 10);
    u8g2.print(text);
    u8g2.sendBuffer();
    listDirectories("/");
    audio.stopSong();
    volumeValue = 15;
    audio.setVolume(volumeValue);
    playFromSelectedFolder();
  }

  if ((currentOption == INTERNET_RADIO) && (menuEnable == true) && (button1.isPressed() || IRokButton == true))
  {
    IRokButton = false;
    menuEnable = false;
    volumeValue = 12;
    audio.setVolume(volumeValue); // dopuszczalny zakres 0...21
    changeStation();
  }

  if ((currentOption == INTERNET_RADIO) && button2.isReleased() && (bankChange == false))
  {
    changeStation();
  }

  if ((bankChange == true) && button2.isPressed()) 
  {
    bankChange = false;
    u8g2.clearBuffer();
    u8g2.setFont(spleen6x12PL);
    u8g2.drawStr(0, 25, "POBIERANIE  STACJI");
    u8g2.drawStr(0, 50, "Z  SERWERA  GITHUB");
    u8g2.sendBuffer();
    currentSelection = 0;
    firstVisibleLine = 0;
    station_nr = 1;
    currentOption = INTERNET_RADIO;
    fetchStationsFromServer();
    changeStation();
    u8g2.clearBuffer();
    u8g2.setFont(spleen6x12PL);
    u8g2.sendBuffer();
  }

  if (IRrightArrow == true)  // Prawy przycisk kierunkowy w pilocie
  {
    IRrightArrow = false;
    station_nr++;
    if (station_nr > stationsCount)
    {
      station_nr = 1;
    }
    Serial.print("Numer stacji do przodu: ");
    Serial.println(station_nr);
    changeStation();
  }

  if (IRleftArrow == true)  // Lewy przycisk kierunkowy w pilocie
  {
    IRleftArrow = false;
    station_nr--;
    if (station_nr < 1)
    {
      station_nr = stationsCount;
    }
    Serial.print("Numer stacji do tyłu: ");
    Serial.println(station_nr);
    changeStation();
  }

  if (IRmenuButton == true)  // Przycisk MODE w pilocie
  {
    IRmenuButton = false;
    timeDisplay = false;
    menuEnable = true;
    displayActive = true;
    displayStartTime = millis();
    currentOption = static_cast<MenuOption>((static_cast<int>(currentOption) + 1) % 2);  // Przełączanie między opcjami do wyboru
    displayMenu();
  }

  if (IRdownArrow == true)  // Dolny przycisk kierunkowy w pilocie
  {
    IRdownArrow = false;
    bank_nr = previous_bank_nr;
    timeDisplay = false;
    displayActive = true;
    displayStartTime = millis();
    station_nr = currentSelection + 1;
    station_nr++;
    if (station_nr > stationsCount) 
    {
      station_nr = 1;
    }
    Serial.print("Numer stacji do do przodu: ");
    Serial.println(station_nr);
    scrollDown(); 
    displayStations();
  }

  if (IRupArrow == true)  // Górny przycisk kierunkowy w pilocie
  {
    IRupArrow = false;
    bank_nr = previous_bank_nr;
    timeDisplay = false;
    displayActive = true;
    displayStartTime = millis();
    station_nr = currentSelection + 1;
    station_nr--;
    if (station_nr < 1) 
    {
      station_nr = stationsCount;
    }
    Serial.print("Numer stacji do tyłu: ");
    Serial.println(station_nr);
    scrollUp(); 
    displayStations();
  }

  if (IRokButton == true)  // Przycisk OK w pilocie
  {
    IRokButton = false;
    if (bankChange == true)
    {
      bankChange = false;
      currentSelection = 0;
      firstVisibleLine = 0;
      station_nr = 1;
      fetchStationsFromServer();
    }
    changeStation();
  }

  if (IRbankUp == true)  // Przycisk FAV+ w pilocie
  {
    IRbankUp = false;
    bankChange = true;
    timeDisplay = false;
    displayActive = true;
    displayStartTime = millis();
    bank_nr++;
    if (bank_nr > 16)
    {
      bank_nr = 1;
    }
    displayBank(); 
  }

  if (IRbankDown == true)  // Przycisk FAV- w pilocie
  {
    IRbankDown = false;
    bankChange = true;
    timeDisplay = false;
    displayActive = true;
    displayStartTime = millis();
    bank_nr--;
    if (bank_nr < 1)
    {
      bank_nr = 16;
    }
    displayBank();
  }


}