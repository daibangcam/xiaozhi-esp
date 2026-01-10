import zipfile
from io import BytesIO
from PIL import Image

x, y = 85, 85  # vị trí dùng ảnh trong code luôn
w, h = 155, 155
crop_box = (x, y, x + w, y + h)

input_zip_path = "loading_frames.zip"
output_zip_path = "loading_frames_crop.zip"

with zipfile.ZipFile(input_zip_path, "r") as zin:
    with zipfile.ZipFile(output_zip_path, "w") as zout:
        for file_name in zin.namelist():
            if file_name.lower().endswith((".jpg", ".jpeg", ".png")):
                with zin.open(file_name) as file:
                    img = Image.open(file)
                    img_cropped = img.crop(crop_box)

                    buf = BytesIO()
                    img_cropped.save(buf, format=img.format)
                    buf.seek(0)

                    zout.writestr(file_name, buf.read())
            else:
                zout.writestr(file_name, zin.read(file_name))

print("Đã tạo ZIP mới với các ảnh đã cắt vùng.")
