using namespace vex;

extern brain Brain;
extern controller Controller;

extern motor motor_l1;
extern motor motor_l2;
extern motor motor_l3;
extern motor motor_r1;
extern motor motor_r2;
extern motor motor_r3;

extern motor_group leftDrive;
extern motor_group rightDrive;

extern motor kicker;
extern motor intake;

extern digital_out wings;

// FOR BRAIN TESTING, USING TEMPORARY DEVICES

extern rotation wheel;
extern limit limitSwitch;

extern inertial imu; // placeholder port value

/**
 * Used to initialize code/tasks/devices added using tools in VEXcode Pro.
 * 
 * This should be called at the start of your int main function.
 */
void  vexcodeInit( void );