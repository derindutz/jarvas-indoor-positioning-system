# Wifi Setup

## Instructions
1. Connect the wifi anchor to your computer.
2. Open the .ino file in the Arduino app.
3. Upload the code.
4. Once the code is uploaded, open the Serial Monitor.
5. Type AT into the Serial Monitor. You should see OK returned.
6. Type AT+CWMODE=1 into Serial. You should see OK returned.
7. Type AT+CWLAP into Serial. You should see a list of access points. Note that the ssid you want to use is in the list.
8. Type AT+CWJAP="ssid","pwd" where ssid is the one you want to use and pwd is the corresponding password.
9. Once you've recieved an IP, type AT+CIFSR. You should see a valid IP address returned.
