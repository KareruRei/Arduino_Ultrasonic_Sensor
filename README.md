# Arduino Intruder Detection System

## Description
This project is meant to be an intruder detection system using an Ultrasonic sensor connected to an Arduino UNO R4 WiFi board which sends data to a PHP backend. The backend then sends data from the Arduino board to the HTML/JS frontend. Real-time data updates happen using WebSockets.

### Tech Stack:
1. Arduino UNO R4 board (Sensor data)
2. HTML/CSS/JS (Frontend)
3. PHP (WebSocket-based server for backend)
4. Apache (HTTP-based Web server for backend)
5. MySQL (Database for logs)

### Arduino

### Backend
The backend has an HTTP-based Web Server which receives HTTP requests primarily for inserting/getting logs to/from a database. There is also a WebSocket-based server using Ratchet which accepts client connections between the Arduino board and the frontend. It is essential for the low-latency flow of data between the Arduino board and the frontend.

### Frontend
![JavaScript](https://img.shields.io/badge/-JavaScript-black?style=flat-square&logo=javascript)
![C++](https://img.shields.io/badge/-C++-00599C?style=flat-square&logo=c)
![HTML5](https://img.shields.io/badge/-HTML5-E34F26?style=flat-square&logo=html5&logoColor=white)
![MySQL](https://img.shields.io/badge/-MySQL-black?style=flat-square&logo=mysql)

## How to Set Up