import cv2
import tkinter as tk
from tkinter import ttk, Label, Frame
from PIL import Image, ImageTk
import threading
import time
from ttkthemes import ThemedTk
import face_recognition
import numpy as np
import os
import requests
import serial

# --- Bluetooth Setup ---
try:
    ser = serial.Serial("COM7", 9600, timeout=1)
    print("Bluetooth serial connection established.")
except Exception as e:
    print("Bluetooth Serial Error:", e)
    ser = None

# --- Known Face Recognition Setup ---
known_faces_path = r"C:\Users\arsha\OneDrive\Desktop\bot\images"
known_images = []
classnames = []
mylist = os.listdir(known_faces_path)

for cl in mylist:
    img_path = os.path.join(known_faces_path, cl)
    crimg = cv2.imread(img_path)
    if crimg is None:
        continue
    known_images.append(crimg)
    classnames.append(os.path.splitext(cl)[0])

def find_encodings(images):
    encodelist = []
    for img in images:
        img_rgb = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
        try:
            encode = face_recognition.face_encodings(img_rgb)[0]
        except IndexError:
            continue
        encodelist.append(encode)
    return encodelist

encodelist_known = find_encodings(known_images)
print("Encoding complete...")

# --- GUI Setup ---
root = ThemedTk(theme="arc")
root.title("SCOUTX - Surveillance Robot Control")
root.geometry("800x600")
root.configure(bg="#2c3e50")

# Video frame
video_frame = Frame(root, bg="#2c3e50")
video_frame.pack(pady=10)
video_label = Label(video_frame, bg="#2c3e50")
video_label.pack()

# Status bar
status_label = Label(root, text="Status: Idle", bd=1, relief=tk.SUNKEN, anchor=tk.W, font=("Helvetica", 10), bg="#34495e", fg="white")
status_label.pack(side=tk.BOTTOM, fill=tk.X)

# Function to send commands
def send_command(command):
    if ser is not None:
        try:
            ser.write((command + "\n").encode('utf-8'))
        except Exception as e:
            print("Error sending command:", e)
    print(f"Command sent: {command}")
    status_label.config(text=f"Status: {command}")

# Function to stop motors when button is released
def stop_motors(event=None):
    send_command("S")

# Buttons for commands
button_frame = Frame(root, bg="#2c3e50")
button_frame.pack(pady=10)

btn_up = ttk.Button(button_frame, text="↑ Forward", width=10)
btn_down = ttk.Button(button_frame, text="↓ Backward", width=10)
btn_left = ttk.Button(button_frame, text="← Left", width=10)
btn_right = ttk.Button(button_frame, text="→ Right", width=10)
btn_stop = ttk.Button(button_frame, text="■ Stop", width=10, command=stop_motors)

# Bind button press and release events
btn_up.bind("<ButtonPress>", lambda event: send_command("F"))
btn_up.bind("<ButtonRelease>", stop_motors)

btn_down.bind("<ButtonPress>", lambda event: send_command("B"))
btn_down.bind("<ButtonRelease>", stop_motors)

btn_left.bind("<ButtonPress>", lambda event: send_command("L"))
btn_left.bind("<ButtonRelease>", stop_motors)

btn_right.bind("<ButtonPress>", lambda event: send_command("R"))
btn_right.bind("<ButtonRelease>", stop_motors)

# Grid layout for buttons
btn_up.grid(row=0, column=1, padx=5, pady=5)
btn_left.grid(row=1, column=0, padx=5, pady=5)
btn_stop.grid(row=1, column=1, padx=5, pady=5)
btn_right.grid(row=1, column=2, padx=5, pady=5)
btn_down.grid(row=2, column=1, padx=5, pady=5)

# ESP32-CAM stream URL
stream_url = "http://192.168.4.1/stream"

# Global variables for video processing
latest_frame = None
face_recognition_active = True

# Function to process video stream and perform face recognition
def update_video():
    global latest_frame, face_recognition_active
    try:
        r = requests.get(stream_url, stream=True, timeout=5)
    except requests.exceptions.RequestException as e:
        print("Error accessing video stream:", e)
        return
    
    bytes_data = bytes()
    frame_count = 0

    for chunk in r.iter_content(chunk_size=1024):
        bytes_data += chunk
        a = bytes_data.find(b'\xff\xd8')
        b = bytes_data.find(b'\xff\xd9')

        if a != -1 and b != -1:
            jpg = bytes_data[a:b+2]
            bytes_data = bytes_data[b+2:]
            frame = cv2.imdecode(np.frombuffer(jpg, dtype=np.uint8), cv2.IMREAD_COLOR)

            if frame is None:
                continue

            # Resize frame for faster processing
            small_frame = cv2.resize(frame, (0, 0), fx=0.5, fy=0.5)

            # Perform face recognition every 5 frames
            if frame_count % 5 == 0 and face_recognition_active:
                # Convert frame to RGB for face recognition
                rgb_small_frame = cv2.cvtColor(small_frame, cv2.COLOR_BGR2RGB)

                # Find face locations and encodings
                face_locations = face_recognition.face_locations(rgb_small_frame)
                face_encodings = face_recognition.face_encodings(rgb_small_frame, face_locations)

                # Check for recognized faces
                recognized = False
                for encoding in face_encodings:
                    matches = face_recognition.compare_faces(encodelist_known, encoding)
                    face_distances = face_recognition.face_distance(encodelist_known, encoding)

                    if len(face_distances) > 0:
                        best_match_index = np.argmin(face_distances)
                        if matches[best_match_index]:
                            recognized = True
                            break

                if recognized:
                    send_command("T")

            # Update the latest frame
            latest_frame = frame.copy()
            frame_count += 1

            # Display video in GUI
            display_frame = cv2.resize(frame, (640, 480))
            img = ImageTk.PhotoImage(image=Image.fromarray(cv2.cvtColor(display_frame, cv2.COLOR_BGR2RGB)))
            video_label.imgtk = img
            video_label.configure(image=img)

            # Sleep to reduce CPU usage
            time.sleep(0.03)

# Start video processing in a separate thread
video_thread = threading.Thread(target=update_video, daemon=True)
video_thread.start()

def on_closing():
    global face_recognition_active
    face_recognition_active = False  # Stop face recognition
    if ser is not None:
        ser.close()
    root.destroy()

root.protocol("WM_DELETE_WINDOW", on_closing)
root.mainloop()
