import zipfile
from io import BytesIO
from PIL import Image

INPUT_ZIP = "noi.zip"
OUTPUT_ZIP = "noi_0.zip"
ICON_FILE = "mic.png"

# chuẩn bị icon 30px
icon = Image.open(ICON_FILE).convert("RGBA")
icon = icon.resize((30, 30), Image.LANCZOS)
iw, ih = icon.size

with zipfile.ZipFile(OUTPUT_ZIP, "w", zipfile.ZIP_DEFLATED) as zout:
    with zipfile.ZipFile(INPUT_ZIP, "r") as zin:
        for name in zin.namelist():

            if not name.lower().endswith((".jpg", ".jpeg", ".png")):
                continue

            # đọc file vào bộ nhớ, tránh lỗi seek on closed file
            file_bytes = BytesIO(zin.read(name))
            base = Image.open(file_bytes).convert("RGB")  # JPG fix alpha

            bw, bh = base.size

            # vị trí icon
            x = (bw - iw) // 2
            y = 60

            # dán icon PNG trong suốt
            base.paste(icon, (x, y), icon)

            # lưu vào RAM trước khi nén vào zip
            mem = BytesIO()
            ext = name.lower().split(".")[-1]

            if ext == "png":
                base.save(mem, format="PNG")
            else:
                base.save(mem, format="JPEG", quality=95)

            mem.seek(0)
            zout.writestr(name, mem.read())

            print("Đã xử lý:", name)

print("Xong → output.zip")
