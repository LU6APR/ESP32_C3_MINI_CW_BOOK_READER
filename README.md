# 📚 CW Book Reader - Morse Code Audiobooks for ESP32-C3

A unique ESP32-C3 based project that transforms classic public domain literature into Morse code audiobooks. Train your CW (Continuous Wave) reception skills while enjoying timeless stories from the world's greatest authors!

## 🎯 Project Idea

**CW Book Reader** is an innovative Morse code training tool that goes beyond simple words or QSOs. Instead of practicing with random characters, this device plays complete stories in Morse code, allowing radio amateurs and CW enthusiasts to:

- **Improve reception skills** through extended listening sessions
- **Enjoy literature** while training (learning by immersion)
- **Practice at different speeds** (10-35 WPM)
- **Pause and resume** at any point
- **Experience classic stories** in three different languages

The first book included is **"20,000 Leagues Under the Sea"** by Jules Verne (1870), adapted to approximately 500 words and available in **Spanish, English, and Portuguese**.

## 📖 Featured Book: 20,000 Leagues Under the Sea

**Original Author:** Jules Verne (1870)  
**Status:** Public Domain  
**Adaptation:** ~500 words summary  
**Languages Available:**
- 🇪🇸 Spanish (Español)
- 🇺🇸 English
- 🇧🇷 Portuguese (Português)

The story follows Professor Aronnax, his servant Conseil, and the Canadian harpooner Ned Land as they embark on an extraordinary journey aboard the mysterious submarine Nautilus, commanded by the enigmatic Captain Nemo.

## ✨ Features

- **Variable Speed Control:** 10, 15, 20, 25, 30, or 35 WPM
- **Play/Pause Functionality:** Pause the transmission at any time and resume from where you left off
- **Auto-Start:** After selecting speed, playback begins automatically after 1.5 seconds
- **Visual Feedback:** OLED display shows current status (speed selection, playing, paused)
- **Centered UI:** All text is perfectly centered on the display
- **Sidetone Monitor:** 700 Hz audio feedback for local monitoring
- **Key Output:** External key output for transmitters or other devices
- **Memory Efficient:** Text stored in PROGMEM to save RAM
- **Complete Morse Table:** Supports letters, numbers, and common punctuation

## 🔧 Hardware Required

This project is designed for the **ESP32-C3 Mini Morse Vertical Trainer** hardware platform.

**Full hardware documentation and schematics:**  
👉 [ESP32-C3 Mini Morse Vertical Trainer](https://github.com/LU6APR/ESP32_C3_MINI_MORSE_VERTICAL_TRAINER)

### Pin Configuration

| Function | GPIO Pin |
|----------|----------|
| Dit Input | GPIO 2 |
| Dah Input | GPIO 3 |
| Command Button | GPIO 7 |
| Sidetone Output | GPIO 1 |
| Key Output | GPIO 6 |
| I2C SDA | GPIO 5 |
| I2C SCL | GPIO 4 |

### Display

- **Type:** SH1106 OLED 128x64
- **Interface:** I2C
- **Library:** U8g2

## 📦 Installation

### Prerequisites

1. **Arduino IDE** or **PlatformIO**
2. **ESP32 Board Package** (version 2.x or later)
3. **Required Libraries:**
   - `U8g2` by oliver (for OLED display)
   - `Wire` (included with ESP32 package)

### Steps

1. Clone or download this repository
2. Open the `.ino` file in Arduino IDE
3. Select board: **ESP32C3 Dev Module**
4. Configure USB CDC On Boot: **Enabled**
5. Upload to your ESP32-C3 Mini

## 🎮 Usage

### Initial Setup

1. Power on the device
2. Watch the splash screen: "CW Aventuras - 20,000 Leguas"
3. The device enters **Speed Selection Mode**

### Selecting Speed

1. Press the **Command Button** (GPIO 7) to cycle through speeds:
   - 10 WPM → 15 WPM → 20 WPM → 25 WPM → 30 WPM → 35 WPM
2. After 1.5 seconds of inactivity, playback begins automatically

### During Playback

- **Press Command Button:** Pause the transmission
- **Press Command Button Again:** Resume from where you paused
- The display shows current status and speed

### Changing Language

To switch between languages, modify the `text[]` array in the code:
- Uncomment the desired language section
- Comment out the others
- Recompile and upload

## 🌍 Language Support

The project includes three complete translations of the same story:

| Language | File/Section | Characters |
|----------|--------------|------------|
| Spanish | `text_es[]` | Standard ASCII (no accents) |
| English | `text_en[]` | Standard ASCII |
| Portuguese | `text_pt[]` | Standard ASCII (no accents) |

**Note:** All special characters (ñ, ç, ã, á, é, etc.) have been converted to ASCII equivalents to ensure compatibility with standard Morse code.

## 📊 Transmission Times

Approximate duration for the 500-word text:

| Speed (WPM) | Duration |
|-------------|----------|
| 10 WPM | ~50 minutes |
| 15 WPM | ~33 minutes |
| 20 WPM | ~25 minutes |
| 25 WPM | ~20 minutes |
| 30 WPM | ~17 minutes |
| 35 WPM | ~14 minutes |

## 🔮 Future Books

Planned additions to the CW Book Reader library:

- "The Time Machine" by H.G. Wells
- "Alice's Adventures in Wonderland" by Lewis Carroll
- "The War of the Worlds" by H.G. Wells
- "Treasure Island" by Robert Louis Stevenson
- "The Adventures of Sherlock Holmes" by Arthur Conan Doyle

All books will be:
- ✅ Public domain
- ✅ Adapted to ~500 words
- ✅ Available in Spanish, English, and Portuguese
- ✅ ASCII-compatible for Morse transmission

## 🛠️ Customization

### Adding Your Own Text

To add a new book or text:

1. Create a new `const char newText[] PROGMEM` array
2. Keep it ASCII-compatible (no special characters)
3. Replace the `text[]` array in the code
4. Update the splash screen title if desired

### Adjusting Timing

- `WAIT_TIME`: Time before auto-start (default: 1500ms)
- `FREQ_SIDETONE`: Sidetone frequency (default: 700Hz)
- `DEBOUNCE_TIME`: Button debounce time (default: 50ms)

## 📄 License

This project is open source and available under the MIT License.

The included text is an adaptation of "20,000 Leagues Under the Sea" by Jules Verne, which is in the **public domain** worldwide.

## 🙏 Credits

- **Original Story:** Jules Verne (1870)
- **Hardware Platform:** [ESP32-C3 Mini Morse Vertical Trainer](https://github.com/LU6APR/ESP32_C3_MINI_MORSE_VERTICAL_TRAINER)
- **Development:** CW Adventures Project
- **Libraries:** U8g2 by oliver

## 📞 Contact & Support

For questions, suggestions, or to contribute new books:
- Open an issue on GitHub
- Contact via amateur radio: **LU6APR**

## 🎓 Educational Use

This project is perfect for:
- Radio amateur clubs
- Morse code training sessions
- STEM education projects
- Language learning (listening practice)
- Public domain literature appreciation

---

**73 de LU6APR** 📡  
*Happy CW Reading!*
