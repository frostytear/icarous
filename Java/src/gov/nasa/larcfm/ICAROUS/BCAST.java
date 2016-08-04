/**
 * ICAROUS Interface
 * Contact: Swee Balachandran (swee.balachandran@nianet.org)
 * 
 * 
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.ICAROUS;

import gov.nasa.larcfm.IO.SeparatedInput;
import gov.nasa.larcfm.Util.ParameterData;
import com.MAVLink.icarous.*;
import java.io.*;


public class BCAST implements Runnable{

    public Thread t;
    public String threadName;
    public Aircraft UAS;
    public AircraftData FlightData;
    public Interface Intf;
    ParameterData parameters;
    
    public BCAST(String name,Aircraft uas,Interface bcastIntf){
	threadName       = name;
	UAS              = uas;               
	FlightData       = UAS.FlightData;
	Intf             = bcastIntf;

	try{
	    FileReader in = new FileReader("params/icarous.txt");
	    SeparatedInput reader = new SeparatedInput(in);

	    reader.readLine();
	    parameters = reader.getParametersRef();
	    
	}
	catch(FileNotFoundException e){
	    System.out.println("parameter file not found");
	}
    }

    public void run(){

	msg_heartbeat_icarous ICAROUSstate = new msg_heartbeat_icarous();
	double ic_hz                       = parameters.getValue("ic_heartbeat_Hz");

	double timeStart                   = (double) (System.nanoTime()/1E9);
	double timeCurrent,timeElapsed;                 
	while(true){	    	    	    	   

	    timeCurrent = (double) (System.nanoTime()/1E9);

	    timeElapsed = timeCurrent - timeStart;

	    if(timeElapsed > 1/ic_hz){
		ICAROUSstate.status = (byte) UAS.GetAircraftState();
	    
		// Broadcast messages here
		Intf.Write(ICAROUSstate);
		
		timeStart = timeCurrent;
	    }


	}
    }

    public void start(){
	System.out.println("Starting "+threadName);
	t = new Thread(this,threadName);
	t.start();
    }
    
}
