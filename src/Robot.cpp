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

	 	double leftgo,rightgo,speed,quality,Arm_in,Arm_out;  //DrC speed scales joysticks output to drive number
	 	double ax, ay,az, bx,by, heading, boffsetx,boffsety, bscaley, bscalex, baveraging, bx_avg, by_avg, pd, pickup_kickballout, shooterWheel, swheelspeed, shotspeed, savg, starget, swindow, pickupWheel, shooter_shoot; //FIX
		bool Arm_buttonin,Arm_buttonout,stop_arm1,stop_arm2,shooter_shootrev,rumble_button;
		bool underglow_button,underglow_prev,underglow_sel;
	  	double shotreader;

	 	double auto_F,auto_spin,auto_estop,auto_Fhalf;//Encoder values
	 	int r_enc,l_enc,arm_dir;
		bool forward1,forward1low,to_ramp,spin,auto_fin;//things for auto

		bool STOP;
		double lstick,rstick;
	  	int i, samples, pi=4.0*atan(1.0);
	  	double ayavg,r;

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

		DigitalInput *lswitch_arm1 = new DigitalInput(8);//reads the arm limit switch
		DigitalInput *lswitch_arm2 = new DigitalInput(9);

		Encoder *shooterwheel =new Encoder(4,5);
		Encoder *lwheel = new Encoder(0,1);
		Encoder *rwheel = new Encoder(2,3);

	  	RobotDrive *robotDrive = new RobotDrive(fLeft, bLeft, fRight, bRight);
	  	RobotDrive *pickupShooter = new RobotDrive(pickup, pickup, shooter, shooter);
	  	RobotDrive *ArmDrive = new RobotDrive(Arm_in,Arm_in,Arm_out,Arm_out);

	 	std::string autoSelected;
	  	LiveWindow *lw = LiveWindow::GetInstance();
	  	SendableChooser *chooser = new SendableChooser();
	  	const std::string autoNameDefault = "FORWARD!";
	  	const std::string autoNameHalf = "3/4";//is called half but actuly 3/4
	  	//const std::string autoNameCustom = "Low Bar";
	  	const std::string autoNameNone = "NONE!";

	void RobotInit()
 		{
	chooser->AddDefault(autoNameDefault, (void*)&autoNameDefault);
		chooser->AddObject(autoNameNone, (void*)&autoNameNone);
		chooser->AddObject(autoNameHalf, (void*)&autoNameHalf);
	//	chooser->AddObject(autoNameCustom, (void*)&autoNameCustom);
			 			SmartDashboard::PutData("Auto Modes", chooser);
 		}

	void AutonomousInit()
	{

			autoSelected = *((std::string*)chooser->GetSelected());

			std::cout << "Auto selected: " << autoSelected << std::endl;

		/*	if(autoSelected == autoNameCustom){
				}
			 else {

			}*/




//AUTO
		shooterwheel->Reset();
		rwheel->Reset();
		lwheel->Reset();
		r_enc=0;
		r_enc=0;
		ay = 0;
	    howdy->Enabled();
		piston_ramp->Set(DoubleSolenoid::Value::kReverse);
		piston->Set(DoubleSolenoid::Value::kOff);

		auto_F=5400;//to wall is ~5040
		auto_Fhalf=3780;//actuly 3/4
		auto_spin=3343;//180 spin ~3343
		auto_estop=9000;//e stop is ~9000
		forward1=0;
		forward1low=0;
		to_ramp=0;
		spin=0;
		auto_fin=0;
		cam=0;
		r = .2 ; // the averaging ratio for the accelerometer readins in auton
		ayavg=0.0;
//AUTO
}

 void AutonomousPeriodic()
	{
	 ay = accel-> GetY();
	 ayavg = (1.0-r)*ayavg+r*ay;   // the smoothed, running average of ay...actually solving a DE with a pole at ir!
		r_enc = abs(rwheel->GetRaw());
	 	l_enc = abs(lwheel->GetRaw());

	 		/*if(autoSelected == autoNameCustom){ //low bar

					if((r_enc<=auto_F)&&(l_enc<=auto_F)&&not forward1low){
					 rightgo=.85;
					 leftgo=.85;
					}

				else if(not forward1low) {
					 	rwheel->Reset();
					 	lwheel->Reset();
					 	forward1low=TRUE;
					 	auto_fin=0;
				}
				else if((r_enc<=auto_spin)&&(l_enc<=auto_spin)&&not spin){
					 rightgo=.85;
					 leftgo=-.85;
				}
				else if(not spin){
					rightgo=0;
					 leftgo=0;
				rwheel->Reset();
				lwheel->Reset();
					spin=TRUE;
				}
				else if((r_enc<=auto_estop)&&(l_enc<=auto_estop)&&(not(ayavg>=.05))&&(not auto_fin)){
						rightgo=-.85;
						leftgo=-.85;
				}
				else{
					rightgo=0;
					leftgo=0;
					auto_fin=1;
				}

	 				}


	 		else*/ if(autoSelected == autoNameDefault){//forward
	 			if((r_enc<=auto_F)&&(l_enc<=auto_F)&& not forward1){
	 				 				rightgo=.85;
	 				 			 	leftgo=.85;
	 				 				}

	 				 				else if(not forward1){
	 				 					rightgo=0;
	 				 					leftgo=0;
	 				 					rwheel->Reset();
	 				 					lwheel->Reset();
	 				 					forward1=TRUE;
	 				 					}

	 			}

	 		else if(autoSelected == autoNameNone){//none
	 			rightgo=0;
	 			leftgo=0;
	 		}

	 	else if(autoSelected == autoNameHalf){// three fourth dist
	 		if((r_enc<=auto_Fhalf)&&(l_enc<=auto_Fhalf)&& not forward1){
	 			 	rightgo=1;
	 			 	leftgo=1;
	 			 			}

	 			 	else if(not forward1){
	 			 				 	rightgo=0;
	 			 				 	leftgo=0;
	 			 				 	rwheel->Reset();
	 			 				 	lwheel->Reset();
	 			 				 	forward1=TRUE;
	 			 				 	}

	 		 	}
			robotDrive->TankDrive(rightgo, leftgo);


			SmartDashboard::PutNumber("r_enc", r_enc);
		 	SmartDashboard::PutNumber("l_enc", l_enc);
		 	SmartDashboard::PutNumber("ayavg", ayavg);

		 	if(not cam){
		 	 				IMAQdxStopAcquisition(session2);
		 	 			 			IMAQdxCloseCamera(session2);


		 	 			 			IMAQdxStopAcquisition(session1);
		 	 			 			 	 IMAQdxCloseCamera(session1);
		 	 			 				frame1 = imaqCreateImage(IMAQ_IMAGE_RGB, 0);
		 	 			 		 						IMAQdxOpenCamera("cam4", IMAQdxCameraControlModeController, &session1);
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
 				speed  = 1; //driving speed for finer control
 				shooterwheel->Reset();
 				rwheel->Reset();
 				lwheel->Reset();
 				howdy->Enabled();
 				piston_ramp->Set(DoubleSolenoid::Value::kReverse);
 				piston->Set(DoubleSolenoid::Value::kOff);
 				frame_act=FALSE;
 				piston_button_prev=0;
 				cam=FALSE;
 				cam_switcher=FALSE;
 				underglow->Set(Relay::kOff);
 				arm_dir=0;
 				stop_arm2=1;
 				stop_arm1=1;
 				shotspeed=0;
 				STOP=0;
 	 	 r = .2;
//TELOP DECLERATIONS
 	}



  	void TeleopPeriodic()
  	{

  		r_enc=abs (lwheel->GetRaw());
  	  	l_enc=abs(rwheel->GetRaw());
  	  	SmartDashboard::PutNumber("rstick",rstick);
  	  	SmartDashboard::PutNumber("lstick",lstick);
 //DRIVE CONTROL
  	  	rightgo = rightDrive-> GetRawAxis(1);
 		leftgo  = leftDrive-> GetRawAxis(1);

 		nitroR   = rightDrive-> GetRawButton(3);
 		nitroL   = leftDrive-> GetRawButton(3);

 		rightgo = -(speed+(1.0-speed)*(double)(nitroR))*rightgo;
 		leftgo  = -(speed+(1.0-speed)*(double)(nitroL))*leftgo;

 	/*	FULL=gamePad->GetRawButton(8);
if(FULL){
	rightgo=1;
	leftgo=1;
}
else{
	rightgo=0;
	leftgo=0;

}*/
 		robotDrive->TankDrive(rightgo, leftgo);


//DRIVE CONTROL


// PICKUPWHEEL
 		pickup_kickballout = gamePad -> GetRawAxis(2);
 		pickup_pickup = gamePad -> GetRawAxis(3);

 	if(abs(pickup_kickballout)>.1){
 			pickupWheel = .75;
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
 			shooterWheel = -.8;

 		 }
 		else if (not shooter_shoot&&shooter_shootrev){
 			shooterWheel = .8;
 		}
 			else {
 			shooterWheel = 0.0;
 			}
 			pickupShooter->TankDrive(pickupWheel,shooterWheel);

 			shotreader=shooterwheel->GetRate();

 			 		 if(abs(shotreader)>=900){ // this will most likley have to be changed
 			 			 shotspeed=1;
 			 		 }
 			 		else{
 					shotspeed=0;
 			 		}

 			  		if(shotspeed){
 			  			gamePad->SetRumble(Joystick::RumbleType::kRightRumble,1);
 			  			gamePad->SetRumble(Joystick::RumbleType::kLeftRumble,1);
 			  		}
 			  		else{
 			  			gamePad->SetRumble(Joystick::RumbleType::kRightRumble,0);
 			  		  	gamePad->SetRumble(Joystick::RumbleType::kLeftRumble,0);
 			  		}

 			  		SmartDashboard::PutNumber("shotreader",shotreader);
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
	 	stop_arm1=lswitch_arm1->Get();
	 	stop_arm2=lswitch_arm2->Get();
	 	if(stop_arm1){
	 		Arm_buttonin= gamePad->GetRawButton(1);
	 	}
	 	else{
	 		Arm_buttonin=0;
	 	}
	 	if(stop_arm2){
	 		Arm_buttonout= gamePad->GetRawButton(3);
	 	}
	 	else{
	 		Arm_buttonout=0;
	 	}


if(Arm_buttonin){
 				Arm->Set(-.25);
 				//arm_dir=1;
			}
 	else if(Arm_buttonout){
 				Arm->Set(.25);
 				//arm_dir=-1;
 			}
 	else{
 		Arm->Set(0);
 		}
SmartDashboard::PutBoolean("STOOOOOPP!2",stop_arm2);
SmartDashboard::PutBoolean("STOOOOOPP!1",stop_arm1);

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
 			 		 						IMAQdxOpenCamera("cam4", IMAQdxCameraControlModeController, &session1);
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
 		 						IMAQdxOpenCamera("cam3", IMAQdxCameraControlModeController, &session2);
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
 		   underglow->Set(Relay::1

//SENSORS
 		ax = accel-> GetX();//DrC   Sensor Section : get orientation of the robot WRT field co-ordinates.
 		ay = accel-> GetY();
 	 ayavg = (1.0-r)*ayavg+r*ay;   // the smoothed, running average of ay...actually solving a DE with a pole at ir!
 		az = accel-> GetZ();//DrC  ax ay az used to define down
 		bx = Bx -> GetVoltage();
 		by = By -> GetVoltage();

 		bx = (bx-boffsetx)*bscalex;
 		bx_avg = bx_avg*(1.0-baveraging)+bx*baveraging;
 		by = (by-boffsety)*bscaley;
 		by_avg = by_avg*(1.0-baveraging)+by*baveraging;
 		heading = 180*atan(by_avg/bx_avg)/pi;
//SENSORS


//SMART DASHPORD
 		SmartDashboard::PutNumber("ax",ax);
 	    SmartDashboard::PutNumber("ayavg",ayavg);
 		SmartDashboard::PutNumber("az",az);
 		SmartDashboard::PutNumber("heading", heading);

 		SmartDashboard::PutNumber("bx", bx_avg);
 		SmartDashboard::PutNumber("by", by_avg);
 		SmartDashboard::PutNumber("l_enc", l_enc);
 		SmartDashboard::PutNumber("r_enc", r_enc);
 	 	SmartDashboard::PutNumber("shooterwheel", shotspeed);

 		SmartDashboard::PutString("Ramp Pistions",pistionramp_server);
 		SmartDashboard::PutString("Frame Pistions",pistion_server);

 	 	SmartDashboard::PutBoolean("cam_switcher", cam_switcher);

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
 * 1ft=~720
 *  RRio Pins
 *  	DIO
 *  	0	A right wheel encoder
 *  	1	B "
 *  	2	B left wheel encoder
 *  	3	A  "
 *  	4	B shooter wheel encoder
 *  	5	A "
 *  	6
 *  	7   (status LED)
 *  	8 lswitcharm_1
 *  	9 lswitcharm_2
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
