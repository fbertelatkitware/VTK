/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile$
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 1993-2002 Ken Martin, Will Schroeder, Bill Lorensen 
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkXMLDataSetWriter.h"

#include "vtkCallbackCommand.h"
#include "vtkDataSet.h"
#include "vtkImageData.h"
#include "vtkObjectFactory.h"
#include "vtkPolyData.h"
#include "vtkRectilinearGrid.h"
#include "vtkStructuredGrid.h"
#include "vtkUnstructuredGrid.h"
#include "vtkXMLImageDataWriter.h"
#include "vtkXMLPolyDataWriter.h"
#include "vtkXMLRectilinearGridWriter.h"
#include "vtkXMLStructuredGridWriter.h"
#include "vtkXMLUnstructuredGridWriter.h"

vtkCxxRevisionMacro(vtkXMLDataSetWriter, "$Revision$");
vtkStandardNewMacro(vtkXMLDataSetWriter);

//----------------------------------------------------------------------------
vtkXMLDataSetWriter::vtkXMLDataSetWriter()
{  
  // Setup a callback for the internal writer to report progress.
  this->ProgressObserver = vtkCallbackCommand::New();
  this->ProgressObserver->SetCallback(&vtkXMLDataSetWriter::ProgressCallbackFunction);
  this->ProgressObserver->SetClientData(this);
}

//----------------------------------------------------------------------------
vtkXMLDataSetWriter::~vtkXMLDataSetWriter()
{
  this->ProgressObserver->Delete();
}

//----------------------------------------------------------------------------
void vtkXMLDataSetWriter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
void vtkXMLDataSetWriter::SetInput(vtkDataSet* input)
{
  this->vtkProcessObject::SetNthInput(0, input);
}

//----------------------------------------------------------------------------
vtkDataSet* vtkXMLDataSetWriter::GetInput()
{
  if(this->NumberOfInputs < 1)
    {
    return 0;
    }
  
  return static_cast<vtkDataSet*>(this->Inputs[0]);
}

//----------------------------------------------------------------------------
int vtkXMLDataSetWriter::WriteInternal()
{  
  vtkDataSet* input = this->GetInput();
  vtkXMLWriter* writer = 0;
  
  // Create a writer based on the data set type.
  switch (input->GetDataObjectType())
    {
    case VTK_IMAGE_DATA:
    case VTK_STRUCTURED_POINTS:
      {
      vtkXMLImageDataWriter* w = vtkXMLImageDataWriter::New();
      w->SetInput(static_cast<vtkImageData*>(input));
      writer = w;
      } break;
    case VTK_STRUCTURED_GRID:
      {
      vtkXMLStructuredGridWriter* w = vtkXMLStructuredGridWriter::New();
      w->SetInput(static_cast<vtkStructuredGrid*>(input));
      writer = w;
      } break;
    case VTK_RECTILINEAR_GRID:
      {
      vtkXMLRectilinearGridWriter* w = vtkXMLRectilinearGridWriter::New();
      w->SetInput(static_cast<vtkRectilinearGrid*>(input));
      writer = w;
      } break;
    case VTK_UNSTRUCTURED_GRID:
      {
      vtkXMLUnstructuredGridWriter* w = vtkXMLUnstructuredGridWriter::New();
      w->SetInput(static_cast<vtkUnstructuredGrid*>(input));
      writer = w;
      } break;
    case VTK_POLY_DATA:
      {
      vtkXMLPolyDataWriter* w = vtkXMLPolyDataWriter::New();
      w->SetInput(static_cast<vtkPolyData*>(input));
      writer = w;
      } break;
    }
  
  // Make sure we got a valid writer for the data set.
  if(!writer)
    {
    vtkErrorMacro("Cannot write dataset type: "
                  << input->GetDataObjectType());
    return 0;
    }
  
  // Copy the settings to the writer.
  writer->SetDebug(this->GetDebug());
  writer->SetFileName(this->GetFileName());
  writer->SetByteOrder(this->GetByteOrder());
  writer->SetCompressor(this->GetCompressor());
  writer->SetBlockSize(this->GetBlockSize());
  writer->SetDataMode(this->GetDataMode());
  writer->SetEncodeAppendedData(this->GetEncodeAppendedData());
  writer->AddObserver(vtkCommand::ProgressEvent, this->ProgressObserver);
  
  // Try to write.
  int result = writer->Write();
  
  // Cleanup.
  writer->RemoveObserver(this->ProgressObserver);
  writer->Delete();
  return result;
}

//----------------------------------------------------------------------------
int vtkXMLDataSetWriter::WriteData()
{
  return 0;
}

//----------------------------------------------------------------------------
const char* vtkXMLDataSetWriter::GetDataSetName()
{
  return "DataSet";
}

//----------------------------------------------------------------------------
const char* vtkXMLDataSetWriter::GetDefaultFileExtension()
{
  return "vtk";
}

//----------------------------------------------------------------------------
void vtkXMLDataSetWriter::ProgressCallbackFunction(vtkObject* caller,
                                                   unsigned long,
                                                   void* clientdata, void*)
{
  vtkProcessObject* w = vtkProcessObject::SafeDownCast(caller);
  if(w)
    {
    reinterpret_cast<vtkXMLDataSetWriter*>(clientdata)->ProgressCallback(w);
    }
}

//----------------------------------------------------------------------------
void vtkXMLDataSetWriter::ProgressCallback(vtkProcessObject* w)
{
  float width = this->ProgressRange[1]-this->ProgressRange[0];
  float internalProgress = w->GetProgress();
  float progress = this->ProgressRange[0] + internalProgress*width;
  this->UpdateProgressDiscrete(progress);
  if(this->AbortExecute)
    {
    w->SetAbortExecute(1);
    }
}
