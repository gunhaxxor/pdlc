import p5 from 'p5';

export default function (p: p5) {
  let canvas: any;

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
    // console.log(pixelStates);
    // const canvasSlot = document.querySelector('#p5-canvas-slot');
    // console.log(canvasSlot);
    // while (canvasSlot && canvasSlot.firstChild) {
    //   console.log('removing stuff inside canvas holder');
    //   canvasSlot.removeChild(canvasSlot.firstChild);
    // }

    canvas = p.createCanvas(p.windowHeight, p.windowHeight);
  };

  // NOTE: Draw is here
  p.draw = () => {
    p.background(40, 150, 70);
  };

  p.windowResized = () => {
    p.resizeCanvas(p.windowHeight, p.windowHeight);
  };
}
