  #include "WPILib.h"
class Robot: public IterativeRobot
{
private:
	LiveWindow *lw = LiveWindow::GetInstance();
	SendableChooser *chooser;
	const std::string autoNameDefault = "Default";
	const std::string autoNameCustom = "My Auto";

	std::string autoSelected;

	Timer *timer = new Timer();

	BuiltInAccelerometer *accel = new BuiltInAccelerometer();

	Joystick *rightJoystick = new Joystick(0);
	Joystick *leftJoystick  = new Joystick(1);

	double rightDrive, leftDrive, a_x, a_y, a_z;

	RobotDrive *robotDrive = new RobotDrive(fLeft, bLeft, fRight, bRight);
	Talon *fRight = new Talon(0);
	Talon *fLeft  = new Talon(1);
	Talon *bRight = new Talon(2);
	Talon *bLeft  = new Talon(3);
	Talon *pickup = new Talon(4);

	DigitalInput *limitswitch = new DigitalInput(2);
	DigitalOutput *led1 = new DigitalOutput(1);

	IMAQdxSession session;
	Image *frame;
	IMAQdxError imaqError;

	void RobotInit() override{
	{

		frame = imaqCreateImage(IMAQ_IMAGE_RGB, 0);
		imaqError = IMAQdxOpenCamera("cam0", IMAQdxCameraControlModeController, &session);
				if(imaqError != IMAQdxErrorSuccess) {
					DriverStation::ReportError("IMAQdxOpenCamera error: " + std::to_string((long)imaqError) + "\n");
							}
	}
		chooser = new SendableChooser();
		chooser->AddDefault(autoNameDefault, (void*)&autoNameDefault);
		chooser->AddObject(autoNameCustom, (void*)&autoNameCustom);
	}

	void OperatorControl(){

			IMAQdxStartAcquisition(session);

			while(IsOperatorControl() && IsEnabled()) {
				IMAQdxGrab(session, frame, true, NULL);
				if(imaqError != IMAQdxErrorSuccess) {
					DriverStation::ReportError("IMAQdxGrab error: " + std::to_string((long)imaqError) + "\n");
				} else {
					imaqDrawShapeOnImage(frame, frame, { 10, 10, 100, 100 }, DrawMode::IMAQ_DRAW_VALUE, ShapeMode::IMAQ_SHAPE_OVAL, 0.0f);
					CameraServer::GetInstance()->SetImage(frame);
				}
				Wait(0.005);
			}

			IMAQdxStopAcquisition(session);
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
	void AutonomousPeriodic()
	{
	}
	void TeleopInit()
	{
	}
	void TeleopPeriodic()
	{
		bool triggerRight = rightJoystick->GetRawButton(1);
		bool triggerLeft = leftJoystick->GetRawButton(1);
		bool buttonLed = rightJoystick->GetRawButton(2);
		bool buttonLed2 = leftJoystick->GetRawButton(2);

		rightDrive = rightJoystick->GetY();
		leftDrive  = leftJoystick->GetY();
		rightDrive = .6*rightDrive;
		leftDrive  = .6*leftDrive;
		robotDrive->TankDrive(rightDrive, leftDrive);

		a_x = accel-> GetX();
		a_y = accel-> GetY();
		a_z = accel-> GetZ();
		SmartDashboard::PutNumber("a_x",a_x);
		SmartDashboard::PutNumber("a_y",a_y);
		SmartDashboard::PutNumber("a_z",a_z);

		SmartDashboard::PutData("Auto Modes", chooser);

		SmartDashboard::PutBoolean("trigger", triggerRight);
		SmartDashboard::PutBoolean("trigger", triggerLeft);
		SmartDashboard::PutBoolean("Led", triggerLeft);
		SmartDashboard::PutBoolean("Led", triggerRight);


		if(triggerRight){
			pickup->Set(1);
		}
		else{
			pickup->Set(0);
		}


		if(triggerLeft){
			pickup->Set(-1);
		}
		else{
		}

	//TODO- GET A GOOD GOD-DAMN LED!!!!!
		if(buttonLed || buttonLed2){
			led1->Pulse(1);
		}
	}


	void TestPeriodic()
	{
		lw->Run();
	}
};
START_ROBOT_CLASS(Robot)
