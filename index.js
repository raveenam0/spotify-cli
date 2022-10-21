const express = require("express");
const app = express();
var fs = require('fs');
const fetch = require("cross-fetch");

//client credentials hidden
var client_id = '';
var client_secret = '';

const port = 8000;
var redirect_uri = 'http://localhost:8000/callback';

//express pipeline
app.use(function(req,res,next) {
  console.log(req.method, req.url);
  next();
})

//make files in public folder available
app.use(express.static("public"));

//converts req body json string into json obj to get key val pairs
app.use(express.json());

app.get('/callback', (req,res) => {
  //get auth code from query param and send req for access token
  let body_obj = {
    grant_type: "authorization_code",
    code: req.query.code,
    redirect_uri: redirect_uri
  }

  //body needs to be encoded as query string
  let body_encoded = new URLSearchParams(body_obj);

  let obj = {
    method: 'POST',
    headers: {
      'Content-Type': "application/x-www-form-urlencoded",
      'Authorization': 'Basic ' + (Buffer.from(client_id + ':' + client_secret).toString('base64'))
    },
    body: body_encoded
  };

  sendPostReq("https://accounts.spotify.com/api/token", obj)
  .then(function(data) {
    //write access token to text file
    fs.writeFile('access_token.txt', data.access_token, function(error) {
      if (error) {
        throw error;
      }
      else {
        console.log("Access token written to file");
      }
    });
  })
  .catch(function(error) {
    console.log("Error:", error);
  })

  res.sendFile(__dirname + "/public/callback.html");
});

async function sendPostReq(url, obj) {
  //await means that we return from fetch fctn and continue on w program but come back to it when it finishes 
  let response = await fetch(url, obj);

  if (response.ok) {
    let data = await response.json();
    //returns a promise bc async fctns return promises
    return data;
  }
  else {
    throw Error(response.status);
  }
}

app.get('/login', (req,res) => {
  //set up auth req url and send it to client
  var scopes = 'user-read-playback-state user-modify-playback-state';
  
  //server side redirect
  const obj = {
    client_id: client_id,
    response_type: 'code',
    redirect_uri: redirect_uri,
    scope: scopes,
    show_dialog: false
  };
  
  //convert to query string
  let searchparams = new URLSearchParams(obj);

  //instead of sending a redirect, send url to client side so they can send req 
  res.send("https://accounts.spotify.com/authorize?" + searchparams);
});

app.get('/', (req,res) => {
  //no file specified -> want browser to display home pg (send back index.html)
  res.sendFile(__dirname + "/public/index.html");
});

//end of pipeline specification

//listen for http reqs
app.listen(port, () => {
  console.log(`The static server is listening on port ${port}`);
});