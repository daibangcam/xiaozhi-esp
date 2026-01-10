# Hướng Dẫn Các File Python (script/)

Dự án này chứa các script Python để xử lý hình ảnh và video cho thiết bị ESP32 chatbot. Dưới đây là mô tả chi tiết từng file:

---

## 📋 Danh Sách Các Script

### 1. **bg.py** - Đổi Nền Trắng Thành Đen
**Mục đích:** Đọc file ZIP chứa hình ảnh, thay thế tất cả pixel trắng thành pixel đen.

**Cách hoạt động:**
- Mở file `read.zip` (đầu vào)
- Duyệt qua từng hình ảnh JPG/PNG trong ZIP
- Tìm tất cả pixel có giá trị RGB > 240 (coi là trắng)
- Đổi thành màu đen (0, 0, 0)
- Lưu kết quả vào thư mục `output_images`

**Cấu hình:**
- `input_zip = "read.zip"` - Tên file ZIP đầu vào
- `output_folder = "output_images"` - Thư mục lưu ảnh
- `threshold = 240` - Ngưỡng nhận diện màu trắng

---

### 2. **crop.py** - Cắt Vùng Hình Ảnh Trong ZIP
**Mục đích:** Cắt một phần nhỏ từ các hình ảnh trong file ZIP.

**Cách hoạt động:**
- Mở `loading_frames.zip`
- Cắt vùng từ tọa độ (85, 85) với kích thước 155x155 pixel
- Lưu các ảnh đã cắt vào `loading_frames_crop.zip`

**Cấu hình:**
- `x, y = 85, 85` - Tọa độ bắt đầu cắt
- `w, h = 155, 155` - Chiều rộng và chiều cao vùng cắt
- `input_zip_path = "loading_frames.zip"` - File ZIP đầu vào
- `output_zip_path = "loading_frames_crop.zip"` - File ZIP đầu ra

---

### 3. **extract.py** - Chuyển Đổi Header C Thành RGB565
**Mục đích:** Đọc file header C (*.h) chứa dữ liệu JPG, chuyển đổi sang định dạng RGB565 (dành cho ESP32).

**Cách hoạt động:**
- Đọc file header C, trích xuất dữ liệu ảnh dạng hex bytes
- Chuyển đổi ảnh JPG sang định dạng RGB (8-bit)
- Chuyển RGB thành RGB565 (16-bit) để tiết kiệm bộ nhớ
- Tạo file header C mới với dữ liệu RGB565 và thông tin kích thước
- Tạo mảng pointer để trỏ tới từng frame

**Công thức chuyển đổi RGB565:**
```
RGB565 = ((R & 0xF8) << 8) | ((G & 0xFC) << 3) | (B >> 3)
```

---

### 4. **frames.py** - Trích Xuất Frame Từ Video
**Mục đích:** Tách các frame từ file video MP4 thành hình ảnh JPG riêng.

**Cách hoạt động:**
- Mở file video `disconnected.mp4`
- Đọc FPS (frame/giây) của video
- Trích xuất frame với tốc độ 30 frame/giây
- Lưu từng frame thành file JPG với tên `frame_00000.jpg`, `frame_00001.jpg`, v.v.

**Cấu hình:**
- `video_path = "disconnected.mp4"` - File video đầu vào
- `output_folder = "disconnected_frames.zip"` - Thư mục lưu frame
- Tốc độ lấy: 30 frame/giây

---

### 5. **offset.py** - Đọc Partition Table ESP32
**Mục đích:** Phân tích file `partitions.bin` (bảng phân vùng bộ nhớ của ESP32).

**Cách hoạt động:**
- Đọc file `partitions.bin`
- Phân tích từng entry (32 byte mỗi entry)
- Hiển thị thông tin về data partition: loại, tên, địa chỉ offset, kích thước

**Thông tin trích xuất:**
- Kiểu partition (type)
- Loại con (subtype)
- Tên partition
- Địa chỉ offset trong bộ nhớ
- Kích thước partition

---

### 6. **paste_icon.py** - Dán Icon Lên Ảnh Trong ZIP
**Mục đích:** Dán icon PNG (có độ trong suốt) vào các hình ảnh trong file ZIP.

**Cách hoạt động:**
- Mở file `noi.zip` và file icon `mic.png`
- Resize icon thành 30x30 pixel
- Duyệt qua từng ảnh trong ZIP
- Chuyển ảnh sang RGB (để xử lý JPG)
- Dán icon vào vị trí giữa, 60 pixel từ trên
- Lưu vào `noi_0.zip`

**Cấu hình:**
- `INPUT_ZIP = "noi.zip"` - File ZIP chứa ảnh
- `OUTPUT_ZIP = "noi_0.zip"` - File ZIP đầu ra
- `ICON_FILE = "mic.png"` - File icon cần dán
- Icon size: 30x30 pixel
- Vị trí dán: giữa chiều ngang, 60 pixel từ trên

---

### 7. **png_jpg.py** - Chuyển Đổi PNG Sang JPG Trong ZIP
**Mục đích:** Chuyển đổi tất cả file PNG thành JPG và lưu vào ZIP mới.

**Cách hoạt động:**
- Mở file `image.zip`
- Duyệt qua từng file PNG
- Tạo nền đen RGB
- Dán ảnh PNG (có alpha) lên nền đen
- Xuất thành JPG với chất lượng 100
- Lưu vào `image_out.zip`

**Cấu hình:**
- `quality=100` - Chất lượng JPG (0-100)
- Nền: đen (0, 0, 0)

---

### 8. **rename.py** - Đổi Tên File Ảnh Trong ZIP
**Mục đích:** Đổi tên file ảnh JPG trong ZIP theo quy tắc định sẵn.

**Cách hoạt động:**
- Mở file `noi.zip`
- Tìm file JPG có tên theo pattern: `ezgif-frame-XXX.jpg`
- Đổi thành: `ezgif-frame-001.jpg`, `ezgif-frame-002.jpg`, v.v. (index từ 1)
- Lưu vào `noi_1.zip`

**Pattern Regex:**
```
(ezgif-frame-)(\d+)(\.jpg)
```

**Cấu hình:**
- `start_index = 1` - Index bắt đầu từ 1

---

### 9. **resize.py** - Thay Đổi Kích Thước Ảnh Trong ZIP
**Mục đích:** Thay đổi kích thước và padding cho ảnh trong ZIP.

**Cách hoạt động:**
- Mở `computer_13825041.zip`
- Thêm padding (khoảng trắng) ở trên/dưới
- Resize ảnh về kích thước 160x100 pixel
- Lưu vào `computer_13825041_resize.zip`

**Cấu hình:**
- `target_size = (160, 100)` - Kích thước mục tiêu
- `top_pad = 0` - Padding trên
- `bottom_pad = 0` - Padding dưới
- Interpolation: `Image.LANCZOS` (chất lượng cao)

---

### 10. **trans.py** - Lật Ảnh Theo Chiều Dọc
**Mục đích:** Lật tất cả ảnh JPG từ trên xuống dưới (flip vertically).

**Cách hoạt động:**
- Đọc từng ảnh JPG từ thư mục `temp`
- Lật ảnh theo chiều dọc (Top ↔ Bottom)
- Lưu vào thư mục `temp_output`

**Cấu hình:**
- `input_folder = "temp"` - Thư mục chứa ảnh
- `output_folder = "temp_output"` - Thư mục lưu kết quả

---

### 11. **zip.py** - Nén Ảnh Trong ZIP Giữ Nguyên Thứ Tự
**Mục đích:** Nén lại các ảnh trong ZIP với chất lượng thấp hơn, giữ nguyên thứ tự alphabet.

**Cách hoạt động:**
- Mở `computer_13825041_resize.zip`
- Sắp xếp file theo thứ tự alphabet
- Chuyển đổi ảnh JPG sang JPG với chất lượng 80%
- Lưu vào `computer_13825041_resize_zip.zip`

**Cấu hình:**
- `quality = 80` - Chất lượng JPG (0-100)

---

## 🔧 Yêu Cầu Thư Viện

```bash
pip install Pillow opencv-python
```

- **Pillow** - Xử lý hình ảnh (PNG, JPG)
- **OpenCV (cv2)** - Xử lý video

---


## 💡 Lưu Ý

- Mỗi script có thể chạy độc lập, chỉ cần file đầu vào đúng tên
- Sửa biến `input_zip`, `output_folder`, `input_folder`, v.v. để thay đổi đường dẫn
- Các ảnh được lưu ở định dạng RGB565 để tiết kiệm bộ nhớ trên ESP32
- File header C (*.h) được tạo từ `extract.py` dùng để nạp vào code ESP32

