import zipfile
import os
from PIL import Image

input_zip = "read.zip"  # File ZIP gốc
output_folder = "output_images"  # Folder lưu ảnh sau khi đổi màu

os.makedirs(output_folder, exist_ok=True)

# Ngưỡng nhận diện màu trắng (tránh lỗi do JPG bị noise)
threshold = 240  # pixel >240 coi như trắng

with zipfile.ZipFile(input_zip, "r") as z:
    for file in z.namelist():
        if file.lower().endswith((".jpg", ".jpeg", ".png")):
            # Mở ảnh
            with z.open(file) as f:
                img = Image.open(f).convert("RGB")

            # Ảnh sang dạng pixel
            pixels = img.load()
            w, h = img.size

            # Thay trắng → đen
            for y in range(h):
                for x in range(w):
                    r, g, b = pixels[x, y]
                    if r > threshold and g > threshold and b > threshold:
                        pixels[x, y] = (0, 0, 0)  # đổi thành đen

            # Lưu ảnh
            out_path = os.path.join(output_folder, os.path.basename(file))
            img.save(out_path)
            print("Đã xử lý:", out_path)
