'use strict';

/**
 * @ngdoc function
 * @name monitorApp.controller:MainCtrl
 * @description
 * # MainCtrl
 * Controller of the monitorApp
 */
angular.module('monitorApp')
    .controller('MainCtrl', function ($scope) {

        $scope.state = 0;

        $scope.$on('deskStatesChanged', function(event, deskStates) {

            $scope.state = deskStates['MB68-2-001'].state;
            console.log($scope.state);
            $scope.$apply();
        });

        $scope.isBusy = function() {
            return $scope.state === '1';
        };
    })

    // OPEN WEBSOCKET
    .run(function ($rootScope) {

        var connection = new WebSocket('ws://localhost:8001');

        // When the connection is open, send some data to the server
        connection.onopen = function () {
            connection.send('Ping'); // Send the message 'Ping' to the server
        };

        // Log errors
        connection.onerror = function (error) {
            console.log('WebSocket Error ' + error);
        };

        // Log messages from the server
        connection.onmessage = function (e) {
            console.log(e.data);
            try {
                var deskStates = JSON.parse(e.data);
                console.log('Receiving desk state from server', deskStates);
                $rootScope.$broadcast('deskStatesChanged', deskStates);
            } catch (err) {
                console.log(err);
            }
        };
    })
;


