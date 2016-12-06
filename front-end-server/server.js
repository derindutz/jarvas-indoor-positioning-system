var express = require('express');
var bodyParser = require('body-parser');
var favicon = require('serve-favicon');
var app = express();

var port = process.env.PORT || 9000;

// app.use(bodyParser.text());
app.use(bodyParser.json());
app.use(bodyParser.urlencoded({ extended: true}));
app.use(express.static(__dirname + "/public"));
app.use(favicon(__dirname + "/public/favicon.ico"));

var location = [2.5, 2.5];

app.get('/location', function(req, res) {
  res.json({"location": location});
});

// POST to /location?x=1.15&y=2.54
app.post('/location', function(req, res) {
  console.log("received location post request: ", req.query)
  location = [
    parseFloat(req.query.x),
    parseFloat(req.query.y)
  ];
  res.sendStatus(200);
});

app.listen(port);
