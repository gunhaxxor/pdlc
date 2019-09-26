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

const matrixSizeX = 5
const matrixSizeY = 5

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
    //where in the matrix it is located
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

var letters = {
    'A': [
        [0, 1, 0, 0],
        [1, 0, 1, 0],
        [1, 0, 1, 0],
        [1, 1, 1, 0],
        [1, 0, 1, 0]
    ],
    'B': [
        [1, 1, 0, 0],
        [1, 0, 1, 0],
        [1, 1, 1, 0],
        [1, 0, 1, 0],
        [1, 1, 0, 0]
    ],
    'C': [
        [1, 1, 1],
        [1],
        [1],
        [1],
        [1, 1, 1]
    ],
    'D': [
        [1, 1],
        [1, , 1],
        [1, , 1],
        [1, , 1],
        [1, 1]
    ],
    'E': [
        [1, 1, 1],
        [1],
        [1, 1, 1],
        [1],
        [1, 1, 1]
    ],
    'F': [
        [1, 1, 1],
        [1],
        [1, 1],
        [1],
        [1]
    ],
    'G': [
        [, 1, 1],
        [1],
        [1, , 1, 1],
        [1, , , 1],
        [, 1, 1]
    ],
    'H': [
        [1, , 1],
        [1, , 1],
        [1, 1, 1],
        [1, , 1],
        [1, , 1]
    ],
    'I': [
        [1, 1, 1],
        [, 1],
        [, 1],
        [, 1],
        [1, 1, 1]
    ],
    'J': [
        [1, 1, 1],
        [, , 1],
        [, , 1],
        [1, , 1],
        [1, 1, 1]
    ],
    'K': [
        [1, , , 1],
        [1, , 1],
        [1, 1],
        [1, , 1],
        [1, , , 1]
    ],
    'L': [
        [1],
        [1],
        [1],
        [1],
        [1, 1, 1]
    ],
    'M': [
        [1, 1, 1, 1, 1],
        [1, , 1, , 1],
        [1, , 1, , 1],
        [1, , , , 1],
        [1, , , , 1]
    ],
    'N': [
        [1, , , 1],
        [1, 1, , 1],
        [1, , 1, 1],
        [1, , , 1],
        [1, , , 1]
    ],
    'O': [
        [1, 1, 1],
        [1, , 1],
        [1, , 1],
        [1, , 1],
        [1, 1, 1]
    ],
    'P': [
        [1, 1, 1],
        [1, , 1],
        [1, 1, 1],
        [1],
        [1]
    ],
    'Q': [
        [0, 1, 1],
        [1, , , 1],
        [1, , , 1],
        [1, , 1, 1],
        [1, 1, 1, 1]
    ],
    'R': [
        [1, 1],
        [1, , 1],
        [1, , 1],
        [1, 1],
        [1, , 1]
    ],
    'S': [
        [1, 1, 1],
        [1],
        [1, 1, 1],
        [, , 1],
        [1, 1, 1]
    ],
    'T': [
        [1, 1, 1],
        [, 1],
        [, 1],
        [, 1],
        [, 1]
    ],
    'U': [
        [1, , 1],
        [1, , 1],
        [1, , 1],
        [1, , 1],
        [1, 1, 1]
    ],
    'V': [
        [1, , , , 1],
        [1, , , , 1],
        [, 1, , 1],
        [, 1, , 1],
        [, , 1]
    ],
    'W': [
        [1, , , , 1],
        [1, , , , 1],
        [1, , , , 1],
        [1, , 1, , 1],
        [1, 1, 1, 1, 1]
    ],
    'X': [
        [1, , , , 1],
        [, 1, , 1],
        [, , 1],
        [, 1, , 1],
        [1, , , , 1]
    ],
    'Y': [
        [1, , 1],
        [1, , 1],
        [, 1],
        [, 1],
        [, 1]
    ],
    'Z': [
        [1, 1, 1, 1, 1],
        [, , , 1],
        [, , 1],
        [, 1],
        [1, 1, 1, 1, 1]
    ],
    '0': [
        [1, 1, 1],
        [1, , 1],
        [1, , 1],
        [1, , 1],
        [1, 1, 1]
    ],
    '1': [
        [, 1],
        [, 1],
        [, 1],
        [, 1],
        [, 1]
    ],
    '2': [
        [1,1,1],
        [0,0,1],
        [1,1,1],
        [1,0,0],
        [1,1,1]
    ],
    '3':[
        [1,1,1],
        [0,0,1],
        [1,1,1],
        [0,0,1],
        [1,1,1]
    ],
    '4':[
        [1,0,1],
        [1,0,1],
        [1,1,1],
        [0,0,1],
        [0,0,1]
    ],
    '5':[
        [1,1,1],
        [1,0,0],
        [1,1,1],
        [0,0,1],
        [1,1,1]
    ],
    '6':[
        [1,1,1],
        [1,0,0],
        [1,1,1],
        [1,0,1],
        [1,1,1]
    ],
    '7':[
        [1,1,1],
        [0,0,1],
        [0,0,1],
        [0,0,1],
        [0,0,1]
    ],
    '8':[
        [1,1,1],
        [1,0,1],
        [1,1,1],
        [1,0,1],
        [1,1,1]
    ],
    '9':[
        [1,1,1],
        [1,0,1],
        [1,1,1],
        [0,0,1],
        [1,1,1]
    ],
    ' ': [
        [, , ,],
        [, , ,],
        [, , ,],
        [, , ,],
        [, , ,]
    ]
}

document.getElementById("textSubmit").onclick = scrollingText

//contains pixels to scroll
var rows

var scrollState = 0-matrixSizeX

function scrollingText(){
    var text = document.getElementById("textInput").value.toUpperCase()
    text = text.split('')
    for(var i = 0; i < matrixSizeX; i++){
        if(i % 2 == 0){
            text.unshift(' ')
        }
    }
    rows = new Array(5)
    scrollState = 0
    for(var i = 0; i < rows.length; i++){
        rows[i] = []
        for(var j = 0; j < text.length; j++){
            rows[i] = rows[i].concat(letters[text[j]][i])
        }
    }
    console.log(rows)
    setTimeout(scrollForward, 1000);
}

function scrollForward(){
    for(var i = 0; i < matrixSizeX; i++){
        if(scrollState+i > 0){
            for(var j = 0; j < buttons[i].length; j++){
                buttons[i][j].changeState(rows[j][scrollState+i])
            }
        }
    }
    scrollState++
    if(scrollState < rows[0].length+matrixSizeX)
    setTimeout(scrollForward, 1000);
}