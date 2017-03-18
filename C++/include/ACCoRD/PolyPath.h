/*
 * Copyright (c) 2011-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#ifndef POLYPATH_H_
#define POLYPATH_H_

#include <string>
#include <vector>
#include "Position.h"
#include "Velocity.h"
#include "Plan.h"
#include "SimplePoly.h"
#include "MovingPolygon3D.h"
#include "SimpleMovingPoly.h"
#include "EuclideanProjection.h"
#include "BoundingRectangle.h"
#include "Triple.h"


namespace larcfm{

/**
 * Polygons are arranged in "paths" similar to aircraft plans (and, in fact, this object can produce one 
 * -- or more -- faux-aircraft plans that approximate the area in question).  The polygon is allowed to 
 * change shape as it travels along the path, but the number of vertices must remain constant.  The shape
 * at non-specified time points is linearly interpolated from the neighboring points.   
 * 
 * Paths with a single entry (at any time) are considered to be static, immobile polygons that exist at 
 * all times
 *  
 *  Paths can have one of 3 modes:
 *  MORPHING (the default): all polygons must have the same number of vertices.  Velocities are calculated by vertex between consecutive steps.
 *  AVG_VEL: Velocities are calculated between the average points of consecutive steps.  Polygons do not need to have the same number of vertices at each step.
 *  USER_VEL: Velocities are user-specified and applied to all vertices.  Polygons do not need to have the same number of vertices at each step.  The last step will have an infinitely continuing velocity.
 *  USER_VEL_FINITE: As USER_VEL, but the path does not continue past the last step.  (One-step paths are considered the same as USER_VEL.)  The last step is mostly ignored except for time and can be any polygon.
 *  
 * Note: polygon support is experimental and the interface is subject to change!
 */
class PolyPath : public ErrorReporter {
public:
    enum PathMode {MORPHING, AVG_VEL, USER_VEL, USER_VEL_FINITE};

  private:
  std::vector<SimplePoly> polyList;
  std::vector<double> times;
  std::vector<Velocity> vlist;
  std::string name;
//  bool morphingPolys;
//  bool calcVelocities;
  PathMode mode;
  mutable ErrorLog error;
  
	/**
	 * Helper function to combine the 2 user vel modes 
	 * @return true, if user velocity
	 */
  bool isUserVel() const;

  public:
  
  /**
   * Constructor
   */
  PolyPath();

  /**
   * Constructor with associated name
   * @param n name string
   */
  PolyPath(const std::string& n);
  
  /**
   * Make a PolyPlan for a non-dynamic area p.
   * @param p
   */
  PolyPath(const std::string& n, const SimplePoly& p);

  /**
   * Construct a new (dynamic) path including polygon p at time t
   */
  PolyPath(const std::string& n, const SimplePoly& p, double t);

  PolyPath(const std::string& n, const SimplePoly& p, const Velocity& v, double t);

  
  PolyPath(const std::string& n, const std::vector<SimplePoly>& ps, const std::vector<double>& ts);
  
  // deep copy
  /**
   * Construct a deep copy of path p
   * @param p path to copy
   */
  PolyPath(const PolyPath& p);
  
  static PolyPath pathFromState(const std::string& n, const SimplePoly& p, const Velocity& v, double tstart, double tend);
  static PolyPath pathFromState(const std::string& n, const MovingPolygon3D& mp, EuclideanProjection proj, double tstart, double tend);

  virtual ~PolyPath() {};

  // deep copy
	/**
	 * Return a deep copy of this path.
	 * 
	 * @return path of polygon
	 */
  PolyPath copy() const;
  
  Triple<Plan,double,double> buildPlan() const;
  
	/**
	 * Return true if this path contains any geodetic points.
	 * @return true, if lat/lon
	 */
  bool isLatLon() const;
  
	/**
	 * Return true if this PolyPath is static (it always exists in a fixed location).
	 * This is the case if there is only one step in the path and the mode is not USER_VEL 
	 * @return true, if static
	 */
  bool isStatic() const;

	/**
	 * Return true if this PolyPath continues to exist past the defined path (it has a velocity, which may be zero, and no definite end point)
	 * This is the case if mode is USER_VEL or if there is only one step in the path. 
	 * @return true, if continuing
	 */
   bool isContinuing() const;

	/**
	 * Get the mode for this path.
	 *  MORPHING: all polygons must have the same number of vertices.  Velocities are calculated by vertex between consecutive steps.
	 *  AVG_VEL: Velocities are calculated between the average points of consecutive steps.  Polygons do not need to have the same 
	 *  number of vertices at each step.
	 *  USER_VEL: Velocities are user-specified and applied to all vertices.  Polygons do not need to have the same number of 
	 *  vertices at each step.  The last step will have an infinitely continuing velocity.
	 *  @return path mode
	 */
   PathMode getPathMode() const;

	/**
	 * Set the mode for this path.
	 *  MORPHING: all polygons must have the same number of vertices.  Velocities are calculated by vertex between consecutive steps.
	 *  AVG_VEL: Velocities are calculated between the average points of consecutive steps.  Polygons do not need to have the same number of vertices at each step.
	 *  USER_VEL: Velocities are user-specified and applied to all vertices.  Polygons do not need to have the same number of vertices at each step.  The last step will have an infinitely continuing velocity.
	 *  Changing away from USER_VEL will erase any stored velocity information.  Changing to USER_VEL will populate the path with the same values
	 *  as AVG_VEL (the last step will have a zero velocity). 
	 *  USER_VEL_FINITE: as USER_VEL
	 *  @param m path mode
	 */
   void setPathMode(PathMode m);

//   bool isCalculateVelocities() const;
//
//   void setCalculateVelocities(bool b);


	/**
	 * Returns the name associated with this path
	 * @return name
	 */
  std::string getName() const;
  
	/**
	 * Sets the name to be associated with this path.
	 * @param n new name
	 */
  void setName(const std::string& n);
  
	/**
	 * Returns the number of time points in this path.  Static paths will have a size of 1.
	 * @return size
	 */
  int size() const;

	/**
	 * This defaults to MORPHING mode.
	 * 
	 * @param p polygon
	 * @param time time
	 */
  void addPolygon(const SimplePoly& p, double time);

	/**
	 * Add a polygon with velocity information.
	 * This defaults to USER_VEL mode.
	 * @param p         position
	 * @param v         velocity
	 * @param time      start time for this polygon
	 */
  void addPolygon(const SimplePoly& p, const Velocity& v, double time);
  
	/**
	 * Remove a poly from this path, by step index.
	 * @param n index
	 */
  void remove(int n);
  
	/**
	 * Returns the start time of this path.  Static paths have a start time of 0.
	 * @return first time
	 */
  double getFirstTime() const;

	/**
	 * Returns the end time of this path.  Static paths have an end time of 0.  Continuing paths have an end time of POSITIVE_INFINITY
	 * @return last time
	 */
  double getLastTime() const;

	/**
	 * Returns an interpolated poly from an arbitrary time on this path.
	 * 
	 * @param time time
	 * @return polygon
	 */
  SimplePoly interpolate(double time) const;
  
	/**
	 * Returns the initial velocity of this path, starting from a given step index.
	 * @param n index
	 * @return velocity
	 */
  Velocity initialVelocity(int n) const;
  
  Velocity initialVertexVelocity(int vert, int n) const;

	/**
	 * Return the segment number that contains 'time' in [s].  If the
	 * time is not contained in the flight plan then -1 is returned.  If
	 * the time for point 1 is 10.0 and the time for point 2 is 20.0, then
	 * getSegment(10.0) will produce 0, getSegment(15.0) will produce 0,
	 * and getSegment(20.0) will produce 1.
	 * 
	 * @param tm time
	 * @return segment number
	 */
	int getSegment(double tm) const;

	/**
	 * Interpolate the poly at the given time
	 * If time is outside the path's limit, return null.
	 * @param time
	 * @return polygon
	 */
  SimplePoly position(double time) const;


	/**
	 * Returns the step index associated with a particular time, if there is a match, otherwise returns a negative value.
	 * -1 indicates a time before the path begins. -x indicates a time after point (x-1), so a time between points 1 and 2 will return index -2. 
	 * A time after the path will return -size-1.
	 * 
	 * @param time time
	 * @return index
	 */
  int getIndex(double time) const;
  
  SimplePoly& getPolyRef(int n);

	/**
	 * Returns a copy of the poly at step index n.
	 * 
	 * @param n index
	 * @return polygon
	 */
  SimplePoly getPoly(int n) const;
  
	/**
	 * Returns the time for a given step index.
	 * 
	 * @param n index
	 * @return time
	 */
  double getTime(int n) const;

	/**
	 * Sets the time t for a given step n.  The order of times must be preserved. 
	 * 
	 * @param n index
	 * @param t time
	 */
  void setTime(int n, double t);

	/**
	 * Set the polygon at index i to be a copy of p
	 * 
	 * @param i index
	 * @param p polygon
	 */
   void setPolygon(int i, const SimplePoly& p);

	/**
	 * Set the user velocity at index i to be v
	 * This only has effect if isCalculatedVelocities() is false.
	 * 
	 * @param i index
	 * @param v velocity
	 */
   void setVelocity(int i, const Velocity& v);


  // returns true if the polypath follows expected behaviors (i.e. increasing times and same size for each poly)
  // sets error messages if otherwise
	/**
	 * Returns true if the polypath follows expected behaviors (i.e. increasing times and same size for each MORPHING poly).
	 * Otherwise this sets error messages.
	 * 
	 * @return true, if polypath performs expected behaviors
	 */
  bool validate() const;
  

	/**
	 * Return a truncated version of this path that ends at time t.  This will change a USER_VEL path into a USER_VEL_FINITE path.
	 * @param t The time to halt the path.
	 * @return A truncated path.  If t &le; the path's start time, then return an empty path.  If t &ge; the path's end time, then return a copy of the original path.
	 */
  PolyPath truncate(double t) const;

  // this does not check poly sizes
  /**
   * A quick and dirty method for building paths one point at a time.
   * This does not check that there are the same number of points at each step -- call validate() on this object
   * when done to confirm that the final path is correct.
   * @param p A new (bottom) point.
   * @param top p's corresponding top altitude 
   * @param time the time for the poly p is a vertex of
   */
  void addVertex(const Position& p, double top, double time);
  
	/**
	 * This adds a vertex to every polygon on this path.  The vertex will be added relative to the centroid in each case, so other polygons may
	 * need to be modified to match.
	 * 
	 * @param n index
	 * @param p position
	 */
  void addVertex(int n, const Position& p);

	/**
	 * Remove the nth vertex from all polygons on the path.
	 * @param vert index of vertex to be removed.
	 */
  void removeVertex(int vert);

  /**
   * Translate the entire path by a Euclidean offset
   * @param off Euclidean offset
   */
  void translate(Vect3 off);


  /**
   * Shift all times in the path by the given number of seconds.
   * @param shift Amount of shift.
   */
  void timeshift(double shift);

	/**
	 * Interpolate the averagePoint velocity at the given time
	 * If time is outside the path's limit, return invalid velocity.
	 * @param time
	 * @return velocity
	 */
  Velocity velocity(double time) const;

  Velocity vertexVelocity(int vert, double time) const;

  Velocity finalVelocity(int i) const;

  Velocity finalVertexVelocity(int vert, int i) const;

	/**
	 * This will return a moving polygon that starts at point i and ends at point i+1
	 * @param i    index
	 * @param proj projection
	 * @return polygon
	 */
  MovingPolygon3D getInitialMovingPolygon(int i, const EuclideanProjection& proj) const;

	/**
	 * This will return a moving polygon that STARTS at time t (relative time 0) and ends at its segment end time.
	 * 
	 * @param time
	 * @param proj
	 * @return polygon
	 */
  MovingPolygon3D getMovingPolygon(double time, const EuclideanProjection& proj) const;

  static std::string pathModeToString(PathMode m);

  static PathMode parsePathMode(const std::string& s);

  /**
   * Returns a string representation of this path.
   */
  std::string toString() const;

  // this outputs name, x y z1 z2 time fixed fields (or lat lon alt)
	/**
	 * Returns a string representation of this path that is compatible with the PolyReader input format.
	 * @return string representation
	 */
  std::string toOutput() const;

  std::string toOutput(int precision, bool tcpColumns) const;

  std::vector<std::string> toStringList(int i, int j, int precision, bool tcpColumns) const;

  SimpleMovingPoly getSimpleMovingPoly(int i) const;

  SimpleMovingPoly getSimpleMovingPoly(double t) const;


  bool contains (const Position& p, double t) const;

  bool contains2D (const Position& p, double t) const;


  BoundingRectangle getBoundingRectangle() const;

	// ErrorReporter Interface Methods

  bool hasError() const {
    return error.hasError();
  }
  bool hasMessage() const {
    return error.hasMessage();
  }
  std::string getMessage() {
    return error.getMessage();
  }
  std::string getMessageNoClear() const {
    return error.getMessageNoClear();
  }
  


  
}; // class
} //namespace
#endif
