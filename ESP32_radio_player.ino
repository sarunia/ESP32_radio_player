
#include "Arduino.h"    //Standardowy nagłówek Arduino, który dostarcza podstawowe funkcje i definicje
#include "WiFiMulti.h"    //Biblioteka do obsługi wielu połączeń WiFi
#include "Audio.h"    //Biblioteka do obsługi funkcji związanych z dźwiękiem i audio
#include "SPI.h"    //Biblioteka do obsługi komunikacji SPI
#include "SD.h"   //Biblioteka do obsługi kart SD
#include "FS.h"   //Biblioteka do obsługi systemu plików
#include <Adafruit_SH110X.h>    //Biblioteka do obsługi wyświetlaczy OLED z kontrolerem SH1106
#include <ezButton.h>  // Biblioteka do obsługi enkodera z przyciskiem
#include <HTTPClient.h>   //Biblioteka do wykonywania żądań HTTP
#include <EEPROM.h>   //Biblioteka do obsługi pamięci EEPROM
#include <Ticker.h>   // Mechanizm tickera (do odświeżania)
#include <algorithm>   // Standardowa biblioteka C++ do manipulacji danymi

#define SD_CS         47  // Pin CS (Chip Select) do komunikacji z kartą SD, wybierany jako interfejs SPI
#define SPI_MOSI      48  // Pin MOSI (Master Out Slave In) dla interfejsu SPI
#define SPI_MISO      0  // Pin MISO (Master In Slave Out) dla interfejsu SPI
#define SPI_SCK       45  // Pin SCK (Serial Clock) dla interfejsu SPI
#define I2S_DOUT      13 // połączenie do pinu DIN na DAC
#define I2S_BCLK      12 // połączenie po pinu BCK na DAC
#define I2S_LRC       14 // połączenie do pinu LCK na DAC
#define i2c_Address 0x3C // inicjalizacja wyświetlacza
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1  // Pin resetu dla wyświetlacza OLED, -1 oznacza brak fizycznego podłączenia pinu resetu
#define CLK_PIN1 6  // Podłączenie z pinu 6 do CLK na enkoderze prawym
#define DT_PIN1  5  // Podłączenie z pinu 5 do DT na enkoderze prawym
#define SW_PIN1  4  // Podłączenie z pinu 4 do SW na enkoderze prawym (przycisk)
#define CLK_PIN2 10  // Podłączenie z pinu 10 do CLK na enkoderze
#define DT_PIN2  11  // Podłączenie z pinu 11 do DT na enkoderze lewym
#define SW_PIN2  1  // Podłączenie z pinu 1 do SW na enkoderze lewym (przycisk)
#define MAX_STATIONS 100    // Maksymalna liczba stacji radiowych, które mogą być przechowywane w jednym banku
#define MAX_LINK_LENGTH 100   // Maksymalna długość linku do stacji radiowej.
#define STATIONS_URL "https://raw.githubusercontent.com/sarunia/ESP32_stream/main/ulubione"    // Adres URL do pliku z listą stacji radiowych.
#define STATIONS_URL1 "https://raw.githubusercontent.com/sarunia/ESP32_stream/main/lista1"    // Adres URL do pliku z listą stacji radiowych.
#define STATIONS_URL2 "https://raw.githubusercontent.com/sarunia/ESP32_stream/main/lista2"    // Adres URL do pliku z listą stacji radiowych.
#define STATIONS_URL3 "https://raw.githubusercontent.com/sarunia/ESP32_stream/main/lista3"    // Adres URL do pliku z listą stacji radiowych.
#define STATIONS_URL4 "https://raw.githubusercontent.com/sarunia/ESP32_stream/main/lista4"    // Adres URL do pliku z listą stacji radiowych.
#define STATIONS_URL5 "https://raw.githubusercontent.com/sarunia/ESP32_stream/main/lista5"    // Adres URL do pliku z listą stacji radiowych.
#define STATIONS_URL6 "https://raw.githubusercontent.com/sarunia/ESP32_stream/main/lista6"    // Adres URL do pliku z listą stacji radiowych.
#define STATIONS_URL7 "https://raw.githubusercontent.com/sarunia/ESP32_stream/main/lista7"    // Adres URL do pliku z listą stacji radiowych.
#define STATIONS_URL8 "https://raw.githubusercontent.com/sarunia/ESP32_stream/main/lista8"    // Adres URL do pliku z listą stacji radiowych.
#define STATIONS_URL9 "https://raw.githubusercontent.com/sarunia/ESP32_stream/main/lista9"    // Adres URL do pliku z listą stacji radiowych.
#define STATIONS_URL10 "https://raw.githubusercontent.com/sarunia/ESP32_stream/main/lista10"    // Adres URL do pliku z listą stacji radiowych.
#define STATIONS_URL11 "https://raw.githubusercontent.com/sarunia/ESP32_stream/main/lista11"    // Adres URL do pliku z listą stacji radiowych.
#define STATIONS_URL12 "https://raw.githubusercontent.com/sarunia/ESP32_stream/main/lista12"    // Adres URL do pliku z listą stacji radiowych.
#define STATIONS_URL13 "https://raw.githubusercontent.com/sarunia/ESP32_stream/main/lista13"    // Adres URL do pliku z listą stacji radiowych.
#define STATIONS_URL14 "https://raw.githubusercontent.com/sarunia/ESP32_stream/main/lista14"    // Adres URL do pliku z listą stacji radiowych.
#define STATIONS_URL15 "https://raw.githubusercontent.com/sarunia/ESP32_stream/main/lista15"    // Adres URL do pliku z listą stacji radiowych.
#define LICZNIK_S1 17   // Numer pinu dla enkodera/licznika S1
#define LICZNIK_S2 18   // Numer pinu dla enkodera/licznika S2
#define LICZNIK_S3 15   // Numer pinu dla enkodera/licznika S3
#define LICZNIK_S4 16   // Numer pinu dla enkodera/licznika S4
#define MAX_FILES 100  // Maksymalna liczba plików lub katalogów w tablicy directories

int directoryCount = 0; // Licznik katalogów
int currentFile = 0;  // Numer bieżącego pliku
int currentSelection = 0;  // Numer aktualnie zaznaczonego katalogu
int firstVisibleLine = 0;  // Numer pierwszej widocznej linii na ekranie OLED z wyborem katalogów odczytanych z karty SD
int button_S1 = 17;  // Przycisk S1 podłączony do pinu 17
int button_S2 = 18;  // Przycisk S2 podłączony do pinu 18
int button_S3 = 15;  // Przycisk S3 podłączony do pinu 15
int button_S4 = 16;  // Przycisk S4 podłączony do pinu 16
int station_nr = 0;  // Numer aktualnie wybranej stacji radiowej z listy
int bank_nr = 0; // Numer aktualnie wybranego banku stacji z listy
int counter = 12; // Początkowa środkowa wartość ustawienia poziomu głośności
int CLK_state1;    // Aktualny stan CLK enkodera prawego
int prev_CLK_state1;   // Poprzedni stan CLK enkodera prawego    
int CLK_state2;    // Aktualny stan CLK enkodera lewego
int prev_CLK_state2;   // Poprzedni stan CLK enkodera lewego          
int licznik_S1 = 0;  // Licznik dla przycisku S1
int licznik_S2 = 0;  // Licznik dla przycisku S2
int licznik_S3 = 0;  // Licznik dla przycisku S3
int licznik_S4 = 0;  // Licznik dla przycisku S4
int stationsCount = 0;    // Aktualna liczba przechowywanych stacji w tablicy
int filteredDirectoriesCount = 0; // Deklaracja globalnej zmiennej przechowującej ilość przefiltrowanych folderów
int currentFileIndex = 0;  // Numer aktualnie wybranego pliku audio ze wskazanego folderu
const int maxVisibleLines = 5;  // Maksymalna liczba widocznych linii na ekranie OLED
bool button_1 = false;    // Zmienna określająca stan przycisku 1
bool button_2 = false;    // Zmienna określająca stan przycisku 2
bool button_3 = false;    // Zmienna określająca stan przycisku 3
bool button_4 = false;    // Zmienna określająca stan przycisku 4
bool encoderButton1 = false;
bool encoderButton2 = false;
bool volume_set = false;    // Zmienna określająca, czy ustawiono poziom głośności.
bool wifi_config = false;   // Zmienna, która jest ustawiana na true po wykonaniu konfiguracji, aby włączyć moduł Wi-Fi i połączyć się z siecią.
bool endOfFile = false;  // Flaga końca odtwarzania pliku audio
bool displayActive = false;   // Flaga określająca, czy wyświetlacz jest aktywny.
bool isPlaying = false;
bool mp3, flac, aac, wav = false;

unsigned long lastDebounceTime_S1 = 0;    // Czas ostatniego debouncingu dla przycisku S1.
unsigned long lastDebounceTime_S2 = 0;    // Czas ostatniego debouncingu dla przycisku S2.
unsigned long lastDebounceTime_S3 = 0;    // Czas ostatniego debouncingu dla przycisku S3.
unsigned long lastDebounceTime_S4 = 0;    // Czas ostatniego debouncingu dla przycisku S4.
unsigned long debounceDelay = 200;  // Czas trwania debouncingu w milisekundach.
unsigned long displayTimeout = 5000;  // Czas wyświetlania komunikatu na ekranie w milisekundach.
unsigned long displayStartTime = 0;   // Czas rozpoczęcia wyświetlania komunikatu.
unsigned long seconds = 0;  // Licznik sekund timera

String directories[MAX_FILES]; // Tablica z indeksami i ścieżkami katalogów
String currentDirectory = "/";  // Ścieżka bieżącego katalogu
String station_name;    // Nazwa aktualnie wybranej stacji radiowej.
String station_data;    // Dodatkowe dane stacji radiowej (jeśli istnieją).
String bitrateString;         // Zmienna przechowująca informację o bitrate
String sampleRateString;      // Zmienna przechowująca informację o sample rate
String bitsPerSampleString;   // Zmienna przechowująca informację o liczbie bitów na próbkę
String artistString;         // Zmienna przechowująca informację o wykonawcy
String titleString;      // Zmienna przechowująca informację o tytule utworu
String path;  // Zmienna przechowująca ścieżkę dostępu do pliku
File root, myFile;  // Obiekty do obsługi plików na karcie SD

Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);    //Inicjalizacja obiektu wyświetlacza OLED
ezButton button1(SW_PIN1);  // Utworzenie obiektu przycisku z enkodera 1 ezButton, podłączonego do pinu 4
ezButton button2(SW_PIN2);  // Utworzenie obiektu przycisku z enkodera 1 ezButton, podłączonego do pinu 1
Audio audio;               // Obiekt do obsługi funkcji związanych z dźwiękiem i audio
WiFiMulti wifiMulti;       // Obiekt do obsługi wielu połączeń WiFi
Ticker timer;  // Obiekt do obsługi timera
String ssid =     "brakdostepu";
String password = "malinowykrul1977comeback";
char stations[MAX_STATIONS][MAX_LINK_LENGTH + 1];   // Tablica przechowująca linki do stacji radiowych (jedna na stację) +1 dla terminatora null.


enum MenuOption
{
  PLAY_FILES,          // Odtwarzacz plików
  INTERNET_RADIO       // Radio internetowe
};
MenuOption currentOption = INTERNET_RADIO;  // Aktualnie wybrana opcja menu (domyślnie radio internetowe)

bool isAudioFile(const char *filename)
{
  // Dodaj więcej rozszerzeń plików audio, jeśli to konieczne
  return (strstr(filename, ".mp3") || strstr(filename, ".wav") || strstr(filename, ".flac"));
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
      Serial.printf("[%d] %s\n", stationsCount + 1, station);

      // Zwiększ licznik zapisanych stacji.
      stationsCount++;
    } 
    else
    {
      // Informacja o błędzie w przypadku zbyt długiego linku do stacji.
      Serial.println("Błąd: Link do stacji jest zbyt długi.");
    }
  } else
  {
    // Informacja o błędzie w przypadku osiągnięcia maksymalnej liczby stacji.
    Serial.println("Błąd: Osiągnięto maksymalną liczbę zapisanych stacji.");
  }
}


//Funkcja odpowiedzialna za odczyt i wyświetlenie informacji o zapisanych stacjach z pamięci EEPROM
void readStationsFromEEPROM()
{   
  Serial.println("Zapisane stacje:");

  // Przeiteruj przez zapisane stacje w pamięci EEPROM.
  for (int i = 0; i < stationsCount; i++)
  {
    // Odczytaj długość linku dla aktualnej stacji.
    int length = EEPROM.read(i * (MAX_LINK_LENGTH + 1));

    // Inicjalizuj tablicę do przechowywania linku stacji.
    char station[MAX_LINK_LENGTH + 1];
    memset(station, 0, sizeof(station));

    // Odczytaj link stacji jako bajty.
    for (int j = 0; j < length; j++)
    {
      station[j] = EEPROM.read(i * (MAX_LINK_LENGTH + 1) + 1 + j);
    }

    // Wydrukuj informacje o zapisanej stacji na Serialu wraz z jej numerem.
    Serial.printf("[%d] %s\n", i + 1, station);
  }
}


// Funkcja odpowiedzialna za zmianę aktualnie wybranej stacji radiowej.
void changeStation()
{    
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


  // Ręczne przycinanie znaków na końcu linku
  int lastValidCharIndex = length - 1;
  while (lastValidCharIndex >= 0 && (station[lastValidCharIndex] < 33 || station[lastValidCharIndex] > 126))
  {
    station[lastValidCharIndex] = '\0';
    lastValidCharIndex--;
  }

  // Wydrukuj nazwę stacji i link na Serialu
  Serial.print("Aktualnie wybrana stacja: ");
  Serial.println(station_nr);
  Serial.print("Link do stacji: ");
  Serial.println(station);

  // Skopiuj pierwsze 21 znaków do zmiennej station_name
  station_name = String(station).substring(0, 21);

  // Wyświetl informacje na ekranie OLED
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0, 0);
  display.println(station_name);
  display.display();

  // Połącz z daną stacją
  audio.connecttohost(station);
}



void fetchStationsFromServer()
{
  // Utwórz obiekt klienta HTTP
  HTTPClient http;

  // Inicjalizuj żądanie HTTP do podanego adresu URL
  if(bank_nr == 0)
    http.begin(STATIONS_URL);
  if(bank_nr == 1)
    http.begin(STATIONS_URL1);
  if(bank_nr == 2)
    http.begin(STATIONS_URL2);
  if(bank_nr == 3)
    http.begin(STATIONS_URL3);
  if(bank_nr == 4)
    http.begin(STATIONS_URL4);
  if(bank_nr == 5)
    http.begin(STATIONS_URL5);
  if(bank_nr == 6)
    http.begin(STATIONS_URL6);
  if(bank_nr == 7)
    http.begin(STATIONS_URL7);
  if(bank_nr == 8)
    http.begin(STATIONS_URL8);
  if(bank_nr == 9)
    http.begin(STATIONS_URL9);
  if(bank_nr == 10)
    http.begin(STATIONS_URL10);
  if(bank_nr == 11)
    http.begin(STATIONS_URL11);
  if(bank_nr == 12)
    http.begin(STATIONS_URL12);
  if(bank_nr == 13)
    http.begin(STATIONS_URL13);
  if(bank_nr == 14)
    http.begin(STATIONS_URL14);
  if(bank_nr == 15)
    http.begin(STATIONS_URL15);  

  // Wykonaj żądanie GET i zapisz kod odpowiedzi HTTP
  int httpCode = http.GET();

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
        sanitizeAndSaveStation(station.c_str());
      }
      
      // Przesuń indeks początkowy do kolejnej linii
      startIndex = endIndex + 1;
    }
  } else {
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



void IRAM_ATTR zlicz_S1() // funkcja obsługi przerwania z przycisku S1
{  
  if ((millis() - lastDebounceTime_S1) > debounceDelay)
  {
    lastDebounceTime_S1 = millis(); // Zapisujemy czas ostatniego debouncingu
    button_1 = true;
  }
}



void IRAM_ATTR zlicz_S2() // funkcja obsługi przerwania z przycisku S2
{    
  if ((millis() - lastDebounceTime_S2) > debounceDelay)
  {
    lastDebounceTime_S2 = millis(); // Zapisujemy czas ostatniego debouncingu
    button_2 = true;
  }
}


void IRAM_ATTR zlicz_S3() // funkcja obsługi przerwania z przycisku S3
{  
  if ((millis() - lastDebounceTime_S3) > debounceDelay)
  {
    lastDebounceTime_S3 = millis(); // Zapisujemy czas ostatniego debouncingu
    button_3 = true;
  }
}



void IRAM_ATTR zlicz_S4() // funkcja obsługi przerwania z przycisku S4
{    
  if ((millis() - lastDebounceTime_S4) > debounceDelay)
  {
    lastDebounceTime_S4 = millis(); // Zapisujemy czas ostatniego debouncingu
    button_4 = true;
  }
}



void wifi_setup()
{
  WiFi.mode(WIFI_STA);  // Ustaw tryb WiFi na klienta (WIFI_STA)
  wifiMulti.addAP(ssid.c_str(), password.c_str());  // Dodaj dostęp do punktu dostępowego (AP) z zadanymi danymi (SSID i hasło)
  wifiMulti.run();  // Uruchom konfigurację wielokrotnego dostępu WiFi
  if(WiFi.status() != WL_CONNECTED) // Sprawdź, czy udało się połączyć z którąś z dostępnych sieci
  {
    WiFi.disconnect(true);  // Jeśli nie, rozłącz WiFi, ponownie uruchom konfigurację wielokrotnego dostępu i spróbuj ponownie
    wifiMulti.run();
  }
  Serial.println("Połączono z siecią WiFi");
  audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT); // Konfiguruj pinout dla interfejsu I2S audio
  audio.setVolume(counter); // Ustaw głośność na podstawie wartości zmiennej counter w zakresie 0...21
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(35, 5);
  display.println("Wi-Fi");
  display.setTextSize(2);
  display.setCursor(10, 35);
  display.println("connected");
  display.display();
  wifi_config = true; // ustawiono konfig wifi
}


void audio_info(const char *info)
{
  // Wyświetl informacje w konsoli szeregowej
  Serial.print("info        ");
  Serial.println(info);
  
  // Znajdź pozycję "BitRate:" w tekście
  int bitrateIndex = String(info).indexOf("BitRate:");
  if (bitrateIndex != -1) {
    // Przytnij tekst od pozycji "BitRate:" do końca linii
    bitrateString = String(info).substring(bitrateIndex + 8, String(info).indexOf('\n', bitrateIndex));
  }

  // Znajdź pozycję "SampleRate:" w tekście
  int sampleRateIndex = String(info).indexOf("SampleRate:");
  if (sampleRateIndex != -1) {
    // Przytnij tekst od pozycji "SampleRate:" do końca linii
    sampleRateString = String(info).substring(sampleRateIndex + 11, String(info).indexOf('\n', sampleRateIndex));
  }

  // Znajdź pozycję "BitsPerSample:" w tekście
  int bitsPerSampleIndex = String(info).indexOf("BitsPerSample:");
  if (bitsPerSampleIndex != -1) {
    // Przytnij tekst od pozycji "BitsPerSample:" do końca linii
    bitsPerSampleString = String(info).substring(bitsPerSampleIndex + 15, String(info).indexOf('\n', bitsPerSampleIndex));
  }

  if (String(info).indexOf("MP3Decoder") != -1)
  {
    mp3 = true;
    flac = false;
  }
  if (String(info).indexOf("FLACDecoder") != -1)
  {
    flac = true;
    mp3 = false;
  }
  
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
  display.println(sampleRateString.substring(1) + "Hz" + "  " + bitsPerSampleString + "bits");
  /*if (mp3 == true)
  {
    mp3 = false;
    display.println(sampleRateString.substring(1) + "Hz" + "  " + bitsPerSampleString + "bits   MP3");
  }
  if (flac == true)
  {
    flac = false;
    display.println(sampleRateString.substring(1) + "Hz" + "  " + bitsPerSampleString + "bits  FLAC");
  }*/

  display.setCursor(0, 46);
  display.println(bitrateString.substring(1) + "b/s");
  if (currentOption == PLAY_FILES)
  {
    for (int y = 55; y <= 63; y++)
    {
      for (int x = 51; x < 127; x++)
      {
        display.drawPixel(x, y, SH110X_BLACK);
      }
    }
    display.setCursor(63, 55);
    display.println("Plik " + String(currentFileIndex));
  }
  // Zaktualizuj ekran OLED
  display.display();
}


void audio_id3data(const char *info)
{
  Serial.print("id3data     ");
  Serial.println(info);

  // Znajdź pozycję "Artist: " lub "ARTIST: " w tekście
  int artistIndex = String(info).indexOf("Artist: ");
  if (artistIndex == -1) {
    artistIndex = String(info).indexOf("ARTIST: ");
  }
  if (artistIndex != -1)
  {
    // Przytnij tekst od pozycji "Artist:" do końca linii
    artistString = String(info).substring(artistIndex + 8, String(info).indexOf('\n', artistIndex));
    Serial.println("Znalazłem artystę: " + artistString);
  }

  // Znajdź pozycję "Title: " lub "TITLE " w tekście
  int titleIndex = String(info).indexOf("Title: ");
  if (titleIndex == -1) {
    titleIndex = String(info).indexOf("TITLE: ");
  }
  if (titleIndex != -1)
  {
    // Przytnij tekst od pozycji "Title: " do końca linii
    titleString = String(info).substring(titleIndex + 7, String(info).indexOf('\n', titleIndex));
    Serial.println("Znalazłem tytuł: " + titleString);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0, 0);
  display.println("   Odtwarzam plik:   ");
  display.setCursor(0, 10);
  /*for (int y = 9; y <= 36; y++)
  {
    for (int x = 0; x < 127; x++)
    {
      display.drawPixel(x, y, SH110X_BLACK);
    }
  }*/
  display.setCursor(0, 10);
  // Przycina artistString do maksymalnie 21 znaków (indeks 0-20) w celu zapewnienia,
  // że zmieści się on na ekranie OLED. Używamy std::min, aby ograniczyć długość
  // do wartości mniejszej spośród 21 i aktualnej długości artistString. 
  // static_cast<int> jest używane do rzutowania wyniku na typ int.
  artistString = artistString.substring(0, std::min(21, static_cast<int>(artistString.length())));
  display.println(artistString);
  display.setCursor(0, 19);
  // Przycina titleString do maksymalnie 42 znaków (indeks 0-41) w celu zapewnienia,
  // że zmieści się on na ekranie OLED. Używamy std::min, aby ograniczyć długość
  // do wartości mniejszej spośród 42 i aktualnej długości titleString.
  // static_cast<int> jest używane do rzutowania wyniku na typ int.
  titleString = titleString.substring(0, std::min(42, static_cast<int>(titleString.length())));
  display.println(titleString);
  display.display();
  // Ustaw timer, aby wywoływał funkcję updateTimer co sekundę
  timer.attach(1, updateTimer);
}



void audio_eof_mp3(const char *info)
{
  endOfFile = true;
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
  station_data = String(info);
  if (station_data.length() > 62)
  {
    station_data = station_data.substring(0, 62); // Ogranicz długość tekstu do 63 znaków dla wyświetlacza OLED
  }
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
  display.println(station_data);
  display.display();
}
void audio_bitrate(const char *info)
{
  Serial.print("bitrate     ");
  Serial.println(info);
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
      display.println(">> Odtwarzacz plikow");
      display.println("   Radio internetowe");
      break;
    case INTERNET_RADIO:
      display.println("   Odtwarzacz plikow");
      display.println(">> Radio internetowe");
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
      Serial.println(path);
      
      // Zwiększ licznik katalogów
      directoryCount++;
      
      // Rekurencyjnie wywołaj funkcję dla podkatalogu
      //printDirectoriesAndSavePaths(entry, numTabs + 1, path);
      
      // Jeżeli to nie katalog System Volume Information, wydrukuj na ekranie OLED
      if (path != "/System Volume Information")
      {
        for (int i = 1; i < 6; i++)
        {
          // Przygotuj pełną ścieżkę dla wyświetlenia
          String fullPath = directories[i];
          
          // Ogranicz długość do 21 znaków
          fullPath = fullPath.substring(1, 22);
          
          // Ustaw pozycję kursora na ekranie OLED
          display.setCursor(0, i * 10);
          
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
}

void scrollDown()
{
  if (currentSelection < directoryCount - 1)
  {
    currentSelection++;

    if (currentSelection >= firstVisibleLine + maxVisibleLines)
    {
      firstVisibleLine++;
    }
  }
}




void playFromSelectedFolder(int folderIndex)
{
  String folder = directories[folderIndex];

  Serial.println("Odtwarzanie plików z wybranego folderu: " + folder);

  File dir = SD.open(folder);

  if (!dir)
  {
    Serial.println("Błąd otwarcia katalogu!");
    return;
  }

  File entry;

  while (entry = dir.openNextFile())
  {
    String fileName = entry.name();

    // Pomijaj pliki, które nie są w zadeklarowanym formacie audio
    if (!isAudioFile(fileName.c_str()))
    {
      Serial.println("Pominięto plik: " + fileName);
      continue;
    }
    if (isAudioFile(fileName.c_str()))
    {
        currentFileIndex++;
    }

    Serial.print("Odtwarzanie pliku: ");
    Serial.println(fileName);

    // Pełna ścieżka do pliku
    String fullPath = folder + "/" + fileName;
    // Odtwarzaj plik
    audio.connecttoFS(SD, fullPath.c_str());

    isPlaying = true;
    // Oczekuj, aż odtwarzanie się zakończy
    while (isPlaying)
    {
      audio.loop(); // Tutaj obsługujemy odtwarzacz w tle
      button1.loop();
      button2.loop();

      if (button_2) //Przejście do kolejnego pliku w folderze
      {
        button_2 = false;
        seconds = 0;
        isPlaying = false; // Zakończ aktualne odtwarzanie
        break;            // Wyjdź z pętli
      }

      if (button_1) //Przejście do poprzedniego pliku w folderze
      {
        button_1 = false;
        seconds = 0;
        // Przejdź do poprzedniego pliku, jeśli dostępny
        currentFileIndex = (currentFileIndex > 0) ? (currentFileIndex - 1) : (0);

        // Odtwórz znaleziony plik
        dir.rewindDirectory(); // Przewiń katalog na początek
        entry = dir.openNextFile(); // Otwórz pierwszy plik w katalogu

        // Przesuń się do wybranego pliku
        for (int i = 0; i < currentFileIndex; i++)
        {
          entry = dir.openNextFile();
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
          }
        }
      }


      if (button_3)
      {
        button_3 = false;
        seconds = 0;
        currentFileIndex = 0;
        // Przełącz do poprzedniego folderu
        dir.close(); // Zamknij bieżący katalog
        folderIndex = (folderIndex > 0) ? (folderIndex - 1) : (directoryCount - 1);
        folder = directories[folderIndex];
        dir = SD.open(folder); // Otwórz nowy katalog
        break;                 // Wyjdź z pętli
      }

      if (button_4)
      {
        button_4 = false;
        seconds = 0;
        currentFileIndex = 0;
        // Przełącz do następnego folderu
        dir.close(); // Zamknij bieżący katalog
        folderIndex = (folderIndex < directoryCount - 1) ? (folderIndex + 1) : 0;
        folder = directories[folderIndex];
        dir = SD.open(folder); // Otwórz nowy katalog
        break;                 // Wyjdź z pętli
      }     

      if (endOfFile == true) //Wymuszenie programowego przejścia do odtwarzania następnego pliku
      {
        endOfFile = false;
        button_2 = true;
      }


      CLK_state1 = digitalRead(CLK_PIN1);
      if (CLK_state1 != prev_CLK_state1 && CLK_state1 == HIGH)
      {
        if (digitalRead(DT_PIN1) == HIGH)
        {
          counter--;
        } else
        {
          counter++;
        }
        audio.setVolume(counter); // zakres 0...21

        // Wyświetlanie komunikatu przez 5 sekund
        for (int y = 0; y <= 54; y++)
        {
          for (int x = 0; x < 127; x++)
          {
            display.drawPixel(x, y, SH110X_BLACK);
          }
        }
        display.setTextSize(2);
        display.setTextColor(SH110X_WHITE);
        display.setCursor(4, 0);
        display.println("Volume set");
        display.setTextSize(3);
        display.setCursor(48, 30);
        display.println(counter);
        display.display();

        // Ustawienie flagi aktywnego wyświetlania
        displayActive = true;
        displayStartTime = millis();
        
        if (counter >= 15)
        {
          counter = 15;
        }
        if (counter <= 5)
        {
          counter = 5;
        }
      }
      prev_CLK_state1 = CLK_state1;

      

      if (displayActive && (millis() - displayStartTime >= displayTimeout))   // Przywracanie poprzedniej zawartości ekranu po 5 sekundach
      {
        for (int y = 0; y <= 54; y++)
        {
          for (int x = 0; x < 127; x++)
          {
            display.drawPixel(x, y, SH110X_BLACK);
          }
        }
        display.setTextSize(1);
        display.setTextColor(SH110X_WHITE);
        display.setCursor(0, 0);
        display.println("   Odtwarzam plik:   ");
        display.setCursor(0, 10);
        artistString = artistString.substring(0, std::min(21, static_cast<int>(artistString.length())));
        display.println(artistString);
        display.setCursor(0, 19);
        titleString = titleString.substring(0, std::min(42, static_cast<int>(titleString.length())));
        display.println(titleString);
        display.setCursor(0, 37);
        display.println(sampleRateString.substring(1) + "Hz" + "  " + bitsPerSampleString + "bits");
        display.setCursor(0, 46);
        display.println(bitrateString.substring(1) + "b/s");
        for (int y = 55; y <= 63; y++)
        {
          for (int x = 51; x < 127; x++)
          {
            display.drawPixel(x, y, SH110X_BLACK);
          }
        }
        display.setCursor(63, 55);
        display.println("Plik " + String(currentFileIndex));
        display.display();
        displayActive = false;
      }

      if (button2.isPressed())
      {
        printToOLED();
      }

      if (button1.isPressed())
      {
        encoderButton1 = true;
        display.clearDisplay();
        audio.stopSong();
        timer.detach();
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

  dir.close();
}



// Funkcja do drukowania na ekranie OLED z uwzględnieniem zaznaczenia
void printToOLED()
{
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0, 0);
  display.print("   LISTA KATALOGOW   ");
  display.println(currentDirectory);

  int displayRow = 1;  // Zaczynamy od drugiego wiersza (pierwszy to nagłówek)

  for (int i = firstVisibleLine; i < min(firstVisibleLine + 6, directoryCount); i++)
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
        display.setCursor(0, displayRow * 10);
        display.print(displayedPath);

        // Przesuń się do kolejnego wiersza
        displayRow++;

        // Zaktualizuj liczbę przefiltrowanych folderów
        filteredDirectoriesCount++;
      }
    }
  }

  // Przywróć domyślne kolory tekstu
  display.setTextColor(SH110X_WHITE);
  display.display();
}


void updateTimer()
{
  // Wywoływana co sekundę przez timer
  // Zwiększ licznik sekund
  seconds++;

  // Wyświetl aktualny czas w sekundach
  // Konwertuj sekundy na minutę i sekundy
  unsigned int minutes = seconds / 60;
  unsigned int remainingSeconds = seconds % 60;
  // Wyświetl aktualny czas w formacie "mm:ss" na ekranie OLED

  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  for (int y = 55; y <= 63; y++)
  {
    for (int x = 0; x < 50; x++)
    {
      display.drawPixel(x, y, SH110X_BLACK);
    }
  }

  // Wyświetaj czas 
  display.setCursor(0, 55);

  // Formatuj czas jako "mm:ss"
  char timeString[10];
  snprintf(timeString, sizeof(timeString), "%02um:%02us", minutes, remainingSeconds);

  // Wydrukuj czas na ekranie OLED
  display.print(timeString);
  display.display();

}



void setup()
{
  // Ustaw pin CS dla karty SD jako wyjście i ustaw go na wysoki stan
  pinMode(SD_CS, OUTPUT);
  digitalWrite(SD_CS, HIGH);

  // Konfiguruj piny enkodera jako wejścia
  pinMode(CLK_PIN1, INPUT);
  pinMode(DT_PIN1, INPUT);

  // Ustaw czas odbicia dla przycisku enkodera na 50 milisekund
  //button1.setDebounceTime(50);
  //button2.setDebounceTime(50);

  // Odczytaj początkowy stan pinu CLK enkodera
  prev_CLK_state1 = digitalRead(CLK_PIN1);
  prev_CLK_state2 = digitalRead(CLK_PIN2);

  // Ustaw pin S1, S2, S3, S4 jako wejścia z rezystorem pull-up
  pinMode(button_S1, INPUT_PULLUP);
  pinMode(button_S2, INPUT_PULLUP);
  pinMode(button_S3, INPUT_PULLUP);
  pinMode(button_S4, INPUT_PULLUP);

  // Przypnij przerwania do enkodera (wywołanie funkcji zlicz_S1 -- zlicz_S4 przy opadającym zboczu)
  attachInterrupt(LICZNIK_S1, zlicz_S1, FALLING);
  attachInterrupt(LICZNIK_S2, zlicz_S2, FALLING);
  attachInterrupt(LICZNIK_S3, zlicz_S3, FALLING);
  attachInterrupt(LICZNIK_S4, zlicz_S4, FALLING);

  // Inicjalizuj interfejs SPI dla obsługi wyświetlacza OLED
  SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
  SPI.setFrequency(1000000);

  // Inicjalizuj komunikację szeregową (Serial)
  Serial.begin(115200);

  // Inicjalizuj pamięć EEPROM z odpowiednim rozmiarem
  EEPROM.begin(MAX_STATIONS * (MAX_LINK_LENGTH + 1));

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
  wifi_setup();
  fetchStationsFromServer();
  station_nr = 6;
  changeStation();
}



void loop()
{
  audio.loop();
  button1.loop();
  button2.loop();

  CLK_state1 = digitalRead(CLK_PIN1);
  if (CLK_state1 != prev_CLK_state1 && CLK_state1 == HIGH)
  {
    if (digitalRead(DT_PIN1) == HIGH)
    {
      counter--;
    } else
    {
      counter++;
    }

    Serial.print("Encoder prawy: ");
    Serial.println(counter);
    audio.setVolume(counter); // zakres 0...21

    // Wyświetlanie komunikatu przez 5 sekund
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SH110X_WHITE);
    display.setCursor(4, 0);
    display.println("Volume set");
    display.setTextSize(3);
    display.setCursor(48, 30);
    display.println(counter);
    display.display();

    // Ustawienie flagi aktywnego wyświetlania
    displayActive = true;
    displayStartTime = millis();
    
    if (counter >= 15)
    {
      counter = 15;
    }
    if (counter <= 5)
    {
      counter = 5;
    }
  }
  prev_CLK_state1 = CLK_state1;

  CLK_state2 = digitalRead(CLK_PIN2);
  if (CLK_state2 != prev_CLK_state2 && CLK_state2 == HIGH)
  {
    directoryCount = 0;
    if (digitalRead(DT_PIN2) == HIGH)
    {
      Serial.println("Encoder lewy: obracam w lewo");
      // Przełącz między opcjami menu
      currentOption = (currentOption == PLAY_FILES) ? INTERNET_RADIO : PLAY_FILES;
      // Ustawienie flagi aktywnego wyświetlania
      //displayActive = true;
      //displayStartTime = millis();
      // Wyświetl aktualne menu
      displayMenu();
    }
    else
    {
      Serial.println("Encoder lewy: obracam w prawo");
      // Przełącz między opcjami menu
      currentOption = (currentOption == PLAY_FILES) ? INTERNET_RADIO : PLAY_FILES;
      // Ustawienie flagi aktywnego wyświetlania
      //displayActive = true;
      //displayStartTime = millis();
      // Wyświetl aktualne menu
      displayMenu();
      
    }
  }
  prev_CLK_state2 = CLK_state2;


  if (displayActive && (millis() - displayStartTime >= displayTimeout))   // Przywracanie poprzedniej zawartości ekranu po 5 sekundach
  {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SH110X_WHITE);
    display.setCursor(0, 0);
    display.println(station_name);
    display.setCursor(0, 10);
    display.println(station_data);
    for (int y = 37; y <= 54; y++)
    {
      for (int x = 0; x < 127; x++)
      {
        display.drawPixel(x, y, SH110X_BLACK);
      }
    }

    // Wyświetl informacje o SampleRate na ekranie OLED
    display.setCursor(0, 37);
    /*if (mp3 == true)
    {
      mp3 = false;
      Serial.println("To jest grane MP3");
      display.println(sampleRateString.substring(1) + "Hz" + "  " + bitsPerSampleString + "bits   MP3");
    }
    if (flac == true)
    {
      flac = false;
      Serial.println("To jest grany FLAC");
      display.println(sampleRateString.substring(1) + "Hz" + "  " + bitsPerSampleString + "bits  FLAC");
    }*/
    display.println(sampleRateString.substring(1) + "Hz" + "  " + bitsPerSampleString + "bits");
    display.setCursor(0, 46);
    display.println(bitrateString.substring(1) + "b/s");

    // Zaktualizuj ekran OLED
    display.display();
    displayActive = false;
  }
  
  

  if (button1.isPressed())  //Przycisk enkodera prawego wciśnięty
  {
    Serial.println("Przycisk enkodera prawego");
    playFromSelectedFolder(currentSelection);
  }

  if (button2.isPressed())  //Przycisk enkodera lewego wciśnięty
  {
    Serial.println("Przycisk enkodera lewego");
    if (currentOption == PLAY_FILES)
    {
      audio.stopSong();
      if (!SD.begin(SD_CS))
      {
        Serial.println("Błąd inicjalizacji karty SD!");
        return;
      }
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(SH110X_WHITE);
      display.setCursor(0, 0);
      display.print("   LISTA KATALOGOW   ");
      display.display();
      listDirectories("/");
    }
  
    if (currentOption == INTERNET_RADIO)
    {
      changeStation();
    }
  }


  // Obsługa przycisku S1
  if (button_1)
  {
    if ((millis() - lastDebounceTime_S1) > debounceDelay)
    {
      button_1 = false;
      licznik_S1 = 0;
      lastDebounceTime_S1 = millis();
      if (currentOption == INTERNET_RADIO)
      {
        station_nr++;
        Serial.print("Przycisk S1 został wciśnięty, licznik = ");
        Serial.println(station_nr);
        changeStation();
      }
    }
  }

  // Obsługa przycisku S2
  if (button_2)
  {
    if ((millis() - lastDebounceTime_S2) > debounceDelay)
    {
      button_2 = false;
      licznik_S2 = 0;
      lastDebounceTime_S2 = millis();
      if (currentOption == INTERNET_RADIO)
      {
        if (station_nr <= 0)
        {
          station_nr = 51;
        }
        lastDebounceTime_S2 = millis();
        station_nr--;
        Serial.print("Przycisk S2 został wciśnięty, licznik = ");
        Serial.println(station_nr);
        changeStation();
      }
    }
  }


    // Obsługa przycisku S3
  if (button_3)
  {
    if ((millis() - lastDebounceTime_S3) > debounceDelay)
    {
      button_3 = false;
      licznik_S3 = 0;
      lastDebounceTime_S3 = millis();
      if (currentOption == INTERNET_RADIO)
      {
        bank_nr++;
        station_nr = 1;
        Serial.print("Przycisk S3 został wciśnięty, bank = ");
        Serial.println(bank_nr);
        // Wyświetlanie nr banku
        display.clearDisplay();
        display.setTextSize(2);
        display.setTextColor(SH110X_WHITE);
        display.setCursor(25, 0);
        display.println("Bank nr");
        display.setTextSize(3);
        display.setCursor(55, 30);
        display.println(bank_nr);
        display.display();
        // Ustawienie flagi aktywnego wyświetlania
        displayActive = true;
        displayStartTime = millis();
        fetchStationsFromServer();
        delay(250);
        changeStation();
      }
      if (currentOption == PLAY_FILES)
      {
        scrollUp();
        printToOLED();
      }
    }
  }

  // Obsługa przycisku S4
  if (button_4)
  {
    if ((millis() - lastDebounceTime_S4) > debounceDelay)
    {
      button_4 = false;
      licznik_S4 = 0;
      lastDebounceTime_S4 = millis();
      if (currentOption == INTERNET_RADIO)
      {
        bank_nr--;
        if (bank_nr < 0)
        {
          bank_nr = 0;
        }
        station_nr = 1;
        Serial.print("Przycisk S4 został wciśnięty, bank = ");
        Serial.println(bank_nr);
        // Wyświetlanie nr banku
        display.clearDisplay();
        display.setTextSize(2);
        display.setTextColor(SH110X_WHITE);
        display.setCursor(25, 0);
        display.println("Bank nr");
        display.setTextSize(3);
        display.setCursor(55, 30);
        display.println(bank_nr);
        display.display();
        // Ustawienie flagi aktywnego wyświetlania
        displayActive = true;
        displayStartTime = millis();
        fetchStationsFromServer();
        delay(250);
        changeStation();
      }
      if (currentOption == PLAY_FILES)
      {
        scrollDown();
        printToOLED();
      }
    }
  }
  
  
}