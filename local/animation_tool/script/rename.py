import zipfile
import os
import re

input_zip = "noi.zip"
output_zip = "noi_1.zip"
start_index = 1  # index bắt đầu

pattern = re.compile(r"(ezgif-frame-)(\d+)(\.jpg)", re.IGNORECASE)

with zipfile.ZipFile(input_zip, "r") as zin, zipfile.ZipFile(output_zip, "w") as zout:

    files = sorted([name for name in zin.namelist() if name.lower().endswith(".jpg")])

    for i, name in enumerate(files, start=start_index):
        m = pattern.match(name)
        if m:
            prefix, _, ext = m.groups()
            new_name = f"{prefix}{i:03d}{ext}"
            data = zin.read(name)
            zout.writestr(new_name, data)
        else:
            # file không match pattern, giữ nguyên tên
            zout.writestr(name, zin.read(name))

print("Hoàn tất đổi tên ảnh theo index trong ZIP!")
