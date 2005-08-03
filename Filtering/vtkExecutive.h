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
// .NAME vtkExecutive - Superclass for all pipeline executives in VTK.
// .SECTION Description
// vtkExecutive is the superclass for all pipeline executives in VTK.
// A VTK executive is responsible for controlling one instance of
// vtkAlgorithm.  A pipeline consists of one or more executives that
// control data flow.  Every reader, source, writer, or data
// processing algorithm in the pipeline is implemented in an instance
// of vtkAlgorithm.

#ifndef __vtkExecutive_h
#define __vtkExecutive_h

#include "vtkObject.h"

class vtkAlgorithm;
class vtkAlgorithmOutput;
class vtkAlgorithmToExecutiveFriendship;
class vtkDataObject;
class vtkExecutiveInternals;
class vtkInformation;
class vtkInformationExecutivePortKey;
class vtkInformationExecutivePortVectorKey;
class vtkInformationIntegerKey;
class vtkInformationRequestKey;
class vtkInformationKeyVectorKey;
class vtkInformationVector;

class VTK_FILTERING_EXPORT vtkExecutive : public vtkObject
{
public:
  vtkTypeRevisionMacro(vtkExecutive,vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Get the algorithm to which this executive has been assigned.
  vtkAlgorithm* GetAlgorithm();

  // Description:
  // Generalized interface for asking the executive to fullfill update
  // requests. In this signature you inputs and outputs are provided and
  // it does not chain up or down stream.
  virtual int ProcessRequest(vtkInformation* request,
                             int forward,
                             vtkInformationVector** inInfo,
                             vtkInformationVector* outInfo);

  // Description:
  // Bring the algorithm's outputs up-to-date.  Returns 1 for success
  // and 0 for failure.
  virtual int Update();
  virtual int Update(int port);

  // Description:
  // Get the number of input/output ports for the algorithm associated
  // with this executive.  Returns 0 if no algorithm is set.
  int GetNumberOfInputPorts();
  int GetNumberOfOutputPorts();

  // Description:
  // Get the number of input connections on the given port.
  int GetNumberOfInputConnections(int port);

  // Description:
  // Get the pipeline information object for the given output port.
  virtual vtkInformation* GetOutputInformation(int port);

  // Description:
  // Get the pipeline information object for all output ports.
  vtkInformationVector* GetOutputInformation();

  // Description:
  // Get the pipeline information for the given input connection.
  vtkInformation* GetInputInformation(int port, int connection);

  // Description:
  // Get the pipeline information vectors for the given input port.
  vtkInformationVector* GetInputInformation(int port);  
  
  // Description:
  // Get the pipeline information vectors for all inputs
  vtkInformationVector** GetInputInformation();

  // Description:
  // Get the executive managing the given input connection.
  vtkExecutive* GetInputExecutive(int port, int connection);

  // Description:
  // Get/Set the data object for an output port of the algorithm.
  virtual vtkDataObject* GetOutputData(int port);
  virtual void SetOutputData(int port, vtkDataObject*, vtkInformation *info);
  virtual void SetOutputData(int port, vtkDataObject*);

  // Description:
  // Get the data object for an output port of the algorithm.
  virtual vtkDataObject* GetInputData(int port, int connection);
  virtual vtkDataObject* GetInputData(int port, int connection, 
                                      vtkInformationVector **inInfoVec);

  // Description:
  // Get the output port that produces the given data object.
  virtual vtkAlgorithmOutput* GetProducerPort(vtkDataObject*);

  // Description:
  // Participate in garbage collection.
  virtual void Register(vtkObjectBase* o);
  virtual void UnRegister(vtkObjectBase* o);

  // Description:
  // Information key to store the executive/port number producing an
  // information object.
  static vtkInformationExecutivePortKey* PRODUCER();

  // Description:
  // Information key to store the executive/port number pairs
  // consuming an information object.
  static vtkInformationExecutivePortVectorKey* CONSUMERS();

  // Description:
  // Information key to store the output port number from which a
  // request is made.
  static vtkInformationIntegerKey* FROM_OUTPUT_PORT();

  // Description:
  // Keys to program vtkExecutive::ProcessRequest with the default
  // behavior for unknown requests.
  static vtkInformationIntegerKey* ALGORITHM_BEFORE_FORWARD();
  static vtkInformationIntegerKey* ALGORITHM_AFTER_FORWARD();
  static vtkInformationIntegerKey* ALGORITHM_DIRECTION();
  static vtkInformationIntegerKey* FORWARD_DIRECTION();
  static vtkInformationKeyVectorKey* KEYS_TO_COPY();
  //BTX
  enum { RequestUpstream, RequestDownstream };
  //ETX

  // Description:
  // An API to CallAlgorithm that allows you to pass in the info objects to
  // be used
  virtual int CallAlgorithm(vtkInformation* request, int direction,
                            vtkInformationVector** inInfo,
                            vtkInformationVector* outInfo);

  // since PipelineMTime is called so often and since it travels the full
  // length of the pipeline every time we have an optimized funciton to
  // handle it. For most executives the request is not used.
  virtual unsigned long ComputePipelineMTime
  (int /* forward */, 
   vtkInformation * /* request */,
   vtkInformationVector ** /* inInfoVec */) { return 0; };
  
protected:
  vtkExecutive();
  ~vtkExecutive();

  // Helper methods for subclasses.
  int InputPortIndexInRange(int port, const char* action);
  int OutputPortIndexInRange(int port, const char* action);

  // Called by methods to check for a recursive pipeline update.  A
  // request should be fulfilled without making another request.  This
  // is used to help enforce that behavior.  Returns 1 if no recursive
  // request is occurring, and 0 otherwise.  An error message is
  // produced automatically if 0 is returned.  The first argument is
  // the name of the calling method (the one that should not be
  // invoked recursively during an update).  The second argument is
  // the recursive request information object, if any.  It is used to
  // construct the error message.
  int CheckAlgorithm(const char* method, vtkInformation* request);

  virtual int ForwardDownstream(vtkInformation* request);
  virtual int ForwardUpstream(vtkInformation* request);
  virtual void CopyDefaultInformation(vtkInformation* request, int direction,
                                      vtkInformationVector** inInfo,
                                      vtkInformationVector* outInfo);

  // Reset the pipeline update values in the given output information object.
  virtual void ResetPipelineInformation(int port, vtkInformation*)=0;

  // Bring the existence of output data objects up to date.
  virtual int UpdateDataObject()=0;

  // Garbage collection support.
  virtual void ReportReferences(vtkGarbageCollector*);

  virtual void SetAlgorithm(vtkAlgorithm* algorithm);

  // The algorithm managed by this executive.
  vtkAlgorithm* Algorithm;

  // Flag set when the algorithm is processing a request.
  int InAlgorithm;

private:

  // Store an information object for each output port of the algorithm.
  vtkInformationVector* OutputInformation;

  // Internal implementation details.
  vtkExecutiveInternals* ExecutiveInternal;

  //BTX
  friend class vtkAlgorithmToExecutiveFriendship;
  //ETX
private:
  vtkExecutive(const vtkExecutive&);  // Not implemented.
  void operator=(const vtkExecutive&);  // Not implemented.
};

#endif
