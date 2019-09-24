const socket = new WebSocket('ws://192.168.1.135:80');

socket.addEventListener('open', function (event) {
    socket.send('socket open');
});

socket.addEventListener('message', function (event) {
    console.log('Message from server ', event.data);
});

function sendData(data){
    socket.send(data);
}