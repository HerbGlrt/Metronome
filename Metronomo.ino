#include <Keypad.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET -1  // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(128, 64, &Wire, OLED_RESET);

const byte ROWS = 4; // Quatro linhas
const byte COLS = 4; // Quatro colunas
char keys[ROWS][COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {9, 8, 7, 6}; // Conectar aos pinos corretos no Arduino
byte colPins[COLS] = {5, 4, 3, 2}; // Conectar aos pinos corretos no Arduino
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

const int ledPins[] = {51, 50, 49, 48, 47, 46, 45, 44};
const int relayPins[] = {22, 23, 24, 25};
#define VIBRATION_PIN 10 // Defina o pino para o módulo de vibração

int bpm;
int numerator;
unsigned long previousMillis = 0;
int currentBeat = 1;
int beatInterval; // Calcula o intervalo de tempo total para uma batida em milissegundos
int subInterval; // Divide o intervalo de batida em 8 partes iguais
int sentido = 1; //  1 é da direita para a esquerda, -1 é da esquerda para a direita
int LEDAtual = 0;

// Array of all bitmaps for convenience. (Total bytes used to store images in PROGMEM = 131088)


void setup() {
    Serial.begin(9600);
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);

    pinMode(VIBRATION_PIN, OUTPUT); // Configura o pino do módulo de vibração como saída

    for (int i = 0; i < 8; i++) {
        pinMode(ledPins[i], OUTPUT);
        digitalWrite(ledPins[i], LOW);
    }

    for (int i = 0; i < 4; i++) {
        pinMode(relayPins[i], OUTPUT);
        digitalWrite(relayPins[i], LOW);
    }

    initialDisplay();
    delay(2000);
    getUserInput();
}


void updateMetronome() {
  digitalWrite(VIBRATION_PIN, HIGH); // Ativa o módulo de vibração
    if (bpm > 0) {
        beatInterval = 60000 / bpm; // Assegura que bpm > 0
        subInterval = (beatInterval - 10) / 8; // Subtrai uma pequena margem para compensar atrasos de processamento
    }

    unsigned long startMillis = millis(); // Começa a contar o tempo para esta batida
    unsigned long nextActionMillis = startMillis; // Próxima ação no tempo
    unsigned long endVibrationMillis = startMillis + 120; // Duração da vibração (500 ms)

    if (currentBeat == 1) {
        for (int i = 0; i < 4; i++) {
            digitalWrite(relayPins[i], LOW);
        }
    } else {
        int relayIndex = (currentBeat - 1) % 4; // Cycle through relays
        digitalWrite(relayPins[relayIndex], !digitalRead(relayPins[relayIndex])); // Toggle the relay state
    }

    for (int i = 0; i < 8; i++) {
        nextActionMillis += subInterval;
        while (millis() < nextActionMillis) {} // Espera ativa até o próximo ponto de ação

        if (millis() >= endVibrationMillis && digitalRead(VIBRATION_PIN) == HIGH) {
            digitalWrite(VIBRATION_PIN, LOW); // Desativa a vibração
        }

        if (LEDAtual - sentido >= 0 && LEDAtual - sentido < 8) {
            digitalWrite(ledPins[LEDAtual - sentido], LOW); // Apaga o LED anterior
        }

        digitalWrite(ledPins[LEDAtual], HIGH); // Acende o LED atual
        LEDAtual += sentido;
        if (LEDAtual >= 8 || LEDAtual < 0) {
            sentido *= -1;
            LEDAtual += sentido;
        }
    }

    if (digitalRead(VIBRATION_PIN) == HIGH) {
        digitalWrite(VIBRATION_PIN, LOW);
    }

    currentBeat = (currentBeat % numerator) + 1;

    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(1);
    display.print("BPM:      ");
    display.println(bpm);
    display.print("Compasso: ");
    display.println(numerator);
    display.setTextSize(2);
    display.println("Compasso");
    display.print("atual: ");
    display.setTextSize(3);
    display.println(currentBeat);
    display.display();
}


void handleKey(char key) {
    switch (key) {
        case 'A': bpm = max(30, bpm + 5); break;
        case 'B': bpm = max(30, bpm - 5); break;
        case '*': getUserInput(); break;
    }
}

void initialDisplay() {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(2);
    display.println("Bem vinda");
    display.println("    ao");
    display.println("metronomo!");
    display.display();
}

void getUserInput() {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(2);
    display.println("Coloque o");
    display.println("BPM:");
    display.display();
    bpm = getNumber();
    
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Coloque o");
    display.println("Compasso:");
    display.display();
    numerator = getNumber();
    
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Configuracoes");
    display.println("Completas!");
    display.display();
    delay(2000);
}

int getNumber() {
    int number = 0;
    char key;
    while ((key = keypad.getKey()) != '#') {
        if (key >= '0' && key <= '9') {
            number = number * 10 + (key - '0');
            display.print(key);
            display.display();
        }
    }
    return number;
}

void loop() {
    unsigned long currentMillis = millis();

    if (currentMillis - previousMillis >= (60000 / bpm)) {
            updateMetronome(); // Chama a função padrão
        }
        previousMillis = currentMillis;

    char key = keypad.getKey();
    if (key != NO_KEY) handleKey(key);
}