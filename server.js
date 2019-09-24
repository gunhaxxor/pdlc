const http = require('http');
const WebSocket = require('ws');
const express = require('express');
var path = require('path');

const app = express();
 
const server = http.createServer(app);
const wss = new WebSocket.Server({ server });
 
wss.on('connection', function connection(ws) {
  ws.on('message', function incoming(message) {
    console.log('received: %s', message);
    //broadcast data to every client connected
    wss.clients.forEach(function each(client) {
        if (client.readyState === WebSocket.OPEN) {
          client.send(message);
        }
    });
  });
 
  ws.send('something');
});

app.use(express.static(__dirname + '/public'));
 
server.listen(process.env.PORT || 80);