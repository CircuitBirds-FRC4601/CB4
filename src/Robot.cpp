#include "WPILib.h"


  class Robot: public IterativeRobot
  {
 private:
 	LiveWindow *lw = LiveWindow::GetInstance();
  	SendableChooser *chooser;
  	const std::string autoNameDefault = "Default";
  	const std::string autoNameCustom = "My Auto";
 	double leftgo,rightgo,buttonShooter;
 	double ax, ay,az, bx,by, heading, boffsetx,boffsety, bscaley, bscalex, pd, pi=4.0*atan(1.0); //DrC defines
  	std::string autoSelected;

  	Joystick *rightDrive = new Joystick(0);
  	Joystick *leftDrive  = new Joystick(1);
  	Joystick *gamePad = new Joystick(2,6,10);//E

  	Talon *fRight = new Talon(0);// remaped all talons E
  	Talon *fLeft = new Talon(2);
  	Talon *bRight = new Talon(1);
  	Talon *bLeft = new Talon(3);
  	Talon *pickup = new Talon(4);//e
  	Talon *shooter = new Talon(5);//e

	AnalogInput *Bx = new AnalogInput(0); //DrC  magnetic x component
	AnalogInput *By = new AnalogInput(1); //DrC  magnetic y component
	AnalogInput *Photo = new AnalogInput(2); //DrC  photodiode response
	BuiltInAccelerometer *accel = new BuiltInAccelerometer(); //DrC what it is...what it is...
	DigitalOutput *led1 = new DigitalOutput(1); //DrC triggerline for the structured lightfield


  	RobotDrive *robotDrive = new RobotDrive(fLeft, bLeft, fRight, bRight);
 void AutonomousInit() {
 		if(autoSelected == autoNameCustom){
 			//Custom Auto goes here
 		}
 		 else {
 			//Default Auto goes here
 		}
 	}
//
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
 	}

  	void TeleopPeriodic()
  	{

  		rightgo = rightDrive->GetY();
 		leftgo  = leftDrive->GetY();
 		rightgo = .6*rightgo;
 		leftgo  = .6*leftgo;
 		robotDrive->TankDrive(rightDrive, leftDrive);
 		buttonShooter=gamePad->GetRawAxis(0);//E this would be the right trigger *may not be axis 0
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
 		if(buttonShooter>.5) //E will activate the shooter if above half depression
 		{
 			shooter->Set(1);
 		}
 		else
 		{
 			shooter->Set(0);
 		}
 		SmartDashboard::PutNumber("ax",ax); //DrC
 	    SmartDashboard::PutNumber("ay",ay); //DrC
 		SmartDashboard::PutNumber("az",az); //DrC
 		SmartDashboard::PutNumber("heading", heading); //DrC
 		SmartDashboard::PutNumber("buttonShooter", buttonShooter);


  	}
  	void TestPeriodic()
 	{
 		lw->Run();
 	}
  };
 START_ROBOT_CLASS(Robot)
