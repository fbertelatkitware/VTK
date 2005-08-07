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
// .NAME vtkCocoaRenderWindowInteractor - implements Cocoa specific functions
// required by vtkRenderWindowInteractor.
//
// .SECTION Description
// The interactor interfaces with vtkCocoaRenderWindow and vtkCocoaGLView
// to trap messages from the Cocoa window manager and send them to vtk.

// IMPORTANT: This header must be in C++ only because it is included by .cxx files.
// That means no Objective C may be used. That's why some instance variables are
// void* instead of what they really should be.  Pitty.

#ifndef __vtkCocoaRenderWindowInteractor_h
#define __vtkCocoaRenderWindowInteractor_h

#include "vtkRenderWindowInteractor.h"


class VTK_RENDERING_EXPORT vtkCocoaRenderWindowInteractor : public vtkRenderWindowInteractor
{
public:
  // Description:
  // Construct object so that light follows camera motion.
  static vtkCocoaRenderWindowInteractor *New();

  vtkTypeRevisionMacro(vtkCocoaRenderWindowInteractor,vtkRenderWindowInteractor);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Initialize the even handler
  virtual void Initialize();

  // Description:
  // Enable/Disable interactions.  By default interactors are enabled when
  // initialized.  Initialize() must be called prior to enabling/disabling
  // interaction. These methods are used when a window/widget is being
  // shared by multiple renderers and interactors.  This allows a "modal"
  // display where one interactor is active when its data is to be displayed
  // and all other interactors associated with the widget are disabled
  // when their data is not displayed.
  virtual void Enable();
  virtual void Disable();

  // Description:
  // This will start up the event loop and never return. If you
  // call this method it will loop processing events until the
  // application is exited.
  virtual void Start();

  // Description:
  // By default the interactor installs a MessageProc callback which
  // intercepts windows messages to the window and controls interactions.
  // MFC or BCB programs can prevent this and instead directly route any mouse/key
  // messages into the event bindings by setting InstallMessgeProc to false.
  vtkSetMacro(InstallMessageProc,int);
  vtkGetMacro(InstallMessageProc,int);
  vtkBooleanMacro(InstallMessageProc,int);

  // Description:
  // Cocoa specific application terminate, calls ClassExitMethod then
  // calls PostQuitMessage(0) to terminate app. An application can Specify
  // ExitMethod for alternative behaviour (i.e. suppresion of keyboard exit)
  void TerminateApp();

  // Description:
  // Cocoa timer methods
  int CreateTimer(int timertype);
  int DestroyTimer();

  // Description:
  // Methods to set the default exit method for the class. This method is
  // only used if no instance level ExitMethod has been defined.  It is
  // provided as a means to control how an interactor is exited given
  // the various language bindings (tcl, Cocoa, etc.).
  static void SetClassExitMethod(void (*f)(void *), void *arg);
  static void SetClassExitMethodArgDelete(void (*f)(void *));

  // Description:
  // These methods correspond to the the Exit, User and Pick
  // callbacks. They allow for the Style to invoke them.
  virtual void ExitCallback();
  
//  int GetButtonDown();
//  void SetButtonDown(int button);

protected:
  vtkCocoaRenderWindowInteractor();
  ~vtkCocoaRenderWindowInteractor();

  int     InstallMessageProc;
  void*   Timer;  // really a vtkCocoaTimer*

  //BTX
  // Description:
  // Class variables so an exit method can be defined for this class
  // (used to set different exit methods for various language bindings,
  // i.e. tcl, java, Cocoa)
  static void (*ClassExitMethod)(void *);
  static void (*ClassExitMethodArgDelete)(void *);
  static void *ClassExitMethodArg;
  //ETX
  
private:
  vtkCocoaRenderWindowInteractor(const vtkCocoaRenderWindowInteractor&);  // Not implemented.
  void operator=(const vtkCocoaRenderWindowInteractor&);  // Not implemented.
};

#endif
