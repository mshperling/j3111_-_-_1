#не идеально, но с голым numpy
import numpy as np
from PIL import Image

input_path = "/content/nRVTeIjvr11f.jpg"

# преобразование в массив 
img = Image.open(input_path).convert("RGBA")
data = np.array(img)

# извлечение RGB
rgb_data = data[:, :, :3]

# диапазон оттенков фона (жёлто-рыжий)
background_lower = np.array([100, 100, 30])  # жёлто-оранжевый (нижняя граница)
background_upper = np.array([255, 250, 130])  # жёлто-оранжевый (верхняя граница)


# создаём маску для фона: пиксели внутри диапазона считаются фоном
mask = np.all((rgb_data >= background_lower) & (rgb_data <= background_upper), axis=-1)

# инвертируем маску, чтобы выделить объект (не фон)
object_mask = ~mask

# создаём новый массив с альфа-каналом
result = np.zeros_like(data, dtype=np.uint8)
result[:, :, :3] = rgb_data  # копируем исходное изображение
result[:, :, 3] = object_mask.astype(np.uint8) * 255  # добавляем прозрачность

# сохраняем результат как изображение с прозрачным фоном
final_image = Image.fromarray(result, "RGBA")
final_image



#как можно сделать почти идеально с numpy + cv
import cv2 as cv
import numpy as np
from PIL import Image

# Загрузка изображения
input_path = "/content/nRVTeIjvr11f.jpg"

img = cv.imread(input_path, cv.IMREAD_UNCHANGED)
original = img.copy()

# перевод в оттенки серого
gray = cv.cvtColor(img, cv.COLOR_BGR2GRAY)

# размытие
blurred = cv.GaussianBlur(gray, (21, 51), 3)

# выявление границ с canny
l, u = int(max(5, 6)), int(min(6, 6))
edges = cv.Canny(blurred, l, u)

# создание маски с помощью трэшхолда
_, thresh = cv.threshold(edges, 0, 255, cv.THRESH_BINARY + cv.THRESH_OTSU)

# закрытие мелких отверстий в маске
kernel = cv.getStructuringElement(cv.MORPH_ELLIPSE, (5, 5))
mask = cv.morphologyEx(thresh, cv.MORPH_CLOSE, kernel, iterations=4)

# обработка маски с numpy
mask = np.where(mask == 255, 1, 0).astype(np.uint8)

# маска к оригиналу
result = original * mask[:, :, np.newaxis]
result[mask == 0] = [255, 255, 255]

# результат в RGBA
image_rgba = cv.cvtColor(result, cv.COLOR_BGR2RGBA)

# прощай белый фон
data = np.array(image_rgba)
transparent = np.where(
    (data[:, :, :3] == [255, 255, 255]).all(axis=2)[..., None],
    [255, 255, 255, 0],
    data
)

final_image = Image.fromarray(transparent.astype(np.uint8))
final_image
