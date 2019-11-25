<template>
  <div class="home">
    <div id="p5-canvas-slot"></div>
  </div>
</template>

<script>
// @ is an alias to /src
import { mapState, mapActions } from "vuex";
import p5 from "p5";

import pixelCanvas from "@/ts/pixel-canvas";

export default {
  name: "home",
  data() {
    return {
      sketch: undefined
    };
  },
  methods: {
    // ...mapActions(["addElement"])
  },
  mounted() {
    console.log("mounted");
    this.sketch = new p5(
      pixelCanvas,
      document.querySelector("#p5-canvas-slot")
    );
  },
  beforeDestroy() {
    console.log("before destroy");
    if (this.sketch) {
      console.log("P5 sketch present. DELETING");
      this.sketch.remove();
    }
  },
  components: {}
};
</script>
