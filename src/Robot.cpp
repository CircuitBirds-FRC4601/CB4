#include "WPILib.h"
//#include "Gamepad.h"  //DrC appears that you may not need this include as well...automatic?
#include "Cameraserver.h"
//#include <stdio.h>   //DrC may not need this one. We may want to use it if we attemp fileI/O with the code.
#include <unistd.h>  //DrC , needed just for the sleep() function...warning sleep() may not be threadsafe!
/*      CB4 Robot Code, team 4601 (Canfield Ohio,the Circuit Birds)
 *
 *
 */
  class Robot: public IterativeRobot
  {

 private:

 	LiveWindow *lw = LiveWindow::GetInstance();
  	SendableChooser *chooser;
  	bool nitroL, nitroR;  //DrC, for speed boost in tank drive
  	bool pickup_pickup,shooter_spinup, piston_button,ramp_act,button_led, speedgood; //DrC
  	int i, samples;
  	const std::string autoNameDefault = "Default";
  	const std::string autoNameCustom = "My Auto";
 	double leftgo,rightgo,speed,quality;  //DrC speed scales joysticks output to drive number
 	double ax, ay,az, bx,by, heading, boffsetx,boffsety, bscaley, bscalex, baveraging, bx_avg, by_avg, pd, strobe_on, strobe_off, reflectedLight, pi=4.0*atan(1.0), pickup_kickballout, shooterWheel, swheelspeed, shotspeed, savg, starget, swindow, pickupWheel, shooter_shoot; //FIX
 	double Auto1_F;//E Auto code variables
 	int auto_server,r_enc,l_enc;
 	std::string autoSelected;

  	DoubleSolenoid *piston = new DoubleSolenoid(0,1);
  	DoubleSolenoid *piston1 = new DoubleSolenoid(2,3);
  	Compressor *howdy= new Compressor(0);//comnpressor does not like the term compress or compressor

  	USBCamera *elmo =new USBCamera("cam3",1);
  	//USBCamera *cam2 =new USBCamera("cam2",1);

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

	DigitalOutput *led1 = new DigitalOutput(4); //DrC triggerline for the structured lightfield
	DigitalOutput *leds1 = new DigitalOutput(9);//status 1
	DigitalOutput *leds2 = new DigitalOutput(8);//status 2

	Encoder *shooterwheel =new Encoder(0,1);
	Encoder *rwheel = new Encoder(2,3);
	Encoder *lwheel = new Encoder(4,5);

	SmartDashboard *auto_selector =new SmartDashboard();// E Simplicity for me to easy call functions

	DriverStation::Alliance Team;// I have no Idea why but I seem only able to get the variable to work this way

  	RobotDrive *robotDrive = new RobotDrive(fLeft, bLeft, fRight, bRight);
  	RobotDrive *pickupShooter = new RobotDrive(pickup, pickup, shooter, shooter); //***

  	void RobotInt(){//E I think were going to need this for when were just an idling robot for some first run code.
		auto_selector->PutNumber("How Shall I Drive Myself Sire?",0); // Adds a Auto code selector on boot up 1-2 no change defaults to 0
		//AFTER AUTO HAS STARTED DO NOT CHANGE THE VALUE
Team = DriverStation::GetInstance().GetAlliance();//This is if we need to switch the magnatometer around and stuff
if(Team==(DriverStation::Alliance::kBlue))//E
{
	SmartDashboard::PutString("Team","Blue!");
	//mag=normal
}
else if(Team==(DriverStation::Alliance::kRed)){
	SmartDashboard::PutString("Team","Red!");
//switch the magnatometer value
}
else{
	SmartDashboard::PutString("Team","NONE?");
}
  	}
 void AutonomousInit() {

		shooterwheel->Reset();
		rwheel->Reset();
		lwheel->Reset();

	    howdy->Enabled();
		piston1->Set(DoubleSolenoid::Value::kOff);
		piston->Set(DoubleSolenoid::Value::kOff);

		Auto1_F=50;//50 rotations

		auto_server = auto_selector->GetNumber("How Shall I Drive Sire?",0);
		if(autoSelected == autoNameCustom){ //E im not sure how these work any ideas any one? Should I just delete?
 			//Custom Auto goes here
 		}
 		 else {
 			//Default Auto goes here
 		}
 	}

 	void AutonomousPeriodic() //Welcome to the Realm of Hearsay. OH! and Elijah.
 	{

 		r_enc = abs(rwheel->GetRaw()); //E have to convert regular encoders into ints for auto
 		l_enc = abs(lwheel->GetRaw());//E the abs is absolute value so i don't have to make anything negative

 		if(auto_server==0){// E I wrote my own Auto Selectors this in theory should let us change auto.
 		auto_server = auto_selector->GetNumber("How Shall I Drive Sire?",1);//If no auto code is specified defaults to auto 1
 		}
 		else if(auto_server==1)//Auto code 1 For straight in front of low bar
 		{//E Hey Doc C can we get the magtometer in this as well I was thinking about doing a launch as well I don't know how to use it.
 		if((r_enc/360<=Auto1_F) & (l_enc/360<=Auto1_F)/*Maghere straight*/ ){//so if rwheel encoder is <= rotations and wheel endoder is <= rotations DRIVE MAN DRIVE
 			rightgo=.7;
 			leftgo=.7;
 			robotDrive->TankDrive(rightgo, leftgo);
 		}

 		else{// STOP MAN!!! STOP DRIVING
 			rightgo=0;
 		 	leftgo=0;
 		 	robotDrive->TankDrive(rightgo, leftgo);
 		}

 			}//END AUTOCODE 1

 		else if(auto_server==2) //Auto code 2
 		{
 			rightgo=0;
 			leftgo=0;

 			SmartDashboard::PutString("What's new?","MOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO and stuffffffffff!");
 			SmartDashboard::PutString("AKA","Auto 2 AINT DONE YET");


 		}//END AUTO 2

 		else{ //Auto 3 Do we really need that many codes? I think by this part if we need this we probably in the land of Hearsay

 			SmartDashboard::PutString("HEY!!!","You know it is only 1-2 right?");
 			rightgo=0;
 			leftgo=0;
 			leds1->Set(1);


 		}//END AUTO 3
 	}//END AUTO

 	void TeleopInit()
 	{
 		CameraServer::GetInstance()->SetQuality(50);
 		//cam2-> SetExposureAuto();
		CameraServer::GetInstance()->StartAutomaticCapture("elmo");
 				//the camera name (ex "cam0") can be found through the roborio web interface
 		//CameraServer::GetInstance()->StartAutomaticCapture("cam2");

		elmo->OpenCamera();
 		elmo->SetFPS(30);
 		elmo-> SetExposureAuto();
 		elmo->SetBrightness(40);
 		elmo->SetWhiteBalanceAuto();

 		// calibration data  -from DrC
 			    boffsetx = 1.4;
 				boffsety = 1.2;
 				bscalex = 1.0;
 				bscaley = 1.0;
 				baveraging = 0.02;  //running averaging in B field determination
 				bx_avg=0.0;
 				by_avg=0.0;
 				samples = 20; //Dr.C.  for the number of stobes cycles used during phase sensitive detection.
 				savg = .3; //Dr.C. for averaging the noise out of the shot wheel speed encoder.
 				shotspeed = 0.0;
 				starget = .7; //DrC target speed for the shooterwheels encoder output
 				swindow = .1; // window (percent) of starget to be good to fire ball! here .1 = 10percent
 				speed  = .6; //driving speed for finer control
 				shooterwheel->Reset();
 				rwheel->Reset();
 				howdy->Enabled();
 				piston1->Set(DoubleSolenoid::Value::kOff);
 				piston->Set(DoubleSolenoid::Value::kOff);
 				ramp_act=0;
 	}

  	void TeleopPeriodic()
  	{
quality=CameraServer::GetInstance()->GetQuality();
  		rightgo = rightDrive-> GetRawAxis(1);
 		leftgo  = leftDrive-> GetRawAxis(1);
 		nitroR   = rightDrive-> GetRawButton(3);
 		nitroL   = leftDrive-> GetRawButton(3);
 		button_led = gamePad->GetRawButton(1);

 		rightgo = -(speed+(1.0-speed)*(double)(nitroR))*rightgo;  //DrC for nitro drive
 		leftgo  = -(speed+(1.0-speed)*(double)(nitroL))*leftgo;   //DrC  ''
 		robotDrive->TankDrive(rightgo, leftgo);
 				if(nitroR&&nitroL){
 		 			leds1->Set(.5);//red overpowers
 		 			leds2->Set(1);//blue
 		 		}
 		 		else if(nitroR){
 		 			leds1->Set(1);
 		 		}
 		 		else if(nitroL){
 		 			leds2->Set(1);//blue
 		 		}
 		 		else{
 		 			leds2->Set(0);
 		 			leds1->Set(0);
 		 		}
 		//Strobey bit section: Phase sensitive detection section. -Dr. C.
 		if(button_led){    // mapped by Christian
 		reflectedLight = 0.0;  //DrC, the phase sensitive detection signal goes in this variable
 		  for(i=1;i<samples;i++){   //DrC basic flash sequence
 			 led1 -> Set(1);
 			 sleep(1);   //DrC, delay meant to let LED output stabilize a bit.
 			 strobe_on = Photo -> GetVoltage(); //DrC
 			 led1 -> Set(0);
 			 sleep(1);
 			 strobe_off = Photo -> GetVoltage(); //DrC
 			 reflectedLight = reflectedLight+strobe_on-strobe_off; //DrC summative
 		  }
 		}
 		// end of Strobey bit
 		// Pickupwheel section
 		pickup_kickballout = gamePad -> GetRawAxis(2);//DrC
 		pickup_pickup = gamePad -> GetRawButton(5);//DrC
 		swheelspeed = shooterwheel->GetRate(); //DrC , gets the signed speed of the shaft. Verified operation with the AMT103-V capacitive sensors, but probably works fine with the optical encoder.
 		shotspeed = shotspeed*(1.0-savg)+savg*swheelspeed;
 		if (abs(shotspeed-starget)/starget<swindow){
 			speedgood=TRUE;
 		}
 		else{
 			speedgood=FALSE;
 		}
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
 		piston_button  = rightDrive-> GetRawButton(1);//E
 		shooter_shoot = gamePad -> GetRawAxis(3);//DrC
 		shooter_spinup = gamePad -> GetRawButton(6);
 		if((abs(shooter_shoot)>.1)&&(speedgood)){ //DrC, (bool)speedgood indicates at within window around target speed.
 			shooterWheel = -1.0;
 		 }
 		 else {//DrC reset it so can have three modes, forward, backwards and nothing!
 			shooterWheel = 0.0;
 		 }
 		if(piston_button)
 		{
 			ramp_act= not ramp_act;
 		}
 		if(ramp_act)
 		{
 		 			piston->Set(DoubleSolenoid::Value::kForward);
 		}
 		else{
 			piston->Set(DoubleSolenoid::Value::kReverse);
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
 		bx_avg = bx_avg*(1.0-baveraging)+bx*baveraging; //Dr. C
 		by = (by-boffsety)*bscaley; //DrC
 		by_avg = by_avg*(1.0-baveraging)+by*baveraging; // Dr. C.
 		heading = 180*atan(by_avg/bx_avg)/pi; //DrC

 		SmartDashboard::PutNumber("ax",ax); //DrC
 	    SmartDashboard::PutNumber("ay",ay); //DrC
 		SmartDashboard::PutNumber("az",az); //DrC
 		SmartDashboard::PutNumber("heading", heading); //DrC
 		SmartDashboard::PutNumber("pd", reflectedLight);
 		SmartDashboard::PutNumber("bx", bx_avg);
 		SmartDashboard::PutNumber("by", by_avg);

 		SmartDashboard::PutData("rwheel", rwheel);  //DrC this example gets data from pointer to method
 	 	SmartDashboard::PutNumber("shooterwheel", shotspeed);
 	 	SmartDashboard::PutNumber("quality", quality);//tells the camEra qyuality
 		//SmartDashboard::PutNumber("rightgo",rightgo); //DrC
 		//SmartDashboard::PutNumber("leftgo",leftgo); //DrC
 		//SmartDashboard::PutNumber("kickballout",pickup_kickballout); //DrC
 		//SmartDashboard::PutNumber("Shooterwheelaxis", shooter_shoot);
//
  	}
  	void TestPeriodic()
 	{
 		lw->Run();
 	}
  };
 START_ROBOT_CLASS(Robot)
  // Robot button-software-electrical map

  //
