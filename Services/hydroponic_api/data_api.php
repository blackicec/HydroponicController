<?php
    include('./application_details.php');

    const DATABASE_NAME = 'hydroponic_garden';

    $server_name = NULL;
    $username = '';
    $password = '';

    // Gather the connection details from our configuration file
    $database_xml_configs = simplexml_load_file("./../database_configuration.xml") 
        or die("Error: Failed to read XML file for database configurations.");

    if($database_xml_configs != false) {
        foreach($database_xml_configs as $configuration) {
            // find our configuration and grab its' values
            if($configuration['database_name'] == DATABASE_NAME) {
                $server_name = $configuration->server;
                $username = $configuration->username;
                $password = $configuration->password;
            }
        }

        // if our server name is still not set, then something is wrong with the configuration value
        if(!isset($server_name) || $server_name == '') {
            die('The server name for configuration:' . DATABASE_NAME . ' has not been set in the configuration.');
        }
    }

    // Now we can connect with our credentials
    $sql_connection = new mysqli($server_name, 'test', $password, DATABASE_NAME);

    $application_details = new ApplicationDetails($sql_connection);

    // Check our request to see what the user is requesting
    if($_SERVER['REQUEST_METHOD'] == 'GET') {
        if(isset($_GET['request']) && $_GET['request'] == 'description') {
            echo $application_details->GetApplicationDescription();
        }
    } else if($_SERVER['REQUEST_METHOD'] == 'POST') {

    }

    mysqli_close($sql_connection);
?>