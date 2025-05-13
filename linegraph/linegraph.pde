import processing.serial.*;

Serial myPort;
String val;
int sensorVal = 0;

int[] dataBuffer;
int bufferSize;
int bufferIndex = 0;
int maxVal = 14000;

void setup() {
  size(1440, 810);
  frameRate(60);
  textAlign(RIGHT, CENTER);
  textSize(12);
  smooth();

  bufferSize = width;
  dataBuffer = new int[bufferSize];

  String portName = "/dev/cu.usbserial-AQ02VYHY";
  myPort = new Serial(this, portName, 115200);
}

void draw() {
  background(40, 35, 30);

  drawGrid();

  if (myPort.available() > 0) {
    val = myPort.readStringUntil('\n'); 
    try {
      sensorVal = Integer.valueOf(val.trim());
    } catch(Exception e) {
      // Ignore malformed input
    }

    // Circular buffer write
    dataBuffer[bufferIndex] = sensorVal;
    bufferIndex = (bufferIndex + 1) % bufferSize;
  }

  drawGlowyWaveform();
}

void drawGrid() {
  stroke(80);
  fill(200);

  for (int v = 0; v <= maxVal; v += 2000) {
    float y = map(v, 0, maxVal, height, 0);
    line(0, y, width, y);
    text(v, 50, y);
  }

  for (int x = 60; x < width; x += 60) {
    line(x, 0, x, height);
  }
}

void drawGlowyWaveform() {
  // Glow pass
  drawWaveform(64, 207, 152, 20, 6);  // Soft glow
  drawWaveform(64, 207, 152, 60, 3);  // Mid glow
  drawWaveform(64, 207, 152, 255, 1.5);  // Core trace
}

void drawWaveform(int r, int g, int b, int alpha, float weight) {
  stroke(r, g, b, alpha);
  strokeWeight(weight);
  noFill();

  beginShape();
  for (int i = 0; i < bufferSize; i++) {
    int index = (bufferIndex + i) % bufferSize;
    float x = i;
    float y = map(dataBuffer[index], 0, maxVal, height, 0);
    vertex(x, y);
  }
  endShape();
}
