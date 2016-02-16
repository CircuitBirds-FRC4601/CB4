#include "WPILib.h"
//#include <stdio.h>   //DrC may not need this one. We may want to use it if we attemp fileI/O with the code.
#include <unistd.h>  //DrC , needed just for the sleep() function...warning sleep() may not be threadsafe!
/*      CB4 Robot Code, team 4601 (Canfield Ohio,the Circuit Birds)
 *
 *
 */
  class Robot: public IterativeRobot
  {
 private:
	  	bool nitroL, nitroR,cam_button;  //DrC, for speed boost in tank drive
	  	bool pickup_pickup,shooter_spinup, piston_button,frame_act,button_led, speedgood; //DrC
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

	  	IMAQdxSession session1;
	  	Image *frame1;
	  	IMAQdxError imaqError1;
	/*	IMAQdxSession session2;
	  	Image *frame2;
	  	IMAQdxError imaqError2;*/

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


		DriverStation::Alliance Team;// I have no Idea why but I seem only able to get the variable to work this way

	  	RobotDrive *robotDrive = new RobotDrive(fLeft, bLeft, fRight, bRight);
	  	RobotDrive *pickupShooter = new RobotDrive(pickup, pickup, shooter, shooter); //**

 	LiveWindow *lw = LiveWindow::GetInstance();
 	Command *auto_selector;
 	SendableChooser *auto_chooser;

 void RobotInit(){
	auto_chooser=new SendableChooser();
	 auto_chooser->AddDefault("Low Bar", auto_chooser);
 }
	void AutonomousInit() {
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
		shooterwheel->Reset();
		rwheel->Reset();
		lwheel->Reset();
	    howdy->Enabled();
		piston1->Set(DoubleSolenoid::Value::kOff);
		piston->Set(DoubleSolenoid::Value::kOff);
		Auto1_F=50;//50 rotations
}
	void AutonomousPeriodic() //Welcome to the Realm of Hearsay. OH! and Elijah.
	{
		r_enc = abs(rwheel->GetRaw()); //E have to convert regular encoders into ints for auto
		l_enc = abs(lwheel->GetRaw());//E the abs is absolute value so i don't have to make anything negative
		if(r_enc){
		}
	}

 	void TeleopInit()
 	{
 		//Camfront
 		frame1 = imaqCreateImage(IMAQ_IMAGE_RGB, 0);
 			//the camera name (ex "cam0") can be found through the roborio web interface
 			imaqError1 = IMAQdxOpenCamera("cam3", IMAQdxCameraControlModeController, &session1);
 			if(imaqError1 != IMAQdxErrorSuccess) {
 				DriverStation::ReportError("IMAQdxOpenCamera error: " + std::to_string((long)imaqError1) + "\n");
 			}
 			imaqError1 = IMAQdxConfigureGrab(session1);
 			//Camback
 		/*	frame2 = imaqCreateImage(IMAQ_IMAGE_RGB, 0);
 	 			//the camera name (ex "cam0") can be found through the roborio web interface
 	 			imaqError2 = IMAQdxOpenCamera("cam2", IMAQdxCameraControlModeController, &session2);
 	 			if(imaqError2 != IMAQdxErrorSuccess) {
 	 				DriverStation::ReportError("IMAQdxOpenCamera error: " + std::to_string((long)imaqError2) + "\n");
 	 			}
 	 			imaqError2 = IMAQdxConfigureGrab(session2);
 	 			*/
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
 				frame_act=0;
 	}

  	void TeleopPeriodic()
  	{
  		rightgo = rightDrive-> GetRawAxis(1);
 		leftgo  = leftDrive-> GetRawAxis(1);
 		nitroR   = rightDrive-> GetRawButton(3);
 		nitroL   = leftDrive-> GetRawButton(3);
 		rightgo = -(speed+(1.0-speed)*(double)(nitroR))*rightgo;  //DrC for nitro drive
 		leftgo  = -(speed+(1.0-speed)*(double)(nitroL))*leftgo;   //DrC  ''
 		robotDrive->TankDrive(rightgo, leftgo);
 		button_led = gamePad->GetRawButton(1);
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
 			frame_act= not frame_act;
 		}
 		if(frame_act)
 		{
 		 			piston->Set(DoubleSolenoid::Value::kForward);
 		 			Wait(5);
 		}
 		else{
 			piston->Set(DoubleSolenoid::Value::kReverse);
 			Wait(5);
 		}
 		cam_button=leftDrive->GetRawButton(1);
 	/*	if(cam_button){
 			IMAQdxStopAcquisition(session1);
 	 	 	 		CameraServer::GetInstance()->SetImage(frame2);
 	 	 	 		IMAQdxStartAcquisition(session2);
 	 	 	 		IMAQdxGrab(session2, frame2, true, NULL);
 	 		}
 	 			IMAQdxStopAcquisition(session2);*/
 	 	 		CameraServer::GetInstance()->SetImage(frame1);
 	 	 		IMAQdxStartAcquisition(session1);
 	 	 		IMAQdxGrab(session1, frame1, true, NULL);
 		pickupShooter->TankDrive(pickupWheel,shooterWheel); //DrC here run the wheels!
 		//Sensor section Dr C
 		ax = accel-> GetX();//DrC   Sensor Section : get orientation of the robot WRT field co-ordinates.
 		ay = accel-> GetY();//DrC
 		az = accel-> GetZ();//DrC  ax ay az used to define down
 		bx = Bx -> GetVoltage();//DrC   Raw Readings
 		by = By -> GetVoltage(); //DrC
 		pd = Photo -> GetVoltage(); //DrC.
 			// here flatten response and assume that robot is level.
 		bx = (bx-boffsetx)*bscalex; //DrC
 		bx_avg = bx_avg*(1.0-baveraging)+bx*baveraging; //Dr. C
 		by = (by-boffsety)*bscaley; //DrC
 		by_avg = by_avg*(1.0-baveraging)+by*baveraging; // Dr. C.
 		heading = 180*atan(by_avg/bx_avg)/pi; //DrC

	        // grab an image, draw the circle, and provide it for the camera server which will
	        // in turn send it to the dashboard.

	        // grab an image, draw the circle, and provide it for the camera server which will
	        // in turn send it to the dashboard.
 //imaqDrawShapeOnImage(frame1, frame1, { 10, 10, 100, 100 }, DrawMode::IMAQ_DRAW_VALUE, ShapeMode::IMAQ_SHAPE_RECT, 0.0f);
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
  	}
  	void TestPeriodic()
 	{
 		lw->Run();
 	}
  };
 START_ROBOT_CLASS(Robot)
  // Robot button-software-electrical map
  //
