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

// .NAME vtkMark - base class for items that are part of a vtkContextScene.
//
// .SECTION Description
// Derive from this class to create custom items that can be added to a
// vtkContextScene.

#ifndef __vtkMark_h
#define __vtkMark_h

#include "vtkContextItem.h"

#include "vtkTable.h"
#include "vtkAbstractArray.h"

class vtkMark;
class vtkPanelMark;

class vtkDataElement
{
public:
  vtkDataElement() :
    Table(NULL), AbstractArray(NULL), Index(-1), Type(SCALAR), Dimension(0) { }
  vtkDataElement(vtkVariant v) :
    Scalar(v), Table(NULL), AbstractArray(NULL), Index(-1), Type(SCALAR), Dimension(0) { }
  vtkDataElement(vtkTable* table) :
    Table(table), AbstractArray(NULL), Index(-1), Type(TABLE), Dimension(0) { }
  vtkDataElement(vtkTable* table, vtkIdType row) :
    Table(table), AbstractArray(NULL), Index(row), Type(TABLE_ROW), Dimension(0) { }
  vtkDataElement(vtkAbstractArray* arr) :
    Table(NULL), AbstractArray(arr), Index(-1), Type(ABSTRACT_ARRAY), Dimension(0) { }
  vtkDataElement(vtkAbstractArray* arr, vtkIdType index, int type) :
    Table(NULL), AbstractArray(arr), Index(index), Type(type), Dimension(0) { }

  enum {
    INVALID,
    TABLE,
    TABLE_ROW,
    ABSTRACT_ARRAY,
    ABSTRACT_ARRAY_TUPLE,
    ABSTRACT_ARRAY_COMPONENT,
    SCALAR
    };

  void SetDimension(int dim)
    {
    this->Dimension = dim;
    }

  vtkIdType GetNumberOfChildren();
  vtkDataElement GetChild(vtkIdType i);
  vtkVariant GetValue(vtkIdType i = 0);
  vtkVariant GetValue(std::string str);

protected:
  int Type;
  int Dimension;

  vtkTable* Table;
  vtkAbstractArray* AbstractArray;
  vtkIdType Index;
  vtkVariant Scalar;
};

template <typename T>
class vtkValue
{
public:
  typedef T (*FunctionType)(vtkMark*, vtkDataElement&);
  vtkValue() : Function(NULL) { }
  vtkValue(FunctionType f) : Function(f) { }
  vtkValue(T v) : Constant(v), Function(NULL) { }
  bool IsConstant()
    { return this->Function == NULL; }
  T GetConstant()
    { return this->Constant; }
  FunctionType GetFunction()
    { return this->Function; }
protected:
  T Constant;
  FunctionType Function;
};

template <typename T>
class vtkValueHolder
{
public:
  vtkValueHolder() : Dirty(true), Set(false) { }

  void UnsetValue()
    { Set = false; }
  void SetValue(vtkValue<T> v)
    { Dirty = true; Set = true; Value = v; }
  vtkValue<T>& GetValue()
    { return Value; }

  T* GetArray(vtkMark* m)
    {
    this->Update(m);
    if (this->Cache.size() == 0)
      {
      return NULL;
      }
    return &(this->Cache[0]);
    }

  T GetConstant(vtkMark* m)
    {
    this->Update(m);
    if (this->Cache.size() > 0)
      {
      return this->Cache[0];
      }
    return this->Value.GetConstant();
    }

  bool IsSet()
    {
    return this->Set;
    }

  bool IsDirty()
    {
    return this->Dirty;
    }

  void SetDirty(bool b)
    {
    this->Dirty = b;
    }

protected:
  void Update(vtkMark* m);

  vtkValue<T> Value;
  std::vector<T> Cache;
  bool Dirty;
  bool Set;
};

class vtkColor
{
public:
  vtkColor() :
    Red(0.0), Green(0.0), Blue(0.0), Alpha(0.0) { }
  vtkColor(double r, double g, double b) :
    Red(r), Green(g), Blue(b), Alpha(0.0) { }
  vtkColor(double r, double g, double b, double a) :
    Red(r), Green(g), Blue(b), Alpha(a) { }
  double Red;
  double Green;
  double Blue;
  double Alpha;
};

class VTK_CHARTS_EXPORT vtkMark : public vtkContextItem
{
public:
  vtkTypeRevisionMacro(vtkMark, vtkContextItem);
  virtual void PrintSelf(ostream &os, vtkIndent indent);

  virtual void Extend(vtkMark* m);

  void SetData(vtkDataElement data)
    {
      this->Data.SetValue(vtkValue<vtkDataElement>(data));
      this->DataChanged();
    }
  vtkDataElement GetData()
    { return this->Data.GetConstant(this); }

  void SetLeft(vtkValue<double> v)
    { this->Left.SetValue(v); }
  vtkValue<double>& GetLeft()
    { return this->Left.GetValue(); }

  void SetRight(vtkValue<double> v)
    { this->Right.SetValue(v); }
  vtkValue<double>& GetRight()
    { return this->Right.GetValue(); }

  void SetTop(vtkValue<double> v)
    { this->Top.SetValue(v); }
  vtkValue<double>& GetTop()
    { return this->Top.GetValue(); }

  void SetBottom(vtkValue<double> v)
    { this->Bottom.SetValue(v); }
  vtkValue<double>& GetBottom()
    { return this->Bottom.GetValue(); }

  void SetTitle(vtkValue<std::string> v)
    { this->Title.SetValue(v); }
  vtkValue<std::string>& GetTitle()
    { return this->Title.GetValue(); }

  void SetLineColor(vtkValue<vtkColor> v)
    { this->LineColor.SetValue(v); }
  vtkValue<vtkColor>& GetLineColor()
    { return this->LineColor.GetValue(); }

  void SetFillColor(vtkValue<vtkColor> v)
    { this->FillColor.SetValue(v); }
  vtkValue<vtkColor>& GetFillColor()
    { return this->FillColor.GetValue(); }

  void SetLineWidth(vtkValue<double> v)
    { this->LineWidth.SetValue(v); }
  vtkValue<double>& GetLineWidth()
    { return this->LineWidth.GetValue(); }

  void SetParent(vtkPanelMark* p)
    { this->Parent = p; }
  vtkPanelMark* GetParent()
    { return this->Parent; }

  void SetIndex(vtkIdType i)
    { this->Index = i; }
  vtkIdType GetIndex()
    { return this->Index; }

//BTX
protected:
  vtkMark();
  ~vtkMark();

  virtual void DataChanged()
    {
    this->Left.SetDirty(true);
    this->Right.SetDirty(true);
    this->Top.SetDirty(true);
    this->Bottom.SetDirty(true);
    this->Title.SetDirty(true);
    this->FillColor.SetDirty(true);
    this->LineColor.SetDirty(true);
    this->LineWidth.SetDirty(true);
    }

  vtkValueHolder<vtkDataElement> Data;
  vtkValueHolder<double> Left;
  vtkValueHolder<double> Right;
  vtkValueHolder<double> Top;
  vtkValueHolder<double> Bottom;
  vtkValueHolder<std::string> Title;
  vtkValueHolder<vtkColor> FillColor;
  vtkValueHolder<vtkColor> LineColor;
  vtkValueHolder<double> LineWidth;

  vtkPanelMark* Parent;
  vtkIdType Index;

private:
  vtkMark(const vtkMark &); // Not implemented.
  void operator=(const vtkMark &);   // Not implemented.
//ETX
};

template <typename T>
void vtkValueHolder<T>::Update(vtkMark* m)
{
  if (!this->Dirty)
    {
    return;
    }
  if (this->Value.IsConstant())
    {
    this->Cache.clear();
    return;
    }
  vtkDataElement d = m->GetData();
  vtkIdType numChildren = d.GetNumberOfChildren();
  this->Cache.resize(numChildren);
  for (vtkIdType i = 0; i < numChildren; ++i)
    {
    m->SetIndex(i);
    vtkDataElement e = d.GetChild(i);
    this->Cache[i] = this->Value.GetFunction()(m, e);
    }
  this->Dirty = false;
}


#endif //__vtkMark_h
