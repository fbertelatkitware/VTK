/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile$
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Thanks:    Thanks to C. Charles Law who developed this class.

Copyright (c) 1993-1995 Ken Martin, Will Schroeder, Bill Lorensen.

This software is copyrighted by Ken Martin, Will Schroeder and Bill Lorensen.
The following terms apply to all files associated with the software unless
explicitly disclaimed in individual files. This copyright specifically does
not apply to the related textbook "The Visualization Toolkit" ISBN
013199837-4 published by Prentice Hall which is covered by its own copyright.

The authors hereby grant permission to use, copy, and distribute this
software and its documentation for any purpose, provided that existing
copyright notices are retained in all copies and that this notice is included
verbatim in any distributions. Additionally, the authors grant permission to
modify this software and its documentation for any purpose, provided that
such modifications are not distributed without the explicit consent of the
authors and that existing copyright notices are retained in all copies. Some
of the algorithms implemented by this software are patented, observe all
applicable patent law.

IN NO EVENT SHALL THE AUTHORS OR DISTRIBUTORS BE LIABLE TO ANY PARTY FOR
DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
OF THE USE OF THIS SOFTWARE, ITS DOCUMENTATION, OR ANY DERIVATIVES THEREOF,
EVEN IF THE AUTHORS HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

THE AUTHORS AND DISTRIBUTORS SPECIFICALLY DISCLAIM ANY WARRANTIES, INCLUDING,
BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
PARTICULAR PURPOSE, AND NON-INFRINGEMENT.  THIS SOFTWARE IS PROVIDED ON AN
"AS IS" BASIS, AND THE AUTHORS AND DISTRIBUTORS HAVE NO OBLIGATION TO PROVIDE
MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.


=========================================================================*/
#include "vtkImageFlip.h"



//----------------------------------------------------------------------------
vtkImageFlip::vtkImageFlip()
{
  this->SetAxes(VTK_IMAGE_X_AXIS);
  this->Dimensionality = 1;
  this->ExecuteDimensionality = 2;
  this->PreserveImageExtent = 0;
}

//----------------------------------------------------------------------------
// Description:
// Image extent is modified by this filter.
void vtkImageFlip::ComputeOutputImageInformation(vtkImageRegion *inRegion,
						 vtkImageRegion *outRegion)
{
  int min, max;

  if ( ! this->PreserveImageExtent)
    {
    inRegion->GetImageExtent(min ,max);
    outRegion->SetImageExtent(-max, -min);
    }
}


//----------------------------------------------------------------------------
// Description:
// What input should be requested.
void vtkImageFlip::ComputeRequiredInputRegionExtent(vtkImageRegion *outRegion, 
						    vtkImageRegion *inRegion)
{
  int min, max;
  int imageMin, imageMax;
  
  outRegion->GetExtent(min, max);
  outRegion->GetImageExtent(imageMin, imageMax);
  if (this->PreserveImageExtent)
    {
    inRegion->SetExtent(-max+imageMax+imageMin, -min+imageMax+imageMin);
    }
  else
    {
    inRegion->SetExtent(-max, -min);
    }
}


//----------------------------------------------------------------------------
// Description:
// This templated function executes the filter for any type of data.
template <class IT, class OT>
static void vtkImageFlipExecute(vtkImageFlip *self,
			 vtkImageRegion *inRegion, IT *inPtr,
			 vtkImageRegion *outRegion, OT *outPtr){
  int min0, max0, min1, max1;
  int idx0, idx1;
  int inInc0, inInc1;
  int outInc0, outInc1;
  IT  *inPtr0, *inPtr1;
  OT  *outPtr0, *outPtr1;

  self = self;
  outPtr = outPtr;
  
  // Get information to march through data 
  inRegion->GetIncrements(inInc0, inInc1);
  outRegion->GetIncrements(outInc0, outInc1);
  outRegion->GetExtent(min0, max0, min1, max1);

  // Loop through ouput pixels
  inPtr1 = inPtr;
  outPtr1 = (OT *)(outRegion->GetScalarPointer(max0, min1));
  for (idx1 = min1; idx1 <= max1; ++idx1){
    outPtr0 = outPtr1;
    inPtr0 = inPtr1;
    for (idx0 = min0; idx0 <= max0; ++idx0){
        *outPtr0 = (OT)(*inPtr0);
        outPtr0 -= outInc0;
        inPtr0  += inInc0;
    }
    outPtr1 += outInc1;
    inPtr1 += inInc1;
  }
}



//----------------------------------------------------------------------------
template <class T>
static void vtkImageFlipExecute(vtkImageFlip *self,
			 vtkImageRegion *inRegion, T *inPtr,
			 vtkImageRegion *outRegion)
{
  void *outPtr = outRegion->GetScalarPointer();
  switch (outRegion->GetScalarType())
    {
    case VTK_FLOAT:
      vtkImageFlipExecute(self, inRegion, (T *)(inPtr), 
			  outRegion, (float *)(outPtr));
      break;
    case VTK_INT:
      vtkImageFlipExecute(self, inRegion, (T *)(inPtr), 
			  outRegion, (int *)(outPtr)); 
      break;
    case VTK_SHORT:
      vtkImageFlipExecute(self, inRegion, (T *)(inPtr), 
			  outRegion, (short *)(outPtr));
      break;
    case VTK_UNSIGNED_SHORT:
      vtkImageFlipExecute(self, inRegion, (T *)(inPtr), 
			  outRegion, (unsigned short *)(outPtr)); 
      break;
    case VTK_UNSIGNED_CHAR:
      vtkImageFlipExecute(self, inRegion, (T *)(inPtr), 
			  outRegion, (unsigned char *)(outPtr)); 
      break;
    default:
      vtkGenericWarningMacro("Execute: Unknown output ScalarType");
      return;
    }
}

//----------------------------------------------------------------------------
// Description:
// This method is passed a input and output region, and executes the filter
// algorithm to fill the output from the input.
// It just executes a switch statement to call the correct function for
// the regions data types.
void vtkImageFlip::Execute(vtkImageRegion *inRegion, 
				vtkImageRegion *outRegion) {
  void *inPtr = inRegion->GetScalarPointer();
  
  vtkDebugMacro(<< "Execute: inRegion = " << inRegion 
		<< ", outRegion = " << outRegion);

  switch (inRegion->GetScalarType())
    {
    case VTK_FLOAT:
      vtkImageFlipExecute(this, inRegion, (float *)(inPtr), outRegion);
      break;
    case VTK_INT:
      vtkImageFlipExecute(this, inRegion, (int *)(inPtr), outRegion);
      break;
    case VTK_SHORT:
      vtkImageFlipExecute(this, inRegion, (short *)(inPtr), outRegion);
      break;
    case VTK_UNSIGNED_SHORT:
      vtkImageFlipExecute(this, inRegion, (unsigned short *)(inPtr), 
			  outRegion);
      break;
    case VTK_UNSIGNED_CHAR:
      vtkImageFlipExecute(this, inRegion, (unsigned char *)(inPtr), outRegion);
      break;
    default:
      vtkErrorMacro(<< "Execute: Unknown input ScalarType");
      return;
    }
}
















