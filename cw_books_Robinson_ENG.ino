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
//   TEXTO EN INGLES - ROBINSON CRUSOE (500 palabras)
// ==========================================
const char text[] PROGMEM = 
"I, Robinson Crusoe, was born in the year 1632 in the city of York, of a good family. "
"My father, a foreigner from Bremen who settled first at Hull, had acquired a good estate "
"and, leaving off trade, lived afterwards at York, where he married my mother, whose "
"relations were named Robinson. From this union I was born, and was called Robinson "
"Kreutznaer, which by corruption of words we now pronounce Crusoe. From my childhood I "
"felt an irresistible inclination for the seafaring life. My father, wishing me to study "
"law, gave me excellent advice against the dangers of adventures, but I could not endure "
"the thought of a quiet life. At nineteen I embarked for the first time bound for London, "
"against my parents' will. My first voyage was fortunate, but the second ended in disaster. "
"Moorish pirates captured us and I was made a slave in Sallee for two years. I managed to "
"escape in a boat with a boy named Xury, and we were rescued by a Portuguese ship bound "
"for Brazil. In Brazil I settled as a sugar planter, but ambition ruined me. I joined an "
"expedition to bring slaves from Africa, but a terrible storm drove us off course. The ship "
"ran aground on a sandbar, and I alone survived. The waves cast me upon an unknown shore. "
"I awoke on a deserted beach with no company but solitude. The ship, stranded near the "
"shore, allowed me to salvage arms, tools, provisions, and gunpowder. I built a fortified "
"tent on a hill and learned to survive. In time, I tamed wild goats, cultivated barley and "
"rice, and built a canoe. In my explorations I discovered the island was inhabited by "
"cannibal savages who came to celebrate their victories. One day, I witnessed their arrival "
"and managed to save one of their prisoners. I called him Friday, for that was the day of "
"his deliverance. I taught him my language, civilized him, and he became my faithful "
"companion. Together we rescued a Spaniard and Friday's father from the cannibals. Later, "
"an English ship anchored off our coast. The crew had mutinied and abandoned the captain "
"and two officers on the island. With cunning and courage, we managed to recover the ship. "
"After twenty-eight years on the island, I finally could return to England. I discovered "
"my family believed me dead, but my plantation in Brazil had generated a small fortune. I "
"married, had three children, and lived happily, though I never forgot my lonely island. My "
"story teaches that patience, work, and faith can overcome any adversity. From that "
"deserted beach to my return to civilization, I learned that man is capable of adapting to "
"any circumstance and that providence always offers hope even in the deepest solitude.";

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
  drawCenteredStr(20, "CW Adventures");
  
  u8g2.setFont(u8g2_font_helvB12_tr);
  drawCenteredStr(42, "Robinson");
  
  u8g2.setFont(u8g2_font_helvB10_tr);
  drawCenteredStr(60, "Crusoe");
  
  u8g2.sendBuffer();
  delay(3000);
}

void showCreditsScreen() {
  u8g2.clearBuffer();
  
  u8g2.setFont(u8g2_font_helvB08_tr);
  drawCenteredStr(18, "Credits to");
  
  u8g2.setFont(u8g2_font_helvB12_tr);
  drawCenteredStr(40, "LU6APR");
  
  u8g2.setFont(u8g2_font_helvB08_tr);
  drawCenteredStr(58, "GP 3.0");
  
  u8g2.sendBuffer();
  delay(3000);
}

void showSpeedSelection() {
  u8g2.clearBuffer();
  
  u8g2.setFont(u8g2_font_helvB10_tr);
  drawCenteredStr(18, "Speed:");
  
  char speedStr[10];
  sprintf(speedStr, "%d WPM", speeds[currentSpeedIndex]);
  u8g2.setFont(u8g2_font_helvB14_tr);
  drawCenteredStr(42, speedStr);
  
  u8g2.setFont(u8g2_font_helvB08_tr);
  drawCenteredStr(60, "Button: change");
  
  u8g2.sendBuffer();
}

void showPlayingScreen() {
  u8g2.clearBuffer();
  
  u8g2.setFont(u8g2_font_helvB10_tr);
  drawCenteredStr(18, "Playing");
  
  char speedStr[10];
  sprintf(speedStr, "%d WPM", speeds[currentSpeedIndex]);
  u8g2.setFont(u8g2_font_helvB14_tr);
  drawCenteredStr(42, speedStr);
  
  u8g2.setFont(u8g2_font_helvB08_tr);
  drawCenteredStr(60, "Button: pause");
  
  u8g2.sendBuffer();
}

void showPausedScreen() {
  u8g2.clearBuffer();
  
  u8g2.setFont(u8g2_font_helvB12_tr);
  drawCenteredStr(22, "PAUSED");
  
  char speedStr[10];
  sprintf(speedStr, "%d WPM", speeds[currentSpeedIndex]);
  u8g2.setFont(u8g2_font_helvB14_tr);
  drawCenteredStr(45, speedStr);
  
  u8g2.setFont(u8g2_font_helvB08_tr);
  drawCenteredStr(60, "Button: continue");
  
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