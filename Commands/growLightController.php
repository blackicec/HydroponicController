<?php 

    /*****************************************/
    echo "<h2>TCP/IP Connection</h2>\n";

    /* Get the port for the WWW service. */
    $service_port = 1989;

    /* Get the IP address for the target host. */
    $address = '127.0.0.1';

    /* Create a TCP/IP socket. */
    $socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
    if ($socket === false) {
            echo "socket_create() failed: reason: " . socket_strerror(socket_last_error()) . "<br>";
    } else {
            echo "OK.<br>";
    }

    echo "Attempting to connect to '$address' on port '$service_port'...";
    $result = socket_connect($socket, $address, $service_port);
    if ($result === false) {
            echo "socket_connect() failed.\nReason: ($result) " .
                socket_strerror(socket_last_error($socket)) . "\n";
    } else {
            echo "OK.<br>";
    }

    $data = $_GET['data'];

	echo 'Sending command to server now . . . ';
    socket_write($socket, $data, strlen($data));
    echo "OK.<br>";
	
?>
