Projekt radia internetowego wraz z odtwarzaczem plikow audio z karty SD. 
Jest to początkowa wersja projektu, który będzie dalej rozwiajny i optymalizowany.
Głównym modułem jest ESP32-S3-WROOM-1-N16R8, który razem z modułem DAC typu PCM5102A tworzą zasadniczą część projektu.
Jako wyświetlacz zastosowałem OLED 2,42" 128x64px biały, obudowa jest produkcji GAINTA G748, do zasilania użyłem popularnego modułu dla płytek stykowych MB102.
Całość jest sterowana z 4 przełączników monostabilnych NO (dwa PS11BBK i dwa PS11BBU) oraz 2 enkoderow obrotowych z przyciskiem.
Opis jak ustawić połączenie WiFi przy pierwszym uruchomieniu jest opisany tu:
https://github.com/tzapu/WiFiManager
w skrócie, np. na telefonie należy odszukać sieć "ESP32 internet radio" i się do niej podłączyć. Następnie w przegladarce internetowej na telefonie należy wpisac adres 192.168.4.1 i wyszukać sieć 
WiFi, z którą będzie się normalnie potem łączył ESP32, wpisać dla wskazanej sieci hasło i zapisac dane, to wszystko. Po restarcie zasilania moduł ESP32 podłaczy się do zapisanej sieci.





Internet Radio Project with Audio File Player from SD Card.
This is the initial version of the project, which will be further developed and optimized.
The main module is the ESP32-S3-WROOM-1-N16R8, which, together with the PCM5102A DAC module, forms the core of the project.
I used a 2.42" 128x64px white OLED display, and the casing is a GAINTA G748. For power supply, I used a popular MB102 module for breadboards.
The whole setup is controlled by 4 NO monostable switches (two PS11BBK and two PS11BBU) and 2 rotary encoders with a button.
Instructions for setting up the WiFi connection on the first start are described here:
https://github.com/tzapu/WiFiManager
In brief, you need to find the "ESP32 internet radio" network on your phone and connect to it.
Then, in your phone's web browser, enter the address 192.168.4.1 and search for the WiFi network that the ESP32 will normally connect to.
Enter the password for the selected network and save the data. That's it. After a power restart, the ESP32 module will connect to the saved network.
