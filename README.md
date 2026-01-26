# Arduino Intruder Detection System

## Description
This project is meant to be an intruder detection system using an Ultrasonic sensor connected to an Arduino UNO R4 WiFi board which sends data to a PHP backend. The backend then sends data from the Arduino board to the HTML/JS frontend. Real-time data updates happen using WebSockets.

### Tech Stack:
- ![Arduino](https://img.shields.io/badge/Arduino-00878F?logo=arduino&logoColor=fff&style=plastic) ![C++](https://img.shields.io/badge/-C++-00599C?style=flat-square&logo=c)

- ![HTML5](https://img.shields.io/badge/-HTML5-E34F26?style=flat-square&logo=html5&logoColor=white) ![JavaScript](https://img.shields.io/badge/-JavaScript-black?style=flat-square&logo=javascript)

- ![PHP](https://img.shields.io/badge/PHP-777BB4?style=flat&logo=php&logoColor=white) ![Apache](https://img.shields.io/badge/Apache-D22128?style=for-the-badge&logo=Apache&logoColor=white)

- ![MySQL](https://img.shields.io/badge/-MySQL-black?style=flat-square&logo=mysql)

### Arduino

### Backend
The backend has an HTTP-based Web Server which receives HTTP requests primarily for inserting/getting logs to/from a database. There is also a WebSocket-based server using Ratchet which accepts client connections between the Arduino board and the frontend. It is essential for the low-latency flow of data between the Arduino board and the frontend.

### Frontend

## How to Set Up (Excluding Arduino Circuit Wiring)

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

### 7th: Run the project
1. Open `XAMPP Control Panel`.
2. Start the `Apache` and `MySQL` modules. Wait for the modules to start running.
3. Run the Arduino board. Wait for it to connect to the WiFi and WebSocket.
4. Open your browser and visit the webpage link.