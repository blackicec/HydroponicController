"use strict"

var hydroponic_application = angular.module('hydroponicApp', []);

hydroponic_application.controller('HydroponicController', function($scope) {
    $scope.thumbnail_source = 'strawberry_plant_favicon.png';
    $scope.description_list = ['No description is currently available.'];

    // Set our description value with the response from the server
    $.get("http://192.168.0.14/hydroponic_garden/services/hydroponic_api/data_api.php", {request: "description"},
        function(data, status, xhr) {
            if(xhr.readyState == 4 && xhr.status == 200) {
                $scope.$apply(function() {
                    $scope.description_list = JSON.parse(data);
                });
            }
        }
    );
});