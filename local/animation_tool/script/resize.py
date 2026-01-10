from PIL import Image, ImageOps
import zipfile
import io

input_zip = "computer_13825041.zip"
output_zip = "computer_13825041_resize.zip"

top_pad = 0
bottom_pad = 0
target_size = (160, 100)

with zipfile.ZipFile(input_zip, "r") as zin, zipfile.ZipFile(output_zip, "w") as zout:

    for name in zin.namelist():
        if not name.lower().endswith((".jpg", ".jpeg", ".png")):
            continue

        with zin.open(name) as file:
            img = Image.open(file)
            img.load()

        padded = ImageOps.expand(
            img, border=(0, top_pad, 0, bottom_pad), fill=(0, 0, 0)
        )

        resized = padded.resize(target_size, Image.LANCZOS)

        img_bytes = io.BytesIO()
        resized.save(img_bytes, format="PNG")
        zout.writestr(name, img_bytes.getvalue())

print("Hoàn tất xử lý tất cả ảnh trong ZIP!")
