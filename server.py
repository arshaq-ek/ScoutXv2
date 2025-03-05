from flask import Flask, render_template
import requests
import keyboard  # Library to detect keyboard input

app = Flask(__name__)

# ESP32-CAM IP address
ESP32_IP = "192.168.1.8"  # Replace with your ESP32-CAM's IP

# Function to send commands to ESP32-CAM
def send_command(command):
    try:
        response = requests.post(f"http://{ESP32_IP}/command", data={"command": command})
        print(f"Command '{command}' sent to ESP32-CAM. Response: {response.text}")
    except requests.exceptions.RequestException as e:
        print(f"Failed to send command '{command}' to ESP32-CAM. Error: {e}")

# Set up arrow key event listeners
def setup_keyboard_listeners():
    keyboard.on_press_key("up", lambda _: send_command("FORWARD"))
    keyboard.on_press_key("down", lambda _: send_command("BACKWARD"))
    keyboard.on_press_key("left", lambda _: send_command("LEFT"))
    keyboard.on_press_key("right", lambda _: send_command("RIGHT"))

@app.route("/")
def index():
    return render_template("index.html")

if __name__ == "__main__":
    # Start keyboard listeners
    setup_keyboard_listeners()
    print("Listening for arrow key presses...")
    
    # Run the Flask server
    app.run(host="0.0.0.0", port=5000)