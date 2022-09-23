const express = require("express");
const app = express();
const port = 8000;

var client_id = 'client_id';
var client_secret = 'client_secret';
var scopes = 'user-read-playback-state user-modify-playback-state';

//express pipeline
app.use(function(req,res,next) {
  console.log(req.method,req.url);
  next();
})

//make files in public folder available
app.use(express.static("public"));

//no file specified -> want browser to display home pg (send back index.html)
app.get('/', (req,res) => {
  res.sendFile(__dirname + "/public/index.html");
});

//end of pipeline specification

//listen for http reqs
app.listen(port, () => {
  console.log(`The static server is listening on port ${port}`);
});