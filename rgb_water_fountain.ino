#include <IRremote.h>
#include <SoftwareSerial.h>

int RECV_PIN = 7;
int WATER_PIN = 4;
int PR = 9;
int PG = 11;
int PB = 5;

int max = 2550;

//long r = 2550; //up to max
//long g = 2550;
//long b = 2550;

long r = 140; //up to max
long g = 910;
long b = 2550;

//long r = 0; //up to max
//long g = 0;
//long b = 0;

int pvr = 0;
int pvg = 0;
int pvb = 0;

boolean newColor = false;
char newColorC = 0;

double bright = 1;

int program = '0'; // 0, 2
int mode = 'X'; // X, W, L
boolean on = 0; // true, false   ;-)

unsigned char fr = 255; // up to 255
unsigned char fg = 255; // 100
unsigned char fb = 255; // 100

int rs = 3; // up to max
int gs = -2;
int bs = 1;

boolean printColor = true;

double hsl[3] = {0, 1, 0.5};

// Pressing 0 changes to program 0 so leds freeze
// Pressing 1 changes mode, no program change
// Pressing 2 starts program 2, soft colour change
// Pressing 3 sets random color and sets program 0
// Pressing other numbers recall colour and sets program 0

// Program 0 still colour
// Program 2, soft colour change

unsigned char colors[15][3] = { 
  { 255,   0,   0 }, {   0, 255,   0 }, {   0,   0, 255 }, //4 5 6
  { 255,  40,   0 }, {   0, 255,  20 }, {  12,  15, 255 }, //7 8 9
  { 255,  96,   0 }, {   0, 255, 150 }, {  14,  91, 255 }, //A B C
  { 255, 128,   0 }, {   0, 232, 192 }, { 160,   0, 255 }, //D E F
  { 255, 200,   0 }, {   0,  71, 255 }, { 255,   0,  90 }  //G H I
  };

void readColor(int n) {
  r = 10 * colors[n][0];
  g = 10 * colors[n][1];
  b = 10 * colors[n][2];
}

void writeColor(int n) {
  colors[n][0] = r / 10;
  colors[n][1] = g / 10;
  colors[n][2] = b / 10;
}

// TODO EEPROM read and write
// TODO Store color with serial command
// TODO Store color with remote control

void pressed(int c) {
  digitalWrite(13, 1);
  if (c != '?') {
    setRed(0);
    setGreen(0);
    setBlue(0);
    delay(10);
    setRed(0x30);
    setGreen(0xFF);
    setBlue(0x10);
  } else if (false) {
    setRed(0);
    setGreen(0);
    setBlue(0);
    delay(10);
    setRed(0xFF);
    setGreen(0x00);
    setBlue(0x00);
    delay(50);
  }
  delay(50);
  digitalWrite(13, 0);
  set();

  if (true) {
    Serial.print("Pressed: ");
    Serial.println((char)c);
  }
  randomSeed(random(1000) + c + millis());
  if (c == 'f') {
    on = !on;
    set();
  } else if (c == '[') {
    on = false;
    set();
  } else if (c == ']') {
    on = true;
    set();
  } else if (on) {
    if (c == '0') {
      program = '0';
      set();
    } else if (c == '1') {
      if (mode == 'X') {
        mode = 'W';
      } else if (mode == 'W') {
        mode = 'L';
      } else {
        mode = 'X';
      }
      set();
    } else if (c == '2') {
      double rgb[] = { r / (double)max, g / (double)max, b / (double)max };
      rgb2hsl(rgb, hsl);
      // Soft change
      program = '2';
    } else if (c == '3') {
      randomRGB();
      program = '0';
      set();
    } else if (c >= '4' && c <= '9') {
      readColor(c - '4');
      program = '0';
      set();
    } else if (c >= 'A' && c <= 'Z') {
      readColor(c + 6 - 'A');
      program = '0';
      set();
    } else if (c == 'w') {
      r = max;
      g = max;
      b = max;
      program = '0';
      set();
    } else if (c == '+' && !newColor) {
      bright += 0.2;
      if (bright > 1) {
        bright = 1;
        setRed(0);
        setGreen(0);
        setBlue(0);
        delay(10);
        setRed(0xFF);
        setGreen(0x30);
        setBlue(0x10);
        delay(50);
      }
      set();
    } else if (c == '-' && !newColor) {
      bright -= 0.2;
      if (bright < 0.2) {
        bright = 0.2;
        setRed(0);
        setGreen(0);
        setBlue(0);
        delay(10);
        setRed(0xFF);
        setGreen(0x30);
        setBlue(0x10);
        delay(50);
      }
      set();
    } else if (c == '+' && newColor) {
      if (newColorC == 0) {
        r = min(max, r + 50);
      } else if (newColorC == 1) {
        g = min(max, g + 50);
      } else if (newColorC == 2) {
        b = min(max, b + 50);
      }
      set();
    } else if (c == '-' && newColor) {
      if (newColorC == 0) {
        r = max(0, r - 50);
      } else if (newColorC == 1) {
        g = max(0, g - 50);
      } else if (newColorC == 2) {
        b = max(0, b - 50);
      }
      set();
    } else if (c == 'r' && newColor) {
      newColorC = 0;
      Serial.println("RED");
    } else if (c == 'g' && newColor) {
      newColorC = 1;
      Serial.println("GREEN");
    } else if (c == 'b' && newColor) {
      newColorC = 2;
      Serial.println("BLUE");
    } else if (c == 'c') {
      newColor = !newColor;
      Serial.print("New color: ");
      Serial.println(newColor ? "ON" : "OFF");
      program = '0';
      set();
      delay(500);
    }
  }
}

void set() {
  int vr, vg, vb;
  if (on && (mode == 'L' || mode == 'X')) {
    vr = (int)(r/10) * bright * bright * fr / 255;
    vg = (int)(g/10) * bright * bright * fg / 255;
    vb = (int)(b/10) * bright * bright * fb / 255;
  } else {
    vr = 0;
    vg = 0;
    vb = 0;
  }
  setRed(vr);
  setGreen(vg);
  setBlue(vb);

  if(printColor){
    printColor = false;
    Serial.print("R:");
    Serial.print(r);
    Serial.print("  G:");
    Serial.print(g);
    Serial.print("  B:");
    Serial.print(b);
    Serial.print("  ");
    Serial.print("VR:");
    Serial.print((int)vr);
    Serial.print("  VG:");
    Serial.print((int)vg);
    Serial.print("  VB:");
    Serial.print((int)vb);
    Serial.println();
  }
  if (!on || mode == 'L') {
    pinMode(WATER_PIN, OUTPUT);
    digitalWrite(WATER_PIN, 1);    
  } else {
    pinMode(WATER_PIN, OUTPUT);
    digitalWrite(WATER_PIN, 0);
  }
}

void setup() {
  Serial.begin(9600);
  setupIr();
  Serial.println("On");
  Serial.println("Running");
}

void loop() {
  long irvalue = readSerialIr();
  if (irvalue != 0) {
    Serial.println(irvalue, HEX);
    pressedIr(irvalue);
  }
  irvalue = readIr();
  if (irvalue != 0) {
    Serial.println(irvalue, HEX);
    pressedIr(irvalue);
  }
  if (Serial.available()) {
    int c = Serial.read();
    pressed(c);
    if (!newColor) {
      while (Serial.available()) {Serial.read();};
    }
  }
  if (on) {
    //Serial.print("Mode: ");
    //Serial.println((char)mode);
    if (mode == 'L' || mode == 'X') {
     // Serial.print("Program: ");
     // Serial.println((char)program);
      if (program == '2') {
        softChangeHSL();
        delay(5);
      } else if (program == '0') {
        set();
        if (!newColor || !Serial.available()) {
          delay(50);
        }
      }
    }
  } else {
    set();
    wait();
  }
}

void wait() {
  delay(100);
}

void fixedColor(int _r, int _g, int _b) {
  r = _r;
  g = _g;
  b = _b;
  set();
  wait();
}


void softChangeHSL() {
  hsl[0]=fmod(hsl[0]+0.001, 1);
  hsl[1]=1;
  hsl[2]=0.5;
  setHSL();
}

void randomRGB() {
  hsl[0] = fmod(hsl[0] + .3 + (random(3)/10.0), 1);
  hsl[1] = (90.0 + random(10)) / 100.0;
  hsl[2] = 0.5;
  setHSL();
}

void setHSL() {
  if (true) {
    Serial.print("H:");
    Serial.print(hsl[0]);
    Serial.print("  S:");
    Serial.print(hsl[1]);
    Serial.print("  L:");
    Serial.print(hsl[2]);
    Serial.println();
  }
  double rgb[3];
  hsl2rgb(hsl, rgb);
  r = rgb[0]*max;  
  g = rgb[1]*max;  
  b = rgb[2]*max;  
  set();
}

void setRed(int v) {
  if (v != pvr) {
    printColor = true;
    pvr = v;
  }
  analogWrite(PR, v);
}

void setGreen(int v) {
  if (v != pvg) {
    printColor = true;
    pvg = v;
  }
  analogWrite(PG, v);
}

void setBlue(int v) {
  if (v != pvb) {
    printColor = true;
    pvb = v;
  }
  analogWrite(PB, v);
}

//void softChange() {
//  if (false) {
//    Serial.print("R:");
//    Serial.print((int)r);
//    Serial.print("  G:");
//    Serial.print((int)g);
//    Serial.print("  B:");
//    Serial.print((int)b);
//    Serial.print(" S  R:");
//    Serial.print((int)rs);
//    Serial.print("  G:");
//    Serial.print((int)gs);
//    Serial.print("  B:");
//    Serial.print((int)bs);
//    Serial.println();
//  }
//  
//  if (r <= abs(rs)) {
//    rs = abs(rs);
//    r = rs + 1;
//  } else if ( r >= max-abs(rs) ) {
//    rs = -abs(rs);
//    r = max-abs(rs);
//  }
//  r = (r + rs);
//  if (g <= abs(gs)) {
//    gs = abs(gs);
//  } else if ( g > max-abs(gs) ) {
//    gs = -abs(gs);
//    g = max-abs(gs);
//  }
//  g = (g + gs);
//  if (b <= abs(bs)) {
//    bs = abs(bs);
//  } else if ( b > max-abs(bs) ) {
//    bs = -abs(bs);
//    b = max-abs(bs);
//  }
//  b = (b + bs);
//}
//

