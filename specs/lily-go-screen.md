# Hardware

- Datasheet link: https://github.com/Xinyuan-LilyGO/LilyGo-T-RGB/blob/master/schematic/T-RGB.pdf
- Documentation: https://lilygo.cc/products/t-rgb?_pos=1&_psq=t-rgb&_ss=e&_v=1.0
- MCU: ESP32-S3R8
- Display: LilyGo T-RGB 2.1-inch TFT LCD (half-circle or full-circle)
- Display Driver: ST7701S
- Display interface: 3-wire SPI + 18-bit RGB (RGB666 physical bus, RGB565 driven by ESP32-S3)
- Display resolution: 480(H)x480(V)
- Touchscreen type: Capacitive touch
- Drive IC model: FT3267 (2.1-inch half-circle) / CST820 (2.1-inch full-circle)

## Pin Mapping

- LCD_BL (LCD backlight control, high level enable): GPIO 46
- RGB Interface (D0-D17):
	- D0: GPIO 44
	- D1: GPIO 21
	- D2: GPIO 18
	- D3: GPIO 17
	- D4: GPIO 16
	- D5: GPIO 15
	- D6: GPIO 14
	- D7: GPIO 13
	- D8: GPIO 12
	- D9: GPIO 11
	- D10: GPIO 10
	- D11: GPIO 9
	- D12: GPIO 43
	- D13: GPIO 7
	- D14: GPIO 6
	- D15: GPIO 5
	- D16: GPIO 3
	- D17: GPIO 2
- RGB Control Signals:
	- VSYNC: GPIO 41
	- HSYNC: GPIO 47
	- DE (Data Enable): GPIO 45
	- PCLK (Pixel Clock): GPIO 42
- Touchscreen (I2C):
	- I2C Data (SDA): GPIO 8
	- I2C Clock (SCL): GPIO 48
	- Interrupt pin (INT): GPIO 1
	- Reset pin (RST): XL9555 IO1 (I2C GPIO expander, not direct ESP32 GPIO)
