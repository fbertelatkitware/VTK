/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile$
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

This file is part of the Visualization Toolkit. No part of this file
or its contents may be copied, reproduced or altered in any way
without the express written consent of the authors.

Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen 1993, 1994 

=========================================================================*/
// .NAME vtkRotationalExtrusionFilter - sweep polygonal data creating "skirt" from free edges and lines, and lines from vertices
// .SECTION Description
// vtkRotationalExtrusionFilter is a modelling filter. It takes polygonal 
// data as input and generates polygonal data on output. The input dataset 
// is swept around the z-axis to create new polygonal primitives. These 
// primitives form a "skirt" or swept surface. For example, sweeping a
// line results in a cylindrical shell, and sweeping a circle creates a 
// torus.
//    There are a number of control parameters for this filter. You can 
// control whether the sweep of a 2D object (i.e., polygon or triangle 
// strip) is capped with the generating geometry via the "Capping" instance
// variable. Also, you can control the angle of rotation, and whether 
// translation along the z-axis is performed along with the rotation.
// (Translation is useful for creating "springs"). You can also adjust 
// the radius of the generating geometry using the "DeltaRotation" instance 
// variable.
//    The skirt is generated by locating certain topological features. Free 
// edges (edges of polygons or triangle strips only used by one polygon or
// triangle strips) generate surfaces. This is true also of lines or 
// polylines. Vertices generate lines.
//    This filter can be used to model axisymmetric objects like cylinders,
// bottles, and wine glasses; or translational/rotational symmetric objects
// like springs or corkscrews.
// .SECTION Caveats
// If the objects sweeps 360 degrees and does not translate, capping is not
// performed. This is because the cap is unnecessary.
//    Some polygonal objects have no free edges (e.g., sphere). When swept 
// this will result in two separate surfaces if capping is on, or no surface
// if capping is off.
// .SECTION See Also
// vtkLinearExtrusionFilter

#ifndef __vtkRotationalExtrusionFilter_h
#define __vtkRotationalExtrusionFilter_h

#include "P2PF.hh"

class vtkRotationalExtrusionFilter : public vtkPolyToPolyFilter 
{
public:
  vtkRotationalExtrusionFilter();
  ~vtkRotationalExtrusionFilter() {};
  char *GetClassName() {return "vtkRotationalExtrusionFilter";};
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Set/Get resolution of sweep operation. Resolution controls the number
  // of intermediate node points.
  vtkSetClampMacro(Resolution,int,1,LARGE_INTEGER);
  vtkGetMacro(Resolution,int);

  // Description:
  // Turn on/off the capping of the skirt.
  vtkSetMacro(Capping,int);
  vtkGetMacro(Capping,int);
  vtkBooleanMacro(Capping,int);

  // Description:
  // Set/Get angle of rotation.
  vtkSetClampMacro(Angle,float,0,360.0);
  vtkGetMacro(Angle,float);

  // Description:
  // Set/Get total amount of translation along the z-axis.
  vtkSetMacro(Translation,float);
  vtkGetMacro(Translation,float);

  // Description:
  // Set/Get change in radius during sweep process.
  vtkSetMacro(DeltaRadius,float);
  vtkGetMacro(DeltaRadius,float);

protected:
  void Execute();
  int Resolution;
  int Capping;
  float Angle;
  float Translation;
  float DeltaRadius;
};

#endif
