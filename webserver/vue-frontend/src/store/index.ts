import Vue from "vue";
import Vuex from "vuex";

import { sendMessage, structParser } from "@/ts/socket";

Vue.use(Vuex);

export default new Vuex.Store({
  state: {
    pixelStates: Array<number>(121)
  },
  mutations: {
    addElement(state, value: number) {
      state.pixelStates.push(value);
    },
    setPixelStates(state, pixelStates) {
      state.pixelStates = pixelStates.slice();
    },
    clearAllPixels(state) {
      for (let i = 0; i < state.pixelStates.length; i++) {
        Vue.set(state.pixelStates, i, 0);
      }
    }
  },
  actions: {
    setPixelStates(context, pixelValues) {
      context.commit("setPixelStates", pixelValues);
    },
    setPixelStatesAndSend(context, pixelValues) {
      if (!arraysEqual(context.state.pixelStates, pixelValues)) {
        let msg = structParser.write({
          startChars: "<<<<",
          pixelStates: pixelValues
          // endChar: "!!!!"
        });
        sendMessage(msg);
      }
      context.commit("setPixelStates", pixelValues);
    },
    sendState(context) {
      let msg = structParser.write({
        startChars: "<<<<",
        pixelStates: context.state.pixelStates
        // endChar: "!!!!"
      });
      sendMessage(msg);
    },
    addElement(context, value) {
      // let msg = new ArrayBuffer(5);
      // sendMessage(msg);
      context.commit("addElement", value);
    }
  },
  modules: {}
});

function arraysEqual(a: Array<any>, b: Array<any>) {
  if (a === b) return true;
  if (a == null || b == null) return false;
  if (a.length != b.length) return false;

  for (var i = 0; i < a.length; ++i) {
    if (a[i] !== b[i]) return false;
  }
  return true;
}
