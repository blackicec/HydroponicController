<?php

    /*
    * This class implements some boilerplate database interaction methods that will
    * be used throughout the application.
    */
    class BaseDatabaseInteraction {
        private $table_name = '';

        function __construct($table_name) {
            $this->table_name = $table_name;
        }

        public function Select($columns_list, $where_clause) {

        }

        // Delete one or more rows from the table based on a given where clause.
        // This method returns true if the removal was successful and false if not.
        public function Delete($where_clause) {
            return true; // or false based on 
        }

        private function query_preparation($query) {
            return mysql_real_escape_string($query);
        }
    }
?>