// "use strict" prohibits the usage of undeclared variables
"use strict"

var hydroponic_application = angular.module('hydroponicApp');

hydroponic_application.controller('HeaderController', function($scope) {

    // Sample data for search typeahead
    // TODO: perform query that reads list of names from database SP
    var searchbar_datasource = [
        "Cherry Tomatoes", "Trinidad Perfume Pepper", "Bush Cucumbers", "Morning Glories",
        "Blue Dwarf Kale"
    ]

    // Constructing the suggestion engine
    var searchbar_datasource = new Bloodhound({
        datumTokenizer: Bloodhound.tokenizers.whitespace,
        queryTokenizer: Bloodhound.tokenizers.whitespace,
        local: searchbar_datasource
    });

    // Set up the navigation bar's typeahead configurations. The first object is the configurations
    // and the second object sets the datasource
    $("#navbar_search").typeahead({
        hint: true,
        highlight: true, /* Enable substring highlighting */
        minLength: 1 /* Specify minimum characters required for showing result */
    },
    {
        name: "searchbar_datasource",
        source: searchbar_datasource
    });

    $scope.searchbar_datasource = searchbar_datasource;
});

$(document).ready(function() {
    // Set up the click event handlers for the remaining menu list items
    $('#menu_items > li').on('click', function() {
        // Clear out any active class setting for the previously selected list item
        $('#menu_items > li').removeClass('active');

        $(this).addClass('active');
    });
});