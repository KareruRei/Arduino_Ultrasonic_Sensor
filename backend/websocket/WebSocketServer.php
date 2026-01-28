<?php

use Amp\Http\Client\HttpClient;
use Ratchet\WebSocket\MessageComponentInterface;
use Ratchet\ConnectionInterface;
use Ratchet\RFC6455\Messaging\MessageInterface;
use Amp\Http\Client\HttpClientBuilder;
use Amp\Http\Client\Request;

class WebSocketServer implements MessageComponentInterface {
    protected ?ConnectionInterface $arduino = null;
    protected array $clients = [];
    private HttpClient $http_client;
    private int $lastLog = 0;

    public function __construct() {
        $this->http_client = new HttpClientBuilder()::buildDefault();
    }

    public function onOpen(ConnectionInterface $conn) {
        $this->clients[$conn->resourceId] = $conn;
        $now = date("Y-m-d H:i:s");
        echo "$now - New client has connected: {$conn->resourceId}\n";
    }

    public function onMessage(ConnectionInterface $from, MessageInterface $data) {
        if (!$data->isBinary()) {
            $decoded = json_decode($data, true) ?? [];

            if (($decoded['role'] ?? '') === 'arduino') {
                $this->arduino = $from;
                unset($this->clients[$from->resourceId]);
            }
            return;
        }
        else if ($from !== $this->arduino) return;

        foreach ($this->clients as $client) {
            $client->send($data);
        }

        if (time() - $this->lastLog < 60*10) return;
        $this->lastLog = time();

        $payload = $data->getPayload();

        $angle = ord($payload[0]);
        $distance = (ord($payload[1]) << 8) | ord($payload[2]);
        $locked = ord($payload[3]) == 1;

        $log_request = new Request('http://localhost/arduino-sensor-backend/post-logs.php', 'POST');

        $request_payload = [
            'angle' => $angle,
            'distance' => $distance,
            'locked' => $locked
        ];

        if ($locked) $request_payload['description'] = 'Intruder detected';

        $log_request->setBody(json_encode($request_payload));
        $this->http_client->request($log_request);
    }

    public function onClose(ConnectionInterface $conn) {
        $now = date("Y-m-d H:i:s");

        if ($conn === $this->arduino) {
            $this->arduino = null;
            echo "$now - Arduino connection: {$conn->resourceId} disconnected.\n";
            return;
        }

        unset($this->clients[$conn->resourceId]);
        echo "$now - Client: {$conn->resourceId} disconnected.\n";
    }

    public function onError(ConnectionInterface $conn, \Exception $e) {
        $conn->close();
    }
}
