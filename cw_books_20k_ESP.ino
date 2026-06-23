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
#define PIN_COMMAND     7    // <-- CAMBIADO a pin 7
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
#define FREQ_SIDETONE   700        // Frecuencia del sidetone en Hz
#define DEBOUNCE_TIME   50         // Tiempo antirrebote en ms
#define WAIT_TIME       1500       // Tiempo de espera antes de iniciar (ms)

// Velocidades disponibles en WPM
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
//   TEXTO A REPRODUCIR
// ==========================================
const char text[] PROGMEM = 
"En 1866, los océanos del mundo son aterrorizados por un supuesto monstruo marino que hunde barcos. "
"El profesor francés Pierre Aronnax, un apasionado biólogo marino, su leal sirviente Conseil y el "
"temperamental arponero canadiense Ned Land se unen a una expedición a bordo del Abraham Lincoln para "
"cazar a la bestia. Tras meses de búsqueda infructuosa, finalmente localizan al monstruo. Sin embargo, "
"durante el ataque, los tres hombres son lanzados al mar y descubren la impactante verdad: no es una "
"criatura, sino el Nautilus, un submarino de tecnología avanzada. Son hechos prisioneros por su "
"enigmático comandante, el Capitán Nemo. Nemo es un hombre brillante, culto y profundamente misántropo "
"que ha roto todo lazo con la humanidad y la superficie. Ha diseñado el Nautilus para vivir exclusivamente "
"en el océano, alimentándose de sus recursos y navegando en total libertad. Aronnax, fascinado por las "
"maravillas científicas y la biblioteca del submarino, acepta su cautiverio, mientras que Ned Land solo "
"piensa en escapar. Así comienza un viaje de veinte mil leguas por los fondos oceánicos. Nemo les muestra "
"un mundo subacuático lleno de maravillas: pasean por bosques marinos, exploran las ruinas sumergidas de "
"la Atlántida, bucean en las minas de carbón de islas volcánicas y descubren la vida en las profundidades "
"abisales. La travesía los lleva por los mares del Japón, las costas de Nueva Guinea, el océano Índico y "
"el Mediterráneo. En un momento crucial, el Nautilus queda atrapado bajo el hielo en el Polo Sur. Tras "
"unos días angustiosos donde casi mueren asfixiados por la falta de oxígeno, logran liberar el submarino "
"y escapar. Más tarde, enfrentan a un terrible calamar gigante en el que un tripulante pierde la vida, "
"mostrando el lado vulnerable de Nemo. Sin embargo, la admiración de Aronnax se torna en horror cuando "
"descubre la verdadera naturaleza del Capitán. Nemo utiliza el Nautilus como un instrumento de venganza "
"contra las naciones imperialistas que oprimen a los pueblos. En un acto devastador, el submarino embiste "
"y hunde un barco de guerra, ahogando a toda su tripulación. Aronnax comprende que Nemo no es solo un "
"científico, sino un hombre consumido por el odio y la sed de sangre. Decididos a huir antes de que el "
"capitán los lleve a la perdición, los tres prisioneros esperan su oportunidad. Mientras el Nautilus es "
"arrastrado hacia el peligroso remolino del Maelstrom, frente a las costas de Noruega, Aronnax, Conseil "
"y Ned Land logran subirse a un bote salvavidas y escapan. Los tres hombres sobreviven y son rescatados "
"por un pescador. Aronnax regresa a la civilización, pero su mente queda para siempre atrapada en las "
"profundidades. Aunque se salva, el profesor nunca olvida al Capitán Nemo, ni las insondables maravillas "
"y terrores que ocultaban los océanos, preguntándose eternamente si el misterioso comandante y su "
"submarino finalmente encontraron la paz en el abismo. Hoy, mirando el mar, escucho el eco de sus "
"hélices, recordando que bajo las olas yace el legado de un genio que eligió las tinieblas del océano "
"antes que la luz.";

// ==========================================
//   FUNCIONES AUXILIARES
// ==========================================

// Dibuja un texto centrado horizontalmente
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
    
    // Verificar si se presionó el botón para pausar
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
//   PANTALLAS (con texto centrado)
// ==========================================

void showSplashScreen() {
  u8g2.clearBuffer();
  
  u8g2.setFont(u8g2_font_helvB10_tr);
  drawCenteredStr(20, "CW aventuras");
  
  u8g2.setFont(u8g2_font_helvB12_tr);
  drawCenteredStr(42, "20.000 leguas");
  
  u8g2.setFont(u8g2_font_helvB10_tr);
  drawCenteredStr(60, "de viaje");
  
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
  drawCenteredStr(60, "Boton: reanudar");
  
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
