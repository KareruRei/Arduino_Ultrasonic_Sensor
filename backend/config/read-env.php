<?php

if (isset($_ENV['DB_HOST']) && isset($_ENV['DB_NAME']) && isset($_ENV['DB_USER']) && isset($_ENV['DB_PASS']))
    exit;

$lines = file(__DIR__ . '/../.env', FILE_IGNORE_NEW_LINES | FILE_SKIP_EMPTY_LINES);

foreach ($lines as $line) {
    if (str_starts_with($line, '#')) continue;

    [$key, $value] = explode('=', $line, 2);
    $_ENV[$key] = $value;
    putenv("$key=$value");
}