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
// .NAME vtkPNGWriter - Writes PNG files.
// .SECTION Description
// vtkPNGWriter writes PNG files. It supports 1 to 4 component data of
// unsigned char or unsigned short

// .SECTION See Also
// vtkPNGReader

#ifndef __vtkPNGWriter_h
#define __vtkPNGWriter_h

#include "vtkImageWriter.h"

class VTK_IO_EXPORT vtkPNGWriter : public vtkImageWriter
{
public:
  static vtkPNGWriter *New();
  vtkTypeRevisionMacro(vtkPNGWriter,vtkImageWriter);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // The main interface which triggers the writer to start.
  virtual void Write();

  // Description:
  // Write the image to memory (a vtkUnsignedCharArray)
  vtkSetMacro(WriteToMemory, unsigned int);
  vtkGetMacro(WriteToMemory, unsigned int);
  vtkBooleanMacro(WriteToMemory, unsigned int);
  
  // Description:
  // When writing to memory this is the result, it will be NULL until the 
  // data is written the first time
  vtkSetObjectMacro(Result, vtkUnsignedCharArray);
  vtkGetObjectMacro(Result, vtkUnsignedCharArray);

protected:
  vtkPNGWriter();
  ~vtkPNGWriter();
  
  void WriteSlice(vtkImageData *data);
  unsigned int WriteToMemory;
  vtkUnsignedCharArray *Result;

private:
  vtkPNGWriter(const vtkPNGWriter&);  // Not implemented.
  void operator=(const vtkPNGWriter&);  // Not implemented.
};

#endif


