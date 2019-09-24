const socket = new WebSocket('ws://192.168.1.135:80');

let newClient = true

socket.addEventListener('open', function (event) {
    socket.send('socket open');
});

socket.addEventListener('message', function (event) {
    console.log('Message from server ', event.data);
    if(event.data == 'socket open'){
        if(!newClient){
            for (var i = 0; i < buttons.length; i++) {
                for(var j = 0; j < buttons[i].length; j++){
                    if(buttons[i][j].on == true){
                        socket.send(JSON.stringify({x: buttons[i][j].posX, y: buttons[i][j].posY, state: buttons[i][j].on}))
                    }
                }
            }
        }
        
    }else{
        try {
            var message = JSON.parse(event.data)
            buttons[message.x][message.y].changeState(message.state)
        } catch (error) {
            console.log(error)
        }
    }
});

function sendData(data){
    socket.send(data);
    newClient = false
}

const matrixSizeX = 32
const matrixSizeY = 32

let smallestSide

var buttons = new Array(matrixSizeX)

function setup(){
    smallestSide = ((windowWidth < windowHeight) ? windowWidth : windowHeight)
    createCanvas(smallestSide, smallestSide);

    for(let i = 0; i < matrixSizeX; i++){
        buttons[i] = new Array(matrixSizeY)
        for(let j = 0; j < matrixSizeY; j++){
            buttons[i][j] = new Button(i*(smallestSide/matrixSizeX),j*(smallestSide/matrixSizeY),smallestSide/matrixSizeX,smallestSide/matrixSizeY, i, j)
        }
    }
}

function draw(){
    background(255);
    // Show all the buttons
    for (var i = 0; i < buttons.length; i++) {
        for(var j = 0; j < buttons[i].length; j++){
            buttons[i][j].display();
        } 
    }
}

function mousePressed() {
    // When the mouse is pressed, we must check every single button
    for (var i = 0; i < buttons.length; i++) {
        for(var j = 0; j < buttons[i].length; j++){
            buttons[i][j].click(mouseX, mouseY);
        }
    }
}

function Button(tempX, tempY, tempW, tempH, posX, posY)  {    
    // Button location and size
    this.x  = tempX;   
    this.y  = tempY;   
    this.w  = tempW;   
    this.h  = tempH;
    this.posX = posX;
    this.posY = posY;   
    // Is the button on or off?
    // Button always starts as off
    this.on = false;    
  
    this.click = function(mx, my) {
      // Check to see if a point is inside the rectangle
      if (mx > this.x && mx < this.x + this.w && my > this.y && my < this.y + this.h) {
        this.on = !this.on;
        sendData(JSON.stringify({x: this.posX, y: this.posY, state: this.on}))
      }
    };
  
    // Draw the rectangle
    this.display = function() {
      rectMode(CORNER);
      stroke(0);
      // The color changes based on the state of the button
      if (!this.on) {
        fill(175);
      } else {
        fill(255);
      }
      rect(this.x,this.y,this.w,this.h);
    } 

    this.changeState = function(newState){
        this.on = newState
    }
}