#include "src/OV2640.h"
#include <WiFi.h>
#include <WebServer.h>

// Select camera model
#define CAMERA_MODEL_AI_THINKER
#include "camera_pins.h"

// Wi-Fi credentials
#define SSID1 "EDUVAMMAL-2.4G"
#define PWD1 "EDUVAMMAL775"

// Motor control pins
#define MOTOR_A_IN1 13  // GPIO 13
#define MOTOR_A_IN2 12  // GPIO 12
#define MOTOR_B_IN1 14  // GPIO 14
#define MOTOR_B_IN2 15  // GPIO 15

OV2640 cam;
WebServer server(80);

// Function to handle MJPEG streaming
void handle_jpg_stream() {
  WiFiClient client = server.client();

  // Send the HTTP header
  client.write("HTTP/1.1 200 OK\r\n");
  client.write("Content-Type: multipart/x-mixed-replace; boundary=123456789000000000000987654321\r\n\r\n");

  while (true) {
    if (!client.connected()) break;
    cam.run();
    int s = cam.getSize();
    client.write("--123456789000000000000987654321\r\n");
    client.write("Content-Type: image/jpeg\r\n");
    client.write("Content-Length: ");
    client.write(String(s).c_str());
    client.write("\r\n\r\n");
    client.write((char *)cam.getfb(), s);
  }
}

// Function to handle single JPEG image
void handle_jpg() {
  WiFiClient client = server.client();

  cam.run();
  if (!client.connected()) return;

  client.write("HTTP/1.1 200 OK\r\n");
  client.write("Content-disposition: inline; filename=capture.jpg\r\n");
  client.write("Content-type: image/jpeg\r\n\r\n");
  client.write((char *)cam.getfb(), cam.getSize());
}

// Function to handle motor control commands
void handle_command() {
  if (server.hasArg("command")) {
    String command = server.arg("command");
    Serial.printf("Received command: %s\n", command.c_str());

    // Control motors based on the command
    if (command == "forward") {
      digitalWrite(MOTOR_A_IN1, HIGH);
      digitalWrite(MOTOR_A_IN2, LOW);
      digitalWrite(MOTOR_B_IN1, HIGH);
      digitalWrite(MOTOR_B_IN2, LOW);
    } else if (command == "backward") {
      digitalWrite(MOTOR_A_IN1, LOW);
      digitalWrite(MOTOR_A_IN2, HIGH);
      digitalWrite(MOTOR_B_IN1, LOW);
      digitalWrite(MOTOR_B_IN2, HIGH);
    } else if (command == "left") {
      digitalWrite(MOTOR_A_IN1, LOW);
      digitalWrite(MOTOR_A_IN2, HIGH);
      digitalWrite(MOTOR_B_IN1, HIGH);
      digitalWrite(MOTOR_B_IN2, LOW);
    } else if (command == "right") {
      digitalWrite(MOTOR_A_IN1, HIGH);
      digitalWrite(MOTOR_A_IN2, LOW);
      digitalWrite(MOTOR_B_IN1, LOW);
      digitalWrite(MOTOR_B_IN2, HIGH);
    } else if (command == "stop") {
      digitalWrite(MOTOR_A_IN1, LOW);
      digitalWrite(MOTOR_A_IN2, LOW);
      digitalWrite(MOTOR_B_IN1, LOW);
      digitalWrite(MOTOR_B_IN2, LOW);
    }

    server.send(200, "text/plain", "Command executed: " + command);
  } else {
    server.send(400, "text/plain", "Missing command");
  }
}

// Function to handle invalid requests
void handleNotFound() {
  server.send(404, "text/plain", "Not Found");
}

void setup() {
  Serial.begin(115200);

  // Initialize motor control pins
  pinMode(MOTOR_A_IN1, OUTPUT);
  pinMode(MOTOR_A_IN2, OUTPUT);
  pinMode(MOTOR_B_IN1, OUTPUT);
  pinMode(MOTOR_B_IN2, OUTPUT);

  // Camera configuration
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size = FRAMESIZE_QVGA;
  config.jpeg_quality = 12;
  config.fb_count = 2;

  // Initialize the camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x\n", err);
    return;
  } else {
    Serial.println("Camera init succeeded");
  }

  // Connect to Wi-Fi
  WiFi.begin(SSID1, PWD1);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
  Serial.println(WiFi.localIP());

  // Set up server endpoints
  server.on("/mjpeg/1", HTTP_GET, handle_jpg_stream);
  server.on("/jpg", HTTP_GET, handle_jpg);
  server.on("/command", HTTP_POST, handle_command);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();  // Handle incoming client requests
}