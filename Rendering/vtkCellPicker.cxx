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
#include "CellPick.hh"

vtkCellPicker::vtkCellPicker()
{
  this->CellId = -1;
  this->SubId = -1;
  for (int i=0; i<3; i++) this->PCoords[i] = 0.0;
}

void vtkCellPicker::IntersectWithLine(float p1[3], float p2[3], float tol, 
                                     vtkActor *a, vtkMapper *m)
{
  int numCells;
  int cellId, i, minCellId, minSubId, subId;
  float x[3], tMin, t, pcoords[3], minXYZ[3], minPcoords[3];
  vtkCell *cell;
  vtkDataSet *input=m->GetInput();

  if ( (numCells = input->GetNumberOfCells()) < 1 ) return;
//
//  Intersect each cell with ray.  Keep track of one closest to 
//  the eye (and within the clipping range).
//
  for (minCellId=(-1),tMin=LARGE_FLOAT,cellId=0; cellId<numCells; cellId++) 
    {
    cell = input->GetCell(cellId);

    if ( cell->IntersectWithLine(p1, p2, tol, t, x, pcoords, subId) 
    && t < tMin )
      {
      minCellId = cellId;
      minSubId = subId;
      for (i=0; i<3; i++)
        {
        minXYZ[i] = x[i];
        minPcoords[i] = pcoords[i];
        }
      tMin = t;
      }
    }
//
//  Now compare this against other actors.
//
  if ( minCellId>(-1) && tMin < this->GlobalTMin ) 
    {
    this->MarkPicked(a, m, tMin, minXYZ);
    this->CellId = minCellId;
    this->SubId = minSubId;
    for (i=0; i<3; i++) this->PCoords[i] = minPcoords[i];
    vtkDebugMacro("Picked cell id= " << minCellId);
    }
}

void vtkCellPicker::Initialize()
{
  this->CellId = (-1);
  this->SubId = (-1);
  for (int i=0; i<3; i++) this->PCoords[i] = 0.0;
  this->vtkPicker::Initialize();
}

void vtkCellPicker::PrintSelf(ostream& os, vtkIndent indent)
{
  this->vtkPicker::PrintSelf(os,indent);

  os << indent << "Cell Id: " << this->CellId << "\n";
  os << indent << "SubId: " << this->SubId << "\n";
  os << indent << "PCoords: (" << this->PCoords[0] << ", " 
     << this->PCoords[1] << ", " << this->PCoords[2] << ")\n";
}
