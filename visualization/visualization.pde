import processing.serial.*;

Serial myPort;        // Create object from Serial class
String val;           // Data received from the serial port
int sensorVal = 0;

void setup() {
  size(720, 480);
  stroke(255);
  noFill();

  String portName = "/dev/cu.usbserial-AQ02VYHY";  // Your actual serial port
  myPort = new Serial(this, portName, 115200);     // Set baud rate to 115200
}

void draw() {
  if (myPort.available() > 0) {
    val = myPort.readStringUntil('\n'); 
    try {
      sensorVal = Integer.valueOf(val.trim());
    } catch(Exception e) {
      // Ignore malformed input
    }
    println(sensorVal);
  }

  background(0);

  // Map sensorVal from 0–8000 to a range that visually affects the curve
  float mappedVal = map(sensorVal, 0, 8000, -100, 100);

  for (int i = 0; i < 200; i += 20) {
    bezier(
      width/2 + mappedVal - (i / 2.0), 40 + i,
      410, 20,
      440, 300,
      240 - (i / 16.0), 300 + (i / 8.0)
    );
  }
}
