<?php

try {
    
}
catch (PDOException $e) {
    error_log($e->getMessage());
}