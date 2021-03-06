/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile$

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkXMLPPolyDataWriter.h"

#include "vtkObjectFactory.h"
#include "vtkXMLPolyDataWriter.h"
#include "vtkPolyData.h"
#include "vtkInformation.h"

vtkCxxRevisionMacro(vtkXMLPPolyDataWriter, "$Revision$");
vtkStandardNewMacro(vtkXMLPPolyDataWriter);

//----------------------------------------------------------------------------
vtkXMLPPolyDataWriter::vtkXMLPPolyDataWriter()
{
}

//----------------------------------------------------------------------------
vtkXMLPPolyDataWriter::~vtkXMLPPolyDataWriter()
{
}

//----------------------------------------------------------------------------
void vtkXMLPPolyDataWriter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
vtkPolyData* vtkXMLPPolyDataWriter::GetInput()
{
  return static_cast<vtkPolyData*>(this->Superclass::GetInput());
}

//----------------------------------------------------------------------------
const char* vtkXMLPPolyDataWriter::GetDataSetName()
{
  return "PPolyData";
}

//----------------------------------------------------------------------------
const char* vtkXMLPPolyDataWriter::GetDefaultFileExtension()
{
  return "pvtp";
}

//----------------------------------------------------------------------------
vtkXMLUnstructuredDataWriter*
vtkXMLPPolyDataWriter::CreateUnstructuredPieceWriter()
{  
  // Create the writer for the piece.
  vtkXMLPolyDataWriter* pWriter = vtkXMLPolyDataWriter::New();
  pWriter->SetInput(this->GetInput());
  return pWriter;
}

//----------------------------------------------------------------------------
int vtkXMLPPolyDataWriter::FillInputPortInformation(
  int vtkNotUsed(port), vtkInformation* info)
{
  info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData");
  return 1;
}
