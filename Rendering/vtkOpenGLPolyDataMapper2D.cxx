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
#include "vtkOpenGLPolyDataMapper2D.h"

#include "vtkActor2D.h"
#include "vtkCellArray.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkPlane.h"
#include "vtkPlaneCollection.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkProperty2D.h"
#include "vtkScalarsToColors.h"
#include "vtkUnsignedCharArray.h"
#include "vtkViewport.h"
#include "vtkWindow.h"
#include "vtkgluPickMatrix.h"
#include "vtkToolkits.h"  // for VTK_USE_GL2PS

#ifdef VTK_USE_GL2PS
#include "gl2ps.h"
#include "vtkGL2PSExporter.h"
#endif // VTK_USE_GL2PS

#include <math.h>

#ifndef VTK_IMPLEMENT_MESA_CXX
vtkCxxRevisionMacro(vtkOpenGLPolyDataMapper2D, "$Revision$");
vtkStandardNewMacro(vtkOpenGLPolyDataMapper2D);
#endif

void vtkOpenGLPolyDataMapper2D::RenderOverlay(vtkViewport* viewport,
                                              vtkActor2D* actor)
{
  int            numPts;
  vtkPolyData    *input= (vtkPolyData *)this->Input;
  int            j;
  vtkPoints      *p, *displayPts;
  vtkCellArray   *aPrim;
  vtkUnsignedCharArray *c=NULL;
  unsigned char  *rgba;
  unsigned char  color[4];
  vtkIdType      *pts = 0;
  vtkIdType      npts = 0;
  int            cellScalars = 0;
  int            cellNum = 0;
  vtkPlaneCollection *clipPlanes;
  vtkPlane           *plane;
  int                 i,numClipPlanes;
  double              planeEquation[4];

  vtkDebugMacro (<< "vtkOpenGLPolyDataMapper2D::Render");

  if ( input == NULL ) 
    {
    vtkErrorMacro(<< "No input!");
    return;
    }
  else
    {
    input->Update();
    numPts = input->GetNumberOfPoints();
    } 

  if (numPts == 0)
    {
    vtkDebugMacro(<< "No points!");
    return;
    }
  
  if ( this->LookupTable == NULL )
    {
    this->CreateDefaultLookupTable();
    }

  // if something has changed regenrate colors and display lists
  // if required
  //
  if ( this->GetMTime() > this->BuildTime || 
       input->GetMTime() > this->BuildTime || 
       this->LookupTable->GetMTime() > this->BuildTime ||
       actor->GetProperty()->GetMTime() > this->BuildTime)
    {
    // sets this->Colors as side effect
    this->MapScalars(actor->GetProperty()->GetOpacity());
    this->BuildTime.Modified();
    }

  // Get the position of the actor
  int size[2];
  size[0] = viewport->GetSize()[0];
  size[1] = viewport->GetSize()[1];
  float *vport = viewport->GetViewport();
  int* actorPos = 
    actor->GetPositionCoordinate()->GetComputedViewportValue(viewport);

  // get window info
  float *tileViewPort = viewport->GetVTKWindow()->GetTileViewport();
  float visVP[4];
  visVP[0] = (vport[0] >= tileViewPort[0]) ? vport[0] : tileViewPort[0];
  visVP[1] = (vport[1] >= tileViewPort[1]) ? vport[1] : tileViewPort[1];
  visVP[2] = (vport[2] <= tileViewPort[2]) ? vport[2] : tileViewPort[2];
  visVP[3] = (vport[3] <= tileViewPort[3]) ? vport[3] : tileViewPort[3];
  if (visVP[0] >= visVP[2])
    {
    return;
    }
  if (visVP[1] >= visVP[3])
    {
    return;
    }
  size[0] = 
    vtkMath::Round(size[0]*(visVP[2] - visVP[0])/(vport[2] - vport[0]));
  size[1] = 
    vtkMath::Round(size[1]*(visVP[3] - visVP[1])/(vport[3] - vport[1]));
  
  // Set up the font color from the text actor
  float*  actorColor = actor->GetProperty()->GetColor();
  color[0] = (unsigned char) (actorColor[0] * 255.0);
  color[1] = (unsigned char) (actorColor[1] * 255.0);
  color[2] = (unsigned char) (actorColor[2] * 255.0);
  color[3] = (unsigned char) (255.0*actor->GetProperty()->GetOpacity());

  // Transform the points, if necessary
  p = input->GetPoints();
  if ( this->TransformCoordinate )
    {
    int *itmp;
    numPts = p->GetNumberOfPoints();
    displayPts = vtkPoints::New();
    displayPts->SetNumberOfPoints(numPts);
    for ( j=0; j < numPts; j++ )
      {
      this->TransformCoordinate->SetValue(p->GetPoint(j));
      itmp = this->TransformCoordinate->GetComputedViewportValue(viewport);
      displayPts->SetPoint(j,itmp[0], itmp[1], 0.0);
      }
    p = displayPts;
    }

  // Set up the coloring
  if ( this->Colors )
    {
    c = this->Colors;
    if (!input->GetPointData()->GetScalars())
      {
      cellScalars = 1;
      }
    }
  vtkDebugMacro(<< c);
  vtkDebugMacro(<< cellScalars);

  // set the colors for the foreground
  glColor4ubv(color);

  // push a 2D matrix on the stack
  glMatrixMode( GL_PROJECTION );
  glPushMatrix();
  glLoadIdentity();
  if(viewport->GetIsPicking())
    {
    vtkgluPickMatrix(viewport->GetPickX(), viewport->GetPickY(),
                     1, 1, viewport->GetOrigin(), viewport->GetSize());
    }
  
  glMatrixMode( GL_MODELVIEW );
  glPushMatrix();
  glLoadIdentity();

  glDisable( GL_TEXTURE_2D );
  glDisable( GL_LIGHTING );
  
  // Assume we want to do Zbuffering for now.
  // we may turn this off later
  glDepthMask(GL_TRUE);

  int *winSize = viewport->GetVTKWindow()->GetSize();
  
  int xoff = static_cast<int>(actorPos[0] - (visVP[0] - vport[0])*
                              winSize[0]);
  int yoff = static_cast<int>(actorPos[1] - (visVP[1] - vport[1])*
                              winSize[1]);

  if ( actor->GetProperty()->GetDisplayLocation() == 
       VTK_FOREGROUND_LOCATION )
    {
    glOrtho(-xoff,-xoff + size[0],
            -yoff, -yoff +size[1], 0, 1);
    }  
  else
    {
    glOrtho(-xoff,-xoff + size[0],
            -yoff, -yoff + size[1], -1, 0);
    }
    
  // Clipping plane stuff

  clipPlanes = this->ClippingPlanes;

  if (clipPlanes == NULL)
    {
    numClipPlanes = 0;
    }
  else
    {
    numClipPlanes = clipPlanes->GetNumberOfItems();
    if (numClipPlanes > 4)
      {
      vtkErrorMacro(<< "Only 4 clipping planes are used with 2D mappers");
      }
    }

  for (i = 0; i < numClipPlanes; i++)
    {
    glEnable((GLenum)(GL_CLIP_PLANE0+i));
    }

  for (i = 0; i < numClipPlanes; i++)
    {
    plane = (vtkPlane *)clipPlanes->GetItemAsObject(i);

    planeEquation[0] = plane->GetNormal()[0];
    planeEquation[1] = plane->GetNormal()[1];
    planeEquation[2] = plane->GetNormal()[2];
    planeEquation[3] = -(planeEquation[0]*plane->GetOrigin()[0]+
                         planeEquation[1]*plane->GetOrigin()[1]+
                         planeEquation[2]*plane->GetOrigin()[2]);
    glClipPlane((GLenum)(GL_CLIP_PLANE0+i),planeEquation);
    }

  aPrim = input->GetPolys();
  for (aPrim->InitTraversal(); aPrim->GetNextCell(npts,pts); cellNum++)
    { 
    glBegin(GL_POLYGON);
    for (j = 0; j < npts; j++) 
      {
      if (c) 
        {
        if (cellScalars)
          {
          rgba = c->GetPointer(4*cellNum);
          }
        else
          {
          rgba = c->GetPointer(4*pts[j]);
          }
        glColor4ubv(rgba);
        }
      glVertex2dv(p->GetPoint(pts[j]));
      }
    glEnd();
    }

  // Set pointsize and linewidth for GL2PS output.
#ifdef VTK_USE_GL2PS
  gl2psPointSize(actor->GetProperty()->GetPointSize()*
                 vtkGL2PSExporter::GetGlobalPointSizeFactor());
  gl2psLineWidth(actor->GetProperty()->GetLineWidth()*
                 vtkGL2PSExporter::GetGlobalLineWidthFactor());
#endif // VTK_USE_GL2PS

  // Set the LineWidth
  glLineWidth(actor->GetProperty()->GetLineWidth());

  // Set the LineStipple
  if (actor->GetProperty()->GetLineStipplePattern() != 0xFFFF)
    {
    glEnable (GL_LINE_STIPPLE);
#ifdef VTK_USE_GL2PS
    gl2psEnable(GL2PS_LINE_STIPPLE);
#endif // VTK_USE_GL2PS
    glLineStipple (actor->GetProperty()->GetLineStippleRepeatFactor(), 
                   actor->GetProperty()->GetLineStipplePattern());
    }
  else
    {
    glDisable (GL_LINE_STIPPLE);
#ifdef VTK_USE_GL2PS
    gl2psDisable(GL2PS_LINE_STIPPLE);
#endif // VTK_USE_GL2PS
    }

  aPrim = input->GetLines();
  for (aPrim->InitTraversal(); aPrim->GetNextCell(npts,pts); cellNum++)
    {
    glBegin(GL_LINE_STRIP);
    for (j = 0; j < npts; j++)
      {
      if (c)
        {
        if (cellScalars)
          {
          rgba = c->GetPointer(4*cellNum);
          }
        else
          {
          rgba = c->GetPointer(4*pts[j]);
          }
        glColor4ubv(rgba);
        }
      glVertex2dv(p->GetPoint(pts[j]));
      }
    glEnd();
    }

  // Set the PointSize
  glPointSize(actor->GetProperty()->GetPointSize());

  aPrim = input->GetVerts();
  glBegin(GL_POINTS);
  for (aPrim->InitTraversal(); aPrim->GetNextCell(npts,pts); cellNum++)
    {
    for (j = 0; j < npts; j++)
      {
      if (c)
        {
        if (cellScalars)
          {
          rgba = c->GetPointer(4*cellNum);
          }
        else
          {
          rgba = c->GetPointer(4*pts[j]);
          }
        glColor4ubv(rgba);
        }
      glVertex2dv(p->GetPoint(pts[j]));
      }
    }
  glEnd();

  if ( this->TransformCoordinate )
    {
    p->Delete();
    }

  for (i = 0; i < numClipPlanes; i++)
    {
    glDisable((GLenum)(GL_CLIP_PLANE0+i));
    }

  // push a 2D matrix on the stack
  glMatrixMode( GL_PROJECTION);
  glPopMatrix();
  glMatrixMode( GL_MODELVIEW);
  glPopMatrix();
  glEnable( GL_LIGHTING);
  
  // Turn it back on in case we've turned it off
  glDepthMask( GL_TRUE );
}
  
//----------------------------------------------------------------------------
void vtkOpenGLPolyDataMapper2D::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
