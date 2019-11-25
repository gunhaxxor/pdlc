// var pixelStates = [];
globalThis.pixelStates = [1, 2, 3, 4, 5, 6, 7, 8, 9];


import Vue from 'vue';
import App from './App.vue';
import router from './router';
import './registerServiceWorker';
import * as socket from '@/js/socket.ts';

socket.initWebSocket();

Vue.config.productionTip = false;


new Vue({
  router,
  render: (h) => h(App),
}).$mount('#app');
