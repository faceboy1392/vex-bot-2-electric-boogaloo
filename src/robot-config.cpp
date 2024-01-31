#include "vex.h"

using namespace vex;
using signature = vision::signature;
using code = vision::code;

brain Brain;
controller Controller(primary);

motor motor_l1(PORT17, ratio6_1, true);
motor motor_l2(PORT20, ratio6_1, true);
motor motor_l3(PORT18, ratio6_1, false);
motor motor_r1(PORT13, ratio6_1, false);
motor motor_r2(PORT11, ratio6_1, false);
motor motor_r3(PORT12, ratio6_1, true);

motor_group leftDrive(motor_l1, motor_l2, motor_l3);
motor_group rightDrive(motor_r1, motor_r2, motor_r3);

motor kicker(PORT14);
motor intake(PORT21);

digital_out wings(Brain.ThreeWirePort.A);

inertial imu(PORT16);
