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
//   TEXTO EN ESPANOL - ROBINSON CRUSOE (500 palabras)
// ==========================================
const char text[] PROGMEM = 
"Yo, Robinson Crusoe, naci en el ano 1632 en la ciudad de York, de una buena familia. "
"Mi padre, extranjero de Bremen establecido primero en Hull, habia adquirido una buena fortuna "
"y, dejando el comercio, vivia en York donde habia casado con mi madre, cuyos parientes eran "
"Robinson. De esta union naci yo, y fui llamado Robinson Kreutznaer, que por corrupcion de "
"palabras pronunciamos ahora Crusoe. Desde mi ninez senti una inclinacion irresistible por la "
"vida marinera. Mi padre, deseoso de que estudiara derecho, me dio excelentes consejos contra "
"los peligros de las aventuras, pero yo no podia soportar la idea de una vida tranquila. A los "
"diecinueve anos me embarque por primera vez rumbo a Londres, contra la voluntad de mis padres. "
"Mi primer viaje fue afortunado, pero el segundo termino en desastre. Los piratas moros nos "
"capturaron y fui hecho esclavo en Sallee durante dos anos. Logre escapar en un bote junto al "
"nino Xury, y fuimos rescatados por un navio portugues que se dirigia a Brasil. En Brasil me "
"estableci como plantador de azucar, pero la ambicion me perdio. Me uni a una expedicion para "
"traer esclavos de Africa, pero una terrible tormenta nos llevo a la deriva. El barco encallo "
"en un banco de arena, y solo yo sobrevivi. Las olas me arrojaron a una costa desconocida. "
"Desperte en una playa desierta, sin mas compania que la soledad. El barco, varado cerca de la "
"orilla, me permitio rescatar armas, herramientas, provisiones y polvora. Construi una tienda "
"fortificada en una colina y aprendi a sobrevivir. Con el tiempo, domestique cabras salvajes, "
"cultive cebada y arroz, y construi una canoa. En mis exploraciones descubri que la isla estaba "
"habitada por salvajes canibales que venian a celebrar sus victorias. Un dia, presencie su "
"llegada y logre salvar a uno de sus prisioneros. Lo llame Viernes, porque ese fue el dia de "
"su liberacion. Le ensene mi idioma, lo civilice y lo converti en mi fiel companero. Juntos "
"rescatamos a un espanol y al padre de Viernes de manos de los canibales. Mas tarde, un barco "
"ingles anclo frente a nuestra costa. La tripulacion se habia amotinado y abandono al capitan "
"y a dos oficiales en la isla. Con astucia y valor, logramos recuperar el navio. Despues de "
"veintiocho anos en la isla, finalmente pude regresar a Inglaterra. Descubri que mi familia me "
"creia muerto, pero mi plantacion en Brasil habia generado una pequena fortuna. Me case, tuve "
"tres hijos y vivi feliz, aunque nunca olvide mi isla solitaria. Mi historia ensena que la "
"paciencia, el trabajo y la fe pueden vencer cualquier adversidad. Desde aquella playa desierta "
"hasta mi regreso a la civilizacion, aprendi que el ser humano es capaz de adaptarse a cualquier "
"circunstancia y que la providencia siempre ofrece esperanza incluso en la mas profunda soledad.";

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
  drawCenteredStr(42, "Robinson");
  
  u8g2.setFont(u8g2_font_helvB10_tr);
  drawCenteredStr(60, "Crusoe");
  
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
  drawCenteredStr(58, "GP 3.0");
  
  u8g2.sendBuffer();
  delay(3000);
}

void showSpeedSelection() {
  u8g2.clearBuffer();
  
  u8g2.setFont(u8g2_font_helvB10_tr);
  drawCenteredStr(18, "Velocidad:");
  
  char speedStr[10];
  sprintf(speedStr, "%d WPM", speeds[currentSpeedIndex]);
  u8g2.setFont(u8g2_font_helvB14_tr);
  drawCenteredStr(42, speedStr);
  
  u8g2.setFont(u8g2_font_helvB08_tr);
  drawCenteredStr(60, "Boton: cambiar");
  
  u8g2.sendBuffer();
}

void showPlayingScreen() {
  u8g2.clearBuffer();
  
  u8g2.setFont(u8g2_font_helvB10_tr);
  drawCenteredStr(18, "Reproduciendo");
  
  char speedStr[10];
  sprintf(speedStr, "%d WPM", speeds[currentSpeedIndex]);
  u8g2.setFont(u8g2_font_helvB14_tr);
  drawCenteredStr(42, speedStr);
  
  u8g2.setFont(u8g2_font_helvB08_tr);
  drawCenteredStr(60, "Boton: pausar");
  
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
  drawCenteredStr(60, "Boton: continuar");
  
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