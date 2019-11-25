import store from "@/store";

let inMsgRaw;
let inMsg;
// const nrOfPixels = 121;

// Warning about this library.
// I think it relies on the javascript engine having objects in a certain order.
// Which is actually not part of the standard and hence not guaranteed.
// https://github.com/firejune/struct.js/issues/1
require("./struct.js");

declare global {
  interface Window {
    Struct: any;
  }
}
// Keep all fields to a size of 4 bytes in order to not fuck up memory alignment
export let structParser = new window.Struct({
  startChars: ["uint8", "<<<<", 4],
  pixelStates: ["uint8", [-1, -1, -1, -1, -1, -1], 121]
  // endChars: ["uint8", ">>>>", 4]
});

let webSocket: WebSocket;
export function initWebSocket() {
  let wsUrl;

  console.log("NODE_ENV: ", process.env.NODE_ENV);
  if (
    process.env.NODE_ENV === "production" ||
    !process.env.VUE_APP_WS_SERVER_URL
  ) {
    const protocol = location.protocol === "https:" ? "wss:" : "ws:";
    wsUrl = protocol + "//" + location.host;
  } else {
    wsUrl = process.env.VUE_APP_WS_SERVER_URL;
  }

  webSocket = new WebSocket(wsUrl);
  webSocket.addEventListener("open", event => {
    console.log("socket opened", event);
    webSocket.send("socket open");
    // setTimeout(() => {
    //   setReferenceValues();
    // }, 2500);
  });

  webSocket.onmessage = event => {
    // inMsgRaw = event.data;
    if (!(event.data instanceof Blob)) {
      console.log("socket received non-binary message: ", event.data);
      return;
    }

    console.log(
      "socket received %i bytes (blob): ",
      event.data.size,
      event.data
    );

    ////Superugly hack! I picked Body because it has the arrayBuffer() function
    // ((event.data as unknown) as Body)
    //   .arrayBuffer()
    event.data.arrayBuffer().then((buffer: ArrayBuffer) => {
      let msgObject = structParser.read(buffer);

      store.dispatch("setPixelStates", msgObject.pixelStates);
    });
  };

  webSocket.onclose = event => {
    console.log("socket closed", event);
    initWebSocket();
  };
  webSocket.onerror = event => {
    console.log("socket error", event);
  };

  console.log("socket state: ", webSocket.readyState);
}

export function sendMessage(msg: ArrayBuffer) {
  if (webSocket.readyState === webSocket.OPEN) {
    webSocket.send(msg);
  }
}

function setReferenceValues() {
  // for (let i = 0; i < pixelStates.length; i++) {
  //   pixelStates[i] = 0.5;
  // }
}
