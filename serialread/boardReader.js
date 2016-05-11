var ws = require("nodejs-websocket")
var serialport = require('serialport');

// Global map which holds all desk states (with desk id as key)
var deskStates = {};

//       _______. _______ .______       __       ___       __         .______     ______   .______     .___________.
//      /       ||   ____||   _  \     |  |     /   \     |  |        |   _  \   /  __  \  |   _  \    |           |
//     |   (----`|  |__   |  |_)  |    |  |    /  ^  \    |  |        |  |_)  | |  |  |  | |  |_)  |   `---|  |----`
//      \   \    |   __|  |      /     |  |   /  /_\  \   |  |        |   ___/  |  |  |  | |      /        |  |
//  .----)   |   |  |____ |  |\  \----.|  |  /  _____  \  |  `----.   |  |      |  `--'  | |  |\  \----.   |  |
//  |_______/    |_______|| _| `._____||__| /__/     \__\ |_______|   | _|       \______/  | _| `._____|   |__|
//
var portName1411 = '/dev/cu.usbmodem1411';
var sp1411 = new serialport.SerialPort(portName1411, {
    baudRate: 9600,
    dataBits: 8,
    parity: 'none',
    stopBits: 1,
    flowControl: false,
    parser: serialport.parsers.readline("\r\n")
});

var portName1421 = '/dev/cu.usbmodem1421';
var sp1421 = new serialport.SerialPort(portName1421, {
    baudRate: 9600,
    dataBits: 8,
    parity: 'none',
    stopBits: 1,
    flowControl: false,
    parser: serialport.parsers.readline("\r\n")
});

var readBoardInfo = function(input) {

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
}


sp1411.on('data', readBoardInfo);
sp1411.on('open', function() {
    console.log('Listening on Serial Port 1411...');
});

sp1421.on('data', readBoardInfo);
sp1421.on('open', function() {
    console.log('Listening on Serial Port 1421...');
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
        console.log("Received "+str)
        conn.sendText(str.toUpperCase()+"!!!")
    })
    conn.on("close", function (code, reason) {
        console.log("Connection closed")
    })
}).listen(8001)


server.on("listening", function () {
    console.log("Socket Ready (localhost:8001) ...")
})