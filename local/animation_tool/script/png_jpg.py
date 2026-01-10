import zipfile
import os
from PIL import Image
import io


def convert_zip_png_to_jpg_zip(zip_path, output_zip_path, quality=100):
    # Tạo zip mới để ghi JPG
    with zipfile.ZipFile(zip_path, "r") as zip_in, zipfile.ZipFile(
        output_zip_path, "w", zipfile.ZIP_DEFLATED
    ) as zip_out:

        for file in zip_in.namelist():
            if file.lower().endswith(".png"):
                print("Converting:", file)

                # Đọc PNG từ zip vào bộ nhớ
                png_data = zip_in.read(file)
                img = Image.open(io.BytesIO(png_data))

                # Nền đen
                black_bg = Image.new("RGB", img.size, (0, 0, 0))

                # Nếu có alpha → paste lên nền đen
                if img.mode in ("RGBA", "LA"):
                    black_bg.paste(img, mask=img.split()[-1])
                else:
                    black_bg.paste(img)

                # Xuất JPG vào buffer
                jpg_buffer = io.BytesIO()
                jpg_name = os.path.splitext(os.path.basename(file))[0] + ".jpg"
                black_bg.save(jpg_buffer, "JPEG", quality=quality)

                # Ghi vào zip mới
                zip_out.writestr(jpg_name, jpg_buffer.getvalue())

    print("Done! Created:", output_zip_path)


convert_zip_png_to_jpg_zip("image.zip", "image_out.zip")
