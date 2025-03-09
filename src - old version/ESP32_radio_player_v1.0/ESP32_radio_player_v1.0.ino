
#include "Arduino.h"              // Standardowy nagłówek Arduino, który dostarcza podstawowe funkcje i definicje
#include "Audio.h"                // Biblioteka do obsługi funkcji związanych z dźwiękiem i audio
#include "SPI.h"                  // Biblioteka do obsługi komunikacji SPI
#include "SD.h"                   // Biblioteka do obsługi kart SD
#include "FS.h"                   // Biblioteka do obsługi systemu plików
#include <Adafruit_SH110X.h>      // Biblioteka do obsługi wyświetlaczy OLED z kontrolerem SH1106
#include <ezButton.h>             // Biblioteka do obsługi enkodera z przyciskiem
#include <HTTPClient.h>           // Biblioteka do wykonywania żądań HTTP
#include <EEPROM.h>               // Biblioteka do obsługi pamięci EEPROM
#include <Ticker.h>               // Mechanizm tickera do odświeżania timera 1s
#include <WiFiManager.h>          // Biblioteka do zarządzania konfiguracją sieci WiFi, opis jak ustawić połączenie WiFi przy pierwszym uruchomieniu jest opisany tu: https://github.com/tzapu/WiFiManager

#define SD_CS         47          // Pin CS (Chip Select) do komunikacji z kartą SD, wybierany jako interfejs SPI
#define SPI_MOSI      48          // Pin MOSI (Master Out Slave In) dla interfejsu SPI
#define SPI_MISO      0           // Pin MISO (Master In Slave Out) dla interfejsu SPI
#define SPI_SCK       45          // Pin SCK (Serial Clock) dla interfejsu SPI
#define I2S_DOUT      13          // połączenie do pinu DIN na DAC
#define I2S_BCLK      12          // połączenie po pinu BCK na DAC
#define I2S_LRC       14          // połączenie do pinu LCK na DAC
#define i2c_Address 0x3C          // inicjalizacja wyświetlacza
#define SCREEN_WIDTH 128          // OLED display width, in pixels
#define SCREEN_HEIGHT 64          // OLED display height, in pixels
#define OLED_RESET -1             // Pin resetu dla wyświetlacza OLED, -1 oznacza brak fizycznego podłączenia pinu resetu
#define CLK_PIN1 6                // Podłączenie z pinu 6 do CLK na enkoderze prawym
#define DT_PIN1  5                // Podłączenie z pinu 5 do DT na enkoderze prawym
#define SW_PIN1  4                // Podłączenie z pinu 4 do SW na enkoderze prawym (przycisk)
#define CLK_PIN2 10               // Podłączenie z pinu 10 do CLK na enkoderze
#define DT_PIN2  11               // Podłączenie z pinu 11 do DT na enkoderze lewym
#define SW_PIN2  1                // Podłączenie z pinu 1 do SW na enkoderze lewym (przycisk)
#define MAX_STATIONS 100          // Maksymalna liczba stacji radiowych, które mogą być przechowywane w jednym banku
#define MAX_LINK_LENGTH 100       // Maksymalna długość linku do stacji radiowej.
#define STATIONS_URL    "https://raw.githubusercontent.com/sarunia/ESP32_stream/main/ulubione"    // Adres URL do pliku z listą stacji radiowych.
#define STATIONS_URL1   "https://raw.githubusercontent.com/sarunia/ESP32_stream/main/lista1"      // Adres URL do pliku z listą stacji radiowych.
#define STATIONS_URL2   "https://raw.githubusercontent.com/sarunia/ESP32_stream/main/lista2"      // Adres URL do pliku z listą stacji radiowych.
#define STATIONS_URL3   "https://raw.githubusercontent.com/sarunia/ESP32_stream/main/lista3"      // Adres URL do pliku z listą stacji radiowych.
#define STATIONS_URL4   "https://raw.githubusercontent.com/sarunia/ESP32_stream/main/lista4"      // Adres URL do pliku z listą stacji radiowych.
#define STATIONS_URL5   "https://raw.githubusercontent.com/sarunia/ESP32_stream/main/lista5"      // Adres URL do pliku z listą stacji radiowych.
#define STATIONS_URL6   "https://raw.githubusercontent.com/sarunia/ESP32_stream/main/lista6"      // Adres URL do pliku z listą stacji radiowych.
#define STATIONS_URL7   "https://raw.githubusercontent.com/sarunia/ESP32_stream/main/lista7"      // Adres URL do pliku z listą stacji radiowych.
#define STATIONS_URL8   "https://raw.githubusercontent.com/sarunia/ESP32_stream/main/lista8"      // Adres URL do pliku z listą stacji radiowych.
#define STATIONS_URL9   "https://raw.githubusercontent.com/sarunia/ESP32_stream/main/lista9"      // Adres URL do pliku z listą stacji radiowych.
#define STATIONS_URL10  "https://raw.githubusercontent.com/sarunia/ESP32_stream/main/lista10"     // Adres URL do pliku z listą stacji radiowych.
#define STATIONS_URL11  "https://raw.githubusercontent.com/sarunia/ESP32_stream/main/lista11"     // Adres URL do pliku z listą stacji radiowych.
#define STATIONS_URL12  "https://raw.githubusercontent.com/sarunia/ESP32_stream/main/lista12"     // Adres URL do pliku z listą stacji radiowych.
#define STATIONS_URL13  "https://raw.githubusercontent.com/sarunia/ESP32_stream/main/lista13"     // Adres URL do pliku z listą stacji radiowych.
#define STATIONS_URL14  "https://raw.githubusercontent.com/sarunia/ESP32_stream/main/lista14"     // Adres URL do pliku z listą stacji radiowych.
#define STATIONS_URL15  "https://raw.githubusercontent.com/sarunia/ESP32_stream/main/lista15"     // Adres URL do pliku z listą stacji radiowych.
#define LICZNIK_S1 17             // Numer pinu dla enkodera/licznika S1
#define LICZNIK_S2 18             // Numer pinu dla enkodera/licznika S2
#define LICZNIK_S3 15             // Numer pinu dla enkodera/licznika S3
#define LICZNIK_S4 16             // Numer pinu dla enkodera/licznika S4
#define MAX_FILES 100             // Maksymalna liczba plików lub katalogów w tablicy directories

int currentSelection = 0;         // Numer aktualnego wyboru na ekranie OLED
int firstVisibleLine = 0;         // Numer pierwszej widocznej linii na ekranie OLED
int button_S1 = 17;               // Przycisk S1 podłączony do pinu 17
int button_S2 = 18;               // Przycisk S2 podłączony do pinu 18
int button_S3 = 15;               // Przycisk S3 podłączony do pinu 15
int button_S4 = 16;               // Przycisk S4 podłączony do pinu 16
int station_nr = 9;               // Numer aktualnie wybranej stacji radiowej z listy, domyślnie stacja nr 9
int stationFromBuffer = 0;        // Numer stacji radiowej przechowywanej w buforze do przywrocenia na ekran po bezczynności
int bank_nr = 1;                  // Numer aktualnie wybranego banku stacji z listy, domyślnie bank nr 1
int bankFromBuffer = 0;           // Numer aktualnie wybranego banku stacji z listy do przywrocenia na ekran po bezczynności
int CLK_state1;                   // Aktualny stan CLK enkodera prawego
int prev_CLK_state1;              // Poprzedni stan CLK enkodera prawego    
int CLK_state2;                   // Aktualny stan CLK enkodera lewego
int prev_CLK_state2;              // Poprzedni stan CLK enkodera lewego          
int counter = 0;                  // Licznik dla przycisków
int stationsCount = 0;            // Aktualna liczba przechowywanych stacji w tablicy
int directoryCount = 0;           // Licznik katalogów
int fileIndex = 0;                // Numer aktualnie wybranego pliku audio ze wskazanego folderu
int folderIndex = 0;              // Numer aktualnie wybranego folderu podczas przełączenia do odtwarzania z karty SD
int totalFilesInFolder = 0;       // Zmienna przechowująca łączną liczbę plików w folderze
int volumeValue = 12;             // Wartość głośności, domyślnie ustawiona na 12
const int maxVisibleLines = 5;    // Maksymalna liczba widocznych linii na ekranie OLED
bool button_1 = false;            // Flaga określająca stan przycisku 1
bool button_2 = false;            // Flaga określająca stan przycisku 2
bool button_3 = false;            // Flaga określająca stan przycisku 3
bool button_4 = false;            // Flaga określająca stan przycisku 4
bool encoderButton1 = false;      // Flaga określająca, czy przycisk enkodera 1 został wciśnięty
bool encoderButton2 = false;      // Flaga określająca, czy przycisk enkodera 2 został wciśnięty
bool fileEnd = false;             // Flaga sygnalizująca koniec odtwarzania pliku audio
bool displayActive = false;       // Flaga określająca, czy wyświetlacz jest aktywny
bool isPlaying = false;           // Flaga określająca, czy obecnie trwa odtwarzanie
bool mp3 = false;                 // Flaga określająca, czy aktualny plik audio jest w formacie MP3
bool flac = false;                // Flaga określająca, czy aktualny plik audio jest w formacie FLAC
bool aac = false;                 // Flaga określająca, czy aktualny plik audio jest w formacie AAC
bool noID3data = false;           // Flaga określająca, czy plik audio posiada dane ID3
bool timeDisplay = true;          // Flaga określająca kiedy pokazać czas na wyświetlaczu, domyślnie od razu po starcie
bool listedStations = false;      // Flaga określająca czy na ekranie jest pokazana lista stacji do wyboru
bool menuEnable = false;          // Flaga określająca czy na ekranie można wyświetlić menu
unsigned long lastDebounceTime = 0;       // Czas ostatniego debouncingu
unsigned long debounceDelay = 300;        // Czas trwania debouncingu w milisekundach
unsigned long displayTimeout = 6000;      // Czas wyświetlania komunikatu na ekranie w milisekundach
unsigned long displayStartTime = 0;       // Czas rozpoczęcia wyświetlania komunikatu
unsigned long seconds = 0;                // Licznik sekund timera

String directories[MAX_FILES];            // Tablica z indeksami i ścieżkami katalogów
String currentDirectory = "/";            // Ścieżka bieżącego katalogu
String stationName;                       // Nazwa aktualnie wybranej stacji radiowej
String stationString;                     // Dodatkowe dane stacji radiowej (jeśli istnieją)
String bitrateString;                     // Zmienna przechowująca informację o bitrate
String sampleRateString;                  // Zmienna przechowująca informację o sample rate
String bitsPerSampleString;               // Zmienna przechowująca informację o liczbie bitów na próbkę
String artistString;                      // Zmienna przechowująca informację o wykonawcy
String titleString;                       // Zmienna przechowująca informację o tytule utworu
String fileNameString;                    // Zmienna przechowująca informację o nazwie pliku

File myFile; // Uchwyt pliku

Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);    //Inicjalizacja obiektu wyświetlacza OLED
ezButton button1(SW_PIN1);                // Utworzenie obiektu przycisku z enkodera 1 ezButton, podłączonego do pinu 4
ezButton button2(SW_PIN2);                // Utworzenie obiektu przycisku z enkodera 1 ezButton, podłączonego do pinu 1
Audio audio;                              // Obiekt do obsługi funkcji związanych z dźwiękiem i audio
Ticker timer;                             // Obiekt do obsługi timera

char stations[MAX_STATIONS][MAX_LINK_LENGTH + 1];   // Tablica przechowująca linki do stacji radiowych (jedna na stację) +1 dla terminatora null

const char* ntpServer = "pool.ntp.org";      // Adres serwera NTP używany do synchronizacji czasu
const long  gmtOffset_sec = 3600;            // Przesunięcie czasu UTC w sekundach
const int   daylightOffset_sec = 3600;       // Przesunięcie czasu letniego w sekundach, dla Polski to 1 godzina

enum MenuOption
{
  PLAY_FILES,          // Odtwarzacz plików
  INTERNET_RADIO,      // Radio internetowe
  BANK_LIST,           // Lista banków stacji radiowych
};
MenuOption currentOption = INTERNET_RADIO;  // Aktualnie wybrana opcja menu (domyślnie radio internetowe)

bool isAudioFile(const char *filename)
{
  // Dodaj więcej rozszerzeń plików audio, jeśli to konieczne
  return (strstr(filename, ".mp3") || strstr(filename, ".MP3") || strstr(filename, ".wav") || strstr(filename, ".WAV") || strstr(filename, ".flac") || strstr(filename, ".FLAC"));
}

void IRAM_ATTR zlicz_S1() // funkcja obsługi przerwania z przycisku S1
{  
  if ((millis() - lastDebounceTime) > debounceDelay)
  {
    lastDebounceTime = millis(); // Zapisujemy czas ostatniego debouncingu
    button_1 = true;
  }
}

void IRAM_ATTR zlicz_S2() // funkcja obsługi przerwania z przycisku S2
{    
  if ((millis() - lastDebounceTime) > debounceDelay)
  {
    lastDebounceTime = millis(); // Zapisujemy czas ostatniego debouncingu
    button_2 = true;
  }
}

void IRAM_ATTR zlicz_S3() // funkcja obsługi przerwania z przycisku S3
{  
  if ((millis() - lastDebounceTime) > debounceDelay)
  {
    lastDebounceTime = millis(); // Zapisujemy czas ostatniego debouncingu
    button_3 = true;
  }
}

void IRAM_ATTR zlicz_S4() // funkcja obsługi przerwania z przycisku S4
{    
  if ((millis() - lastDebounceTime) > debounceDelay)
  {
    lastDebounceTime = millis(); // Zapisujemy czas ostatniego debouncingu
    button_4 = true;
  }
}

void printLocalTime()
{
  // Struktura przechowująca informacje o czasie
  struct tm timeinfo;

  // Sprawdź, czy udało się pobrać czas z lokalnego zegara czasu rzeczywistego
  if (!getLocalTime(&timeinfo))
  {
    // Wyświetl komunikat o niepowodzeniu w pobieraniu czasu
    Serial.println("Nie udało się uzyskać czasu");
    return; // Zakończ funkcję, gdy nie udało się uzyskać czasu
  }

  // Konwertuj godzinę, minutę i sekundę na stringi w formacie "HH:MM:SS"
  char timeString[9]; // Bufor przechowujący czas w formie tekstowej
  snprintf(timeString, sizeof(timeString), "%02d:%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
  display.print(timeString);
  display.display();
}

//Funkcja odpowiedzialna za zapisywanie informacji o stacji do pamięci EEPROM.
void saveStationToEEPROM(const char* station)
{   
  // Sprawdź, czy istnieje jeszcze miejsce na kolejną stację w pamięci EEPROM.
  if (stationsCount < MAX_STATIONS)
  {
    int length = strlen(station);

    // Sprawdź, czy długość linku nie przekracza ustalonego maksimum.
    if (length <= MAX_LINK_LENGTH)
    {
      // Zapisz długość linku jako pierwszy bajt.
      EEPROM.write(stationsCount * (MAX_LINK_LENGTH + 1), length);

      // Zapisz link jako kolejne bajty w pamięci EEPROM.
      for (int i = 0; i < length; i++)
      {
        EEPROM.write(stationsCount * (MAX_LINK_LENGTH + 1) + 1 + i, station[i]);
      }

      // Potwierdź zapis do pamięci EEPROM.
      EEPROM.commit();

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
  } else
  {
    // Informacja o błędzie w przypadku osiągnięcia maksymalnej liczby stacji.
    Serial.println("Błąd: Osiągnięto maksymalną liczbę zapisanych stacji");
  }
}

// Funkcja odpowiedzialna za zmianę aktualnie wybranej stacji radiowej.
void changeStation()
{  
  mp3 = flac = aac = false;
  stationString.remove(0);  // Usunięcie wszystkich znaków z obiektu stationString

  // Odczytaj link stacji o aktualnym numerze station_nr
  char station[MAX_LINK_LENGTH + 1];
  memset(station, 0, sizeof(station));

  // Odczytaj długość linku
  int length = EEPROM.read((station_nr - 1) * (MAX_LINK_LENGTH + 1));

  // Odczytaj link jako bajty
  for (int j = 0; j < length; j++)
  {
    station[j] = EEPROM.read((station_nr - 1) * (MAX_LINK_LENGTH + 1) + 1 + j);
  }

  // Skopiuj pierwsze 21 znaków do zmiennej stationName - będzie to wyświetlone w pierwszej linii na wyświetlaczu
  stationName = String(station).substring(0, 21);

  // Ręczne przycinanie znaków na końcu linku
  int lastValidCharIndex = length - 1;
  while (lastValidCharIndex >= 0 && (station[lastValidCharIndex] < 33 || station[lastValidCharIndex] > 126))
  {
    station[lastValidCharIndex] = '\0';
    lastValidCharIndex--;
  }

  // Weryfikacja, czy w linku znajduje się "http" lub "https"
  char* validLink = strstr(station, "http://");
  if (validLink == NULL)
  {
    validLink = strstr(station, "https://");
  }

  if (validLink != NULL) 
  {
    // Ustawienie station na początek właściwego linku
    strcpy(station, validLink);

    // Wydrukuj nazwę stacji i link na serialu
    Serial.print("Aktualnie wybrana stacja: ");
    Serial.println(station_nr);
    Serial.print("Link do stacji: ");
    Serial.println(station);

    // Wyświetl informacje na ekranie OLED
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SH110X_WHITE);
    display.setCursor(0, 0);
    display.println(stationName);
    display.display();

    // Połącz z daną stacją
    audio.connecttohost(station);
    seconds = 0;
    stationFromBuffer = station_nr;
    bankFromBuffer = bank_nr;
    saveStationOnSD();
  }
  else
  {
    Serial.println("Błąd: link stacji nie zawiera 'http' lub 'https'");
  }
}

void fetchStationsFromServer()
{
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
    Serial.println("Stacje pobrane z serwera:");

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
        //Serial.print("Nowa stacja: ");
        //Serial.println(station);
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
  char sanitizedStation[MAX_LINK_LENGTH + 1];
  
  // Indeks pomocniczy dla przetwarzania
  int j = 0;

  // Przeglądaj każdy znak stacji i sprawdź czy jest to drukowalny znak ASCII
  for (int i = 0; i < MAX_LINK_LENGTH && station[i] != '\0'; i++)
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
  // Wyświetl informacje w konsoli szeregowej
  Serial.print("info        ");
  Serial.println(info);
  // Znajdź pozycję "BitRate:" w tekście
  int bitrateIndex = String(info).indexOf("BitRate:");
  if (bitrateIndex != -1)
  {
    // Przytnij tekst od pozycji "BitRate:" do końca linii
    bitrateString = String(info).substring(bitrateIndex + 8, String(info).indexOf('\n', bitrateIndex));
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
    noID3data = true;
    Serial.println("Brak ID3 - nazwa pliku: " + fileNameString);
    if (fileNameString.length() > 63)
    {
      fileNameString = String(fileNameString).substring(0, 63);
    }
  }

  if (String(info).indexOf("MP3Decoder") != -1)
  {
    mp3 = true;
    flac = false;
    aac = false;
  }

  if (String(info).indexOf("FLACDecoder") != -1)
  {
    flac = true;
    mp3 = false;
    aac = false;
  }

  if (String(info).indexOf("AACDecoder") != -1)
  {
    aac = true;
    flac = false;
    mp3 = false;
  }

  if ((currentOption == INTERNET_RADIO) && ((mp3 == true) || (flac == true) || (aac == true)))
  {
    
    display.setTextSize(1);
    display.setTextColor(SH110X_WHITE);
    for (int y = 37; y <= 54; y++)
    {
      for (int x = 0; x < 127; x++)
      {
        display.drawPixel(x, y, SH110X_BLACK);
      }
    }
    display.setCursor(0, 37);
    display.println(sampleRateString.substring(1) + "Hz " + bitsPerSampleString + "bit");
    
    display.setCursor(0, 47);
    display.println(bitrateString.substring(1) + "b/s  Bank " + String(bank_nr));
    for (int y = 55; y <= 63; y++)
    {
      for (int x = 51; x < 127; x++)
      {
        display.drawPixel(x, y, SH110X_BLACK);
      }
    }
    display.setCursor(66, 56);
    display.println("Stacja " + String(station_nr));
    //display.display();
  }

  if (currentOption == PLAY_FILES)
  {
    display.setTextSize(1);
    display.setTextColor(SH110X_WHITE);
    if (noID3data == true)
    {
      for (int y = 9; y <= 36; y++)
      {
        for (int x = 0; x < 127; x++)
        {
          display.drawPixel(x, y, SH110X_BLACK);
        }
      }
      display.setCursor(0, 10);
      display.println(fileNameString);
    }
    for (int y = 37; y <= 54; y++)
    {
      for (int x = 0; x < 127; x++)
      {
        display.drawPixel(x, y, SH110X_BLACK);
      }
    }
    display.setCursor(0, 37);
    display.println(sampleRateString.substring(1) + "Hz " + bitsPerSampleString + "bit");
    
    display.setCursor(0, 47);
    display.println(bitrateString.substring(1) + "b/s Plik " + String(fileIndex) + "/" + String(totalFilesInFolder));
    for (int y = 56; y <= 63; y++)
    {
      for (int x = 51; x < 127; x++)
      {
        display.drawPixel(x, y, SH110X_BLACK);
      }
    }
    display.setCursor(66, 56);
    display.println("Folder " + String(folderIndex));
    display.display();
    seconds = 0;
  }
}

void processText(String &text)
{
  for (int i = 0; i < text.length(); i++)
  {
    switch (text[i])
    {
      case (char)0xC2:
        switch (text[i+1])
        {
          case (char)0xB3: text.setCharAt(i, 0x10); break; // Zamiana na pojedynczy znak "ł"
        }
        text.remove(i+1, 1); // Usunięcie kolejnego bajtu
        break;
      case (char)0xC3:
        switch (text[i+1])
        {
          case (char)0xB1: text.setCharAt(i, 0x0E); break; // Zamiana na pojedynczy znak "ń"
          case (char)0xB3: text.setCharAt(i, 0x0F); break; // Zamiana na pojedynczy znak "ó"
          case (char)0xBA: text.setCharAt(i, 0x16); break; // Zamiana na pojedynczy znak "ź"
          case (char)0xBB: text.setCharAt(i, 0x1D); break; // Zamiana na pojedynczy znak "Ż"
          
        }
        text.remove(i+1, 1); // Usunięcie kolejnego bajtu
        break;
      case (char)0xC4:
        switch (text[i+1])
        {
          case (char)0x85: text.setCharAt(i, 0x11); break; // Zamiana na pojedynczy znak "ą"
          case (char)0x99: text.setCharAt(i, 0x13); break; // Zamiana na pojedynczy znak "ę"
          case (char)0x87: text.setCharAt(i, 0x14); break; // Zamiana na pojedynczy znak "ć"
          case (char)0x84: text.setCharAt(i, 0x19); break; // Zamiana na pojedynczy znak "Ą"
          case (char)0x98: text.setCharAt(i, 0x1A); break; // Zamiana na pojedynczy znak "Ę"
          case (char)0x86: text.setCharAt(i, 0x1B); break; // Zamiana na pojedynczy znak "Ć"
        }
        text.remove(i+1, 1); // Usunięcie kolejnego bajtu
        break;
      case (char)0xC5:
        switch (text[i+1])
        {
          case (char)0x82: text.setCharAt(i, 0x10); break; // Zamiana na pojedynczy znak "ł"
          case (char)0x84: text.setCharAt(i, 0x0E); break; // Zamiana na pojedynczy znak "ń"
          case (char)0x9B: text.setCharAt(i, 0x12); break; // Zamiana na pojedynczy znak "ś"
          case (char)0xBB: text.setCharAt(i, 0x1D); break; // Zamiana na pojedynczy znak "Ż"
          case (char)0xBC: text.setCharAt(i, 0x15); break; // Zamiana na pojedynczy znak "ż"
          case (char)0x83: text.setCharAt(i, 0x17); break; // Zamiana na pojedynczy znak "Ń"
          case (char)0x9A: text.setCharAt(i, 0x18); break; // Zamiana na pojedynczy znak "Ś"
          case (char)0x81: text.setCharAt(i, 0x1C); break; // Zamiana na pojedynczy znak "Ł"
          case (char)0xB9: text.setCharAt(i, 0x1E); break; // Zamiana na pojedynczy znak "Ź"
        }
        text.remove(i+1, 1); // Usunięcie kolejnego bajtu
        break;
    }
  }
}

void audio_id3data(const char *info)
{
  Serial.print("id3data     ");
  Serial.println(info);
  
  // Znajdź pozycję "Artist: " lub "ARTIST: " w tekście
  int artistIndex = String(info).indexOf("Artist: ");
  if (artistIndex == -1)
  {
    artistIndex = String(info).indexOf("ARTIST: ");
  }

  if (artistIndex != -1)
  {
    // Przytnij tekst od pozycji "Artist:" do końca linii
    artistString = String(info).substring(artistIndex + 8, String(info).indexOf('\n', artistIndex));
    Serial.println("Znalazłem artystę: " + artistString);

    // Pomocnicza pętla w celu wyłapania bajtów artistString na serial terminalu 
    /*for (int i = 0; i < artistString.length(); i++) {
      Serial.print("0x");
      if (artistString[i] < 0x10) {
        Serial.print("0"); // Dodaj zero przed pojedynczymi cyframi w formacie hex
      }
      Serial.print(artistString[i], HEX); // Drukowanie znaku jako wartość hex
      Serial.print(" "); // Dodanie spacji po każdym bajcie
    }
    Serial.println(); // Nowa linia po zakończeniu drukowania bajtów*/

    processText(artistString);
  }
  
  // Znajdź pozycję "Title: " lub "TITLE " w tekście
  int titleIndex = String(info).indexOf("Title: ");
  if (titleIndex == -1)
  {
    titleIndex = String(info).indexOf("TITLE: ");
  }
  
  if (titleIndex != -1)
  {
    // Przytnij tekst od pozycji "Title: " do końca linii
    titleString = String(info).substring(titleIndex + 7, String(info).indexOf('\n', titleIndex));
    Serial.println("Znalazłem tytuł: " + titleString);

    // Pomocnicza pętla w celu wyłapania bajtów titleString na serial terminalu 
    /*for (int i = 0; i < titleString.length(); i++) {
      Serial.print("0x");
      if (titleString[i] < 0x10) {
        Serial.print("0"); // Dodaj zero przed pojedynczymi cyframi w formacie hex
      }
      Serial.print(titleString[i], HEX); // Drukowanie znaku jako wartość hex
      Serial.print(" "); // Dodanie spacji po każdym bajcie
    }
    Serial.println(); // Nowa linia po zakończeniu drukowania bajtów*/

    processText(titleString);
  }
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0, 0);
  display.println("   Odtwarzam plik:   ");

  if (artistString.length() > 21)
  {
    artistString = artistString.substring(0, 21); // Ogranicz długość tekstu do 21 znaków dla wyświetlacza OLED
  }
  display.setCursor(0, 10);
  display.println(artistString);

  if (titleString.length() > 42)
  {
    titleString = titleString.substring(0, 42); // Ogranicz długość tekstu do 42 znaków dla wyświetlacza OLED
  }
  display.setCursor(0, 19);
  display.println(titleString);
  display.display();
}

void audio_bitrate(const char *info)
{
  Serial.print("bitrate     ");
  Serial.println(info);
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
  stationString = String(info);

  if (stationString.length() > 62)
  {
    stationString = stationString.substring(0, 62); // Ogranicz długość tekstu do 63 znaków dla wyświetlacza OLED
  }

  // Pomocnicza pętla w celu wyłapania bajtów stationString na serial terminalu 
  /*for (int i = 0; i < stationString.length(); i++)  // Pętla iteruje przez każdy znak w `stationString`
  {
    Serial.print("0x"); // Wyświetla prefiks "0x" przed wartością heksadecymalną
    if (stationString[i] < 0x10)
    {
      Serial.print("0"); // Dodaj zero przed pojedynczymi cyframi w formacie hex
    }
    Serial.print(stationString[i], HEX); // // Wyświetla wartość znaku w formacie heksadecymalnym.
    Serial.print(" "); // Dodanie spacji po każdym bajcie
  }
  Serial.println(); // Nowa linia po zakończeniu drukowania bajtów*/

  processText(stationString);   // Wywołuje funkcję `processText`, która przetwarza tekst zawarty w `stationString`

  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  for (int y=9; y<=36; y++)
  {
    for (int x=0; x<127; x++)
    {
    display.drawPixel(x, y, SH110X_BLACK); 
    }
  }   
  display.setCursor(0, 10);
  display.println(stationString);
  display.display();
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
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(35, 0);
  display.println("MENU");
  display.setTextSize(1);
  display.setCursor(0, 20);
  switch (currentOption)
  {
    case PLAY_FILES:
      display.println(">> Odtwarzacz plik" + String((char)0x0F) + "w");
      display.println("   Radio internetowe");
      display.println("   Lista bank"  + String((char)0x0F) + "w");
      break;
    case INTERNET_RADIO:
      display.println("   Odtwarzacz plik" + String((char)0x0F) + "w");
      display.println(">> Radio internetowe");
      display.println("   Lista bank"  + String((char)0x0F) + "w");
      break;
    case BANK_LIST:
      display.println("   Odtwarzacz plik" + String((char)0x0F) + "w");
      display.println("   Radio internetowe");
      display.println(">> Lista bank"  + String((char)0x0F) + "w");
      break;
  }
  display.display();
}

void printDirectoriesAndSavePaths(File dir, int numTabs, String currentPath)
{
  directoryCount = 0;
  while (true)
  {
    // Otwórz kolejny plik w katalogu
    File entry = dir.openNextFile();
    
    // Jeżeli nie ma więcej plików, przerwij pętlę
    if (!entry)
    {
      break;
    }

    // Sprawdź, czy to katalog
    if (entry.isDirectory())
    {
      // Utwórz pełną ścieżkę do bieżącego katalogu
      String path = currentPath + "/" + entry.name();

      // Zapisz pełną ścieżkę do tablicy
      directories[directoryCount] = path;
      
      // Wydrukuj numer indeksu i pełną ścieżkę
      Serial.print(directoryCount);
      Serial.print(": ");
      Serial.println(path.substring(1));
      
      // Zwiększ licznik katalogów
      directoryCount++;
      
      // Rekurencyjnie wywołaj funkcję dla podkatalogu
      //printDirectoriesAndSavePaths(entry, numTabs + 1, path);
      
      // Jeżeli to nie katalog System Volume Information, wydrukuj na ekranie OLED
      if (path != "/System Volume Information")
      {
        for (int i = 1; i < 7; i++)
        {
          // Przygotuj pełną ścieżkę dla wyświetlenia
          String fullPath = directories[i];
          
          // Ogranicz długość do 21 znaków
          fullPath = fullPath.substring(1, 22);

          // Pomocnicza pętla w celu wyłapania bajtów fullPath na serial terminalu 
          /*for (int i = 0; i < fullPath.length(); i++)
          {
            Serial.print("0x");
            if (fullPath[i] < 0x10)
            {
              Serial.print("0"); // Dodaj zero przed pojedynczymi cyframi w formacie hex
            }
            Serial.print(fullPath[i], HEX); // Drukowanie znaku jako wartość hex
            Serial.print(" "); // Dodanie spacji po każdym bajcie
          }
          Serial.println(); // Nowa linia po zakończeniu drukowania bajtów*/

          processText(fullPath);
          
          // Ustaw pozycję kursora na ekranie OLED
          display.setCursor(0, i * 9);
          
          // Wydrukuj skróconą ścieżkę na ekranie OLED
          display.print(fullPath);
        }
      }
      // Wyświetl aktualny stan ekranu OLED
      display.display();
    }
    // Zamknij plik
    entry.close();
  }
}

void listDirectories(const char *dirname)
{
  File root = SD.open(dirname);
  if (!root)
  {
    Serial.println("Błąd otwarcia katalogu!");
    return;
  }
  printDirectoriesAndSavePaths(root, 0, ""); // Początkowo pełna ścieżka jest pusta
  Serial.println("Wylistowano katalogi z karty SD");
  root.close();
  scrollDown();
  printFoldersToOLED();
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
  // Dodaj dodatkowy wydruk do diagnostyki
  Serial.print("Scroll Up: CurrentSelection = ");
  Serial.println(currentSelection);
}

void scrollDown()
{
  if (currentSelection < maxSelection())
  {
    currentSelection++;
    if (currentSelection >= firstVisibleLine + maxVisibleLines)
    {
      firstVisibleLine++;
    }
    // Dodaj dodatkowy wydruk do diagnostyki
    Serial.print("Scroll Down: CurrentSelection = ");
    Serial.println(currentSelection);
  }
}

int maxSelection()
{
  if (currentOption == INTERNET_RADIO)
  {
    return stationsCount - 1;
  }
  else if (currentOption == PLAY_FILES)
  {
    return directoryCount - 1;
  }
  return 0; // Zwraca 0, jeśli żaden warunek nie jest spełniony
}

void playFromSelectedFolder()
{
  
  String folder = directories[folderIndex];

  Serial.println("Odtwarzanie plików z wybranego folderu: " + folder);

  File root = SD.open(folder);

  if (!root) {
    Serial.println("Błąd otwarcia katalogu!");
    return;
  }
  
  totalFilesInFolder = 0;
  fileIndex = 1; // Domyślnie start odtwarzania od pierwszego pliku audio w folderze
  while (File entry = root.openNextFile())  // Zliczanie wszystkich plików audio w folderze
  {
    String fileName = entry.name();
    if (isAudioFile(fileName.c_str()))
    {
      totalFilesInFolder++;
      //Serial.println(fileName);
    }
    entry.close();
  }
  root.rewindDirectory(); // Przewiń katalog na początek

  File entry;

  while (entry = root.openNextFile())
  {
    String fileName = entry.name();

    // Pomijaj pliki, które nie są w zadeklarowanym formacie audio
    if (!isAudioFile(fileName.c_str()))
    {
      Serial.println("Pominięto plik: " + fileName);
      continue;
    }
    fileNameString = fileName;
    Serial.print("Odtwarzanie pliku: ");
    Serial.print(fileIndex); // Numeracja pliku
    Serial.print("/");
    Serial.print(totalFilesInFolder); // Łączna liczba plików w folderze
    Serial.print(" - ");
    Serial.println(fileName);
    
    // Pełna ścieżka do pliku
    String fullPath = folder + "/" + fileName;
    // Odtwarzaj plik
    audio.connecttoFS(SD, fullPath.c_str());

    isPlaying = true;
    noID3data = false;

    // Oczekuj, aż odtwarzanie się zakończy
    while (isPlaying)
    {
      audio.loop(); // Tutaj obsługujemy odtwarzacz w tle
      button1.loop();
      button2.loop();

      if (button_1) //Przejście do kolejnego pliku w folderze
      {
        counter = 0;
        button_1 = false;
        isPlaying = false;
        audio.stopSong();
        fileIndex++;
        if (fileIndex > totalFilesInFolder)
        {
          Serial.println("To był ostatni plik w folderze");
          folderIndex++;
          playFromSelectedFolder();
        }
        break;            // Wyjdź z pętli
      }

      if (button_2) //Przejście do poprzedniego pliku w folderze
      {
        counter = 0;
        button_2 = false;
        audio.stopSong();
        fileIndex--;
        if (fileIndex < 1)
        {
          fileIndex = 1;
        }
        // Odtwórz znaleziony plik
        root.rewindDirectory(); // Przewiń katalog na początek
        entry = root.openNextFile(); // Otwórz pierwszy plik w katalogu

        // Przesuń się do wybranego pliku
        for (int i = 1; i < fileIndex; i++)
        {
          entry = root.openNextFile();
          if (!entry)
          {
              break; // Wyjdź, jeśli nie znaleziono pliku
          }
        }
        
        // Sprawdź, czy udało się otworzyć plik
        if (entry)
        {
          // Zaktualizuj pełną ścieżkę do pliku
          String fullPath = folder + "/" + entry.name();

          // Odtwórz tylko w przypadku, gdy to jest szukany plik
          if (isAudioFile(entry.name()))
          {
            audio.connecttoFS(SD, fullPath.c_str());
            isPlaying = true;
            Serial.print("Odtwarzanie pliku: ");
            Serial.print(fileIndex); // Numeracja pliku
            Serial.print("/");
            Serial.print(totalFilesInFolder); // Łączna liczba plików w folderze
            Serial.print(" - ");
            Serial.println(fileName);
          }
        }
      } 

      if (fileEnd == true) // Przejście do odtwarzania następnego pliku
      {
        fileEnd = false;
        button_1 = true;
      }

      CLK_state1 = digitalRead(CLK_PIN1);
      if (CLK_state1 != prev_CLK_state1 && CLK_state1 == HIGH)
      {
        timeDisplay = false;
        displayActive = true;
        displayStartTime = millis();
        if (digitalRead(DT_PIN1) == HIGH)
        {
          volumeValue--;
          if (volumeValue < 5)
          {
            volumeValue = 5;
          }
        }
        else
        {
          volumeValue++;
          if (volumeValue > 15)
          {
            volumeValue = 15;
          }
        }
        audio.setVolume(volumeValue); // zakres 0...21
        Serial.print("Wartość głośności: ");
        Serial.println(volumeValue);
        display.clearDisplay();
        display.setTextSize(2);
        display.setTextColor(SH110X_WHITE);
        display.setCursor(4, 0);
        display.println("Volume set");
        display.setTextSize(3);
        display.setCursor(48, 30);
        display.println(volumeValue);
        display.display();
      }
      prev_CLK_state1 = CLK_state1;

      CLK_state2 = digitalRead(CLK_PIN2);
      if (CLK_state2 != prev_CLK_state2 && CLK_state2 == HIGH)
      {
        folderIndex = currentSelection;
        timeDisplay = false;
        if (digitalRead(DT_PIN2) == HIGH)
        {
          folderIndex--;
          if (folderIndex < 1)
          {
            folderIndex = 1;
          }
          Serial.print("Numer folderu do tyłu: ");
          Serial.println(folderIndex);
          scrollUp();
          printFoldersToOLED();
        }
        else
        {
          folderIndex++;
          if (folderIndex > (directoryCount - 1))
          {
            folderIndex = directoryCount - 1;
          }
          Serial.print("Numer folderu przodu: ");
          Serial.println(folderIndex);
          scrollDown();
          printFoldersToOLED();
        }
        displayActive = true;
        displayStartTime = millis();
      }
      prev_CLK_state2 = CLK_state2;

      if (displayActive && (millis() - displayStartTime >= displayTimeout))   // Przywracanie poprzedniej zawartości ekranu po 6 sekundach
      {
        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(SH110X_WHITE);
        display.setCursor(0, 0);
        display.println("   Odtwarzam plik:   ");
        
        if (noID3data == true)
        {
          display.setCursor(0, 10);
          display.println(fileNameString);
        }
        else
        {
          display.setCursor(0, 10);
          display.println(artistString);
          display.setCursor(0, 19);
          display.println(titleString);
        }

        display.setCursor(0, 37);
        display.println(sampleRateString.substring(1) + "Hz " + bitsPerSampleString + "bit");
        display.setCursor(0, 47);
        display.println(bitrateString.substring(1) + "b/s Plik " + String(fileIndex) + "/" + String(totalFilesInFolder));
        display.setCursor(66, 56);
        display.println("Folder " + String(folderIndex));
        display.display();
        displayActive = false;
        timeDisplay = true;
        currentOption == PLAY_FILES;
      }

      if (button2.isPressed())
      {
        audio.stopSong();
        playFromSelectedFolder();
      }

      if (button1.isPressed())
      {
        audio.stopSong();
        display.clearDisplay();
        encoderButton1 = true;
        break;
      }
    }
    
    if (encoderButton1 == true)
    {
      encoderButton1 = false;
      displayMenu();
      break;
    }
  }
  root.close();
}

// Funkcja do drukowania folderów na ekranie OLED z uwzględnieniem zaznaczenia
void printFoldersToOLED()
{
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0, 0);
  display.println("   LISTA KATALOG" + String((char)0x1F) + "W"); // Wyświetla napis "LISTA KATALOGÓW" na ekranie, 0x1F reprezentuje literę 'Ó'
  display.println(currentDirectory);

  int displayRow = 1;  // Zaczynamy od drugiego wiersza (pierwszy to nagłówek)

  for (int i = firstVisibleLine; i < min(firstVisibleLine + 7, directoryCount); i++)
  {
    String fullPath = directories[i];

    // Pomijaj "System Volume Information"
    if (fullPath != "/System Volume Information")
    {
      // Sprawdź, czy ścieżka zaczyna się od aktualnego katalogu
      if (fullPath.startsWith(currentDirectory))
      {
        // Ogranicz długość do 21 znaków
        String displayedPath = fullPath.substring(currentDirectory.length(), currentDirectory.length() + 21);

        processText(displayedPath);

        // Podświetlenie zaznaczenia
        if (i == currentSelection)
        {
          display.setTextColor(SH110X_BLACK, SH110X_WHITE);
        }
        else
        {
          display.setTextColor(SH110X_WHITE);
        }

        // Wyświetl wiersz
        display.setCursor(0, displayRow * 9);
        display.print(displayedPath);
        for (int y = 61; y <= 63; y++) // Wygaszenie 2 ostatnich linii wyświetlacza
        {
          for (int x = 0; x < 127; x++)
          {
            display.drawPixel(x, y, SH110X_BLACK);
          }
        }
        // Przesuń się do kolejnego wiersza
        displayRow++;
      }
    }
  }
  // Przywróć domyślne kolory tekstu
  display.setTextColor(SH110X_WHITE);
  display.display();
}

// Funkcja do drukowania listy stacji radiowych na ekranie OLED z uwzględnieniem zaznaczenia
void printStationsToOLED()
{
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0, 0);
  display.println("STACJE RADIOWE " + String(station_nr) + "/" + String(stationsCount));

  int displayRow = 1;  // Zaczynamy od drugiego wiersza (pierwszy to nagłówek)

  for (int i = firstVisibleLine; i < min(firstVisibleLine + 7, stationsCount); i++)
  {
    char station[MAX_LINK_LENGTH + 1];
    memset(station, 0, sizeof(station));

    // Odczytaj długość linku
    int length = EEPROM.read(i * (MAX_LINK_LENGTH + 1));

    // Odczytaj link jako bajty
    for (int j = 0; j < min(length, 21); j++)
    {
      station[j] = EEPROM.read(i * (MAX_LINK_LENGTH + 1) + 1 + j);
    }

    // Podświetlenie zaznaczenia
    if (i == currentSelection)
    {
      display.setTextColor(SH110X_BLACK, SH110X_WHITE);
    }
    else
    {
      display.setTextColor(SH110X_WHITE);
    }

    // Wyświetl nazwę stacji (pierwsze 21 znaków)
    display.setCursor(0, displayRow * 9);
    display.print(station);
    // Dodaj dodatkowy wydruk do diagnostyki
    //Serial.print("Wyświetlona stacja: ");
    //Serial.println(station);
    for (int y = 61; y <= 63; y++) // Wygaszenie 2 ostatnich linii wyświetlacza
    {
      for (int x = 0; x < 127; x++)
      {
        display.drawPixel(x, y, SH110X_BLACK);
      }
    }
    // Przesuń się do kolejnego wiersza
    displayRow++;
  }

  // Przywróć domyślne kolory tekstu
  display.setTextColor(SH110X_WHITE);
  display.display();
}

void updateTimer()  // Wywoływana co sekundę przez timer
{
  // Zwiększ licznik sekund
  seconds++;

  // Wyświetl aktualny czas w sekundach
  // Konwertuj sekundy na minutę i sekundy
  unsigned int minutes = seconds / 60;
  unsigned int remainingSeconds = seconds % 60;

  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);

  if (timeDisplay == true)
  {
    if (audio.isRunning() == true)
    {
      if (mp3 == true)
      {
        display.setCursor(102, 37);
        display.print("MP3");
        display.display();
      }
      if (flac == true)
      {
        display.setCursor(102, 37);
        display.print("FLAC");
        display.display();
      }
      if (aac == true)
      {
        display.setCursor(102, 37);
        display.print("AAC");
        display.display();
      }
    }

    display.setCursor(0, 56);
    for (int y = 55; y <= 63; y++)
    {
      for (int x = 0; x < 55; x++)
      {
        display.drawPixel(x, y, SH110X_BLACK);
      }
    }
    if (currentOption == PLAY_FILES)
    {
      // Formatuj czas jako "mm:ss"
      char timeString[10];
      snprintf(timeString, sizeof(timeString), "%02um:%02us", minutes, remainingSeconds);
      display.print(timeString);
      display.display();
    }
    if ((currentOption == INTERNET_RADIO) && ((mp3 == true) || (flac == true) || (aac == true)))
    {
      printLocalTime();
    }
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
      myFile.close();
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
      myFile.close();
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
      myFile.close();
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
      myFile.close();
      Serial.println("Utworzono i zapisano bank_nr.txt na karcie SD.");
    }
    else
    {
      Serial.println("Błąd podczas tworzenia pliku bank_nr.txt.");
    }
  }
}

void readStationFromSD()
{
  // Sprawdź, czy karta SD jest dostępna
  if (!SD.begin(47))
  {
    Serial.println("Nie można znaleźć karty SD. Ustawiam domyślne wartości.");
    station_nr = 9;
    bank_nr = 1;
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

  // Odczytaj początkowy stan pinu CLK enkodera
  prev_CLK_state1 = digitalRead(CLK_PIN1);
  prev_CLK_state2 = digitalRead(CLK_PIN2);

  // Ustaw pin S1, S2, S3, S4 jako wejścia z rezystorem pull-up
  pinMode(button_S1, INPUT_PULLUP);
  pinMode(button_S2, INPUT_PULLUP);
  pinMode(button_S3, INPUT_PULLUP);
  pinMode(button_S4, INPUT_PULLUP);

  // Przypnij przerwania do przycisków (wywołanie funkcji zlicz_S1 -- zlicz_S4 przy narastającym zboczu)
  attachInterrupt(LICZNIK_S1, zlicz_S1, RISING);
  attachInterrupt(LICZNIK_S2, zlicz_S2, RISING);
  attachInterrupt(LICZNIK_S3, zlicz_S3, RISING);
  attachInterrupt(LICZNIK_S4, zlicz_S4, RISING);

  audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT); // Konfiguruj pinout dla interfejsu I2S audio
  audio.setVolume(volumeValue); // Ustaw głośność na podstawie wartości zmiennej volumeValue w zakresie 0...21

  // Inicjalizuj interfejs SPI dla obsługi wyświetlacza OLED
  SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
  SPI.setFrequency(1000000);

  // Inicjalizuj komunikację szeregową (Serial)
  Serial.begin(115200);

  // Inicjalizuj pamięć EEPROM z odpowiednim rozmiarem
  EEPROM.begin((MAX_STATIONS * (MAX_LINK_LENGTH + 1)));

  // Oczekaj 250 milisekund na włączenie się wyświetlacza OLED
  delay(250);

  // Inicjalizuj wyświetlacz OLED z podanym adresem I2C
  display.begin(i2c_Address, true);
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(15, 5);
  display.println("Internet");
  display.setTextSize(2);
  display.setCursor(35, 35);
  display.println("Radio");
  display.display();
  // Inicjalizacja WiFiManagera
  WiFiManager wifiManager;

  readStationFromSD();

  // Rozpoczęcie konfiguracji Wi-Fi i połączenie z siecią, jeśli konieczne
  if (wifiManager.autoConnect("ESP Internet Radio"))
  {
    Serial.println("Połączono z siecią WiFi");
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SH110X_WHITE);
    display.setCursor(35, 5);
    display.println("Wi-Fi");
    display.setTextSize(2);
    display.setCursor(10, 35);
    display.println("connected");
    display.display();
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    timer.attach(1, updateTimer);   // Ustaw timer, aby wywoływał funkcję updateTimer co sekundę
    fetchStationsFromServer();
    changeStation();
  }
  else
  {
    Serial.println("Brak połączenia z siecią WiFi");
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SH110X_WHITE);
    display.setCursor(35, 5);
    display.println("Wi-Fi");
    display.setCursor(40, 25);
    display.println("not");
    display.setCursor(10, 45);
    display.println("connected");
    display.display();
  }
}

void loop()
{
  audio.loop();
  button1.loop();
  button2.loop();
  
  CLK_state1 = digitalRead(CLK_PIN1);
  if (CLK_state1 != prev_CLK_state1 && CLK_state1 == HIGH)
  {
    timeDisplay = false;
    displayActive = true;
    displayStartTime = millis();
    if (menuEnable == true)  // Przewijanie menu prawym enkoderem
    {
      int DT_state1 = digitalRead(DT_PIN1);
      switch(currentOption)
      {
        case PLAY_FILES:
          if (DT_state1 == HIGH)
          {
            currentOption = BANK_LIST;
          }
          else
          {
            currentOption = INTERNET_RADIO;
          }
          break;
          
        case INTERNET_RADIO:
          if (DT_state1 == HIGH)
          {
            currentOption = PLAY_FILES;
          }
          else
          {
            currentOption = BANK_LIST;
          }
          break;
          
        case BANK_LIST:
          if (DT_state1 == HIGH)
          {
            currentOption = INTERNET_RADIO;
          }
          else
          {
            currentOption = PLAY_FILES;
          }
          break;
      }
      displayMenu();
    }

    else  // Regulacja głośności
    {
      if (digitalRead(DT_PIN1) == HIGH)
      {
        volumeValue--;
        if (volumeValue < 5)
        {
          volumeValue = 5;
        }
      } 
      else
      {
        volumeValue++;
        if (volumeValue > 15)
        {
          volumeValue = 15;
        }
      }
      Serial.print("Wartość głośności: ");
      Serial.println(volumeValue);
      audio.setVolume(volumeValue); // zakres 0...21
      display.clearDisplay();
      display.setTextSize(2);
      display.setTextColor(SH110X_WHITE);
      display.setCursor(4, 0);
      display.println("Volume set");
      display.setTextSize(3);
      display.setCursor(48, 30);
      display.println(volumeValue);
      display.display();
    }
  }
  prev_CLK_state1 = CLK_state1;

  CLK_state2 = digitalRead(CLK_PIN2);
  if (CLK_state2 != prev_CLK_state2 && CLK_state2 == HIGH) 
  {
    timeDisplay = false;
    displayActive = true;
    displayStartTime = millis();

    if (currentOption == INTERNET_RADIO)  // Przewijanie listy stacji radiowych
    {
      station_nr = currentSelection + 1;
      if (digitalRead(DT_PIN2) == HIGH)
      {
        station_nr--;
        if (station_nr < 1)
        {
          station_nr = 1;
        }
        //Serial.print("Numer stacji do tyłu: ");
        //Serial.println(station_nr);
        scrollUp();
      }
      else
      {
        station_nr++;
        if (station_nr > stationsCount)
        {
          station_nr = stationsCount;
        }
        //Serial.print("Numer stacji do przodu: ");
        //Serial.println(station_nr);
        scrollDown();
      }
      printStationsToOLED();
    }

    if (currentOption == BANK_LIST) // Przewijanie listy banków stacji radiowych
    {
      if (digitalRead(DT_PIN2) == HIGH)
      {
        bank_nr--;
        if (bank_nr < 1)
        {
          bank_nr = 16;
        }
      } 
      else
      {
        bank_nr++;
        if (bank_nr > 16)
        {
          bank_nr = 1;
        }
      }
      Serial.print("Numer banku: ");
      Serial.println(bank_nr);
      display.clearDisplay();
      display.setTextSize(2);
      display.setTextColor(SH110X_WHITE);
      display.setCursor(25, 0);
      display.println("Bank nr");
      display.setTextSize(3);
      display.setCursor(55, 30);
      display.println(bank_nr);
      display.display();
    }
  }
  prev_CLK_state2 = CLK_state2;

  if (displayActive && (millis() - displayStartTime >= displayTimeout))   // Przywracanie poprzedniej zawartości ekranu po 6 sekundach
  {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SH110X_WHITE);
    display.setCursor(0, 0);
    display.println(stationName);
    display.setCursor(0, 10);
    display.println(stationString);
    display.setCursor(0, 37);
    display.println(sampleRateString.substring(1) + "Hz " + bitsPerSampleString + "bit");

    display.setCursor(102, 37);
    if (mp3 == true)
    {
      display.print("MP3");
    }
    if (flac == true)
    {
      display.print("FLAC");
    }
    if (aac == true)
    {
      display.print("AAC");
    }
    
    display.setCursor(0, 47);
    display.println(bitrateString.substring(1) + "b/s  Bank " + String(bankFromBuffer));
    display.setCursor(66, 56);
    display.println("Stacja " + String(stationFromBuffer));
    display.display();
    displayActive = false;
    timeDisplay = true;
    listedStations = false;
    menuEnable = false;
    currentOption = INTERNET_RADIO;
  }
  
  if ((currentOption == PLAY_FILES) && (button1.isPressed()) && (menuEnable == true))
  {
    if (!SD.begin(SD_CS))
    {
      Serial.println("Błąd inicjalizacji karty SD!");
      return;
    }
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SH110X_WHITE);
    display.setCursor(0, 0);
    display.println("   LISTA KATALOG" + String((char)0x1F) + "W"); // Wyświetla komunikat "LISTA KATALOGÓW" na ekranie, 0x1F reprezentuje literę 'Ó'
    display.display();
    folderIndex = 1;
    currentSelection = 0;
    firstVisibleLine = 0;
    listDirectories("/");
    playFromSelectedFolder();
  }

  if ((currentOption == INTERNET_RADIO) && (button1.isPressed()) && (menuEnable == true))
  {
    changeStation();
  }

  if (button1.isPressed())
  {
    timeDisplay = false;
    displayMenu();
    menuEnable = true;
    displayActive = true;
    displayStartTime = millis();
  }

  if ((currentOption == INTERNET_RADIO) && (button2.isPressed()))
  {
    changeStation();
  }

  if ((currentOption == BANK_LIST) && (button2.isPressed()))
  {
    display.clearDisplay();
    currentSelection = 0;
    firstVisibleLine = 0;
    station_nr = 1;
    currentOption = INTERNET_RADIO;
    fetchStationsFromServer();
    changeStation();
  }

  // Obsługa przycisku S1
  if (button_1)
  {
    counter = 0;
    button_1 = mp3 = aac = flac = false;
    Serial.println("Przycisk S1 został wciśnięty");
    if (currentOption == INTERNET_RADIO)
    {
      station_nr++;
      if (station_nr > stationsCount)
      {
        station_nr = 1;
      }
      Serial.println(station_nr);
      changeStation();
    }
  }

  // Obsługa przycisku S2
  if (button_2)
  {
    counter = 0;
    button_2 = mp3 = aac = flac = false;
    Serial.println("Przycisk S2 został wciśnięty");
    if (currentOption == INTERNET_RADIO)
    {
      station_nr--;
      if (station_nr < 1)
      {
        station_nr = stationsCount;
      }
      Serial.println(station_nr);
      changeStation();
    }
  }

  // Obsługa przycisku S3
  if (button_3)
  {
    counter = 0;
    button_3 = mp3 = aac = flac = false;
    Serial.println("Przycisk S3 został wciśnięty");
    if (currentOption == INTERNET_RADIO)
    {
      timeDisplay = false;
      currentSelection = 0;
      firstVisibleLine = 0;
      bank_nr++;
      if (bank_nr > 16)
      {
        bank_nr = 16;
      }
      station_nr = 1;
      Serial.println(bank_nr);
      display.clearDisplay();
      display.setTextSize(2);
      display.setTextColor(SH110X_WHITE);
      display.setCursor(25, 0);
      display.println("Bank nr");
      display.setTextSize(3);
      display.setCursor(55, 30);
      display.println(bank_nr);
      display.display();
      fetchStationsFromServer();
      changeStation();
      timeDisplay = true;
    }
  }

  // Obsługa przycisku S4
  if (button_4)
  {
    counter = 0;
    button_4 = mp3 = aac = flac = false;
    Serial.println("Przycisk S4 został wciśnięty");
    if (currentOption == INTERNET_RADIO)
    {
      timeDisplay = false;
      currentSelection = 0;
      firstVisibleLine = 0;
      bank_nr--;
      if (bank_nr < 1)
      {
        bank_nr = 1;
      }
      station_nr = 1;
      Serial.println(bank_nr);
      display.clearDisplay();
      display.setTextSize(2);
      display.setTextColor(SH110X_WHITE);
      display.setCursor(25, 0);
      display.println("Bank nr");
      display.setTextSize(3);
      display.setCursor(55, 30);
      display.println(bank_nr);
      display.display();
      fetchStationsFromServer();
      changeStation();
      timeDisplay = true;
    }
  }
}