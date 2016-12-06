
void setup() {
  // Serial is for communication between computer and board.
  // Serial1 is for communication between board and wifi module.
  Serial.begin(74880); // 74880
  Serial1.begin(115200);
  delay(1000);
  Serial1.print("AT+UART_CUR=74880,8,1,0,0\r\n");
  delay(2000);
  Serial1.begin(74880); 
}

void loop() {
  if(Serial1.available() > 0){
    Serial.write(Serial1.read());
  }
  if(Serial.available() > 0){
    Serial1.write(Serial.read());
  }
}

// AT -> OK
// AT+CWMODE=1 -> OK
// AT+CWLAP -> find ssid
// AT+CWJAP="ssid","pwd"
// AT+CIFSR -> ensure valid IP
