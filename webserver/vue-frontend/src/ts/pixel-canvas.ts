import p5 from "p5";
import store from "@/store/index";

export default function(p: p5) {
  let canvas: any;
  let nrOfPixels = 0;
  let nrOfCols = 0;
  let nrOfRows = 0;
  let pixelSize = 0;
  let pixelStates: Array<number>;

  let animationMode = 0;
  const nrOfAnimationModes = 3 + 1;

  let eraserActive = false;

  const saturation = 0.7;
  const grey = p.color(128);
  // const colors = [
  //   p.lerpColor(grey, p.color(255, 150, 0, 255), saturation),
  //   p.lerpColor(grey, p.color(0, 0, 255, 255), saturation),
  //   p.lerpColor(grey, p.color(153, 76, 0, 255), saturation),
  //   p.lerpColor(grey, p.color(255, 255, 0, 255), saturation),
  //   p.lerpColor(grey, p.color(255, 0, 0, 255), saturation),
  //   p.lerpColor(grey, p.color(0, 255, 0, 255), saturation),
  // ];
  // NOTE: Set up is here
  p.setup = () => {
    console.log(store.state.pixelStates);
    // console.log(pixelStates);
    // const canvasSlot = document.querySelector('#p5-canvas-slot');
    // console.log(canvasSlot);
    // while (canvasSlot && canvasSlot.firstChild) {
    //   console.log('removing stuff inside canvas holder');
    //   canvasSlot.removeChild(canvasSlot.firstChild);
    // }
    p.angleMode(p.DEGREES);
    p.frameRate(30);
    p.noiseDetail(2, 0.5);

    let sendLoop = () => {
      setTimeout(() => {
        store.dispatch("setPixelStatesAndSend", pixelStates);
        console.log("sending msg frame is: %i", p.frameCount);
        sendLoop();
      }, 40);
    };
    // sendLoop();

    store.commit("clearAllPixels");
    store.dispatch("sendState");

    let size = p.min(p.windowHeight, p.windowWidth);
    canvas = p.createCanvas(size, size);
  };

  // NOTE: Draw is here
  p.draw = () => {
    p.background(0, 0, 170);
    p.fill(255);

    pixelStates = store.state.pixelStates.slice();
    nrOfPixels = store.state.pixelStates.length;
    nrOfCols = Math.floor(Math.sqrt(nrOfPixels));
    nrOfRows = Math.floor(Math.sqrt(nrOfPixels));
    if (nrOfCols == 0) {
      return;
    }
    pixelSize = p.width / nrOfCols;

    switch (animationMode) {
      case 1:
        animation1Update();
        break;
      case 2:
        animation2Update();
        break;
      case 3:
        animation3Update();
        break;
      default:
        break;
    }

    for (let i = 0; i < nrOfPixels; i++) {
      const pixel = store.state.pixelStates[i];
      let x = (i % nrOfCols) * pixelSize;
      let y = Math.floor(i / nrOfCols) * pixelSize;
      if (0 <= pixelStates[i] && pixelStates[i] <= 255) {
        // color;
        p.fill(p.floor(pixelStates[i]));
        p.square(x, y, pixelSize);
      } else {
        console.log("invalid pixelState for index %i: %i", i, pixelStates[i]);
      }
    }

    if (p.mouseIsPressed) {
      // console.log(p.mouseX, p.mouseY);
      let pixelIndex = getPixelIndexAtCoordinate(p.mouseX, p.mouseY);
      if (!(pixelIndex === -1)) {
        if (eraserActive) {
          pixelStates[pixelIndex] -= 40;
        } else {
          pixelStates[pixelIndex] += 40;
        }
        pixelStates[pixelIndex] = p.constrain(pixelStates[pixelIndex], 0, 255);
        store.dispatch("setPixelStatesAndSend", pixelStates);
      }
    }
  };

  p.keyPressed = () => {
    if (p.key == "x") {
      eraserActive = !eraserActive;
    } else if (p.key == "a") {
      animationMode++;
      animationMode %= nrOfAnimationModes;
    }
  };

  p.windowResized = () => {
    console.log("window resized!!");
    let size = p.min(p.windowHeight, p.windowHeight);
    p.resizeCanvas(size, size);
  };

  function animation1Update() {
    for (let i = 0; i < pixelStates.length; i++) {
      let colRow = getColumnRowForIndex(i);
      // pixelStates[i] = p.floor(
      //   p.map(
      //     p.noise(colRow.x / 8, colRow.y / 8, p.frameCount * 0.02),
      //     0.2,
      //     0.8,
      //     0,
      //     255,
      //     true
      //   )
      // );

      pixelStates[i] = p.floor(
        p.map(
          p.noise(
            colRow.x / 8 + p.sin(90 + p.frameCount / 4.5) * 3,
            colRow.y / 8 + p.sin(p.frameCount / 4) * 3,
            p.frameCount * 0.02
          ),
          0.3,
          0.7,
          0,
          255,
          true
        )
      );
    }

    store.dispatch("setPixelStatesAndSend", pixelStates);
  }

  function animation2Update() {
    for (let i = 0; i < pixelStates.length; i++) {
      let colRow = getColumnRowForIndex(i);
      let horizontalWeight = p.sin(p.frameCount / 4);
      let verticalWeight = p.sin(p.frameCount / 2 + 180);
      let inputValue =
        p.frameCount * 2 +
        p.map(colRow.x, 0, nrOfCols, 0, 360 * horizontalWeight) +
        p.map(colRow.y, 0, nrOfCols, 0, 360 * verticalWeight);
      let brightness = p.sin(inputValue);
      brightness = p.floor((255 * (brightness + 1)) / 2);
      pixelStates[i] = brightness;
    }
    store.dispatch("setPixelStatesAndSend", pixelStates);
  }

  function animation3Update() {
    if (p.frameCount % 10) {
      return;
    }
    for (let i = 0; i < pixelStates.length; i++) {
      pixelStates[i] = p.floor(p.random(255));
    }
    store.dispatch("setPixelStatesAndSend", pixelStates);
  }

  function getColumnRowForIndex(i: number): p5.Vector {
    let x = i % nrOfCols;
    let y = Math.floor(i / nrOfCols);
    return p.createVector(x, y);
  }

  function getPixelIndexAtCoordinate(x: number, y: number) {
    if (!(0 < x) || !(x < p.width) || !(0 < y) || !(y < p.height)) {
      return -1;
    }
    let lastRowOffset = Math.floor(x / pixelSize);
    let allPreviousRows = Math.floor(y / pixelSize) * nrOfCols;
    return allPreviousRows + lastRowOffset;
  }
}
