<?php

use Ratchet\MessageComponentInterface;
use Ratchet\ConnectionInterface;

class WebSocketServer implements MessageComponentInterface {
    protected ?ConnectionInterface $arduino = null;
    protected array $clients = [];

    public function onOpen(ConnectionInterface $conn) {
        $this->clients[$conn->resourceId] = $conn;
        $now = date("Y-m-d H:i:s");
        echo "$now - New client has connected: {$conn->resourceId}\n";
    }

    public function onMessage(ConnectionInterface $from, $data) {
        $decoded = json_decode($data, true) ?? [];

        if (($decoded['role'] ?? '') === 'arduino') {
            $this->arduino = $from;
            unset($this->clients[$from->resourceId]);
            return;
        }

        if ($from === $this->arduino) {
            foreach ($this->clients as $client) {
                $client->send($data);
            }
        }
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
