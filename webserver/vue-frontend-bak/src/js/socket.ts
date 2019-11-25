let inMsgRaw;
let inMsg;
// const nrOfPixels = 121;

export function initWebSocket() {
  let wsUrl;
  console.log('NODE_ENV: ', process.env.NODE_ENV);
  if (process.env.NODE_ENV === 'production') {
    const protocol = location.protocol === 'https:' ? 'wss:' : 'ws:';
    wsUrl = protocol + '//' + location.host;
  } else {
    wsUrl = process.env.VUE_APP_WS_SERVER_URL;
  }

  const webSocket = new WebSocket(wsUrl);
  webSocket.addEventListener('open', event => {
    console.log('socket opened', event);
    webSocket.send('socket open');
    setTimeout(() => {
      setReferenceValues();
    }, 2500);
  });

  webSocket.onmessage = event => {
    inMsgRaw = event.data;
  };

  webSocket.onclose = event => {
    console.log('socket closed', event);
    initWebSocket();
  };
  webSocket.onerror = event => {
    console.log('socket error', event);
  };

  console.log('socket state: ', webSocket.readyState);
}

function setReferenceValues() {
  // for (let i = 0; i < pixelStates.length; i++) {
  //   pixelStates[i] = 0.5;
  // }
}
