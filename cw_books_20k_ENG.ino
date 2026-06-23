// ==========================================
//   LIBRERÍAS
// ==========================================
#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>

// ==========================================
//   CONFIGURACIÓN DE PINES
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
//   CONFIGURACIÓN CW
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
//   TABLA DE CÓDIGO MORSE
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
//   TEXTO EN INGLÉS
// ==========================================
const char text[] PROGMEM = 
"In 1866, the world's oceans are terrorized by a supposed sea monster that sinks ships. "
"French professor Pierre Aronnax, a passionate marine biologist, his loyal servant Conseil, "
"and the hot-tempered Canadian harpooner Ned Land join an expedition aboard the Abraham Lincoln "
"to hunt the beast. After months of fruitless search, they finally locate the monster. However, "
"during the attack, the three men are thrown into the sea and discover the shocking truth: it is "
"not a creature, but the Nautilus, an advanced technology submarine. They are taken prisoner by "
"its enigmatic commander, Captain Nemo. Nemo is a brilliant, cultured, and deeply misanthropic "
"man who has broken all ties with humanity and the surface. He has designed the Nautilus to live "
"exclusively in the ocean, feeding on its resources and sailing in total freedom. Aronnax, "
"fascinated by the scientific wonders and the submarine's library, accepts his captivity, while "
"Ned Land only thinks of escape. Thus begins a journey of twenty thousand leagues through the "
"ocean floors. Nemo shows them an underwater world full of wonders: they walk through sea "
"forests, explore the submerged ruins of Atlantis, dive in the coal mines of volcanic islands, "
"and discover life in the abyssal depths. The voyage takes them through the seas of Japan, the "
"coasts of New Guinea, the Indian Ocean, and the Mediterranean. At a crucial moment, the "
"Nautilus becomes trapped under ice at the South Pole. After anguishing days where they nearly "
"suffocate from lack of oxygen, they manage to free the submarine and escape. Later, they face "
"a terrible giant squid in which a crewman loses his life, showing Nemo's vulnerable side. "
"However, Aronnax's admiration turns to horror when he discovers the Captain's true nature. "
"Nemo uses the Nautilus as an instrument of revenge against the imperialist nations that oppress "
"peoples. In a devastating act, the submarine rams and sinks a warship, drowning its entire "
"crew. Aronnax understands that Nemo is not just a scientist, but a man consumed by hatred and "
"bloodlust. Determined to flee before the captain leads them to perdition, the three prisoners "
"await their opportunity. As the Nautilus is dragged toward the dangerous Maelstrom whirlpool "
"off the coast of Norway, Aronnax, Conseil, and Ned Land manage to board a lifeboat and escape. "
"The three men survive and are rescued by a fisherman. Aronnax returns to civilization, but his "
"mind remains forever trapped in the depths. Although he is saved, the professor never forgets "
"Captain Nemo, nor the unfathomable wonders and terrors hidden by the oceans, wondering "
"eternally if the mysterious commander and his submarine finally found peace in the abyss. "
"Today, looking at the sea, I hear the echo of its propellers, remembering that beneath the "
"waves lies the legacy of a genius who chose the ocean's darkness over the light.";

// ==========================================
//   FUNCIONES AUXILIARES
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

void showSplashScreen() {
  u8g2.clearBuffer();
  
  u8g2.setFont(u8g2_font_helvB10_tr);
  drawCenteredStr(20, "CW Adventures");
  
  u8g2.setFont(u8g2_font_helvB12_tr);
  drawCenteredStr(42, "20,000 Leagues");
  
  u8g2.setFont(u8g2_font_helvB10_tr);
  drawCenteredStr(60, "Under the Sea");
  
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
  drawCenteredStr(60, "Button: resume");
  
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