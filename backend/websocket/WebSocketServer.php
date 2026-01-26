<?php

use Ratchet\MessageComponentInterface;
use Ratchet\ConnectionInterface;

class WebSocketServer implements MessageComponentInterface {
    protected ?ConnectionInterface $arduino = null;
    protected array $clients = [];

    public function onOpen(ConnectionInterface $conn) {
        $this->clients[$conn->resourceId] = $conn;
        echo "{date('Y-m-d H:i:s')} - New client has connected: {$conn->resourceId}\n";
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
        if ($conn === $this->arduino) {
            $this->arduino = null;
            echo "{date('Y-m-d H:i:s')} - Arduino connection: {$conn->resourceId} disconnected.\n";
            return;
        }

        unset($this->clients[$conn->resourceId]);
        echo "{date('Y-m-d H:i:s')} - Client: {$conn->resourceId} disconnected.\n";
    }

    public function onError(ConnectionInterface $conn, \Exception $e) {
        $conn->close();
    }
}
