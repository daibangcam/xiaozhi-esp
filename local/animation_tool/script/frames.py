import cv2
import os

video_path = "disconnected.mp4"
output_folder = "disconnected_frames.zip"
os.makedirs(output_folder, exist_ok=True)

cap = cv2.VideoCapture(video_path)
fps = cap.get(cv2.CAP_PROP_FPS)  # số frame trên giây của video

count = 0
success = True

while success:
    # Tính frame cần đọc dựa trên fps
    for i in range(int(fps / 30)):  # lấy 30 frame/giây
        success, frame = cap.read()
        if not success:
            break
    if success:
        frame_filename = os.path.join(output_folder, f"frame_{count:05d}.jpg")
        cv2.imwrite(frame_filename, frame)
        count += 1

cap.release()
print(f"Đã lưu {count} ảnh vào thư mục '{output_folder}'")
