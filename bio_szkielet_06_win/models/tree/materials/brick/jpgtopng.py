from PIL import Image
import os

path = os.path.abspath(os.getcwd())

for file in os.listdir(path):
    if file.endswith(".jpg") or file.endswith(".tga"):
        img = Image.open(file)
        file_name, file_ext = os.path.splitext(file)
        img.save(f"{file_name}.png")
        os.remove(file)
