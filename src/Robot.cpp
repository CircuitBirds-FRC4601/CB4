#include "WPILib.h"
#include "Gamepad.h"
#include "DoubleSolenoid.h"
  class Robot: public IterativeRobot
  {

 private:
 	LiveWindow *lw = LiveWindow::GetInstance();
  	SendableChooser *chooser;
  	bool nitroL, nitroR;  //DrC, for speed boost in tank drive
  	bool pickup_pickup,shooter_spinup, piston_buttonu,piston_buttond,button_led; //DrC
  	const std::string autoNameDefault = "Default";
  	const std::string autoNameCustom = "My Auto";
 	double leftgo,rightgo,speed;  //DrC speed scales joysticks output to drive number
 	double ax, ay,az, bx,by, heading, boffsetx,boffsety, bscaley, bscalex, pd, pi=4.0*atan(1.0), pickup_kickballout, shooterWheel, pickupWheel, shooter_shoot; //FIX
  	std::string autoSelected;

  	DoubleSolenoid *piston = new DoubleSolenoid(0,1);
  	DoubleSolenoid *piston1 = new DoubleSolenoid(2,3);
  	
  	Compressor *howdy= new Compressor(0);//comnpressor does not like the term compress or compressor
  	
  	Joystick *rightDrive = new Joystick(0,2,9);//DrC
  	Joystick *leftDrive  = new Joystick(1,2,9);//DrC
	Joystick *gamePad = new Joystick(2,6,9);//DrC
  	Talon *fRight = new Talon(1); // remaped all talons E and DrC
  	Talon *fLeft = new Talon(0);
  	Talon *bRight = new Talon(2);
  	Talon *bLeft = new Talon(3);
  	Talon *pickup = new Talon(4);// pickup
  	Talon *shooter = new Talon(5);// main shooterwheel

	AnalogInput *Bx = new AnalogInput(0); //DrC  magnetic x component
	AnalogInput *By = new AnalogInput(1); //DrC  magnetic y component
	AnalogInput *Photo = new AnalogInput(2); //DrC  photodiode response
	BuiltInAccelerometer *accel = new BuiltInAccelerometer(); //DrC what it is...what it is...
	DigitalOutput *led1 = new DigitalOutput(1); //DrC triggerline for the structured lightfield

	Encoder *lwheel =new Encoder(0,1);
	Encoder *rwheel = new Encoder(2,3);

  	RobotDrive *robotDrive = new RobotDrive(fLeft, bLeft, fRight, bRight);
  	RobotDrive *pickupShooter = new RobotDrive(pickup, pickup, shooter, shooter); //***
 void AutonomousInit() {
	 howdy->Enabled();
		lwheel->Reset();
		rwheel->Reset();
		piston1->Set(DoubleSolenoid::Value::kOff);
 		if(autoSelected == autoNameCustom){
 			//Custom Auto goes here
 		}
 		 else {
 			//Default Auto goes here
 		}
 	}

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
 		// calibration data  -from DrC
 			    boffsetx = 1.4;
 				boffsety = 1.2;
 				bscalex = 1.0;
 				bscaley = 1.0;
 				lwheel->Reset();
 				rwheel->Reset();
 				howdy->Enabled();
 				piston1->Set(DoubleSolenoid::Value::kOff);
 	}

  	void TeleopPeriodic()
  	{

  		rightgo = rightDrive-> GetRawAxis(1);
 		leftgo  = leftDrive-> GetRawAxis(1);
 		nitroR   = rightDrive-> GetRawButton(3);
 		nitroL   = leftDrive-> GetRawButton(3);
 		button_led = gamePad->GetRawButton(1);

 		speed  = .6;
		rightgo = -(speed+(1.0-speed)*(double)(nitroR))*rightgo;  //DrC for nitro drive
 		leftgo  = -(speed+(1.0-speed)*(double)(nitroL))*leftgo;   //DrC  ''
 		robotDrive->TankDrive(rightgo, leftgo);

 		//Strobey bit
 		if(button_led){

 		led1->Pulse(1);
 		}
 		// Pickupwheel section
 		pickup_kickballout = gamePad -> GetRawAxis(2);//DrC
 		pickup_pickup = gamePad -> GetRawButton(5);//rC
 		lwheel->GetRaw();
 		rwheel->GetRaw();
 		if(abs(pickup_kickballout)>.1){ //DrC
 			pickupWheel = 0.7;
 		}
 		else if(pickup_pickup){
 			pickupWheel = -0.7;
 		}
 		else{
 			pickupWheel=0.0;
 		}

 		// right bumper speed up the wheel, right trigger  to shoot.
 		piston_buttonu   = gamePad-> GetRawButton(3);
 		piston_buttond   = gamePad-> GetRawButton(4);
 		shooter_shoot = gamePad -> GetRawAxis(3);//DrC
 		shooter_spinup = gamePad -> GetRawButton(6);
 		if(abs(shooter_shoot)>.1){ //DrC
 			shooterWheel = 1.0;
 		 }
 		 else {//DrC reset it so can have three modes, forward, backwards and nothing!
 			shooterWheel = 0.0;
 		 }
 		if(piston_buttonu)
 		{
 			piston->Set(DoubleSolenoid::Value::kReverse);
 		}
 		if(piston_buttond)
 		{
 		 			piston->Set(DoubleSolenoid::Value::kForward);
 		}

 		pickupShooter->TankDrive(pickupWheel,shooterWheel); //DrC here run the wheels!
 		//Sensor section Dr C
 		ax = accel-> GetX();//DrC   Sensor Section : get orientation of the robot WRT field co-ordinates.
 		ay = accel-> GetY();//DrC
 		az = accel-> GetZ();//DrC  ax ay az used to define down
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
 		SmartDashboard::PutNumber("pd", pd);
 		SmartDashboard::PutNumber("bx", bx);
 		SmartDashboard::PutNumber("by", by);
 		SmartDashboard::PutData("rwheel", rwheel);
 	 	SmartDashboard::PutData("lwheel", lwheel);
 		//SmartDashboard::PutNumber("rightgo",rightgo); //DrC
 		//SmartDashboard::PutNumber("leftgo",leftgo); //DrC
 		//SmartDashboard::PutNumber("kickballout",pickup_kickballout); //DrC
 		//SmartDashboard::PutNumber("Shooterwheelaxis", shooter_shoot);

  	}
  	void TestPeriodic()
 	{
 		lw->Run();
 	}
  };
 START_ROBOT_CLASS(Robot)
