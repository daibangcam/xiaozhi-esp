from PIL import Image
import re
import sys
import os
from io import BytesIO


def extract_jpg_bytes_from_header(header_path):
    """Đọc file header, trả về dict {frame_name: bytes}"""
    with open(header_path, "r") as f:
        content = f.read()

    frames = {}
    # Tìm tất cả các mảng uint8_t frame
    frame_blocks = re.findall(
        r"const uint8_t (\w+)\[\] PROGMEM = \{(.*?)\};", content, re.S
    )
    for name, data_block in frame_blocks:
        hex_bytes = re.findall(r"0x([0-9A-Fa-f]{2})", data_block)
        frames[name] = bytes(int(b, 16) for b in hex_bytes)
    return frames


def rgb_to_rgb565(r, g, b):
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3)


def convert_frames_to_rgb565(frames_dict, out_header_path):
    with open(out_header_path, "w") as f:
        f.write("#include <stdint.h>\n\n")
        frame_names = []
        for frame_name, jpg_bytes in frames_dict.items():
            img = Image.open(BytesIO(jpg_bytes)).convert("RGB")
            width, height = img.size
            pixels = list(img.getdata())
            rgb_name = f"{frame_name}_rgb565"
            frame_names.append(rgb_name)

            # Lưu trực tiếp vào PROGMEM (Flash)
            f.write(f"const uint16_t {rgb_name}[{width*height}] PROGMEM = {{\n")
            for i, (r, g, b) in enumerate(pixels):
                val = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3)
                f.write(f"0x{val:04X}, ")
                if (i + 1) % 12 == 0:
                    f.write("\n")
            f.write("\n};\n\n")
            f.write(f"const uint16_t {rgb_name}_width = {width};\n")
            f.write(f"const uint16_t {rgb_name}_height = {height};\n\n")

        # Tạo mảng frame pointer
        f.write("const uint16_t* const mo_mat_rgb565_frames[] PROGMEM = {\n")
        for name in frame_names:
            f.write(f"  {name},\n")
        f.write("};\n\n")
        f.write(f"const uint16_t mo_mat_NUM_FRAMES = {len(frame_names)};\n")
        f.write("AnimationInfo mo_mat_rgb565 = {\n")
        f.write("  mo_mat_rgb565_frames,\n")
        f.write("  mo_mat_NUM_FRAMES\n")
        f.write("};\n")

    print(f"Xong! File RGB565 header (PROGMEM): {out_header_path}")


if __name__ == "__main__":
    if len(sys.argv) < 3:
        print(
            "Cách dùng: python jpg_header_to_rgb565.py <input_header.h> <output_header.h>"
        )
    else:
        frames = extract_jpg_bytes_from_header(sys.argv[1])
        convert_frames_to_rgb565(frames, sys.argv[2])
