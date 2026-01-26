<?php

require __DIR__ . '/../vendor/autoload.php';

use Ratchet\Http\HttpServer;
use Ratchet\Server\IoServer;
use Ratchet\WebSocket\WsServer;
require __DIR__ . '/WebSocketServer.php';

$port = 8080;
$server = IoServer::factory(
    new HttpServer(
        new WsServer(new WebSocketServer())
    ),
    $port
);

$server->run();