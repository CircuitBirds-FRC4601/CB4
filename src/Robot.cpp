  #include "WPILib.h"
  #include "Gamepad.h"
/*
    Circuit Birds 4 Robot Code

  CURRENT STATUS: failures at run time associated with joystick interrupt handling. (Dr. C., 1/25/2016)
  TODO:
     fix
     test shooter code
     magnetometer calibration/testing
     Phase sensitive detection code.
     The list goes on and on....
*/
class Robot: public IterativeRobot
{
private:
	LiveWindow *lw = LiveWindow::GetInstance();
	SendableChooser *chooser;
	const std::string autoNameDefault = "Default";
	const std::string autoNameCustom = "My Auto";

	std::string autoSelected;
	Timer *timer = new Timer();
	Joystick *rightJoystick = new Joystick(0);
	Joystick *leftJoystick  = new Joystick(1);
	Joystick *xbox = new Joystick(3);
	double rightDrive, leftDrive, a_x, a_y, a_z;
	double ax, ay, az, bx,by, pd, boffsetx, boffsety, bscalex, bscaley, heading, pi=4.0*atan(1.0) ; //DrC defines
	RobotDrive *robotDrive = new RobotDrive(fLeft, bLeft, fRight, bRight);
	Talon *fRight = new Talon(0);
	Talon *fLeft  = new Talon(1);
	Talon *bRight = new Talon(2);
	Talon *bLeft  = new Talon(3);
	Talon *pickup = new Talon(4);
	Talon *shooter = new Talon(5);

	DigitalInput *limitswitch = new DigitalInput(2);
	DigitalOutput *led1 = new DigitalOutput(1);
	AnalogInput *Bx = new AnalogInput(0); //DrC  magnetic x component
        AnalogInput *By = new AnalogInput(1); //DrC  magnetic y component
        AnalogInput *Photo = new AnalogInput(2); //DrC  photodiode response
        BuiltInAccelerometer *accel = new BuiltInAccelerometer(); //DrC what it is...what it is...

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
		// calibration data  -from DrC
		boffsetx = 1.4;
		boffsety = 1.2;
		bscalex = 1.0;
		bscaley = 1.0;
	}
	void TeleopPeriodic()
	{
		bool triggerRight = rightJoystick->GetRawButton(1);
		bool triggerLeft = leftJoystick->GetRawButton(1);
		bool buttonLed = rightJoystick->GetRawButton(2);
		bool buttonLed2 = leftJoystick->GetRawButton(2);
		bool buttonshooter = xbox->GetRawAxis(6);
		rightDrive = rightJoystick->GetY();
		leftDrive  = leftJoystick->GetY();
		rightDrive = .6*rightDrive;
		leftDrive  = .6*leftDrive;
		robotDrive->TankDrive(rightDrive, leftDrive);

		ax = accel-> GetX();//DrC   Sensor Section : get orientation of the robot WRT field co-ordinates.
		ay = accel-> GetY();//DrC
		az = accel-> GetZ();//DrC   ax ay az used to define down
		bx = Bx -> GetVoltage();//DrC   Raw Readings
		by = By -> GetVoltage(); //DrC
        	pd = Photo -> GetVoltage(); //DrC
			// here flatten response and assume that robot is level.
		bx = (bx-boffsetx)*bscalex; //DrC
		by = (by-boffsety)*bscaley; //DrC
		heading = 180*atan(by/bx)/pi; //DrC
		SmartDashboard::PutNumber("ax",ax); //DrC
		SmartDashboard::PutNumber("ay",ay); //DrC
		SmartDashboard::PutNumber("az",az); //DrC
		SmartDashboard::PutNumber("heading", heading); //DrC
		SmartDashboard::PutData("Auto Modes", chooser);
		SmartDashboard::PutBoolean("trigger", triggerRight);
		SmartDashboard::PutBoolean("trigger", triggerLeft);
		SmartDashboard::PutBoolean("Led", triggerLeft);
		SmartDashboard::PutBoolean("Led", triggerRight);
		SmartDashboard::PutBoolean("buttonShooter", buttonshooter);

		if(triggerRight){
			pickup->Set(1);
		}
		else{
			pickup->Set(0);
		}

//
		if(triggerLeft){
			pickup->Set(-1);
		}
		else{
		}
		if(buttonshooter){
			shooter->Set(1);
		}
		else{
			shooter->Set(0);
		}
	// Flashing the LED
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
