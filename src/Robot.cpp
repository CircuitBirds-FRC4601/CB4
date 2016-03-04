#include "WPILib.h"
//#include <stdio.h>
#include <unistd.h>
/*      CB4 Robot Code, team 4601 (Canfield Ohio,the Circuit Birds)
 *
 *
 */
  class Robot: public IterativeRobot
  {
 private:
	  	bool nitroL, nitroR,cam_button,cam_button1,ramp_in,ramp_out,cam,cam_switcher;  //DrC, for speed boost in tank drive
	  	bool pickup_pickup, piston_button,frame_act,button_led, speedgood,piston_button_prev; //DrC
	  	int i, samples;
	 	double leftgo,rightgo,speed,quality,Arm_in,Arm_out;  //DrC speed scales joysticks output to drive number
	 	double ax, ay,az, bx,by, heading, boffsetx,boffsety, bscaley, bscalex, baveraging, bx_avg, by_avg, pd, strobe_on, strobe_off, reflectedLight, pi=4.0*atan(1.0), pickup_kickballout, shooterWheel, swheelspeed, shotspeed, savg, starget, swindow, pickupWheel, shooter_shoot; //FIX
	 	double auto_F,auto_server,Autolow_F;//E Auto code variables
	 	int r_enc,l_enc,shot_enc,auto_serversub,arm_dir;
		bool forward1,to_ramp;//things for auto
		bool Arm_buttonin,Arm_buttonout,stop_arm,shooter_shootrev;
		bool underglow_button,underglow_prev,underglow_sel;

	 	std::string autoSelected;

	  	DoubleSolenoid *piston = new DoubleSolenoid(0,1);
	  	DoubleSolenoid *piston_ramp = new DoubleSolenoid(2,3);
	  	Compressor *howdy= new Compressor(0);//comnpressor does not like the term compress or compressor
		 std::string pistion_server = "None§";
		 std::string pistionramp_server="None§";

	  	IMAQdxSession session1;
	  	Image *frame1;
	  	IMAQdxError imaqError1;
	  	IMAQdxSession session2;
	  	Image *frame2;
	 	IMAQdxError imaqError2;

	  	Joystick *rightDrive = new Joystick(0,2,9);//DrC
	  	Joystick *leftDrive  = new Joystick(1,2,9);//DrC
		Joystick *gamePad = new Joystick(2,6,9);//DrC

	  	Talon *fRight = new Talon(1); // remaped all talons E and DrC
	  	Talon *fLeft = new Talon(0);
	  	Talon *bRight = new Talon(2);
	  	Talon *bLeft = new Talon(3);
	  	Talon *pickup = new Talon(4);// pickup
	  	Talon *shooter = new Talon(5);// main shooterwheel
		Talon *Arm = new Talon(7);

		AnalogInput *Bx = new AnalogInput(0); //DrC  magnetic x component
		AnalogInput *By = new AnalogInput(1); //DrC  magnetic y component
		AnalogInput *Auto_sel = new AnalogInput(3);
		//AnalogInput *Photo = new AnalogInput(2); //DrC  photodiode response
		BuiltInAccelerometer *accel = new BuiltInAccelerometer(); //DrC what it is...what it is...

		Relay *underglow =new Relay(0);

		DigitalOutput *leds1 = new DigitalOutput(7);//status 1

		DigitalInput *lswitch_arm = new DigitalInput(8);//reads the arm limit switch

		Encoder *shooterwheel =new Encoder(4,5);
		Encoder *lwheel = new Encoder(0,1);
		Encoder *rwheel = new Encoder(2,3);

		DriverStation::Alliance Team;// I have no Idea why but I seem only able to get the variable to work this way

	  	RobotDrive *robotDrive = new RobotDrive(fLeft, bLeft, fRight, bRight);
	  	RobotDrive *pickupShooter = new RobotDrive(pickup, pickup, shooter, shooter); //**Arm_in,Arm_out
	  	RobotDrive *ArmDrive = new RobotDrive(Arm_in,Arm_in,Arm_out,Arm_out);

	  	LiveWindow *lw = LiveWindow::GetInstance();
	  	SendableChooser *chooser = new SendableChooser();
	  	const std::string autoNameDefault = "Low Bar§";//needs fixed still broken
	  	const std::string autoNameCustom = "FORWARD!!!!!!";



 	void RobotInit()
 		{
 			chooser->AddDefault(autoNameDefault, (void*)&autoNameDefault);
 			chooser->AddObject(autoNameCustom, (void*)&autoNameCustom);
 			SmartDashboard::PutData("Auto Modes", chooser);

 		}

	void AutonomousInit()
	{
		{
			autoSelected = *((std::string*)chooser->GetSelected());
			//std::string autoSelected = SmartDashboard::GetString("Auto Selector", autoNameDefault);
			std::cout << "Auto selected: " << autoSelected << std::endl;

			if(autoSelected == autoNameCustom){
				}
			 else {
				//Default Auto goes here
			}
		}
		/*//TEAM DISPLAY
		Team = DriverStation::GetInstance().GetAlliance();//This is if we need to switch the magnatometer around and stuff
			if(Team==(DriverStation::Alliance::kBlue))
					{
				SmartDashboard::PutString("Team","Blue!");
					//mag=normal
					}
				else if(Team==(DriverStation::Alliance::kRed)){
					SmartDashboard::PutString("Team","Red!");
						//switch the magnetometer value
						}
					else{
						SmartDashboard::PutString("Team","NONE?!");
						}
//TEAM DISPLAY*/


//AUTO
		shooterwheel->Reset();
		rwheel->Reset();
		lwheel->Reset();
	    howdy->Enabled();
		piston_ramp->Set(DoubleSolenoid::Value::kOff);
		piston->Set(DoubleSolenoid::Value::kOff);

		auto_F=68.75;//50 rotations is about 25 in // 137.5 should get us past the outer works mabey so about 68.75 in so about half a foot past start of outer works
		forward1=0;
		to_ramp=0;
		cam=0;
		r_enc=0;
		r_enc=0;
		auto_server=Auto_sel->GetValue();
		cam=FALSE;
//AUTO
}

 void AutonomousPeriodic()
	{

		r_enc = abs(rwheel->GetRaw())/360;
	 	l_enc = abs(lwheel->GetRaw())/360;

	 		if(autoSelected == autoNameCustom){ //forward

	 			if((r_enc<=auto_F)&&(l_enc<=auto_F)&& not forward1){
	 				rightgo=.5;
	 			 	leftgo=.5;
	 				}

	 				else if(not forward1) {
	 					rightgo=0;
	 					leftgo=0;
	 					rwheel->Reset();
	 					lwheel->Reset();
	 					forward1=TRUE;
	 					}
	 				}
	 			if(autoSelected == autoNameDefault) {//lowbar
	 					if((r_enc<=auto_F)&&(l_enc<=auto_F)&& not forward1){
	 					 rightgo=-.5;
	 					 leftgo=-.5;
	 					}

	 				else if(not forward1) {
	 					 	rightgo=0;
	 					 	leftgo=0;
	 					 	rwheel->Reset();
	 					 	lwheel->Reset();
	 					 	forward1=TRUE;
	 				}

	 			}
	robotDrive->TankDrive(rightgo, leftgo);

			SmartDashboard::PutNumber("auto_server", auto_server);
			SmartDashboard::PutNumber("r_enc", r_enc);
		 	SmartDashboard::PutNumber("l_enc", l_enc);
		 	if(not cam){
		 	 				IMAQdxStopAcquisition(session2);
		 	 			 			IMAQdxCloseCamera(session2);

		 	 			 			IMAQdxStopAcquisition(session1);
		 	 			 			 			 			IMAQdxCloseCamera(session1);
		 	 			 				frame1 = imaqCreateImage(IMAQ_IMAGE_RGB, 0);
		 	 			 		 						IMAQdxOpenCamera("cam3", IMAQdxCameraControlModeController, &session1);
		 	 			 		 						IMAQdxConfigureGrab(session1);
		 	 			 		 						IMAQdxStartAcquisition(session1);
		 	 				cam=TRUE;
		 	 			}
		 	 		IMAQdxGrab(session1, frame1, true, NULL);
		 	 			CameraServer::GetInstance()->SetImage(frame1);//Elmo
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
 				lwheel->Reset();
 				howdy->Enabled();
 				piston_ramp->Set(DoubleSolenoid::Value::kOff);
 				piston->Set(DoubleSolenoid::Value::kOff);
 				frame_act=FALSE;
 				piston_button_prev=0;
 				cam=FALSE;
 				cam_switcher=FALSE;
 				underglow->Set(Relay::kOff);
 				arm_dir=0;
 				stop_arm=1;
//TELOP DECLERATIONS
 	}
  	void TeleopPeriodic()
  	{

  		r_enc=lwheel->GetRaw();
  	  	l_enc=rwheel->GetRaw();
  		shot_enc=shooterwheel->GetRaw();
/*  		auto_server=Auto_sel->GetValue();  // for testing only
  	 if(auto_server<=300){
	    auto_serversub=1;
  	 }
  	 	 else if(auto_server<=800){
	     auto_serversub=2;
    }
  	 	 	 else if(auto_server<=1800){
	         auto_serversub=3;
    }*/
 //DRIVE CONTROL
  		rightgo = rightDrive-> GetRawAxis(1);
 		leftgo  = leftDrive-> GetRawAxis(1);
 		nitroR   = rightDrive-> GetRawButton(3);
 		nitroL   = leftDrive-> GetRawButton(3);
 		rightgo = -(speed+(1.0-speed)*(double)(nitroR))*rightgo;
 		leftgo  = -(speed+(1.0-speed)*(double)(nitroL))*leftgo;
 		robotDrive->TankDrive(rightgo, leftgo);
//DRIVE CONTROL


/*//STROBEY BIT SECTION
 		button_led = gamePad->GetRawButton(1);
 		if(button_led){
 		reflectedLight = 0.0;  //DrC, the phase sensitive detection signal goes in this variable
 		  for(i=1;i<samples;i++){   //DrC basic flash sequence
 			// led1 -> Set(1);
 			 usleep(100);  //DrC, delay meant to let LED output stabilize a bit.
 			 strobe_on = Photo -> GetVoltage();
 			// led1 -> Set(0);
 			 usleep(100);
 			 strobe_off = Photo -> GetVoltage();
 			 reflectedLight = reflectedLight+strobe_on-strobe_off; //DrC summative
 		  }
 		}
//STROBEY BIT SECTION*/


// PICKUPWHEEL
 		pickup_kickballout = gamePad -> GetRawAxis(2);
 		pickup_pickup = gamePad -> GetRawAxis(3);
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
 		shooter_shoot = gamePad -> GetRawButton(6);
 		shooter_shootrev =gamePad->GetRawButton(5);
 		if(shooter_shoot&&not shooter_shootrev){
 			shooterWheel = -.75;
 		 }
 		else if (not shooter_shoot&&shooter_shootrev){
 			shooterWheel = .75;
 		}
 			else {
 			shooterWheel = 0.0;
 			}
 			pickupShooter->TankDrive(pickupWheel,shooterWheel);
//SHOOTER WHEEL


 //PISTON CONTROL AREA
 		piston_button_prev = piston_button;
 		piston_button  = leftDrive-> GetRawButton(1);
 		ramp_in=gamePad->GetRawButton(4);
 	 	ramp_out=gamePad->GetRawButton(2);
		if((piston_button!=piston_button_prev)&&piston_button)
	 		{
	 			frame_act= not frame_act;
	 		}
		if(frame_act){
			piston->Set(DoubleSolenoid::Value::kForward);
			pistion_server="FORWARD!";
 		}
			else{
				piston->Set(DoubleSolenoid::Value::kReverse);
				pistion_server="RETREAT";
				}
		if((ramp_in)&&(not ramp_out)){
 			piston_ramp->Set(DoubleSolenoid::Value::kForward);
 			pistionramp_server="FORWARD!";
 		}
		if((ramp_out)&&(not ramp_in)){
 			piston_ramp->Set(DoubleSolenoid::Value::kReverse);
 			pistionramp_server="RETREAT";
 		}
//END OF PISTON CONTROL AREA


//ARM CONTROL
 	Arm_buttonin= gamePad->GetRawButton(1);
 	Arm_buttonout= gamePad->GetRawButton(3);
 	stop_arm=lswitch_arm->Get();
	if(not stop_arm){
	 		Arm->Set(-.5*arm_dir);
	 		usleep(400);
	 		arm_dir=0;
	 		}
	else if(Arm_buttonin){
 				Arm->Set(.5);
 				arm_dir=1;
			}
 	else if(Arm_buttonout){
 				Arm->Set(-.5);
 				arm_dir=-1;
 			}
 	else{
 		Arm->Set(0);
 		}
SmartDashboard::PutBoolean("STOOOOOPP!",stop_arm);

//CAMERA CONTROL
 		cam_button=leftDrive->GetRawButton(2);
 	//	cam_button1=rightDrive->GetRawButton(2);
 		cam_switcher=rightDrive->GetRawButton(1);

 		if(not cam_switcher){
 			if(not cam){
 				IMAQdxStopAcquisition(session2);
 			 			IMAQdxCloseCamera(session2);

 			 			IMAQdxStopAcquisition(session1);
 			 			 			 			IMAQdxCloseCamera(session1);
 			 				frame1 = imaqCreateImage(IMAQ_IMAGE_RGB, 0);
 			 		 						IMAQdxOpenCamera("cam3", IMAQdxCameraControlModeController, &session1);
 			 		 						IMAQdxConfigureGrab(session1);
 			 		 						IMAQdxStartAcquisition(session1);
 				cam=TRUE;
 			}
 		IMAQdxGrab(session1, frame1, true, NULL);
 			CameraServer::GetInstance()->SetImage(frame1);//Elmo
 		}

 		else{
 			if(cam){
 				IMAQdxStopAcquisition(session1);
 			 				IMAQdxCloseCamera(session1);

 				frame2 = imaqCreateImage(IMAQ_IMAGE_RGB, 0);
 		 						IMAQdxOpenCamera("cam2", IMAQdxCameraControlModeController, &session2);
 		 						IMAQdxConfigureGrab(session2);
 		 						IMAQdxStartAcquisition(session2);
 	 					cam=FALSE;
 			}
 	 					IMAQdxGrab(session2, frame2, true, NULL);
 	 					 CameraServer::GetInstance()->SetImage(frame2);//BERT
 		}

//CAM CONTROL


//UNDERGLOW
 		/*
 		underglow_prev = underglow_button;
 		 		underglow_button  = gamePad-> GetRawButton(8);
 		if((underglow_button!=underglow_prev)&&underglow_button)
 			 		{
 			 			underglow_sel= not underglow_sel;
 			 		}
 		if(underglow_sel){
 			underglow->Set(Relay::kForward);
 		}
 		else{
 			underglow->Set(Relay::kOff);
 		}
 		*/
 		underglow_button  = gamePad-> GetRawButton(3);
 		if(underglow_button){
 		   underglow->Set(Relay::kForward);
 			}
 			else{
 			underglow->Set(Relay::kOff);
 			}
//UNDERGLOW


//SENSORS
 		ax = accel-> GetX();//DrC   Sensor Section : get orientation of the robot WRT field co-ordinates.
 		ay = accel-> GetY();
 		az = accel-> GetZ();//DrC  ax ay az used to define down
 		bx = Bx -> GetVoltage();
 		by = By -> GetVoltage();
 	//	pd = Photo -> GetVoltage();
 			// here flatten response and assume that robot is level.
 		bx = (bx-boffsetx)*bscalex;
 		bx_avg = bx_avg*(1.0-baveraging)+bx*baveraging;
 		by = (by-boffsety)*bscaley;
 		by_avg = by_avg*(1.0-baveraging)+by*baveraging;
 		heading = 180*atan(by_avg/bx_avg)/pi;
//SENSORS


//SMART DASHPORD
 		SmartDashboard::PutNumber("ax",ax);
 	    SmartDashboard::PutNumber("ay",ay);
 		SmartDashboard::PutNumber("az",az);
 		SmartDashboard::PutNumber("heading", heading);
 		SmartDashboard::PutNumber("pd", reflectedLight);
 		SmartDashboard::PutNumber("bx", bx_avg);
 		SmartDashboard::PutNumber("by", by_avg);

		SmartDashboard::PutBoolean("cam_switcher", cam_switcher);
 		SmartDashboard::PutNumber("auto_server", auto_server);
 		SmartDashboard::PutNumber("l_enc", l_enc);
 		SmartDashboard::PutNumber("r_enc", r_enc);
 		SmartDashboard::PutNumber("shot_enc", shot_enc);
 		SmartDashboard::PutString("Frame Pistions",pistion_server);
 		SmartDashboard::PutString("Ramp Pistions",pistionramp_server);
 	 	SmartDashboard::PutNumber("shooterwheel", shotspeed);


//SMART DASHPORD

  	}
  	void TestPeriodic()
 	{
 		lw->Run();
 	}
  };
 START_ROBOT_CLASS(Robot)
/* Hardware map of the robot "Shadow"  (CB4)
 *
 *
 *  RRio Pins
 *  	DIO
 *  	0	A right wheel encoder
 *  	1	B "
 *  	2	B left wheel encoder
 *  	3	A  "
 *  	4	B shooter wheel encoder
 *  	5	A
 *  	6   (bumper frame)
 *  	7   (status LED)
 *  	8   arm limit switch (magnetic reed switch)
 *  	9
 *
 *  	Analog
 *  	0
 *  	1
 *  	2
 *  	3	AutonSelect
 *
 *		PWM
 *		0  Front Left drive motor PWM
 *		1  Front Right drive motor "
 *		2  Back Right drive motor	"
 *		3  back left drive motor	"
 *		4  pickup pwm (no encoder)
 *		5  shooter motor pwm
 *		6  may be broken
 *		7 arm motor pwm
 *		8
 *		9
 *
 *		Relay
 *		0   Underlighting control...
 *		1
 *		2
 *		3
 *
 *	    Pnuematic
 *		0  lift piston double solenoid, channel 1
 *		1  lift piston " , channel 2
 *		2  shooter ramp pistons double solonoid channel 1
 *		3  schooter ramp pistons " , channel 2
 *
 *
 *
 *
 *
 */
