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
// .NAME vtkDataObject - general representation of visualization data
// .SECTION Description
// vtkDataObject is an general representation of visualization data. It serves
// to encapsulate instance variables and methods for visualization network 
// execution, as well as representing data consisting of a field (i.e., just
// an unstructured pile of data). This is to be compared with a vtkDataSet,
// which is data with geometric and/or topological structure.
//
// vtkDataObjects are used to represent arbitrary repositories of data via the
// vtkFieldData instance variable. These data must be eventually mapped into a
// concrete subclass of vtkDataSet before they can actually be displayed.
//
// .SECTION See Also
// vtkDataSet vtkFieldData vtkDataObjectSource vtkDataObjectFilter
// vtkDataObjectMapper vtkDataObjectToDataSet 
// vtkFieldDataToAttributeDataFilter

#ifndef __vtkDataObject_h
#define __vtkDataObject_h

#include "vtkObject.h"

class vtkAlgorithmOutput;
class vtkExecutive;
class vtkFieldData;
class vtkInformation;
class vtkProcessObject;
class vtkSource;
class vtkStreamingDemandDrivenPipelineToDataObjectFriendship;
class vtkExtentTranslator;
class vtkInformationDataObjectKey;
class vtkInformationDoubleKey;
class vtkInformationDoubleVectorKey;
class vtkInformationIntegerKey;
class vtkInformationIntegerVectorKey;
class vtkInformationStringKey;
class vtkStreamingDemandDrivenPipeline;
class vtkInformationInformationVectorKey;

#define VTK_PIECES_EXTENT   0
#define VTK_3D_EXTENT       1

class VTK_FILTERING_EXPORT vtkDataObject : public vtkObject
{
public:
  static vtkDataObject *New();

  vtkTypeRevisionMacro(vtkDataObject,vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Set/Get the source object creating this data object.
  vtkGetObjectMacro(Source,vtkSource);
  void SetSource(vtkSource *s);

  // Description:
  // Set/Get the information object associated with this data object.
  vtkGetObjectMacro(Information, vtkInformation);
  virtual void SetInformation(vtkInformation*);

  // Description:
  // Get/Set the pipeline information object that owns this data
  // object.
  vtkGetObjectMacro(PipelineInformation, vtkInformation);
  virtual void SetPipelineInformation(vtkInformation*);

  // Description:
  // Get the port currently producing this object.
  virtual vtkAlgorithmOutput* GetProducerPort();

  // Description:
  // Data objects are composite objects and need to check each part for MTime.
  // The information object also needs to be considered.
  unsigned long int GetMTime();

  // Description:
  // Restore data object to initial state,
  virtual void Initialize();

  // Description:
  // Release data back to system to conserve memory resource. Used during
  // visualization network execution.  Releasing this data does not make 
  // down-stream data invalid, so it does not modify the MTime of this 
  // data object.
  void ReleaseData();

  // Description:
  // Return flag indicating whether data should be released after use  
  // by a filter.
  int ShouldIReleaseData();

  // Description:
  // Get the flag indicating the data has been released.
  vtkGetMacro(DataReleased,int);
  
  // Description:
  // Turn on/off flag to control whether this object's data is released
  // after being used by a filter.
  void SetReleaseDataFlag(int);
  int GetReleaseDataFlag();
  vtkBooleanMacro(ReleaseDataFlag,int);

  // Description:
  // Turn on/off flag to control whether every object releases its data
  // after being used by a filter.
  static void SetGlobalReleaseDataFlag(int val);
  void GlobalReleaseDataFlagOn() {this->SetGlobalReleaseDataFlag(1);};
  void GlobalReleaseDataFlagOff() {this->SetGlobalReleaseDataFlag(0);};
  static int GetGlobalReleaseDataFlag();

  // Description:
  // Assign or retrieve field data to this data object.
  virtual void SetFieldData(vtkFieldData*);
  vtkGetObjectMacro(FieldData,vtkFieldData);
  
  // Handle the source/data loop.
  virtual void Register(vtkObjectBase* o);
  virtual void UnRegister(vtkObjectBase* o);

  // Description:
  // Provides opportunity for the data object to insure internal 
  // consistency before access. Also causes owning source/filter 
  // (if any) to update itself. The Update() method is composed of 
  // UpdateInformation(), PropagateUpdateExtent(), 
  // TriggerAsynchronousUpdate(), and UpdateData().
  virtual void Update();

  // Description:
  // WARNING: INTERNAL METHOD - NOT FOR GENERAL USE. 
  // THIS METHOD IS PART OF THE PIPELINE UPDATE FUNCTIONALITY.
  // Update all the "easy to update" information about the object such
  // as the extent which will be used to control the update.
  // This propagates all the way up then back down the pipeline.
  // As a by-product the PipelineMTime is updated.
  virtual void UpdateInformation();

  // Description:
  // WARNING: INTERNAL METHOD - NOT FOR GENERAL USE. 
  // THIS METHOD IS PART OF THE PIPELINE UPDATE FUNCTIONALITY.
  // The update extent for this object is propagated up the pipeline.
  // This propagation may early terminate based on the PipelineMTime.
  virtual void PropagateUpdateExtent();

  // Description:
  // WARNING: INTERNAL METHOD - NOT FOR GENERAL USE. 
  // THIS METHOD IS PART OF THE PIPELINE UPDATE FUNCTIONALITY.
  // Propagate back up the pipeline for ports and trigger the update on the
  // other side of the port to allow for asynchronous parallel processing in
  // the pipeline.
  // This propagation may early terminate based on the PipelineMTime.
  virtual void TriggerAsynchronousUpdate();

  // Description:
  // WARNING: INTERNAL METHOD - NOT FOR GENERAL USE. 
  // THIS METHOD IS PART OF THE PIPELINE UPDATE FUNCTIONALITY.
  // Propagate the update back up the pipeline, and perform the actual 
  // work of updating on the way down. When the propagate arrives at a
  // port, block and wait for the asynchronous update to finish on the
  // other side.
  // This propagation may early terminate based on the PipelineMTime.
  virtual void UpdateData();

  // Description:
  // Get the estimated size of this data object itself. Should be called
  // after UpdateInformation() and PropagateUpdateExtent() have both been 
  // called. Should be overridden in a subclass - otherwise the default
  // is to assume that this data object requires no memory.
  // The size is returned in kilobytes.
  virtual unsigned long GetEstimatedMemorySize();

  // Description:
  // A generic way of specifying an update extent.  Subclasses
  // must decide what a piece is.  When the NumberOfPieces is zero, then
  // no data is requested, and the source will not execute.
  virtual void SetUpdateExtent(int piece,int numPieces, int ghostLevel);
  void SetUpdateExtent(int piece, int numPieces)
    {this->SetUpdateExtent(piece, numPieces, 0);}

  // Description:
  // Set the update extent for data objects that use 3D extents. Using this
  // method on data objects that set extents as pieces (such as vtkPolyData or
  // vtkUnstructuredGrid) has no real effect.
  // Don't use the set macro to set the update extent
  // since we don't want this object to be modified just due to
  // a change in update extent. When the volume of the extent is zero (0, -1,..), 
  // then no data is requested, and the source will not execute.
  virtual void SetUpdateExtent(int x0, int x1, int y0, int y1, int z0, int z1);
  virtual void SetUpdateExtent(int extent[6]);
  virtual int* GetUpdateExtent();
  virtual void GetUpdateExtent(int& x0, int& x1, int& y0, int& y1,
                               int& z0, int& z1);
  virtual void GetUpdateExtent(int extent[6]);

  // Description:
  // Return class name of data type. This is one of VTK_STRUCTURED_GRID, 
  // VTK_STRUCTURED_POINTS, VTK_UNSTRUCTURED_GRID, VTK_POLY_DATA, or
  // VTK_RECTILINEAR_GRID (see vtkSetGet.h for definitions).
  // THIS METHOD IS THREAD SAFE
  virtual int GetDataObjectType() {return VTK_DATA_OBJECT;}
  
  // Description:
  // Used by Threaded ports to determine if they should initiate an
  // asynchronous update (still in development).
  unsigned long GetUpdateTime();

  // Description:
  // If the whole input extent is required to generate the requested output
  // extent, this method can be called to set the input update extent to the
  // whole input extent. This method assumes that the whole extent is known
  // (that UpdateInformation has been called)
  void SetUpdateExtentToWholeExtent();

  // Description:
  // Get the cumulative modified time of everything upstream.  Does
  // not include the MTime of this object.
  unsigned long GetPipelineMTime();

  // Description:
  // Return the actual size of the data in kilobytes. This number
  // is valid only after the pipeline has updated. The memory size
  // returned is guaranteed to be greater than or equal to the
  // memory required to represent the data (e.g., extra space in
  // arrays, etc. are not included in the return value).
  virtual unsigned long GetActualMemorySize();

  // Description:
  // Copy the generic information (WholeExtent ...)
  void CopyInformation( vtkDataObject *data );

  // Description:
  // By default, there is no type specific information
  virtual void CopyTypeSpecificInformation( vtkDataObject *data ) 
    {this->CopyInformation( data );};
  
  // Description:
  // Set / Get the update piece and the update number of pieces. Similar
  // to update extent in 3D.
  void SetUpdatePiece(int piece);
  void SetUpdateNumberOfPieces(int num);
  virtual int GetUpdatePiece();
  virtual int GetUpdateNumberOfPieces();
  
  // Description:
  // Set / Get the update ghost level and the update number of ghost levels.
  // Similar to update extent in 3D.
  void SetUpdateGhostLevel(int level);
  virtual int GetUpdateGhostLevel();
  
  // Description:
  // This request flag indicates whether the requester can handle 
  // more data than requested.  Right now it is used in vtkImageData.
  // Image filters can return more data than requested.  The the 
  // consumer cannot handle this (i.e. DataSetToDataSetFitler)
  // the image will crop itself.  This functionality used to be in 
  // ImageToStructuredPoints.
  virtual void SetRequestExactExtent(int flag);
  virtual int GetRequestExactExtent();
  vtkBooleanMacro(RequestExactExtent, int);
  
  // Description:
  // Set/Get the whole extent of this data object.  
  // The whole extent is meta data for structured data sets.
  // It gets set by the source during the update information call.
  virtual void SetWholeExtent(int x0, int x1, int y0, int y1, int z0, int z1);
  virtual void SetWholeExtent(int extent[6]);
  virtual int* GetWholeExtent();
  virtual void GetWholeExtent(int& x0, int& x1, int& y0, int& y1,
                              int& z0, int& z1);
  virtual void GetWholeExtent(int extent[6]);
  
  // Description:
  // Set/Get the whole bounding box of this data object.  
  // The whole whole bounding box is meta data for data sets
  // It gets set by the source during the update information call.
  virtual void SetWholeBoundingBox(double x0, double x1, double y0, 
                                   double y1, double z0, double z1);
  virtual void SetWholeBoundingBox(double bb[6]);
  virtual double* GetWholeBoundingBox();
  virtual void GetWholeBoundingBox(double& x0, double& x1, double& y0, 
                                   double& y1, double& z0, double& z1);
  virtual void GetWholeBoundingBox(double extent[6]);
  
  // Description:
  // Set/Get the maximum number of pieces that can be requested.  
  // The maximum number of pieces is meta data for unstructured data sets.
  // It gets set by the source during the update information call.
  // A value of -1 indicates that there is no maximum.  A value of
  virtual void SetMaximumNumberOfPieces(int);
  virtual int GetMaximumNumberOfPieces();

  // Description:
  // Copy information about this data object to its
  // PipelineInformation from its own Information for the given
  // request.  If the second argument is not NULL then it is the
  // pipeline information object for the input to this data object's
  // producer.
  virtual void CopyInformationToPipeline(vtkInformation* request,
                                         vtkInformation* input);

  // Description:
  // Copy information about this data object from the
  // PipelineInformation to its own Information for the given request.
  virtual void CopyInformationFromPipeline(vtkInformation* request);

  // Description:
  // Return the information object within the input information object's
  // field data corresponding to the specified association 
  // (FIELD_ASSOCIATION_POINTS or FIELD_ASSOCIATION_CELLS) and attribute
  // (SCALARS, VECTORS, NORMALS, TCOORDS, or TENSORS)
  static vtkInformation *GetActiveFieldInformation(vtkInformation *info, 
    int fieldAssociation, int attributeType);

  // Description:
  // Return the information object within the input information object's
  // field data corresponding to the specified association 
  // (FIELD_ASSOCIATION_POINTS or FIELD_ASSOCIATION_CELLS) and name.
  static vtkInformation *GetNamedFieldInformation(vtkInformation *info, 
    int fieldAssociation, const char *name);
  
  // Description:
  // Set the named array to be the active field for the specified type
  // (SCALARS, VECTORS, NORMALS, TCOORDS, or TENSORS) and association
  // (FIELD_ASSOCIATION_POINTS or FIELD_ASSOCIATION_CELLS).  Returns the 
  // active field information object and creates on entry if one not found.
  static vtkInformation *SetActiveAttribute(vtkInformation *info,
    int fieldAssociation, const char *attributeName, int attributeType);

  // Description:
  // Set the name, array type, number of components, and number of tuples
  // within the passed information object for the active attribute of type
  // attributeType (in specified association, FIELD_ASSOCIATION_POINTS or
  // FIELD_ASSOCIATION_CELLS).  If there is not an active attribute of the
  // specified type, an entry in the information object is created.  If
  // arrayType, numComponents, or numTuples equal to -1, or name=NULL the 
  // value is not changed.
  static void SetActiveAttributeInfo(vtkInformation *info, 
    int fieldAssociation, int attributeType, const char *name, int arrayType,
    int numComponents, int numTuples);

  // Description:
  // Convenience version of previous method for use (primarily) by the Imaging
  // filters. If arrayType or numComponents == -1, the value is not changed.
  static void SetPointDataActiveScalarInfo(vtkInformation *info,
    int arrayType, int numComponents);

  // Description:
  // This method is called by the source when it executes to generate data.
  // It is sort of the opposite of ReleaseData.
  // It sets the DataReleased flag to 0, and sets a new UpdateTime.
  void DataHasBeenGenerated();

  // Description:
  // make the output data ready for new data to be inserted. For most 
  // objects we just call Initialize. But for vtkImageData we leave the old
  // data in case the memory can be reused.
  virtual void PrepareForNewData() {this->Initialize();};

  // Description:
  // Shallow and Deep copy.  These copy the data, but not any of the 
  // pipeline connections.
  virtual void ShallowCopy(vtkDataObject *src);  
  virtual void DeepCopy(vtkDataObject *src);

  // Description:
  // An object that will translate pieces into structured extents.
  void SetExtentTranslator(vtkExtentTranslator* translator);
  vtkExtentTranslator* GetExtentTranslator();

  // Description:
  // The ExtentType will be left as VTK_PIECES_EXTENT for data objects 
  // such as vtkPolyData and vtkUnstructuredGrid. The ExtentType will be 
  // changed to VTK_3D_EXTENT for data objects with 3D structure such as 
  // vtkImageData (and its subclass vtkStructuredPoints), vtkRectilinearGrid,
  // and vtkStructuredGrid. The default is the have an extent in pieces,
  // with only one piece (no streaming possible).
  virtual int GetExtentType() { return VTK_PIECES_EXTENT; };

  // Description:
  // This method crops the data object (if necesary) so that the extent
  // matches the update extent.
  virtual void Crop();

  //BTX
  // Description:
  // Possible values for the FIELD_ASSOCIATION information entry.
  enum FieldAssociations
  {
    FIELD_ASSOCIATION_POINTS,
    FIELD_ASSOCIATION_CELLS,
    FIELD_ASSOCIATION_NONE
  };
  //ETX

  //BTX
  // Description:
  // Possible values for the FIELD_OPERATION information entry.
  enum FieldOperations
  {
    FIELD_OPERATION_PRESERVED,
    FIELD_OPERATION_REINTERPOLATED,
    FIELD_OPERATION_MODIFIED,
    FIELD_OPERATION_REMOVED
  };
  //ETX

  static vtkInformationStringKey* DATA_TYPE_NAME();
  static vtkInformationDataObjectKey* DATA_OBJECT();
  static vtkInformationIntegerKey* DATA_EXTENT_TYPE();
  static vtkInformationIntegerVectorKey* DATA_EXTENT();
  static vtkInformationIntegerKey* DATA_PIECE_NUMBER();
  static vtkInformationIntegerKey* DATA_NUMBER_OF_PIECES();
  static vtkInformationIntegerKey* DATA_NUMBER_OF_GHOST_LEVELS();
  static vtkInformationIntegerKey* DATA_TIME_INDEX();
  static vtkInformationDoubleKey* DATA_TIME();
  static vtkInformationInformationVectorKey* POINT_DATA_VECTOR();
  static vtkInformationInformationVectorKey* CELL_DATA_VECTOR();
  static vtkInformationIntegerKey* FIELD_ARRAY_TYPE();
  static vtkInformationIntegerKey* FIELD_ASSOCIATION();
  static vtkInformationIntegerKey* FIELD_ATTRIBUTE_TYPE();
  static vtkInformationIntegerKey* FIELD_ACTIVE_ATTRIBUTE();
  static vtkInformationIntegerKey* FIELD_NUMBER_OF_COMPONENTS();
  static vtkInformationIntegerKey* FIELD_NUMBER_OF_TUPLES();
  static vtkInformationIntegerKey* FIELD_OPERATION();
  static vtkInformationStringKey* FIELD_NAME();
  static vtkInformationDoubleVectorKey* ORIGIN();
  static vtkInformationDoubleVectorKey* SPACING();

protected:

  vtkDataObject();
  ~vtkDataObject();

  // General field data associated with data object      
  vtkFieldData  *FieldData;  

  // Who generated this data as output?
  vtkSource     *Source;     

  // Keep track of data release during network execution
  int DataReleased; 

  // When was this data last generated?
  vtkTimeStamp UpdateTime;  

  // Get the executive that manages this data object.
  vtkExecutive* GetExecutive();

  // Get the port number producing this data object.
  int GetPortNumber();

  virtual void ReportReferences(vtkGarbageCollector*);

  // Arbitrary extra information associated with this data object.
  vtkInformation* Information;

  // Reference the pipeline information object that owns this data
  // object.
  vtkInformation* PipelineInformation;

  //BTX
  // Check whether this data object is owned by a vtkStreamingDemandDrivenPipeline.
  vtkStreamingDemandDrivenPipeline* TrySDDP(const char* method);
  typedef vtkStreamingDemandDrivenPipeline SDDP;
  //ETX

  //BTX
  friend class vtkStreamingDemandDrivenPipelineToDataObjectFriendship;
  //ETX
private:
  // Helper method for the ShallowCopy and DeepCopy methods.
  void InternalDataObjectCopy(vtkDataObject *src);

private:
  vtkDataObject(const vtkDataObject&);  // Not implemented.
  void operator=(const vtkDataObject&);  // Not implemented.
};

#endif

