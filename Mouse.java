import processing.serial.*;
import java.awt.Robot;
import java.awt.event.InputEvent;
import java.io.Serial;

Serial myPort; // Serial port
Robot robot;

boolean verticalScroll = false;
boolean horizontalScroll = false;
boolean autoClicker = false;
int autoClickSpeed = 1;

void setup() {
  size(400, 400); // GUI window (optional)
  myPort = new Serial(this, "COM3", 115200); // Adjust COM port
  try {
    robot = new Robot();
  } catch (Exception e) {
    e.printStackTrace();
  }
}

void draw() {
  if (myPort.available() > 0) {
    String input = myPort.readStringUntil('\n');
    if (input != null) {
      parseInput(input.trim());
    }
  }
}

void parseInput(String input) {
  if (input.startsWith("M:")) {
    String[] data = input.substring(2).split(":");
    if (data.length == 2) {
      handleMouseMovement(input);
    }
  } 
  else if (input.startsWith("B:")) {
    String[] data = input.substring(2).split(":");
    if (data.length == 2) {
      int button = Integer.parseInt(data[0]);
      String state = data[1];
      handleButton(button, state);
    }
  }
  else if(input.startsWith("SCROLL")) {

  }
}

void handleMouseMovement(String input) {
  String[] data = input.substring(2).split(":");
  int moveX = Integer.parseInt(data[0]);
  int moveY = Integer.parseInt(data[1]);
  robot.mouseMove(mouseX + moveX, mouseY + moveY);
}

void handleButton(int button, String state) {
  int mask = 0;
  switch (button) {
    case 1: mask = InputEvent.BUTTON1_DOWN_MASK; break; // Left click
    case 2: mask = InputEvent.BUTTON3_DOWN_MASK; break; // Right click
    case 3: mask = InputEvent.BUTTON2_DOWN_MASK; break; // Middle click
  }
  if (mask != 0) {
    if (state.equals("DOWN")) {
      robot.mousePress(mask);
    } else if (state.equals("UP")) {
      robot.mouseRelease(mask);
    }
  }
}

void handleButton(String input) {
    if (input.equals("B1_DOWN")) {
        robot.mousePress(InputEvent.BUTTON1_DOWN_MASK);
    }
    else if (input.equals("B1_UP")) {
        robot.mouseRelease(InputEvent.BUTTON1_DOWN_MASK);
    }
    else if (input.equals("B2_DOWN")) {
        robot.mousePress(InputEvent.BUTTON3_DOWN_MASK);
    }
    else if (input.equals("B2_UP")) {
        robot.mouseRelease(InputEvent.BUTTON3_DOWN_MASK);
    }
    else if (input.equals("B1_DOUBLECLICK")) {
      robot.mousePress(InputEvent.BUTTON1_DOWN_MASK);
      robot.mouseRelease(InputEvent.BUTTON1_DOWN_MASK);
      delay(50);
      robot.mousePress(InputEvent.BUTTON1_DOWN_MASK);
      robot.mouseRelease(InputEvent.BUTTON1_DOWN_MASK);
    }

    if (mask != 0) {
        if (state.equals("DOWN")) {
            robot.mousePress(mask);
        }
        else if (state.equals("UP")) {
            robot.mouseRelease(mask);
        }
    }
  }