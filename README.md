# Arduino Intruder Detection System

## Overview
This project implements a **real-time intruder detection system** using an **ultrasonic sensor** mounted on a rotating servo motor and controlled by an **Arduino UNO R4 WiFi**. The system scans a 180° field of view and detects objects within a **50 cm range**.

Detected data is transmitted to a **PHP backend** and displayed on a **web interface** in real time. A **WebSocket server** is used to ensure low-latency communication between the Arduino board and connected clients, while a traditional HTTP API handles database logging and data retrieval.

---

## Features
- 180° environmental scanning using a servo-mounted ultrasonic sensor
- Real-time data streaming via WebSockets
- Detection logging with MySQL
- Web-based interface for live and historical monitoring
- Automatic backend database initialization

---

## Tech Stack

### Hardware
- Arduino UNO R4 WiFi
- Ultrasonic Sensor
- Servo Motor
- Active Buzzer and LED

### Software
- **Arduino / C++**
- **PHP (Apache + Ratchet WebSocket)**
- **MySQL**
- **HTML, CSS, JavaScript**

---

## Arduino Circuit Schematic
![Schematic Diagram](/readme-images/schematic-diagram.jpg)

---

## Setup Guide  
*(Arduino circuit wiring not included)*

### Prerequisites
Make sure the following are installed:
1. Git
2. PHP
3. Composer
4. XAMPP

---

### 1st: Get the following if you haven't already
1. Git
2. PHP
3. composer
4. XAMPP

### 2nd: Clone the GitHub Repository
```
git clone https://github.com/KareruRei/Arduino_Ultrasonic_Sensor.git
```

### 3rd: Set up the WiFi credentials to be used by Arduino
1. Create this file: `arduino/arduino_secrets.h`.
2. Paste the following content inside the created file. The specified WiFi network must be the same one that the Arduino board and Web servers are connected to. For those using Arduino R4 WiFi, take note that they are only designed to operate on 2.4 GHz bands.
    ```
    #define SECRET_SSID "<Your WiFi network's name>"
    #define SECRET_PASS "<Your WiFi network's password>"
    #define SECRET_WS_SERVER "<IP Address of the machine running the Web servers>"
    #define SECRET_WS_PORT 8080
    ```

### 4th: Install the PHP backend's dependencies
```
composer install
```

### 5th: Set up the PHP backend's .env file
1. Create this file: `backend/.env`
2. Paste the following content inside the created file:
    ```
    DB_HOST=localhost
    DB_PORT=3306
    DB_NAME=IntruderDetection
    DB_USER=intruderdetection_admin
    DB_PASS=1ntrud3rd3t3ct10n_4dm1n_p4$$w0rd
    DB_ROOT_PASS=
    ```
    Edit the last line to include your database root user's password (leave as is if the password is blank). The backend must authenticate as root to automatically set up the database from scratch. After that, the backend will always authenticate as a different user.

### 6th: Expose the backend API endpoints to xampp/htdocs
- Windows (Run Command Prompt as Administrator)
    ```
    cd "full/path/to/xampp/htdocs"
    mklink /J arduino-sensor-backend "full/path/to/Arduino_Ultrasonic_Sensor/backend/api"
    ```
    
- Linux terminal
    ```
    cd "full/path/to/xampp/htdocs"
    ln -s "full/path/to/Arduino_Ultrasonic_Sensor/backend/api" arduino-sensor-backend
    ```

### 7th: Expose the webpage to xampp/htdocs
- Windows (Run Command Prompt as Administrator)
    ```
    cd "full/path/to/xampp/htdocs"
    mklink /J radar-system "full/path/to/Arduino_Ultrasonic_Sensor/frontend/dashboard.html"
    ```
    
- Linux terminal
    ```
    cd "full/path/to/xampp/htdocs"
    ln -s "full/path/to/Arduino_Ultrasonic_Sensor/frontend/dashboard.html" radar-system
    ```

### 7th: Run the project
1. Open `XAMPP Control Panel`.
2. Start the `Apache` and `MySQL` modules. Wait for the modules to start running.
3. Go to the `backend/websocket` working directory and paste this in a terminal:
    ```
    php start-websocket.php
    ```
4. Run the Arduino board. Wait for it to connect to the WiFi and WebSocket.
5. Open your browser and visit this webpage link:
    ```
    http://localhost/radar-system
    ```