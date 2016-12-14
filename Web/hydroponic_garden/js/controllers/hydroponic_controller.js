"use strict"

var hydroponic_application = angular.module('hydroponicApp', []);

hydroponic_application.controller('HydroponicController', function($scope) {
    $scope.Systems = [
        {plant_name:'Cucumber (Pickle Bush)', date_planted:'11/20/2016'},
        {plant_name:'Trinidad Perfume Pepper', date_planted:'11/20/2016'},
        {plant_name:'Aji Mango Habanero Pepper', date_planted:'11/20/2016'},
    ];

    $scope.thumbnail_source = 'strawberry_plant_favicon.png';
    $scope.description = 'No description is currently available.';

    // Set our description value with the response from the server
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
        if(this.readyState == 4 && this.status == 200) {
            var response = this;
            $scope.$apply(function() {
                $scope.description = response.responseText;
            });
        }
    }

    xhttp.open('GET', 'http://192.168.0.14/hydroponic_garden/services/hydroponic_api/data_api.php?request=description');
    xhttp.send();
});