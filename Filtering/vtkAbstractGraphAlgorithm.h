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
// .NAME vtkAbstractGraphAlgorithm - Superclass for algorithms that produce only AbstractGraph as output
//
// .SECTION Description
// vtkAbstractGraphAlgorithm is a convenience class to make writing algorithms
// easier. It is also designed to help transition old algorithms to the new
// pipeline architecture. There are some assumptions and defaults made by this
// class you should be aware of. This class defaults such that your filter
// will have one input port and one output port. If that is not the case
// simply change it with SetNumberOfInputPorts etc. See this class
// constructor for the default. This class also provides a FillInputPortInfo
// method that by default says that all inputs will be AbstractGraph. If that
// isn't the case then please override this method in your subclass. This
// class breaks out the downstream requests into separate functions such as
// ExecuteData and ExecuteInformation.  For new algorithms you should
// implement RequestData( request, inputVec, outputVec) but for older filters
// there is a default implementation that calls the old ExecuteData(output)
// signature. For even older filters that don't implement ExecuteData the
// default implementation calls the even older Execute() signature.

#ifndef __vtkAbstractGraphAlgorithm_h
#define __vtkAbstractGraphAlgorithm_h

#include "vtkAlgorithm.h"
#include "vtkAbstractGraph.h" // makes things a bit easier

class vtkDataSet;

class VTK_FILTERING_EXPORT vtkAbstractGraphAlgorithm : public vtkAlgorithm
{
public:
  static vtkAbstractGraphAlgorithm *New();
  vtkTypeRevisionMacro(vtkAbstractGraphAlgorithm,vtkAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // see vtkAlgorithm for details
  virtual int ProcessRequest(vtkInformation*,
                             vtkInformationVector**,
                             vtkInformationVector*);

  // Description:
  // Get the output data object for a port on this algorithm.
  vtkAbstractGraph* GetOutput() { return this->GetOutput(0); }
  vtkAbstractGraph* GetOutput(int index);

  // Description:
  // Set an input of this algorithm. You should not override these
  // methods because they are not the only way to connect a pipeline.
  // Note that these methods support old-style pipeline connections.
  // When writing new code you should use the more general
  // vtkAlgorithm::SetInputConnection().  These methods transform the
  // input index to the input port index, not an index of a connection
  // within a single port.
  void SetInput(vtkDataObject * obj) { this->SetInput(0, obj); }
  void SetInput(int index, vtkDataObject* obj);

protected:
  vtkAbstractGraphAlgorithm();
  ~vtkAbstractGraphAlgorithm();

  // convenience method
  virtual int RequestInformation(vtkInformation* request,
                                 vtkInformationVector** inputVector,
                                 vtkInformationVector* outputVector);

  // Description:
  // This is called by the superclass.
  // This is the method you should override.
  virtual int RequestData(vtkInformation* request,
                          vtkInformationVector** inputVector,
                          vtkInformationVector* outputVector);

  // Description:
  // This is called by the superclass.
  // This is the method you should override.
  virtual int RequestUpdateExtent(vtkInformation*,
                                  vtkInformationVector**,
                                  vtkInformationVector*);

  // Description:
  // By default, creates the same output type as the input type.
  virtual int RequestDataObject(vtkInformation*, 
                                vtkInformationVector** inputVector, 
                                vtkInformationVector* outputVector);


  // see algorithm for more info
  virtual int FillOutputPortInformation(int port, vtkInformation* info);
  virtual int FillInputPortInformation(int port, vtkInformation* info);

private:
  vtkAbstractGraphAlgorithm(const vtkAbstractGraphAlgorithm&);  // Not implemented.
  void operator=(const vtkAbstractGraphAlgorithm&);  // Not implemented.
};

#endif
