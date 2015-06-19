#!/usr/bin/node
// Main greenhouse controller executable
//
// Requires 'modbus' NPM module. Installation: 'npm install modbus'

var mb = require('modbus').create();
//var util = require('util');
var path = require('path');

//console.log(util.inspect(mb, {showHidden: false, depth: null}));

var Settings = {
	STATUS_REGISTER_ADDR: 0,
	LAMP_SUPPLY_REGISTER_ADDR: 1,
	LIGHT_SENSOR_REGISTER_ADDR: 2,
	MAX_REGISTER_ADDR: 2,
	HTTP_SERVER_PORT: 9999,

	ENDPOINT_ID: 1,
	SERIAL_DEVICE: '/dev/ttyUSB0',
	SERIAL_BAUD_RATE: 38400,
	SERIAL_PARITY: 'E',
	SERIAL_DATA_BITS: 8,
	SERIAL_STOP_BIT: 1
};

var lightingSubsystem = require(path.join(__dirname, 'lib', 'LightingSubsystem'));
var subsystems = {};

var ctx = mb.createMaster({
	con: mb.createConRtu(Settings.ENDPOINT_ID, Settings.SERIAL_DEVICE, Settings.SERIAL_BAUD_RATE,
		     Settings.SERIAL_PARITY, Settings.SERIAL_DATA_BITS, Settings.SERIAL_STOP_BIT),
	onConnect: function () {
		console.log('onConnect');
		ctx.setReg(Settings.LAMP_SUPPLY_REGISTER_ADDR, 0);
		subsystems.lighting = new lightingSubsystem.LightingSubsystem(ctx,
			Settings.STATUS_REGISTER_ADDR, Settings.LIGHT_SENSOR_REGISTER_ADDR,
			Settings.LAMP_SUPPLY_REGISTER_ADDR);
	},
	onDestroy: function () {
		console.log('onDestroy');
	}
});

var http = require("http");

http.createServer(function(request, response) {
	// TODO: Web-GUI handling code is here
	response.writeHead(200, {"Content-Type": "text/plain"});
	response.write("Hello World");
	response.end();
}).listen(Settings.HTTP_SERVER_PORT);
