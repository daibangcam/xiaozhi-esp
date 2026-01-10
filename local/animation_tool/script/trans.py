from PIL import Image
import os

input_folder = "temp"
output_folder = "temp_output"
os.makedirs(output_folder, exist_ok=True)

for file in os.listdir(input_folder):
    if file.lower().endswith(".jpg"):
        img = Image.open(os.path.join(input_folder, file))
        flipped = img.transpose(Image.FLIP_TOP_BOTTOM)
        flipped.save(os.path.join(output_folder, file))
        print("Done:", file)
