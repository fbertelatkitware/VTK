/*=========================================================================

  Program:   Visualization Library
  Module:    $RCSfile$
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

This file is part of the Visualization Library. No part of this file or its 
contents may be copied, reproduced or altered in any way without the express
written consent of the authors.

Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen 1993, 1994 

=========================================================================*/
//
// DataSetToDataSetFilter takes any dataset as input and copies it through, 
// changing the point attributes along the way.
//
#ifndef __vlDataSetToDataSetFilter_h
#define __vlDataSetToDataSetFilter_h

#include "DataSetF.hh"
#include "DataSet.hh"

class vlDataSetToDataSetFilter : public vlDataSetFilter,  public vlDataSet 
{
public:
  vlDataSetToDataSetFilter();
  ~vlDataSetToDataSetFilter();
  char *GetClassName() {return "vlDataSetToDataSetFilter";};
  vlDataSet *MakeObject() {return this->DataSet->MakeObject();};
  int NumCells() {return this->DataSet->NumCells();}
  int NumPoints() {return this->DataSet->NumPoints();}
  int CellDimension(int cellId) {return this->DataSet->CellDimension(cellId);}
  void CellPoints(int cellId, vlIdList& ptId) 
    {this->DataSet->CellPoints(cellId, ptId);}
  void Initialize();
  float *GetPoint(int i) {return this->DataSet->GetPoint(i);}
  void GetPoints(vlIdList& ptId, vlFloatPoints& fp)
    {this->DataSet->GetPoints(ptId,fp);}
  void ComputeBounds() {this->DataSet->ComputeBounds();};
  vlMapper *MakeMapper();
  void Update();

protected:
  vlDataSet *DataSet;

};

#endif


