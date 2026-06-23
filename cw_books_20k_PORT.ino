// ==========================================
//   BIBLIOTECAS
// ==========================================
#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>

// ==========================================
//   CONFIGURACAO DE PINOS
// ==========================================
#define PIN_DIT         2
#define PIN_DAH         3
#define PIN_COMMAND     7
#define PIN_SIDETONE    1
#define PIN_KEY_OUT     6

#define I2C_SDA         5
#define I2C_SCL         4

// ==========================================
//   DISPLAY
// ==========================================
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

// ==========================================
//   CONFIGURACAO CW
// ==========================================
#define FREQ_SIDETONE   700
#define DEBOUNCE_TIME   50
#define WAIT_TIME       1500

const int speeds[] = {10, 15, 20, 25, 30, 35};
const int numSpeeds = 6;
int currentSpeedIndex = 0;
unsigned long lastButtonPress = 0;
bool waitingToStart = false;
bool isPlaying = false;
bool isPaused = false;
int currentCharIndex = 0;

// ==========================================
//   TABELA DE CODIGO MORSE
// ==========================================
struct MorseCode {
  char character;
  uint8_t code;
  uint8_t length;
};

const MorseCode morseTable[] PROGMEM = {
  {'A', 0b01, 2}, {'B', 0b1000, 4}, {'C', 0b1010, 4}, {'D', 0b100, 3},
  {'E', 0b0, 1}, {'F', 0b0010, 4}, {'G', 0b110, 3}, {'H', 0b0000, 4},
  {'I', 0b00, 2}, {'J', 0b0111, 4}, {'K', 0b101, 3}, {'L', 0b0100, 4},
  {'M', 0b11, 2}, {'N', 0b10, 2}, {'O', 0b111, 3}, {'P', 0b0110, 4},
  {'Q', 0b1101, 4}, {'R', 0b010, 3}, {'S', 0b000, 3}, {'T', 0b1, 1},
  {'U', 0b001, 3}, {'V', 0b0001, 4}, {'W', 0b011, 3}, {'X', 0b1001, 4},
  {'Y', 0b1011, 4}, {'Z', 0b1100, 4},
  {'0', 0b11111, 5}, {'1', 0b01111, 5}, {'2', 0b00111, 5}, {'3', 0b00011, 5},
  {'4', 0b00001, 5}, {'5', 0b00000, 5}, {'6', 0b10000, 5}, {'7', 0b11000, 5},
  {'8', 0b11100, 5}, {'9', 0b11110, 5},
  {'.', 0b010101, 6}, {',', 0b110011, 6}, {'?', 0b001100, 6}, {'\'', 0b011110, 6},
  {'!', 0b101011, 6}, {'/', 0b10010, 5}, {'(', 0b10110, 5}, {')', 0b101101, 6},
  {'&', 0b01000, 5}, {':', 0b111000, 6}, {';', 0b101010, 6}, {'=', 0b10001, 5},
  {'+', 0b01010, 5}, {'-', 0b100001, 6}, {'_', 0b001101, 6}, {'"', 0b010010, 6},
  {'$', 0b0001001, 7}, {'@', 0b011010, 6}
};

const int morseTableSize = sizeof(morseTable) / sizeof(morseTable[0]);

// ==========================================
//   TEXTO EM PORTUGUES
// ==========================================
const char text[] PROGMEM = 
"Em 1866, os oceanos do mundo sao aterrorizados por um suposto monstro marinho que afunda navios. "
"O professor frances Pierre Aronnax, um apaixonado biologo marinho, seu leal servo Conseil e o "
"temperamental arpoador canadense Ned Land se unem a uma expedicao a bordo do Abraham Lincoln para "
"cacar a besta. Apos meses de busca infrutifera, finalmente localizam o monstro. Porem, durante o "
"ataque, os tres homens sao lancados ao mar e descobrem a verdade chocante: nao e uma criatura, mas "
"o Nautilus, um submarino de tecnologia avancada. Sao feitos prisioneiros por seu enigmatico "
"comandante, o Capitao Nemo. Nemo e um homem brilhante, culto e profundamente misantropo que rompeu "
"todo laco com a humanidade e a superficie. Projetou o Nautilus para viver exclusivamente no oceano, "
"alimentando-se de seus recursos e navegando em total liberdade. Aronnax, fascinado pelas maravilhas "
"cientificas e pela biblioteca do submarino, aceita seu cativeiro, enquanto Ned Land so pensa em "
"escapar. Assim comeca uma viagem de vinte mil leguas pelos fundos oceanicos. Nemo lhes mostra um "
"mundo subaquatico cheio de maravilhas: passeiam por florestas marinhas, exploram as ruinas "
"submersas da Atlantida, mergulham nas minas de carvao de ilhas vulcanicas e descobrem a vida nas "
"profundezas abissais. A travessia os leva pelos mares do Japao, as costas da Nova Guine, o oceano "
"Indico e o Mediterraneo. Em um momento crucial, o Nautilus fica preso sob o gelo no Polo Sul. Apos "
"dias angustiantes onde quase morrem asfixiados pela falta de oxigenio, conseguem libertar o "
"submarino e escapar. Mais tarde, enfrentam uma terrivel Lula gigante na qual um tripulante perde a "
"vida, mostrando o lado vulneravel de Nemo. Porem, a admiracao de Aronnax se torna em horror quando "
"descobre a verdadeira natureza do Capitao. Nemo usa o Nautilus como instrumento de vinganca contra "
"as nacoes imperialistas que oprimem os povos. Em um ato devastador, o submarino embate e afunda um "
"navio de guerra, afogando toda sua tripulacao. Aronnax compreende que Nemo nao e so um cientista, "
"mas um homem consumido pelo odio e sede de sangue. Decididos a fugir antes que o capitao os leve a "
"perdicao, os tres prisioneiros esperam sua oportunidade. Enquanto o Nautilus e arrastado para o "
"perigoso redemoinho do Maelstrom, frente as costas da Noruega, Aronnax, Conseil e Ned Land "
"conseguem subir a um bote salva-vidas e escapam. Os tres homens sobrevivem e sao resgatados por um "
"pescador. Aronnax regressa a civilizacao, mas sua mente fica para sempre presa nas profundidades. "
"Embora se salve, o professor nunca esquece o Capitao Nemo, nem as insondaveis maravilhas e terrores "
"que ocultavam os oceanos, perguntando-se eternamente se o misterioso comandante e seu submarino "
"finalmente encontraram a paz no abismo. Hoje, olhando o mar, escuto o eco de suas helices, "
"lembrando que sob as ondas jaz o legado de um genio que escolheu as trevas do oceano antes que a "
"luz.";

// ==========================================
//   FUNCOES AUXILIARES
// ==========================================

void drawCenteredStr(int16_t y, const char* text) {
  int16_t x = (u8g2.getDisplayWidth() - u8g2.getStrWidth(text)) / 2;
  u8g2.drawStr(x, y, text);
}

unsigned long getDitDuration(int wpm) {
  return 1200 / wpm;
}

bool getMorseCode(char c, uint8_t &code, uint8_t &length) {
  c = toupper(c);
  for (int i = 0; i < morseTableSize; i++) {
    if (pgm_read_byte(&morseTable[i].character) == c) {
      code = pgm_read_byte(&morseTable[i].code);
      length = pgm_read_byte(&morseTable[i].length);
      return true;
    }
  }
  return false;
}

void playDit(unsigned long ditDuration) {
  digitalWrite(PIN_KEY_OUT, HIGH);
  tone(PIN_SIDETONE, FREQ_SIDETONE);
  delay(ditDuration);
  noTone(PIN_SIDETONE);
  digitalWrite(PIN_KEY_OUT, LOW);
  delay(ditDuration);
}

void playDah(unsigned long ditDuration) {
  digitalWrite(PIN_KEY_OUT, HIGH);
  tone(PIN_SIDETONE, FREQ_SIDETONE);
  delay(ditDuration * 3);
  noTone(PIN_SIDETONE);
  digitalWrite(PIN_KEY_OUT, LOW);
  delay(ditDuration);
}

void playCharacter(char c, unsigned long ditDuration) {
  uint8_t code, length;
  
  if (c == ' ') {
    delay(ditDuration * 4);
    return;
  }
  
  if (getMorseCode(c, code, length)) {
    for (int i = length - 1; i >= 0; i--) {
      if ((code >> i) & 1) {
        playDah(ditDuration);
      } else {
        playDit(ditDuration);
      }
    }
    delay(ditDuration * 2);
  }
}

void playText() {
  unsigned long ditDuration = getDitDuration(speeds[currentSpeedIndex]);
  int len = strlen(text);
  
  for (currentCharIndex = 0; currentCharIndex < len; currentCharIndex++) {
    char c = pgm_read_byte(&text[currentCharIndex]);
    playCharacter(c, ditDuration);
    
    if (digitalRead(PIN_COMMAND) == LOW) {
      delay(DEBOUNCE_TIME);
      if (digitalRead(PIN_COMMAND) == LOW) {
        isPaused = true;
        noTone(PIN_SIDETONE);
        digitalWrite(PIN_KEY_OUT, LOW);
        showPausedScreen();
        
        while (isPaused) {
          if (digitalRead(PIN_COMMAND) == LOW) {
            delay(DEBOUNCE_TIME);
            if (digitalRead(PIN_COMMAND) == LOW) {
              isPaused = false;
              showPlayingScreen();
            }
          }
          delay(10);
        }
      }
    }
  }
}

// ==========================================
//   TELAS
// ==========================================

void showSplashScreen() {
  u8g2.clearBuffer();
  
  u8g2.setFont(u8g2_font_helvB10_tr);
  drawCenteredStr(20, "CW Aventuras");
  
  u8g2.setFont(u8g2_font_helvB12_tr);
  drawCenteredStr(42, "20.000 Leguas");
  
  u8g2.setFont(u8g2_font_helvB10_tr);
  drawCenteredStr(60, "Submarinas");
  
  u8g2.sendBuffer();
  delay(3000);
}

void showCreditsScreen() {
  u8g2.clearBuffer();
  
  u8g2.setFont(u8g2_font_helvB08_tr);
  drawCenteredStr(18, "Creditos a");
  
  u8g2.setFont(u8g2_font_helvB12_tr);
  drawCenteredStr(40, "LU6APR");
  
  u8g2.setFont(u8g2_font_helvB08_tr);
  drawCenteredStr(58, GP 3.0");
  
  u8g2.sendBuffer();
  delay(3000);
}

void showSpeedSelection() {
  u8g2.clearBuffer();
  
  u8g2.setFont(u8g2_font_helvB10_tr);
  drawCenteredStr(18, "Velocidade:");
  
  char speedStr[10];
  sprintf(speedStr, "%d WPM", speeds[currentSpeedIndex]);
  u8g2.setFont(u8g2_font_helvB14_tr);
  drawCenteredStr(42, speedStr);
  
  u8g2.setFont(u8g2_font_helvB08_tr);
  drawCenteredStr(60, "Botao: mudar");
  
  u8g2.sendBuffer();
}

void showPlayingScreen() {
  u8g2.clearBuffer();
  
  u8g2.setFont(u8g2_font_helvB10_tr);
  drawCenteredStr(18, "Reproduzindo");
  
  char speedStr[10];
  sprintf(speedStr, "%d WPM", speeds[currentSpeedIndex]);
  u8g2.setFont(u8g2_font_helvB14_tr);
  drawCenteredStr(42, speedStr);
  
  u8g2.setFont(u8g2_font_helvB08_tr);
  drawCenteredStr(60, "Botao: pausar");
  
  u8g2.sendBuffer();
}

void showPausedScreen() {
  u8g2.clearBuffer();
  
  u8g2.setFont(u8g2_font_helvB12_tr);
  drawCenteredStr(22, "PAUSADO");
  
  char speedStr[10];
  sprintf(speedStr, "%d WPM", speeds[currentSpeedIndex]);
  u8g2.setFont(u8g2_font_helvB14_tr);
  drawCenteredStr(45, speedStr);
  
  u8g2.setFont(u8g2_font_helvB08_tr);
  drawCenteredStr(60, "Botao: continuar");
  
  u8g2.sendBuffer();
}

// ==========================================
//   SETUP
// ==========================================
void setup() {
  pinMode(PIN_DIT, INPUT_PULLUP);
  pinMode(PIN_DAH, INPUT_PULLUP);
  pinMode(PIN_COMMAND, INPUT_PULLUP);
  pinMode(PIN_SIDETONE, OUTPUT);
  pinMode(PIN_KEY_OUT, OUTPUT);
  
  digitalWrite(PIN_KEY_OUT, LOW);
  noTone(PIN_SIDETONE);
  
  Wire.begin(I2C_SDA, I2C_SCL);
  u8g2.begin();
  
  showSplashScreen();
  showCreditsScreen();
  showSpeedSelection();
}

// ==========================================
//   LOOP
// ==========================================
void loop() {
  if (digitalRead(PIN_COMMAND) == LOW) {
    delay(DEBOUNCE_TIME);
    if (digitalRead(PIN_COMMAND) == LOW) {
      if (!isPlaying) {
        currentSpeedIndex = (currentSpeedIndex + 1) % numSpeeds;
        showSpeedSelection();
        lastButtonPress = millis();
        waitingToStart = true;
      }
    }
  }
  
  if (waitingToStart && !isPlaying) {
    if (millis() - lastButtonPress >= WAIT_TIME) {
      waitingToStart = false;
      isPlaying = true;
      isPaused = false;
      currentCharIndex = 0;
      showPlayingScreen();
      playText();
      isPlaying = false;
      showSpeedSelection();
    }
  }
  
  delay(10);
}
