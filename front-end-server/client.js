var request = require('superagent');
var Chartist = require('chartist');

var location = {x: 2.5, y: 2.5};
var beacons = [
  {x: 0, y: 0},
  {x: 5, y: 0},
  {x: 0, y: 5}
]

var chart = new Chartist.Line('.ct-chart', {
  series: [
    beacons,
    [location],
    [location]
  ]
}, {
  axisX: {
    type: Chartist.AutoScaleAxis,
    low: 0,
    scaleMinSpace: 30
  },
  axisY: {
    low: 0
  },
  showLine: false,
  height: 500
});

// Seed beacons
request
  .get('https://jarvas-api.herokuapp.com/beacons')
  .end(function(err, res) {
    if (err) {
      console.log(err);
    }
    var newBeacons = [];
    for (var id in res.body.beacons) {
      var b = res.body.beacons[id];
      newBeacons.push({x: b[0], y: b[1]});
    }
    beacons = newBeacons;
    chart.data.series[0] = beacons;
    chart.update();
  });

function formatLocation(location) {
  return location[0] + ", " + location[1]
}


var updating = false;
setInterval(function() {
  request
    .get('https://jarvas-api.herokuapp.com/location')
    .end(function(err, res) {
      if (err) {
        console.log(err);
      }
      var newLocation = res.body;
      var x = newLocation.x, y = newLocation.y;
      if (location.x !== x || location.y !== y) {
        location.x = x;
        location.y = y;
        chart.data.series[1] = [location];
        chart.data.series[2] = [location];
        // Update chart only once per second to prevent animation stutter
        if (!updating) {
          updating = true;
          setTimeout(function() {
            chart.update();
            updating = false;
          }, 500);
        }
      }
    });
}, 300);

