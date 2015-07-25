IRrecv irrecv(RECV_PIN);

decode_results results;

SoftwareSerial irSerial(2, 3); // RX, TX

void setupIr(void){
  irSerial.begin(9600);
  irrecv.enableIRIn(); // Start the receiver
}

long readSerialIr(void) {
  if (!irSerial.available()) {
    return 0;
  }
  delay(50);
  char c = irSerial.read();
  while (irSerial.available() && c != '[') {
    c = irSerial.read();
  }
  if (c != '[') {
    return 0;
  }
  int res = 0;
  char buff[9];
  buff[0] = 0;
  for (int n = 0; n < 16 && irSerial.available() ; n++) {
    c = irSerial.read();
    if (c == ']') {
      break;
    }
    buff[n] = c;
    buff[n+1] = 0;
  }
  if (c != ']') {
    return 0;
  }
  return strtol(buff, NULL, 16);
}

long readIr(void) {
  long res = 0;
  if (irrecv.decode(&results)) {
    dump(&results);
    res = results.value;
    irrecv.resume(); // Receive the next value
  }
  return res;
}

void pressedIr(long value) {
  switch (value) {
    case 0xB4B40CF3:
      pressed('f');
      break;
    case 0xFF609F: // LED BULB OFF
      pressed('[');
      break;
    case 0xFFE01F: // LED BULB ON
      pressed(']'); 
      break;
    case 0xFFD02F: // LED BULB WHITE
      pressed('w'); 
      break;
    case 0xFF906F: // LED BULB RED1
      pressed('4'); 
      break;
    case 0xFF10EF: // LED BULB GREEN1
      pressed('5'); 
      break;
    case 0xFF50AF: // LED BULB BLUE1
      pressed('6'); 
      break;
    case 0xFFB04F: // LED BULB RED2
      pressed('7'); 
      break;
    case 0xFF30CF: // LED BULB GREEN2
      pressed('8'); 
      break;
    case 0xFF708F: // LED BULB BLUE2
      pressed('9'); 
      break;
    case 0xFFA857: // LED BULB RED3
      pressed('A'); 
      break;
    case 0xFF9867: // LED BULB RED4
      pressed('D'); 
      break;
    case 0xFF8877: // LED BULB RED5
      pressed('G'); 
      break;
    case 0xFF28D7: // LED BULB GREEN3
      pressed('B'); 
      break;
    case 0xFF18E7: // LED BULB GREEN4
      pressed('E'); 
      break;
    case 0xFF08F7: // LED BULB GREEN5
      pressed('H'); 
      break;
    case 0xFF6897: // LED BULB BLUE3
      pressed('C'); 
      break;
    case 0xFF58A7: // LED BULB BLUE4
      pressed('F'); 
      break;
    case 0xFF48B7: // LED BULB BLUE5
      pressed('I'); 
      break;
    case 0xFFF00F: // LED BULB FLASH
      pressed('0'); 
      break;
    case 0xFFE817: // LED BULB STROBE
      pressed('1'); 
      break;
    case 0xFFD827: // LED BULB FADE
      pressed('2'); 
      break;
    case 0xFFC837: // LED BULB SMOOTH
      pressed('3'); 
      break;
    case 0xFFA05F: // LED BULB +
      pressed('+'); 
      break;
    case 0xFF20DF: // LED BULB -
      pressed('-'); 
      break;
    default: // Not decoded
      pressed('?'); 
      break;
  }
}


// Dumps out the decode_results structure.
// Call this after IRrecv::decode()
// void * to work around compiler issue
//void dump(void *v) {
//  decode_results *results = (decode_results *)v
void dump(decode_results *results) {
  int count = results->rawlen;
  if (results->decode_type == UNKNOWN) {
    Serial.print("Unknown encoding: ");
  }
  else if (results->decode_type == NEC) {
    Serial.print("Decoded NEC: ");
  }
  else if (results->decode_type == SONY) {
    Serial.print("Decoded SONY: ");
  }
  else if (results->decode_type == RC5) {
    Serial.print("Decoded RC5: ");
  }
  else if (results->decode_type == RC6) {
    Serial.print("Decoded RC6: ");
  }
  else if (results->decode_type == PANASONIC) {
    Serial.print("Decoded PANASONIC - Address: ");
    Serial.print(results->address,HEX);
    Serial.print(" Value: ");
  }
  else if (results->decode_type == LG) {
     Serial.print("Decoded LG: ");
  }
  else if (results->decode_type == JVC) {
     Serial.print("Decoded JVC: ");

  }
  else if (results->decode_type == AIWA_RC_T501) {
    Serial.print("Decoded AIWA RC T501: ");
  }
  else if (results->decode_type == WHYNTER) {
     Serial.print("Decoded Whynter: ");
  }
  Serial.print(results->value, HEX);
  Serial.print(" (");
  Serial.print(results->bits, DEC);
  Serial.println(" bits)");
  Serial.print("Raw (");
  Serial.print(count, DEC);
  Serial.print("): ");

  for (int i = 0; i < count; i++) {
    if ((i % 2) == 1) {
      Serial.print(results->rawbuf[i]*USECPERTICK, DEC);
    }
    else {
      Serial.print(-(int)results->rawbuf[i]*USECPERTICK, DEC);
    }
    Serial.print(" ");
  }
  Serial.println("");
}
