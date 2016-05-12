'use strict';

angular.module('monitorApp')
    .controller('MainCtrl', function ($scope) {

        $scope.deskStates = {
            'MB68-2-001': { state: '0', user: 'U112586' },
            'MB68-2-002': { state: '0', user: 'U112093' }
        };
        $scope.searchString = '';

        $scope.$on('deskStatesChanged', function(event, deskStates) {
            $scope.deskStates = deskStates;
            $scope.$apply();
        });

        $scope.isBusy = function(deskId) {
            return $scope.deskStates[deskId] && $scope.deskStates[deskId].state === '1' && !isSearchActive();
        };

        $scope.isFree = function(deskId) {
            return $scope.deskStates[deskId] && $scope.deskStates[deskId].state === '0' && !isSearchActive();
        };

        $scope.isSearchMatch = function(deskId) {
            if (isSearchActive()) {
                return $scope.deskStates[deskId].user.toLowerCase() === $scope.searchString.toLowerCase()&& $scope.deskStates[deskId].state === '1';
            }
            return false;
        }

        function isSearchActive() {
            return $scope.searchString.length > 0;
        }


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

                // MOCK
/*
                deskStates = {
                    'MB68-2-001': { state: '0', user: 'U112093' },
                    'MB68-2-002': { state: '1', user: 'U112586' }
                };*/

                $rootScope.$broadcast('deskStatesChanged', deskStates);

            } catch (err) {
                console.log(err);
            }
        };
    })
;


