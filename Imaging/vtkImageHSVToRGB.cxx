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
#include <math.h>
#include "vtkImageHSVToRGB.h"



//----------------------------------------------------------------------------
vtkImageHSVToRGB::vtkImageHSVToRGB()
{
  this->Maximum = 255.0;
}


//----------------------------------------------------------------------------
// Description:
// This templated function executes the filter for any type of data.
template <class T>
static void vtkImageHSVToRGBExecute(vtkImageHSVToRGB *self,
				    vtkImageData *inData, T *inPtr,
				    vtkImageData *outData, T *outPtr,
				    int outExt[6], int id)
{
  int idxX, idxY, idxZ;
  int maxX, maxY, maxZ;
  int inIncX, inIncY, inIncZ;
  int outIncX, outIncY, outIncZ;
  unsigned long count = 0;
  unsigned long target;
  float R, G, B, H, S, V;
  float max = self->GetMaximum();
  float temp;
  float third = max / 3.0;;
  
  // find the region to loop over
  maxX = outExt[1] - outExt[0]; 
  maxY = outExt[3] - outExt[2]; 
  maxZ = outExt[5] - outExt[4];
  target = (unsigned long)((maxZ+1)*(maxY+1)/50.0);
  target++;
  
  // Get increments to march through data 
  inData->GetContinuousIncrements(outExt, inIncX, inIncY, inIncZ);
  outData->GetContinuousIncrements(outExt, outIncX, outIncY, outIncZ);

  // Loop through ouput pixels
  for (idxZ = 0; idxZ <= maxZ; idxZ++)
    {
    for (idxY = 0; idxY <= maxY; idxY++)
      {
      if (!id) 
	{
	if (!(count%target))
	  {
	  self->UpdateProgress(count/(50.0*target));
	  }
	count++;
	}
      for (idxX = 0; idxX <= maxX; idxX++)
	{
	// Pixel operation
	H = (float)(*inPtr); inPtr++;
	S = (float)(*inPtr); inPtr++;
	V = (float)(*inPtr); inPtr++;

	// compute rgb assuming S = 1.0;
	if (H >= 0.0 && H <= third) // red -> green
	  {
	  G = H/third;
	  R = 1.0 - G;
	  B = 0.0;
	  }
	else if (H >= third && H <= 2.0*third) // green -> blue
	  {
	  B = (H - third)/third;
	  G = 1.0 - B;
	  R = 0.0;
	  }
	else // blue -> red
	  {
	  R = (H - 2.0 * third)/third;
	  B = 1.0 - R;
	  G = 0.0;
	  }
	
	// add Saturation to the equation.
	S = S / max;
	//R = S + (1.0 - S)*R;
	//G = S + (1.0 - S)*G;
	//B = S + (1.0 - S)*B;
	// what happend to this?
	R = S*R + (1.0 - S);
	G = S*G + (1.0 - S);
	B = S*B + (1.0 - S);
	
	// Use value to get actual RGB 
	// normalize RGB first then apply value
	temp = R + G + B; 
	//V = 3 * V / (temp * max);
	// and what happend to this?
	V = 3 * V / (temp);
	R = R * V;
	G = G * V;
	B = B * V;
	
	// clip below 255
	//if (R > 255.0) R = max;
	//if (G > 255.0) G = max;
	//if (B > 255.0) B = max;
	// mixed constant 255 and max ?????
	if (R > max)
	  {
	  R = max;
	  }
	if (G > max)
	  {
	  G = max;
	  }
	if (B > max)
	  {
	  B = max;
	  }
	
	// assign output.
	*outPtr = (T)(R); outPtr++;
	*outPtr = (T)(G); outPtr++;
	*outPtr = (T)(B); outPtr++;
	}
      outPtr += outIncY;
      inPtr += inIncY;
      }
    outPtr += outIncZ;
    inPtr += inIncZ;
    }
}

//----------------------------------------------------------------------------
void vtkImageHSVToRGB::ThreadedExecute(vtkImageData *inData, 
				       vtkImageData *outData,
				       int outExt[6], int id)
{
  void *inPtr = inData->GetScalarPointerForExtent(outExt);
  void *outPtr = outData->GetScalarPointerForExtent(outExt);
  
  vtkDebugMacro(<< "Execute: inData = " << inData 
		<< ", outData = " << outData);
  
  // this filter expects that input is the same type as output.
  if (inData->GetScalarType() != outData->GetScalarType())
    {
    vtkErrorMacro(<< "Execute: input ScalarType, " << inData->GetScalarType()
                  << ", must match out ScalarType " << outData->GetScalarType());
    return;
    }
  
  // need three components for input and output
  if (inData->GetNumberOfScalarComponents() != 3)
    {
    vtkErrorMacro("Input has too few components");
    return;
    }
  if (outData->GetNumberOfScalarComponents() != 3)
    {
    vtkErrorMacro("Output has too few components");
    return;
    }

  switch (inData->GetScalarType())
    {
    case VTK_FLOAT:
      vtkImageHSVToRGBExecute(this, 
			      inData, (float *)(inPtr), 
			      outData, (float *)(outPtr), outExt, id);
      break;
    case VTK_INT:
      vtkImageHSVToRGBExecute(this, 
			      inData, (int *)(inPtr), 
			      outData, (int *)(outPtr), outExt, id);
      break;
    case VTK_SHORT:
      vtkImageHSVToRGBExecute(this, 
			      inData, (short *)(inPtr), 
			      outData, (short *)(outPtr), outExt, id);
      break;
    case VTK_UNSIGNED_SHORT:
      vtkImageHSVToRGBExecute(this, 
			      inData, (unsigned short *)(inPtr), 
			      outData, (unsigned short *)(outPtr), 
			      outExt, id);
      break;
    case VTK_UNSIGNED_CHAR:
      vtkImageHSVToRGBExecute(this, 
			      inData, (unsigned char *)(inPtr), 
			      outData, (unsigned char *)(outPtr), 
			      outExt, id);
      break;
    default:
      vtkErrorMacro(<< "Execute: Unknown ScalarType");
      return;
    }
}

void vtkImageHSVToRGB::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkImageFilter::PrintSelf(os,indent);

  os << indent << "Maximum: " << this->Maximum << "\n";
}

