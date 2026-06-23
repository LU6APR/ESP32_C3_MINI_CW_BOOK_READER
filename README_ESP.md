# 📚 Lector de Libros CW - Audiolibros en Código Morse para ESP32-C3

Un proyecto único basado en ESP32-C3 que transforma literatura clásica de dominio público en audiolibros de código Morse. ¡Entrena tus habilidades de recepción CW mientras disfrutas de historias atemporales de los más grandes autores del mundo!

## 🎯 Idea del Proyecto

**Lector de Libros CW** es una herramienta innovadora de entrenamiento en código Morse que va más allá de simples palabras o QSOs. En lugar de practicar con caracteres aleatorios, este dispositivo reproduce historias completas en código Morse, permitiendo a los radioaficionados y entusiastas del CW:

- **Mejorar las habilidades de recepción** a través de sesiones de escucha extendidas
- **Disfrutar de la literatura** mientras entrenan (aprendizaje por inmersión)
- **Practicar a diferentes velocidades** (10-35 WPM)
- **Pausar y reanudar** en cualquier momento
- **Experimentar historias clásicas** en tres idiomas diferentes

El primer libro incluido es **"20.000 Leguas de Viaje Submarino"** de Julio Verne (1870), adaptado a aproximadamente 500 palabras y disponible en **español, inglés y portugués**.

## 📖 Libro Destacado: 20.000 Leguas de Viaje Submarino

**Autor Original:** Julio Verne (1870)  
**Estado:** Dominio Público  
**Adaptación:** Resumen de ~500 palabras  
**Idiomas Disponibles:**
- 🇪🇸 Español
- 🇺🇸 Inglés (English)
- 🇧🇷 Portugués (Português)

La historia sigue al profesor Aronnax, su sirviente Conseil y el arponero canadiense Ned Land mientras se embarcan en un viaje extraordinario a bordo del misterioso submarino Nautilus, comandado por el enigmático Capitán Nemo.

## ✨ Características

- **Control de Velocidad Variable:** 10, 15, 20, 25, 30 o 35 WPM
- **Funcionalidad Play/Pausa:** Pausa la transmisión en cualquier momento y reanuda desde donde lo dejaste
- **Inicio Automático:** Después de seleccionar la velocidad, la reproducción comienza automáticamente después de 1.5 segundos
- **Retroalimentación Visual:** La pantalla OLED muestra el estado actual (selección de velocidad, reproduciendo, pausado)
- **UI Centrada:** Todo el texto está perfectamente centrado en la pantalla
- **Monitor de Sidetone:** Retroalimentación de audio de 700 Hz para monitoreo local
- **Salida de Llave:** Salida de llave externa para transmisores u otros dispositivos
- **Eficiente en Memoria:** Texto almacenado en PROGMEM para ahorrar RAM
- **Tabla Morse Completa:** Soporta letras, números y puntuación común

## 🔧 Hardware Requerido

Este proyecto está diseñado para la plataforma de hardware **ESP32-C3 Mini Morse Vertical Trainer**.

**Documentación completa del hardware y esquemáticos:**  
👉 [ESP32-C3 Mini Morse Vertical Trainer](https://github.com/LU6APR/ESP32_C3_MINI_MORSE_VERTICAL_TRAINER)

### Configuración de Pines

| Función | Pin GPIO |
|---------|----------|
| Entrada Dit | GPIO 2 |
| Entrada Dah | GPIO 3 |
| Botón de Comando | GPIO 7 |
| Salida Sidetone | GPIO 1 |
| Salida de Llave | GPIO 6 |
| I2C SDA | GPIO 5 |
| I2C SCL | GPIO 4 |

### Pantalla

- **Tipo:** SH1106 OLED 128x64
- **Interfaz:** I2C
- **Librería:** U8g2

## 📦 Instalación

### Requisitos Previos

1. **Arduino IDE** o **PlatformIO**
2. **Paquete de Placas ESP32** (versión 2.x o posterior)
3. **Librerías Requeridas:**
   - `U8g2` por oliver (para pantalla OLED)
   - `Wire` (incluida con el paquete ESP32)

### Pasos

1. Clona o descarga este repositorio
2. Abre el archivo `.ino` en Arduino IDE
3. Selecciona la placa: **ESP32C3 Dev Module**
4. Configura USB CDC On Boot: **Enabled**
5. Sube el código a tu ESP32-C3 Mini

## 🎮 Uso

### Configuración Inicial

1. Enciende el dispositivo
2. Observa la pantalla de inicio: "CW Aventuras - 20.000 Leguas"
3. El dispositivo entra en **Modo de Selección de Velocidad**

### Seleccionando Velocidad

1. Presiona el **Botón de Comando** (GPIO 7) para ciclar entre velocidades:
   - 10 WPM → 15 WPM → 20 WPM → 25 WPM → 30 WPM → 35 WPM
2. Después de 1.5 segundos de inactividad, la reproducción comienza automáticamente

### Durante la Reproducción

- **Presionar Botón de Comando:** Pausa la transmisión
- **Presionar Botón de Comando Nuevamente:** Reanuda desde donde pausaste
- La pantalla muestra el estado actual y la velocidad

### Cambiando Idioma

Para cambiar entre idiomas, modifica el array `text[]` en el código:
- Descomenta la sección del idioma deseado
- Comenta las otras
- Recompila y sube

## 🌍 Soporte de Idiomas

El proyecto incluye tres traducciones completas de la misma historia:

| Idioma | Archivo/Sección | Caracteres |
|--------|-----------------|------------|
| Español | `text_es[]` | ASCII estándar (sin acentos) |
| Inglés | `text_en[]` | ASCII estándar |
| Portugués | `text_pt[]` | ASCII estándar (sin acentos) |

**Nota:** Todos los caracteres especiales (ñ, ç, ã, á, é, etc.) han sido convertidos a equivalentes ASCII para garantizar compatibilidad con el código Morse estándar.

## 📊 Tiempos de Transmisión

Duración aproximada para el texto de 500 palabras:

| Velocidad (WPM) | Duración |
|-----------------|----------|
| 10 WPM | ~50 minutos |
| 15 WPM | ~33 minutos |
| 20 WPM | ~25 minutos |
| 25 WPM | ~20 minutos |
| 30 WPM | ~17 minutos |
| 35 WPM | ~14 minutos |

## 🔮 Libros Futuros

Adiciones planificadas a la biblioteca del Lector de Libros CW:

- "La Máquina del Tiempo" de H.G. Wells
- "Alicia en el País de las Maravillas" de Lewis Carroll
- "La Guerra de los Mundos" de H.G. Wells
- "La Isla del Tesoro" de Robert Louis Stevenson
- "Las Aventuras de Sherlock Holmes" de Arthur Conan Doyle

Todos los libros serán:
- ✅ De dominio público
- ✅ Adaptados a ~500 palabras
- ✅ Disponibles en español, inglés y portugués
- ✅ Compatibles con ASCII para transmisión Morse

## 🛠️ Personalización

### Agregando Tu Propio Texto

Para agregar un nuevo libro o texto:

1. Crea un nuevo array `const char nuevoTexto[] PROGMEM`
2. Mantenlo compatible con ASCII (sin caracteres especiales)
3. Reemplaza el array `text[]` en el código
4. Actualiza el título de la pantalla de inicio si lo deseas

### Ajustando Tiempos

- `WAIT_TIME`: Tiempo antes del inicio automático (predeterminado: 1500ms)
- `FREQ_SIDETONE`: Frecuencia del sidetone (predeterminado: 700Hz)
- `DEBOUNCE_TIME`: Tiempo de antirrebote del botón (predeterminado: 50ms)

## 📄 Licencia

Este proyecto es de código abierto y está disponible bajo la Licencia MIT.

El texto incluido es una adaptación de "20.000 Leguas de Viaje Submarino" de Julio Verne, que se encuentra en **dominio público** a nivel mundial.

## 🙏 Créditos

- **Historia Original:** Julio Verne (1870)
- **Plataforma de Hardware:** [ESP32-C3 Mini Morse Vertical Trainer](https://github.com/LU6APR/ESP32_C3_MINI_MORSE_VERTICAL_TRAINER)
- **Desarrollo:** Proyecto CW Aventuras
- **Librerías:** U8g2 por oliver

## 📞 Contacto y Soporte

Para preguntas, sugerencias o para contribuir con nuevos libros:
- Abre un issue en GitHub
- Contacta vía radioafición: **LU6APR**

## 🎓 Uso Educativo

Este proyecto es perfecto para:
- Clubes de radioafición
- Sesiones de entrenamiento en código Morse
- Proyectos educativos STEM
- Aprendizaje de idiomas (práctica de escucha)
- Apreciación de literatura de dominio público

---

**73 de LU6APR** 📡  
*¡Felices Lecturas CW!*
