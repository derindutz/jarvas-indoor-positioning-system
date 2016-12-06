curl -X POST localhost:8000/reset
curl -X POST localhost:8000/beacons -H "Content-Type: application/json" -d '{"beacons": [{"id": "a", "location": [1.2, 2.3]}, {"id": "b", "location": [1.5, 3.6]}, {"id": "c", "location": [8.0, 3.5]}]}'
curl -X POST localhost:8000/range -H "Content-Type: application/json" -d '{"range": 1.45, "id": "a"}'
curl -X POST localhost:8000/range -H "Content-Type: application/json" -d '{"range": 2.45, "id": "b"}'
curl -X POST localhost:8000/range -H "Content-Type: application/json" -d '{"range": 9.6, "id": "c"}'
curl localhost:8000/location
