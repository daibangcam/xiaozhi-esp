import zipfile
from PIL import Image
import io


input_zip = "computer_13825041_resize.zip"
output_zip = f"computer_13825041_resize_zip.zip"
quality = 80

with zipfile.ZipFile(input_zip, "r") as zip_in, zipfile.ZipFile(
    output_zip, "w"
) as zip_out:

    # sort tên file theo thứ tự alphabet
    for file_name in sorted(zip_in.namelist()):
        if file_name.lower().endswith(".jpg"):
            with zip_in.open(file_name) as f:
                im = Image.open(f)
                buffer = io.BytesIO()
                im.save(buffer, format="JPEG", quality=quality)
                buffer.seek(0)
                zip_out.writestr(file_name, buffer.read())
        else:
            zip_out.writestr(file_name, zip_in.read(file_name))

print(f"Đã nén ảnh trong {input_zip} sang {output_zip} giữ nguyên tên và thứ tự")
