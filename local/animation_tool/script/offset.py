import struct

with open("partitions.bin", "rb") as f:
    data = f.read()

entry_size = 32
for i in range(0, len(data), entry_size):
    entry = data[i : i + entry_size]
    if len(entry) < entry_size:
        break
    type_, subtype = entry[0], entry[1]
    offset = struct.unpack("<I", entry[4:8])[0]
    size = struct.unpack("<I", entry[8:12])[0]
    name_bytes = entry[12:28]
    name_bytes = bytes(b for b in name_bytes if b != 0xFF)
    name_bytes = name_bytes.split(b"\x00")[0]
    try:
        name = name_bytes.decode("ascii")
    except:
        name = ""
    if type_ == 2:
        print(
            f"Data partition: subtype=0x{subtype:X}, name='{name}', offset=0x{offset:X}, size={size}"
        )
