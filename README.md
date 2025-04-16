# SSD1306 Library 📚

This is a lightweight and efficient library for controlling SSD1306 OLED displays. It is designed to work seamlessly with the ESP32-S3 microcontroller and does **not** require any Adafruit libraries, making it a standalone solution for your projects.

## Features ✨

- 📺 Supports SSD1306 OLED displays
- 🔗 No dependency on external libraries like Adafruit
- ⚡ Optimized for the ESP32-S3 microcontroller
- 🧩 Easy-to-use API for quick integration
- 💨 Lightweight and fast

## Compatibility ✅

This library has been tested on the following hardware:

- ✅ **ESP32-S3**

## Installation 🛠️

1. Clone or download this repository.
2. Place the `ssd1306` folder in the `lib` directory of your PlatformIO project.
3. Include the library in your code:

   ```cpp
   #include "ssd1306.h"
   ```

## Usage 🚀

Here is a basic example to get started:

```cpp
#include "Arduino.h"
#include "ssd1306.h"
#include "Wire.h"

// Last value is the splash screen (optional), set true to disable it. Default is false.
puroPixel_SSD1306 display(0x3C, 128, 64, &Wire, false);

void setup() {
    Wire.begin(8, 9, 1000000); // Start I2C connection (SDA, SCL, freq)
    display.begin();           // Initialize the display
    display.clear();           // Clear the buffer
    display.drawText(0, 0, "Hello, SSD1306!", 1, 1); // You may also define scale and font color (optional)
    display.update();          // Send buffer to screen
}

void loop() {
    // Your logic here
}
```

## Notes 📝

- This library is specifically designed for the ESP32-S3 and may not work on other microcontrollers without modifications.
- Since it does not rely on Adafruit libraries, it is a lightweight alternative for projects where minimal dependencies are preferred.

## Testing 🧪

This library has been thoroughly tested on the ESP32-S3 using the Arduino framework to ensure reliability and performance.  
If you encounter any issues, please report them in the repository.

## License 📄

This library is open-source and available under the [MIT License](LICENSE).

## Contributions 🤝

Contributions, bug reports, and feature requests are welcome!  
Feel free to open an issue or submit a pull request.

## Contact 📬

For questions or support, please reach out to the repository maintainer.

---

🎉 Enjoy using the SSD1306 library with your ESP32-S3 projects!
