const http = require('http');
const WebSocket = require('ws');
 
const server = http.createServer();

const wss = new WebSocket.Server({ server });
 
wss.on('connection', function connection(ws) {
  ws.on('message', function incoming(message) {
    console.log('received: %s', message);
    ws.send('HOO');
  });
 
  ws.send('something');
});

 
server.listen(5000);
console.log("test")