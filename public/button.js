'use strict';

let loginbutton = document.getElementById("loginbutton");
loginbutton.addEventListener("click", goToLogin);

function goToLogin() {
  //send get req to server login page -> server will then send resp to browser w redirect req to spotify auth endpt
  let url = "http://localhost:8000/login";
  sendGetReq(url)
  .then(function(data) {
    //replace current url with auth url on client-side instead of redirecting and using a cors proxy
    window.location = data;
  })
  .catch(function(err) {
    //catching either network error or http error
    console.log("Error:", err);
  });
}

async function sendGetReq(url) {
  //await means that we return from fetch fctn and continue on w program but come back to it when it finishes 
  let response = await fetch(url, {method: 'GET'});
  if (response.ok) {
    let data = await response.text();
    //returns a promise bc async fctns return promises
    return data;
  }
  else {
    throw Error(response.status);
  }
}