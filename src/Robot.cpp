#include "WPILib.h"
class Robot: public IterativeRobot
{
private:
	LiveWindow *lw = LiveWindow::GetInstance();
	SendableChooser *chooser;
	const std::string autoNameDefault = "Default";
	const std::string autoNameCustom = "My Auto";
	double rightDrive, leftDrive, ax, ay, az, lift;
	std::string autoSelected;
	BuiltInAccelerometer *accel = new BuiltInAccelerometer();
	Joystick *rightStick = new Joystick(0);
	Joystick *leftStick  = new Joystick(1);
	Talon *fRight = new Talon(0);
	Talon *fLeft = new Talon(1);
	Talon *bRight = new Talon(2);
	Talon *bLeft = new Talon(3);
	Talon *pickup = new Talon(4);
	RobotDrive *robotDrive = new RobotDrive(fLeft, bLeft, fRight, bRight);





	void RobotInit()
	{
		chooser = new SendableChooser();
		chooser->AddDefault(autoNameDefault, (void*)&autoNameDefault);
		chooser->AddObject(autoNameCustom, (void*)&autoNameCustom);
	}


	/**
	 * This autonomous (along with the chooser code above) shows how to select between different autonomous modes
	 * using the dashboard. The sendable chooser code works with the Java SmartDashboard. If you prefer the LabVIEW
	 * Dashboard, remove all of the chooser code and uncomment the GetString line to get the auto name from the text box
	 * below the Gyro
	 *
	 * You can add additional auto modes by adding additional comparisons to the if-else structure below with additional strings.
	 * If using the SendableChooser make sure to add them to the chooser code above as well.
	 */
	void AutonomousInit()
	{
		autoSelected = *((std::string*)chooser->GetSelected());
		//std::string autoSelected = SmartDashboard::GetString("Auto Selector", autoNameDefault);
		std::cout << "Auto selected: " << autoSelected << std::endl;

		if(autoSelected == autoNameCustom){
			//Custom Auto goes here
		} else {
			//Default Auto goes here
		}
	}
	//rightgo
	void AutonomousPeriodic()
	{
		if(autoSelected == autoNameCustom){
			//Custom Auto goes here
		} else {
			//Default Auto goes here
		}
	}

	void TeleopInit()
	{

	}

	void TeleopPeriodic()
	{

		rightDrive = rightStick->GetY();
		leftDrive  = leftStick->GetY();
		rightDrive = .6*rightDrive;
		leftDrive  = .6*leftDrive;
		robotDrive->TankDrive(rightDrive, leftDrive);
		ax = accel-> GetX();
		ay = accel-> GetY();
		az = accel-> GetZ();
		SmartDashboard::PutData("Auto Modes", chooser);
		SmartDashboard::PutNumber("ax",ax);
		SmartDashboard::PutNumber("ay",ay);
		SmartDashboard::PutNumber("az",az);
		bool triggerRight = rightStick->GetRawButton(1);
		bool triggerLeft = leftStick->GetRawButton(1);
		SmartDashboard::PutBoolean("trigger", triggerRight);
		SmartDashboard::PutBoolean("trigger", triggerLeft);
		if(triggerRight || triggerLeft){
			pickup->Set(.3);
		}
		else{
			pickup->Set(0);
		}
	}
	void TestPeriodic()
	{
		lw->Run();
	}
};

START_ROBOT_CLASS(Robot)
