### Phần mềm
- **PlatformIO**: IDE cho ESP32
- **Arduino Framework**: Core Arduino
- **Thư viện phụ thuộc**: Xem `platformio.ini`

## 🚀 Cài Đặt & Biên Dịch

### 1. Clone hoặc tải dự án
```bash
git clone <repository>
cd esp32_client
```

### 2. Cài đặt PlatformIO
Cài đặt PlatformIO IDE hoặc extension VS Code.

### 3. Cấu hình bo mạch
Chỉnh sửa `platformio.ini` và chọn environment phù hợp:
```ini
[env:4d_systems_esp32s3_gen4_r8n16]
```

### 4. Chọn cấu hình phần cứng
Chỉnh sửa [include/setup.h](include/setup.h) để chọn cấu hình bo mạch:


### 5. Biên dịch & Upload
```bash
pio run -t upload
# hoặc qua PlatformIO IDE
```

## 📁 Cấu Trúc Dự Án

```
esp32_client/
├── src/                           # Mã nguồn chính
│   ├── main.cpp                   # Điểm vào chương trình
│   ├── obj.cpp                    # Quản lý đối tượng toàn cục
│   ├── touch.cpp                  # Xử lý cảm ứng
│   ├── utils.cpp                  # Tiện ích chung
│   └── core/
│       ├── audio/
│       │   ├── mic.cpp            # Thu âm PCM từ micro
│       │   └── spk.cpp            # Phát âm thanh qua loa
│       ├── display/
│       │   ├── display.cpp        # Điều khiển màn hình LCD
│       │   ├── animation.cpp      # Xử lý hoạt ảnh
│       │   └── led.cpp            # Điều khiển LED RGB
│       └── protocol/
│           ├── connection.cpp     # Kết nối WiFi & WebSocket
│           └── ota.cpp            # Cập nhật firmware OTA
├── include/                       # File header
│   ├── setup.h, obj.h, touch.h, utils.h
│   └── core/                      # Header thư viện core
├── boards/
│   └── configs/                   # Cấu hình phần cứng cho từng mẫu
│       ├── cube-1.54tft-240x240.h
│       ├── mach_thuong-*.h
│       └── mach_tim-*.h
├── data/                          # Tài nguyên (hình ảnh, SPIFFS)
├── lib/                           # Thư viện tùy chỉnh
├── platformio.ini                 # Cấu hình PlatformIO
└── build.ps1                      # Script xây dựng (PowerShell)
```

## 🎨 Hoạt Ảnh Có Sẵn

Dự án hỗ trợ 15+ hoạt ảnh biểu cảm:
- 😴 `sleep.h` - Ngủ
- 😴 `wake_up.h` - Thức dậy
- 😊 `blink.h` - Chớp mắt
- 😴 `wink.h` - Nháy mắt
- 😐 `squint.h` - Nheo mắt
- 😠 `angry.h` - Tức giận
- 😢 `sad.h` - Buồn
- 🤔 `think.h` - Suy nghĩ
- ⏳ `loading.h` - Đang tải
- 🔄 `roll.h` - Xoay
- 🎭 `shake.h` - Lắc
- ⚡ `light_shake_01.h`, `light_shake_02.h` - Lắc nhẹ
- 💡 `light_blink.h` - Chớp nhẹ
- ↗️ `tilt_right.h` - Nghiêng phải
- ❌ `disconnected.h` - Mất kết nối

## ⚙️ Cấu Hình

### Setup.h
Định nghĩa các chân GPIO, cấu hình I2S, WiFi, và các tham số khác.

### Cấu Hình Bo Mạch
Chọn cấu hình phù hợp từ thư mục `boards/configs/`:
- **cube-1.54tft-240x240.h** - Bo mạch Cube (màn hình 1.54")
- **mach_thuong-1.28tft-240x240.h** - Mẫu tiêu chuẩn (1.28")
- **mach_thuong-1.54tft-240x240.h** - Mẫu tiêu chuẩn (1.54")
- **mach_tim-1.28tft-240x240.h** - Mẫu "Tim" (1.28")
- **mach_tim-1.54tft-240x240.h** - Mẫu "Tim" (1.54")

## 📡 Giao Thức Kết Nối

- **WiFi**: WiFi Manager với AP fallback
- **WebSocket**: Truyền dữ liệu thời gian thực
- **Audio**: Định dạng PCM qua WebSocket

## 🔌 Chân GPIO Điển Hình

Xem file cấu hình bo mạch tương ứng trong `boards/configs/` để biết chi tiết.

## 🛠️ Lệnh Build

### Sử dụng PlatformIO CLI:
```bash
# Build
pio run

# Build và upload
pio run -t upload

# Clean build
pio run -t clean

# Monitor serial
pio device monitor -b 115200
```

### Sử dụng script PowerShell (Windows):
```powershell
./build.ps1
```

## 📚 Thư Viện Phụ Thuộc

- **WebSockets** (links2004) - Giao tiếp WebSocket
- **WiFiManager** - Quản lý kết nối WiFi
- **Adafruit NeoPixel** - Điều khiển LED RGB
- **TFT_eSPI** (bodmer) - Driver màn hình LCD
- **TJpg_Decoder** (bodmer) - Giải mã ảnh JPEG
- **ESP32-audioI2S** (esphome) - Xử lý âm thanh I2S

## 🐛 Gỡ Lỗi

Kích hoạt Serial Monitor để xem log:
```bash
pio device monitor -b 115200
```

Các debug flag có thể được thêm vào `platformio.ini`:
```ini
build_flags = 
    -Wno-attributes
    -DDEBUGGING=1
```

## 📜 Giấy Phép

Copyright (C) 2025 TrongDung143

Dự án này được cấp phép theo **GNU General Public License v3.0 (GPLv3)**.

Xem file [LICENSE](LICENSE) để biết toàn bộ nội dung giấy phép.

## 👨‍💻 Tác Giả

**TrongDung143**

## 📞 Liên Hệ & Hỗ Trợ
[facebook](https://www.facebook.com/ltd.nma.143)
[email](trongdung143@gmail.com)

