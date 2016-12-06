curl -X POST 'jarvas-api.herokuapp.com/beacons' -H 'Content-Type: application/json' -d '{"beacons": [{"id": "a", "location": [0, 1]}, {"id": "b", "location": [1, 0]}, {"id": "c", "location": [0, 0]} ]}'

curl -X POST 'jarvas-api.herokuapp.com/range?id=a&r=1.0'
curl -X POST 'jarvas-api.herokuapp.com/range?id=b&r=1.0'
curl -X POST 'jarvas-api.herokuapp.com/range?id=c&r=1.414'
