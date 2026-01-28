<?php
header('Access-Control-Allow-Origin: *');
header('Access-Control-Allow-Methods: POST, OPTIONS');
header('Access-Control-Allow-Headers: Content-Type');
header('Content-Type: application/json');

http_response_code(200);

if ($_SERVER['REQUEST_METHOD'] === "OPTIONS") exit;
if ($_SERVER['REQUEST_METHOD'] !== 'POST') {
    http_response_code(405);
    echo 'This endpoint accepts Post Requests only';
    exit;
}

require __DIR__ . '/../config/bootstrap.php';

$data = json_decode(file_get_contents('php://input'), true);

if ($data === null && json_last_error() !== JSON_ERROR_NONE) {
    http_response_code(400);
    echo 'Invalid JSON';
    exit;
}

try {
    $pdo->beginTransaction();

    $sensor_stmt = $pdo->prepare('INSERT INTO SensorLog (sensor, sensor_data) VALUES ("Ultrasonic Sensor", :sensor_data)');
    $sensor_stmt->execute(['sensor_data' => $data['distance']]);

    if (isset($data['description']) && isset($data['locked'])) {
        $sensor_id = $pdo->lastInsertId();

        $event_stmt = $pdo->prepare('INSERT INTO EventLog (sensorlog_id, description, intruder_detected) VALUES (:id, :desc, :intruder)');
        $event_stmt->execute([
            'id' => $sensor_id,
            'desc' => $data['description'],
            'intruder' => $data['locked']
        ]);
    }

    $pdo->commit();
}
catch (PDOException $e) {
    $pdo->rollBack();
    error_log($e->getMessage());
    http_response_code(500);
    echo 'Something went wrong. Please try again later.';
}