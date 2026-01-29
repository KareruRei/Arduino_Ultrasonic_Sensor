<?php

require __DIR__ . '/../vendor/autoload.php';

use React\EventLoop\Factory;
use React\Socket\Server as ReactServer;
use Ratchet\Http\HttpServer;
use Ratchet\Server\IoServer;
use Ratchet\WebSocket\WsServer;
require __DIR__ . '/WebSocketServer.php';

$port = 8080;
$loop = Factory::create();
$wsServer = new WebSocketServer($loop); // pass the loop to your class
$socket = new ReactServer("0.0.0.0:$port", $loop);

$server = new IoServer(
    new HttpServer(
        new WsServer($wsServer)
    ),
    $socket,
    $loop
);

$loop->run();