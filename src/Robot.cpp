#include "WPILib.h"
//#include <stdio.h>   //DrC may not need this one. We may want to use it if we attemp fileI/O with the code.
#include <unistd.h>  //DrC , needed just for the usleep() function
/*      CB4 Robot Code, team 4601 (Canfield Ohio,the Circuit Birds)
 *
 *
 */
  class Robot: public IterativeRobot
  {
 private:
	  	bool nitroL, nitroR,cam_button,cam_button1,ramp_in,ramp_out,cam;  //DrC, for speed boost in tank drive
	  	bool pickup_pickup,shooter_spinup, piston_button,frame_act,button_led, speedgood,piston_button_prev; //DrC
	  	int i, samples;
	  	const std::string autoNameDefault = "Default";
	  	const std::string autoNameCustom = "My Auto";
	 	double leftgo,rightgo,speed,quality;  //DrC speed scales joysticks output to drive number
	 	double ax, ay,az, bx,by, heading, boffsetx,boffsety, bscaley, bscalex, baveraging, bx_avg, by_avg, pd, strobe_on, strobe_off, reflectedLight, pi=4.0*atan(1.0), pickup_kickballout, shooterWheel, swheelspeed, shotspeed, savg, starget, swindow, pickupWheel, shooter_shoot; //FIX
	 	double Auto1_F;//E Auto code variables
	 	int r_enc,l_enc;
		bool forward1;//things for auto
	 	std::string autoSelected;

	  	DoubleSolenoid *piston = new DoubleSolenoid(0,1);
	  	DoubleSolenoid *piston_ramp = new DoubleSolenoid(2,3);
	  	Compressor *howdy= new Compressor(0);//comnpressor does not like the term compress or compressor

	  	IMAQdxSession session1;
	  	Image *frame1;
	  	IMAQdxError imaqError1;

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

		Encoder *shooterwheel =new Encoder(4,5);
		Encoder *lwheel = new Encoder(0,3);
		Encoder *rwheel = new Encoder(1,2);

		DriverStation::Alliance Team;// I have no Idea why but I seem only able to get the variable to work this way

	  	RobotDrive *robotDrive = new RobotDrive(fLeft, bLeft, fRight, bRight);
	  	RobotDrive *pickupShooter = new RobotDrive(pickup, pickup, shooter, shooter); //**


 	LiveWindow *lw = LiveWindow::GetInstance();


	void AutonomousInit()
	{

//TEAM DISPLAY
		Team = DriverStation::GetInstance().GetAlliance();//This is if we need to switch the magnatometer around and stuff
	if(Team==(DriverStation::Alliance::kBlue))//E
		{
			SmartDashboard::PutString("Team","Blue!");
		//mag=normal
		}
	else if(Team==(DriverStation::Alliance::kRed)){
		SmartDashboard::PutString("Team","Red!");
		//switch the magnetometer value
		}
	else{
	SmartDashboard::PutString("Team","NONE?");
		}
//TEAM DISPLAY

//AUTO
		shooterwheel->Reset();
		rwheel->Reset();
		lwheel->Reset();
	    howdy->Enabled();
		piston_ramp->Set(DoubleSolenoid::Value::kOff);
		piston->Set(DoubleSolenoid::Value::kOff);

		Auto1_F=137.5;//50 rotations is about 25 in // 137.5 should get us past the outer works mabey so about 68.75 in so about half a foot past start of outer works
	forward1=0;


}

 void AutonomousPeriodic()
	{
		r_enc = abs(rwheel->GetRaw())/360;
			l_enc = abs(lwheel->GetRaw())/360;
	if((r_enc<=Auto1_F)&&(l_enc<=Auto1_F)&& not forward1){
			rightgo=.7;
	 			leftgo=.7;
		}
		else{
			rightgo=.0;
				leftgo=.0;
			r_enc = abs(rwheel->GetRaw())/360;
				l_enc = abs(lwheel->GetRaw())/360;
		}
		if(forward1&&(r_enc<=Auto1_F)&&(l_enc<=Auto1_F)){

		}
		robotDrive->TankDrive(rightgo, leftgo);


		SmartDashboard::PutNumber("r_enc", r_enc);
		SmartDashboard::PutNumber("l_enc", l_enc);


	}
 	void TeleopInit()
 	{

// CALIBRATION
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
// CALIBRATION


//TELOP DECLERATIONS
 				speed  = .7; //driving speed for finer control
 				shooterwheel->Reset();
 				rwheel->Reset();
 				howdy->Enabled();
 				piston_ramp->Set(DoubleSolenoid::Value::kOff);
 				piston->Set(DoubleSolenoid::Value::kOff);
 				frame_act=0;
 				piston_button_prev=0;
 				cam=0;
//TELOP DECLERATIONS

 	}

  	void TeleopPeriodic()
  	{


//DRIVE CONTROL
  		rightgo = rightDrive-> GetRawAxis(1);
 		leftgo  = leftDrive-> GetRawAxis(1);
 		nitroR   = rightDrive-> GetRawButton(3);
 		nitroL   = leftDrive-> GetRawButton(3);
 		rightgo = -(speed+(1.0-speed)*(double)(nitroR))*rightgo;
 		leftgo  = -(speed+(1.0-speed)*(double)(nitroL))*leftgo;
 		robotDrive->TankDrive(rightgo, leftgo);
//DRIVE CONTROL


//DR C'S BORDAM ZONE
 		/*
 				if(nitroR&&nitroL){       // this section for testing digital devices
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
 		 */
//DR C'S BORDAM ZONE


//STROBEY BIT SECTION
 		button_led = gamePad->GetRawButton(1);
 		if(button_led){
 		reflectedLight = 0.0;  //DrC, the phase sensitive detection signal goes in this variable
 		  for(i=1;i<samples;i++){   //DrC basic flash sequence
 			 led1 -> Set(1);
 			 usleep(100);  //DrC, delay meant to let LED output stabilize a bit.
 			 strobe_on = Photo -> GetVoltage();
 			 led1 -> Set(0);
 			 usleep(100);
 			 strobe_off = Photo -> GetVoltage();
 			 reflectedLight = reflectedLight+strobe_on-strobe_off; //DrC summative
 		  }
 		}
//STROBEY BIT SECTION


// PICKUPWHEEL
 		pickup_kickballout = gamePad -> GetRawAxis(2);
 		pickup_pickup = gamePad -> GetRawButton(5);
 		if(abs(pickup_kickballout)>.1){
 			pickupWheel = 0.7;
 		}
 		else if(pickup_pickup){
 			pickupWheel = -0.7;
 		}
 		else{
 			pickupWheel=0.0;
 		}
// PICKUP WHEEL


//SHOOTER WHEEL
 		swheelspeed = shooterwheel->GetRate();
 	 		shotspeed = shotspeed*(1.0-savg)+savg*swheelspeed;

 	 		if (abs(shotspeed-starget)/starget<swindow){
 	 			speedgood=TRUE;
 	 		}
 	 		else{
 	 			speedgood=FALSE;
 	 		}

 		shooter_shoot = gamePad -> GetRawAxis(3);
 		shooter_spinup = gamePad -> GetRawButton(6);
 		if((abs(shooter_shoot)>.1)&&(speedgood)){ //DrC, (bool)speedgood indicates at within window around target speed.
 			shooterWheel = -1.0;
 		 }
 		 else {
 			shooterWheel = 0.0;
 		 }
 		pickupShooter->TankDrive(pickupWheel,shooterWheel);
 //SHOOTER WHEEL


 //PISTON CONTROL AREA
 		piston_button  = leftDrive-> GetRawButton(1);
 		ramp_in=gamePad->GetRawButton(4);
 	 	ramp_out=gamePad->GetRawButton(2);
 		if((piston_button)&&(not piston_button_prev)&&(not frame_act))
 		{
 			frame_act= TRUE;
 		}
 		if((piston_button)&&(not piston_button_prev)&&(frame_act))
 		{
 			frame_act=FALSE;
 		}
 		if(frame_act){
 		piston->Set(DoubleSolenoid::Value::kForward);
 		}
 		else{
 			piston->Set(DoubleSolenoid::Value::kReverse);
 		}
 		piston_button_prev = piston_button;

 		if((ramp_in)&&(not ramp_out)){
 			piston_ramp->Set(DoubleSolenoid::Value::kForward);
 		}
 		if((ramp_out)&&(not ramp_in)){
 			piston_ramp->Set(DoubleSolenoid::Value::kReverse);
 		}
//END OF PISTON CONTROL AREA


//CAMERA CONTROL
 		cam_button=leftDrive->GetRawButton(2);
 		cam_button1=rightDrive->GetRawButton(2);

 		if(cam_button&&not cam){
 			frame1 = imaqCreateImage(IMAQ_IMAGE_RGB, 0);
 			imaqError1 = IMAQdxOpenCamera("cam3", IMAQdxCameraControlModeController, &session1);
 			if(imaqError1 != IMAQdxErrorSuccess) {
 				DriverStation::ReportError("IMAQdxOpenCamera error: " + std::to_string((long)imaqError1) + "\n");
 			}
 			imaqError1 = IMAQdxConfigureGrab(session1);
 	 		IMAQdxStartAcquisition(session1);
	        CameraServer::GetInstance()->SetImage(frame1);
	 			IMAQdxStartAcquisition(session1);
	 			 	 	 IMAQdxGrab(session1, frame1, true, NULL);
	 			cam=1;
 		}
 		if(cam_button1&&not cam){
 			cam=1;

 		}
 		if(cam){
 			  CameraServer::GetInstance()->SetImage(frame1);
 				IMAQdxStartAcquisition(session1);
 				 			 	 	 		IMAQdxGrab(session1, frame1, true, NULL);
 		}
//CAM CONTROL


//SENSORS
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
//SENSORS


//SMASH DASHPORD
 		SmartDashboard::PutNumber("ax",ax); //DrC
 		SmartDashboard::PutNumber("ay",ay); //DrC
 		SmartDashboard::PutNumber("az",az); //DrC
 		SmartDashboard::PutNumber("heading", heading); //DrC
 		SmartDashboard::PutNumber("pd", reflectedLight);
 		SmartDashboard::PutNumber("bx", bx_avg);
 		SmartDashboard::PutNumber("by", by_avg);
 		SmartDashboard::PutData("rwheel", rwheel);  //DrC this example gets data from pointer to method
 	 	SmartDashboard::PutNumber("shooterwheel", shotspeed);
//SMASH DASHPORD
  	}
  	void TestPeriodic()
 	{
 		lw->Run();
 	}
  };
 START_ROBOT_CLASS(Robot)
