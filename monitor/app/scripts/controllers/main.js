'use strict';

/**
 * @ngdoc function
 * @name monitorApp.controller:MainCtrl
 * @description
 * # MainCtrl
 * Controller of the monitorApp
 */
angular.module('monitorApp')
  .controller('MainCtrl', function () {
    this.awesomeThings = [
      'HTML5 Boilerplate',
      'AngularJS',
      'Karma'
    ];
  })

    .run(function() {
        //var connection = new WebSocket('ws://localhost:8001', ['soap', 'xmpp']);
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
            console.log('Server: ' + e.data);
        };
    })
;


