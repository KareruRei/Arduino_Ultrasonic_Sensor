<?php
header('Access-Control-Allow-Origin: *');
header('Access-Control-Allow-Methods: GET, OPTIONS');
header('Access-Control-Allow-Headers: Content-Type');
header('Content-Type: application/json');

http_response_code(200);

if ($_SERVER['REQUEST_METHOD'] === "OPTIONS") exit;
if ($_SERVER['REQUEST_METHOD'] !== 'GET') {
    http_response_code(405);
    exit;
}

require __DIR__ . '/../../config/bootstrap.php';

try {
    $stmt = ($_GET['type'] ?? 'sensor') === 'event' ?
        $pdo->query('SELECT * FROM EventLog LIMIT 14') :
        $pdo->query('SELECT * FROM SensorLog LIMIT 14');

    $rows = $stmt->fetchAll(PDO::FETCH_NUM);
    echo json_encode($rows);
}
catch (PDOException $e) {
    error_log($e->getMessage());
    http_response_code(500);
    echo 'Something went wrong. Please try again later.';
}