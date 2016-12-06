from flask import Flask, request, jsonify
from flask_cors import CORS, cross_origin

import positioning as p
import os

app = Flask(__name__)
CORS(app)

ranges = {}
beacons = {}

# POST to /range?id=wow&r=2.54
# GET to /range?id=wow
@app.route('/range', methods=['GET', 'POST'])
def range():
    if request.method == 'POST':
        anchor_id = request.args.get('id')
        anchor_range = request.args.get('r')
        ranges[anchor_id] = float(anchor_range)
        return "", 200
    elif request.method == 'GET':
        anchor_id = request.args.get('id')
        if anchor_id in ranges:
            return str(ranges[anchor_id]), 200
        else:
            return "range not found", 404

# Expected json:
# {
#     "beacons": [
#         {
#             "id": "someid",
#             "location": [1.2, 2.3]
#         },
#         ...
#     ]
# }
@app.route('/beacons', methods=['GET', 'POST'])
def putBeacons():
    if request.method == 'POST':
        json = request.get_json()
        assert 'beacons' in json
        beaconJson = json['beacons']
        for b in beaconJson:
            print b
            assert 'id' in b
            assert 'location' in b
            beacons[b['id']] = b['location']
        keys = sorted(beacons.keys())
        beaconLocations = []
        for k in keys:
            beaconLocations.append(beacons[k])
        p.setup_positioning(beaconLocations)
        return "", 200
    elif request.method == 'GET':
        return jsonify(beacons=beacons), 200

# Actual shit lol
@app.route('/location', methods=['GET'])
def getLocation():
    beaconIds = sorted(beacons.keys())
    beaconRanges = []
    for i in beaconIds:
        beaconRanges.append(ranges[i])
    location = p.iterate_position(beaconRanges[0], beaconRanges[1], beaconRanges[2])
    return jsonify(x=location[0,0], y=location[0, 1])

@app.route('/reset', methods=['POST'])
def resetState():
    ranges = {}
    beacons = {}
    return str(ranges) + " " + str(beacons), 200

if __name__ == '__main__':
    port = int(os.environ.get("PORT", 5000))
    app.run(host='0.0.0.0', port=port)
