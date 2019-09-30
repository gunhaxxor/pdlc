const socket = new WebSocket('ws://192.168.1.135:80');

let isNewClient = true

socket.addEventListener('open', function (event) {
    socket.send('socket open');
});

socket.addEventListener('message', function (event) {
    console.log('Message from server ', event.data);
    //check if someone new connected and send state of all buttons
    if(event.data == 'socket open'){
        if(!isNewClient){
            sendValues()
        }
        
    }else{
        try {
            var message = JSON.parse(event.data)
            var binary = message.values.toString(2)
            binary = ("00000000000000000000000000000000".substr(binary.length) + binary).split("").reverse().join("")
            console.log(binary)
            var counter = 0;
            var insideCounter = 0;
            for(var i = 0; i < matrixSizeX; i++){
                for(var j = 0; j < matrixSizeY; j++){
                    if(counter >= message.id*32&&counter < (message.id+1)*32){
                        buttons[i][j].on = parseInt(binary[insideCounter])
                        insideCounter++
                    }
                    counter++
                }
            }
        } catch (error) {
            console.log(error)
        }
    }
});

function sendValues(){
    var values = []
    for(var i = matrixSizeX-1; i >= 0; i--){
        for(var j = matrixSizeY-1; j >= 0; j--){
            values.push(buttons[i][j].on ? 1 : 0)
        }
    }
    values = values.reverse().join('').match(/.{1,32}/g)
    for(var i = 0; i < values.length; i++){
        socket.send(JSON.stringify({id: i, values: parseInt(values[i].split("").reverse().join(""), 2)}))
    }
    console.log(values)
}


function sendData(data){
    socket.send(data);
    isNewClient = false
}
//y≥5 for scrolling text
const matrixSizeX = 11
const matrixSizeY = 11

let smallestSide

var buttons = new Array(matrixSizeX)

function setup(){
    smallestSide = ((windowWidth < windowHeight) ? windowWidth : windowHeight)
    createCanvas(smallestSide, smallestSide);

    //Create all buttons
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
    this.on = false;    
  
    this.click = function(mx, my) {
      // Check to see if a point is inside the rectangle
      if (mx > this.x && mx < this.x + this.w && my > this.y && my < this.y + this.h) {
        this.on = !this.on;
        this.sendState()
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

    this.changeState = function(newState, send){
        if(this.on != newState && send){
            this.on = newState
            this.sendState()
        }else{
            this.on = newState
        }
        
    }

    this.sendState = function(){
        sendValues();
    }
}

document.getElementById("textSubmit").onclick = scrollingText

//contains pixels to scroll
var rows

var scrollState = 0-matrixSizeX

function scrollingText(){
    var text = document.getElementById("textInput").value.toUpperCase()
    text = text.split('')
    for(var i = 0; i < matrixSizeX/4; i++){
        text.unshift(' ')
    }
    rows = new Array(5*Math.floor(matrixSizeY/5))
    scrollState = 0
    for(var i = 0; i < rows.length; i++){
        rows[i] = []
        for(var j = 0; j < text.length; j++){
            rows[i] = rows[i].concat(letters[text[j]][Math.floor(i/Math.floor(matrixSizeY/5))])
        }
    }

    console.log(rows)
    scrollForward()
}

function scrollForward(){
    for(var i = 0; i < matrixSizeX; i++){
        if(scrollState+i > 0){
            for(var j = 0; j < rows.length; j++){
                buttons[i][j].changeState(rows[j][scrollState+Math.floor(i/Math.floor(matrixSizeY/5))], false)
            }
        }
    }
    sendValues()
    scrollState++
    if(scrollState < rows[0].length+matrixSizeX)
    setTimeout(scrollForward, 2000);
}