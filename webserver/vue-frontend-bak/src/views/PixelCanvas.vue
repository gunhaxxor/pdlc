<template>
  <div id="p5-canvas-slot"></div>
</template>


<script lang="ts">
// import p5init from '@/js/pixel-canvas.ts';

// import p5 from 'p5';

// import p5 from 'p5/lib/p5.min.js';

// const sketch = new p5(
//   p5init,
//   document.querySelector('#p5-canvas-slot') as HTMLElement,
// );

// let sketch: any;
export default {
  name: 'pixelCanvas',
  mounted() {
    console.log('mounted!');
  },
  beforeDestroy() {
    console.log('beforeDestroyed called');
    // document.querySelector("#p5-canvas-slot").removeChild();
    // sketch.remove();
  },
  components: {},
};
</script>
