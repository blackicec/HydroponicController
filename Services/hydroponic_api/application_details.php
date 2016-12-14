<?php
    include_once('./base_database_interaction.php');

    class ApplicationDetails extends BaseDatabaseInteraction {

        private $sql_connection = NULL;
        private $table_name = 'application_details';

        function __construct($sqlconnection) {
            parent::__construct($this->table_name);
            $this->sql_connection = $sqlconnection;
        }

        // Query the table for the description of this project and return that value
        function GetApplicationDescription() {
            if(!isset($this->sql_connection)) {
                return 'No description available.';
            }

            // TODO: Once implemented, the code below can simply call the parent class and pass over the relevant data
            $resource_id = "about_this_application";
            $query = "SELECT DETAILS FROM $this->table_name WHERE RESOURCE_ID = '$resource_id'";

            $result = $this->sql_connection->query($query);

            if($result->num_rows > 0) {
                $data_row = $result->fetch_assoc();

                return $data_row['DETAILS'];
            } else {
                return 'No description available.';
            }
        }
    }
?>