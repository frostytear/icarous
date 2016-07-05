package gov.nasa.larc.ICAROUS;

import java.util.*;

public class Position{

    float lat;
    float lon;
    float alt_msl;
    float alt_agl;

    public Position(){
	lat   = 0.0f;
	lon   = 0.0f;
	alt_msl   = 0.0f;
	alt_agl   = 0.0f;
    }

    public Position(float lat_in,float lon_in,float altmsl_in){
	lat   = lat_in;
	lon   = lon_in;
	alt_msl   = altmsl_in;
    }

    public Position(float lat_in,float lon_in,float altmsl_in,float altagl_in){
	lat     = lat_in;
	lon     = lon_in;
	alt_msl   = altmsl_in;
	alt_agl   = altagl_in;
    }

    public void UpdatePosition(float lat_in,float lon_in,float altmsl_in,float altagl_in){
	lat   = lat_in;
	lon   = lon_in;
	alt_msl   = altmsl_in;
	alt_agl   = altagl_in;
    }

}

class Waypoint extends Position{

    int id;
    float heading;

    public Waypoint(){
	super();
	id        =0;
	heading   =0.0f;
    }

    public Waypoint(int id_in,float lat_in,float lon_in,float altmsl_in,
		    float heading_in){
	super(lat_in,lon_in,altmsl_in);
	id        = id_in;
	heading   = heading_in;
    }
        
}

class Obstacle extends Position{

    int id;

    public Obstacle(int id_in,float lat_in, float lon_in, float altmsl_in){
	super(lat_in,lon_in,altmsl_in);
	id = id_in;
    }
    
}

class FlightPlan{

    public List wayPoints;
    public int numWayPoints;
    public float maxHorDev;
    public float maxVerDev;
    public float standOffDist;
    public Iterator wpIt = null;

    public FlightPlan(){
	wayPoints    = new ArrayList();
    }
    
    public void FlightPlanInfo(int num,float HorDev, float VerDev, float standDist){

	numWayPoints = num;
	maxHorDev    = HorDev;
	maxVerDev    = VerDev;
	standOffDist = standDist;
    }

    public void AddWaypoints(int index,Waypoint wp){
	wayPoints.add(index,wp);

	if(wpIt != null){
	    wpIt = wayPoints.iterator();
	}
    }

    public Waypoint GetWaypoints(int index){
	return (Waypoint) wayPoints.get(index);
    }

    
}







