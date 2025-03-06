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

# Buttons for commands
def send_command(command):
    print(f"Command sent: {command}")
    status_label.config(text=f"Status: {command}")

button_frame = Frame(root, bg="#2c3e50")
button_frame.pack(pady=10)

forward_button = ttk.Button(button_frame, text="Forward", command=lambda: send_command("Forward"))
forward_button.grid(row=0, column=1, padx=5, pady=5)

backward_button = ttk.Button(button_frame, text="Backward", command=lambda: send_command("Backward"))
backward_button.grid(row=2, column=1, padx=5, pady=5)

left_button = ttk.Button(button_frame, text="Left", command=lambda: send_command("Left"))
left_button.grid(row=1, column=0, padx=5, pady=5)

right_button = ttk.Button(button_frame, text="Right", command=lambda: send_command("Right"))
right_button.grid(row=1, column=2, padx=5, pady=5)

stop_button = ttk.Button(button_frame, text="Stop", command=lambda: send_command("Stop"))
stop_button.grid(row=1, column=1, padx=5, pady=5)

# Global variables for video processing
latest_frame = None
face_recognition_active = True

# Function to process video stream and perform face recognition
def update_video():
    global latest_frame, face_recognition_active
    cap = cv2.VideoCapture(0)  # Use webcam instead of IP cam
    if not cap.isOpened():
        print("Error: Could not open webcam.")
        return

    frame_count = 0
    while True:
        ret, frame = cap.read()
        if not ret:
            print("Error: Could not read frame.")
            break

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
                        name = classnames[best_match_index].upper()
                        recognized = True
                        print(f"Face recognized: {name}")
                        send_command(f"Face Recognized: {name}")
                        break

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

    cap.release()

# Start video processing in a separate thread
video_thread = threading.Thread(target=update_video, daemon=True)
video_thread.start()

def on_closing():
    global face_recognition_active
    face_recognition_active = False  # Stop face recognition
    root.destroy()

root.protocol("WM_DELETE_WINDOW", on_closing)
root.mainloop()