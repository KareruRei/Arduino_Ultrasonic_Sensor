<?php

use Ratchet\WebSocket\MessageComponentInterface;
use Ratchet\ConnectionInterface;
use Ratchet\RFC6455\Messaging\MessageInterface;
use React\Http\Browser;

class WebSocketServer implements MessageComponentInterface {
    protected ?ConnectionInterface $arduino = null;
    protected array $clients = [];
    private Browser $browser;
    private int $last_log = 0;
    private bool $last_state = false;

    public function __construct(\React\EventLoop\LoopInterface $loop) {
        $this->browser = new Browser($loop);
    }

    public function onOpen(ConnectionInterface $conn) {
        $this->clients[$conn->resourceId] = $conn;
        echo "New client has connected: {$conn->resourceId}\n";
    }

    public function onMessage(ConnectionInterface $from, MessageInterface $data) {
        if (!$data->isBinary()) {
            $decoded = json_decode($data, true) ?? [];

            if (($decoded['role'] ?? '') === 'arduino') {
                $this->arduino = $from;
                unset($this->clients[$from->resourceId]);
                echo "New arduino client: {$from->resourceId}\n";
            }
            return;
        }
        else if ($from !== $this->arduino) return;

        foreach ($this->clients as $client) {
            $client->send($data);
        }

        $payload = $data->getPayload();
        $locked = ord($payload[3]) == 1;

        if ($locked === $this->last_state && time() - $this->last_log < 10) return;
    
        $angle = ord($payload[0]);
        $distance = (ord($payload[1]) << 8) | ord($payload[2]);

        $request_payload = [
            'angle' => $angle,
            'distance' => $distance,
            'locked' => $locked
        ];

        if ($locked) $request_payload['description'] = 'Intruder detected';
        else if ($this->last_state) $request_payload['description'] = 'Intruder has left';

        $this->last_log = time();
        $this->last_state = $locked;

        $this->browser->post(
            'http://127.0.0.1/arduino-sensor-backend/post-logs.php',
            ['Content-Type' => 'application/json'],
            json_encode($request_payload)
        )->then(
            function ($response) { echo "Data was logged!\n"; },
            function ($error) { echo "\nFailed to log: {$error->getMessage()}\n\n"; }
        );
    }

    public function onClose(ConnectionInterface $conn) {
        if ($conn === $this->arduino) {
            $this->arduino = null;
            echo "Arduino client: {$conn->resourceId} disconnected.\n";
            return;
        }

        unset($this->clients[$conn->resourceId]);
        echo "Client: {$conn->resourceId} disconnected.\n";
    }

    public function onError(ConnectionInterface $conn, \Exception $e) {
        echo "\nError with client: {$conn->resourceId}\n";
        echo "Closing connection due to: {$e->getMessage()}\n\n";
        $conn->close();
    }
}
