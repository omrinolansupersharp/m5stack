Here’s a clear summary of what each of the three files in your Arduino project does:

---

### 📄 `MyProject.ino` — **Main Program**
- This is the **entry point** of your Arduino sketch.
- It contains the standard `setup()` and `loop()` functions.
- It uses functions from `LEDController.h` to:
  - Set up pin 13 as an output.
  - Blink an LED on pin 13 every 500 milliseconds.

---

### 📄 `LEDController.h` — **Header File**
- Declares the functions `initLED()` and `blinkLED()` so they can be used in other files.
- Uses `#pragma once` to ensure the file is only included once during compilation.
- Think of it as a **"function menu"** for other files to know what’s available.

---

### 📄 `LEDController.cpp` — **Source File**
- Contains the **actual code (definitions)** for the functions declared in the header.
- Implements:
  - `initLED(int pin)`: sets the pin mode to output.
  - `blinkLED(int pin, int delayTime)`: turns the LED on and off with a delay.

---

### 🧠 Analogy
Imagine you're building a machine:
- `MyProject.ino` is the **control panel**.
- `LEDController.h` is the **instruction manual**.
- `LEDController.cpp` is the **machinery** that does the work.
