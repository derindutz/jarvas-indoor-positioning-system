# Tag

## Instructions

1. Connect the tag to your computer.
2. Open the .ino file in the Arduino app.
3. Upload the code.

##CALIBRATION IS IMPORTANT!

1. Make sure you know which anchors have which addresses.
2. Set CALIBRATION_OFFSET to true.
3. For each anchor, put the tag as close to the anchor as possible. 
4. Record each of the offset values, and put them in the BEACON_N_OFFSET for each beacon.
5. Set CALIBRATION_OFFSET to false, and CALIBRATION_MULTIPLIER to true.
6. Get a meterstick
7. For each anchor, put the tag exactly 1 meter away from the anchor, and record the multiplier.
8. Put these values in the BEACON_N_MULTIPLIER for each beacon.
9. Place beacons in edges of room, and measure their coordinates in meters in the real space. TIP: Put them in a right triangle for easy measuring.
10. Record these values in BEACON_N_X and BEACON_N_Y for each beacon


