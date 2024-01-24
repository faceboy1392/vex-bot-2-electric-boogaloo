#include "vex.h"

using namespace vex;
competition Competition;

#include <math.h>
#include <utility>
#include <string>
#include <algorithm>

#include "robot-config.h"
#include "Graph.h"

using std::abs;
using std::make_pair;
using std::max;
using std::min;
using std::pair;

template <typename T>
T clamp(T value, T minValue, T maxValue)
{
  return max(minValue, min(maxValue, value));
}
/**
 * @brief min and max are inclusive
 */
template <typename T>
bool threshold(T value, T minValue, T maxValue)
{
  return value >= minValue && value <= maxValue;
}

Drive chassis(
    // Specify your drive setup below. There are seven options:
    // ZERO_TRACKER_NO_ODOM, ZERO_TRACKER_ODOM, TANK_ONE_ENCODER, TANK_ONE_ROTATION, TANK_TWO_ENCODER, TANK_TWO_ROTATION, HOLONOMIC_TWO_ENCODER, and HOLONOMIC_TWO_ROTATION
    // For example, if you are not using odometry, put ZERO_TRACKER_NO_ODOM below:
    ZERO_TRACKER_NO_ODOM,

    // Add the names of your Drive motors into the motor groups below, separated by commas, i.e. motor_group(Motor1,Motor2,Motor3).
    // You will input whatever motor names you chose when you configured your robot using the sidebar configurer, they don't have to be "Motor1" and "Motor2".

    // Left Motors:
    motor_group(motor_l1, motor_l2, motor_l3),

    // Right Motors:
    motor_group(motor_r1, motor_r2, motor_r3),

    // Specify the PORT NUMBER of your inertial sensor, in PORT format (i.e. "PORT1", not simply "1"):
    PORT5,

    // Input your wheel diameter. (4" omnis are actually closer to 4.125"):
    3.25,

    // External ratio, must be in decimal, in the format of input teeth/output teeth.
    // If your motor has an 84-tooth gear and your wheel has a 60-tooth gear, this value will be 1.4.
    // If the motor drives the wheel directly, this value is 1:
    0.6,

    // Gyro scale, this is what your gyro reads when you spin the robot 360 degrees.
    // For most cases 360 will do fine here, but this scale factor can be very helpful when precision is necessary.
    360,

    /*---------------------------------------------------------------------------*/
    /*                                  PAUSE!                                   */
    /*                                                                           */
    /*  The rest of the drive constructor is for robots using POSITION TRACKING. */
    /*  If you are not using position tracking, leave the rest of the values as  */
    /*  they are.                                                                */
    /*---------------------------------------------------------------------------*/

    // If you are using ZERO_TRACKER_ODOM, you ONLY need to adjust the FORWARD TRACKER CENTER DISTANCE.

    // FOR HOLONOMIC DRIVES ONLY: Input your drive motors by position. This is only necessary for holonomic drives, otherwise this section can be left alone.
    // LF:      //RF:
    PORT1, -PORT2,

    // LB:      //RB:
    PORT3, -PORT4,

    // If you are using position tracking, this is the Forward Tracker port (the tracker which runs parallel to the direction of the chassis).
    // If this is a rotation sensor, enter it in "PORT1" format, inputting the port below.
    // If this is an encoder, enter the port as an integer. Triport A will be a "1", Triport B will be a "2", etc.
    3,

    // Input the Forward Tracker diameter (reverse it to make the direction switch):
    2.75,

    // Input Forward Tracker center distance (a positive distance corresponds to a tracker on the right side of the robot, negative is left.)
    // For a zero tracker tank drive with odom, put the positive distance from the center of the robot to the right side of the drive.
    // This distance is in inches:
    1,
    // Input the Sideways Tracker Port, following the same steps as the Forward Tracker Port:
    2,

    // Sideways tracker diameter (reverse to make the direction switch):
    -2.75,

    // Sideways tracker center distance (positive distance is behind the center of the robot, negative is in front):
    5.5);

void theFunctionThatDisplaysStuffOnTheScreen()
{
  auto s = Brain.Screen;
  s.render();

  Graph batteryGraph(make_pair(0, 105), s.getStringWidth("hhhhhhhhhhhh"), 50, make_pair(0, 180), make_pair(0, 1.1));
  batteryGraph.addVariable("lvl", color().yellow);
  batteryGraph.drawGraph();

  int i = 1;
  const int desiredFPS = 10;
  const double ms = 1000.0 / (double)desiredFPS;
  int prev = 0;
  int current = Brain.timer(msec);
  int delta;

  while (1)
  {
    current = Brain.timer(msec);
    delta = current - prev;
    if ((ms - delta) > 0)
      wait(ms - delta, msec);
    prev = Brain.timer(msec);

    s.setPenColor(color().white);
    s.setFont(fontType::mono20);
    s.setCursor(1, 1);
    s.print("Battery");
    s.setCursor(2, 1);
    s.print("lvl  - ");
    s.print(Brain.Battery.capacity());
    s.setCursor(3, 1);
    s.print("volt - ");
    s.print(Brain.Battery.voltage());
    s.setCursor(4, 1);
    s.print("amps - ");
    s.print(Brain.Battery.current());
    s.setCursor(5, 1);
    s.print("temp - ");
    s.print(Brain.Battery.temperature());

    batteryGraph.addSegmentToVariable("lvl", Point(Brain.timer(seconds), Brain.Battery.capacity() / 100.0));
    if (Brain.timer(seconds) > 180)
      batteryGraph.resize(make_pair(0 + max(0.0, Brain.timer(seconds) - 1000.0), Brain.timer(seconds)), make_pair(0, 1.1));

    s.setFont(fontType::mono15);
    s.setPenColor(color().white);

    s.clearLine(12);
    s.clearLine(14);
    s.clearLine(15);

    s.setCursor(12, 1);
    if (!motor_l1.installed())
      s.setFillColor(color().red);
    else if (vexDeviceMotorOverTempFlagGet(vexDeviceGetByIndex(motor_l1.index())))
      s.setFillColor(color().orange);
    s.print("m1:");
    s.setFillColor(color().transparent);
    s.print(motor_l1.voltage());

    s.setCursor(14, 1);
    if (!motor_l2.installed())
      s.setFillColor(color().red);
    else if(vexDeviceMotorOverTempFlagGet(vexDeviceGetByIndex(motor_l2.index())))
      s.setFillColor(color().orange);
    s.print("m2:");
    s.setFillColor(color().transparent);
    s.print(motor_l2.voltage());

    s.setCursor(15, 1);
    if (!motor_l3.installed())
      s.setFillColor(color().red);
    else if (vexDeviceMotorOverTempFlagGet(vexDeviceGetByIndex(motor_l3.index())))
      s.setFillColor(color().orange);
    s.print("m3:");
    s.setFillColor(color().transparent);
    s.print(motor_l3.voltage());

    s.setCursor(12, 12);
    if (!motor_r1.installed())
      s.setFillColor(color().red);
    else if (vexDeviceMotorOverTempFlagGet(vexDeviceGetByIndex(motor_r1.index())))
      s.setFillColor(color().orange);
    s.print("m4:");
    s.setFillColor(color().transparent);
    s.print(motor_r1.voltage());

    s.setCursor(14, 12);
    if (!motor_r2.installed())
      s.setFillColor(color().red);
    else if (vexDeviceMotorOverTempFlagGet(vexDeviceGetByIndex(motor_r2.index())))
      s.setFillColor(color().orange);
    s.print("m5:");
    s.setFillColor(color().transparent);
    s.print(motor_r2.voltage());

    s.setCursor(15, 12);
    if (!motor_r3.installed())
      s.setFillColor(color().red);
    else if (vexDeviceMotorOverTempFlagGet(vexDeviceGetByIndex(motor_r3.index())))
      s.setFillColor(color().orange);
    s.print("m6:");
    s.setFillColor(color().transparent);
    s.print(motor_r3.voltage());

    char *str = new char[20];
    sprintf(str, "sbob/out%i.png", i);
    bool result = Brain.Screen.drawImageFromFile(str, 160, 0);
    if (!result)
      break;
    i += 2;

    s.render();
    // wait(200, msec);
  }
}

void sbob()
{
  if (Brain.SDcard.isInserted())
  {

    int i = 1;

    const int desiredFPS = 10;

    const double ms = 1000.0 / (double)desiredFPS;
    int prev = 0;
    int current = Brain.timer(msec);
    int delta;
    while (1)
    {
      current = Brain.timer(msec);
      delta = current - prev;
      if ((ms - delta) > 0)
        wait(ms - delta, msec);
      prev = Brain.timer(msec);
      char *str = new char[20];
      sprintf(str, "sbob/out%i.png", i);
      bool result = Brain.Screen.drawImageFromFile(str, 160, 0);
      Brain.Screen.render();
      if (!result)
        break;
      i += 2;    
    }
  }
}

enum AutonMode
{
  RED_OFFENSIVE,
  RED_DEFENSIVE,
  BLUE_DEFENSIVE,
  BLUE_OFFENSIVE,
  AUTON_SKILLS,
  DRIVER_SKILLS,
  NONE
};

AutonMode selectedAuton;

void autonSelector()
{
  auto s = Brain.Screen;
  // draw field to the screen
  int ox = 64; // offset x
  s.setPenWidth(1);
  s.setPenColor(color().black);
  // gray background
  s.setFillColor(color(40, 40, 40));
  s.drawRectangle(ox - 1, -1, 242, 242);
  s.setPenColor(color().white);
  s.setPenWidth(2);
  // white tape
  s.drawLine(ox + 3, 118, ox + 237, 118);
  s.drawLine(ox + 3, 122, ox + 237, 122);
  s.drawLine(ox + 3, 140, ox + 120, 140);
  s.drawLine(ox + 120, 100, ox + 237, 100);
  s.setPenColor(color().black);
  s.setPenWidth(1);
  // blue goal
  s.setFillColor(color().blue);
  s.drawRectangle(ox, 80, 40, 80);
  // red goal
  s.setFillColor(color().red);
  s.drawRectangle(ox + 200, 80, 40, 80);
  s.setPenColor(color().black);
  s.setPenWidth(5);
  // barrier
  s.drawLine(ox + 80, 40, ox + 160, 40);
  s.drawLine(ox + 120, 40, ox + 120, 200);
  s.drawLine(ox + 80, 200, ox + 160, 200);
  // red elevation and match load bars
  s.setPenColor(color().red);
  s.drawLine(ox + 5, 35, ox + 35, 5);
  s.drawLine(ox + 5, 205, ox + 35, 235);
  s.drawLine(ox + 120, 200, ox + 120, 240);
  // blue elevation and match load bars
  s.setPenColor(color().blue);
  s.drawLine(ox + 235, 35, ox + 205, 5);
  s.drawLine(ox + 235, 205, ox + 205, 235);
  s.drawLine(ox + 120, 0, ox + 120, 40);

  // other options
  s.setPenColor(color().white);
  s.setPenWidth(2);
  s.drawLine(320, 0, 320, 240);
  s.drawLine(320, 80, 480, 80);
  s.drawLine(320, 160, 480, 160);

  s.setPenColor(color().white);
  s.setFillColor(color().transparent);
  s.printAt(330, 40 + s.getStringHeight("Skills Auton") / 2.0, "Skills Auton");
  s.printAt(330, 120 + s.getStringHeight("Driver Auton") / 2.0, "Driver Auton");
  s.printAt(330, 200 + s.getStringHeight("No Auton") / 2.0, "No Auton");

  // get input
  bool chosen = false;
  while (!chosen)
  {
    while (!s.pressing())
      ;
    while (s.pressing())
      ;
    int xPos = s.xPosition(), yPos = s.yPosition();

    chosen = true;
    if (threshold(xPos, ox, ox + 120) && threshold(yPos, 0, 120))
    {
      // top left
      selectedAuton = BLUE_OFFENSIVE;
    }
    else if (threshold(xPos, ox + 120, ox + 240) && threshold(yPos, 0, 120))
    {
      // top right
      selectedAuton = BLUE_DEFENSIVE;
    }
    else if (threshold(xPos, ox, ox + 120) && threshold(yPos, 120, 240))
    {
      // bottom left
      selectedAuton = RED_DEFENSIVE;
    }
    else if (threshold(xPos, ox + 120, ox + 240) && threshold(yPos, 120, 240))
    {
      // bottom right
      selectedAuton = RED_OFFENSIVE;
    }
    else if (threshold(xPos, 320, 480) && threshold(yPos, 0, 80))
    {
      selectedAuton = AUTON_SKILLS;
    }
    else if (threshold(xPos, 320, 480) && threshold(yPos, 80, 160))
    {
      selectedAuton = DRIVER_SKILLS;
    }
    else if (threshold(xPos, 320, 480) && threshold(yPos, 160, 240))
    {
      selectedAuton = NONE;
    }
    else
      chosen = false;
  }

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wwritable-strings"
  char *text;
  switch (selectedAuton)
  {
  case BLUE_OFFENSIVE:
    text = "Blue OFF Zone";
    s.setFillColor(color().blue);
    break;
  case BLUE_DEFENSIVE:
    text = "Blue DEF Zone";
    s.setFillColor(color().blue);
    break;
  case RED_DEFENSIVE:
    text = "RED DEF Zone";
    s.setFillColor(color().red);
    break;
  case RED_OFFENSIVE:
    text = "RED OFF Zone";
    s.setFillColor(color().red);
    break;
  case AUTON_SKILLS:
    text = "Auton Skills";
    break;
  case DRIVER_SKILLS:
    text = "Driver Skills";
    break;
  case NONE:
    text = "No Auton";
    break;
  }
#pragma GCC diagnostic pop
  s.clearScreen();
  s.setFont(fontType::mono60);
  s.printAt(240 - s.getStringWidth(text) / 2.0, 120 + s.getStringHeight(text) / 2.0, text);
}

void pre_auton(void)
{
  // Initializing Robot Configuration. DO NOT REMOVE!
  default_constants();

  imu.startCalibration();
  wait(500, msec);
  waitUntil(!imu.isCalibrating());
  wait(500, msec);

  leftDrive.setStopping(coast);
  rightDrive.setStopping(coast);

  autonSelector();

  // thread(sbob).detach();
  thread(theFunctionThatDisplaysStuffOnTheScreen).detach();
}

void autonomous(void)
{
}

void usercontrol(void)
{
  // Controller.ButtonX.pressed([]() { wings.set(!wings.value()); });
  while (1)
  {
    chassis.control_arcade();

    wait(20, msec);
  }
}

int main()
{
  // Set up callbacks for autonomous and driver control periods.
  Competition.autonomous(autonomous);
  Competition.drivercontrol(usercontrol);

  // Run the pre-autonomous function.
  pre_auton();

  // Prevent main from exiting with an infinite loop.
  while (true)
  {
    wait(100, msec);
  }
}
