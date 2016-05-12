var ws = require("nodejs-websocket")
var serialport = require('serialport');
var _ = require('lodash')

var ports = ['/dev/cu.usbmodem14121', '/dev/cu.usbmodem14131'];
//var ports = ['/dev/cu.usbmodem1411'];

// Global map which holds all desk states (with desk id as key)
//var deskStates = {};
var deskStates = {
    'MB68-2-001': { state: '0', user: '' },
    'MB68-2-002': { state: '0', user: '' }
};

//       _______. _______ .______       __       ___       __         .______     ______   .______     .___________.
//      /       ||   ____||   _  \     |  |     /   \     |  |        |   _  \   /  __  \  |   _  \    |           |
//     |   (----`|  |__   |  |_)  |    |  |    /  ^  \    |  |        |  |_)  | |  |  |  | |  |_)  |   `---|  |----`
//      \   \    |   __|  |      /     |  |   /  /_\  \   |  |        |   ___/  |  |  |  | |      /        |  |
//  .----)   |   |  |____ |  |\  \----.|  |  /  _____  \  |  `----.   |  |      |  `--'  | |  |\  \----.   |  |
//  |_______/    |_______|| _| `._____||__| /__/     \__\ |_______|   | _|       \______/  | _| `._____|   |__|
//


var readBoardInfo = function (input) {

    // parse ARD_STATE string
    if (input.startsWith("ARD_STATE")) {
        var arr = input.split("//");

        var state = arr[1];
        var deskId = arr[2];

        var userHex = arr[3].trim(); // hex


        var userTokens = userHex.split(" ");
        var user = '';
        for (i = 0; i < 7; i++) {
            user += hex2a(userTokens[i]);
        }

        console.log('Processing Arduino state change...');
        console.log(deskId);
        console.log(state);
        console.log(user);
        console.log(userHex);

        deskStates[deskId] = {
            "state": state,
            "user": user
        }

        // send deskStates
        if (websocketConnection) {
            websocketConnection.sendText(JSON.stringify(deskStates));
        }

        console.log("Desk States Map", deskStates);
    }
};

_.forEach(ports, function (portName) {
    var port = new serialport.SerialPort(portName, {
        baudRate: 9600,
        dataBits: 8,
        parity: 'none',
        stopBits: 1,
        flowControl: false,
        parser: serialport.parsers.readline("\r\n")
    });

    port.on('data', readBoardInfo);
    port.on('open', function () {
        console.log('Listening on Serial Port :' + portName);
    });
});

// helper function: convert hex to ascii
function hex2a(hexx) {
    var hex = hexx.toString();//force conversion
    var str = '';
    for (var i = 0; i < hex.length; i += 2)
        str += String.fromCharCode(parseInt(hex.substr(i, 2), 16));
    return str;
}

//  ____    __    ____  _______ .______        _______.  ______     ______  __  ___  _______ .___________.
//  \   \  /  \  /   / |   ____||   _  \      /       | /  __  \   /      ||  |/  / |   ____||           |
//   \   \/    \/   /  |  |__   |  |_)  |    |   (----`|  |  |  | |  ,----'|  '  /  |  |__   `---|  |----`
//    \            /   |   __|  |   _  <      \   \    |  |  |  | |  |     |    <   |   __|      |  |
//     \    /\    /    |  |____ |  |_)  | .----)   |   |  `--'  | |  `----.|  .  \  |  |____     |  |
//      \__/  \__/     |_______||______/  |_______/     \______/   \______||__|\__\ |_______|    |__|
//
var websocketConnection = undefined;

var server = ws.createServer(function (conn) {
    console.log("New connection")
    websocketConnection = conn;
    conn.on("text", function (str) {
        console.log("Received " + str)
        conn.sendText(str.toUpperCase() + "!!!")
    })
    conn.on("close", function (code, reason) {
        console.log("Connection closed")
    })
}).listen(8001)


server.on("listening", function () {
    console.log("Socket Ready (localhost:8001) ...");
})