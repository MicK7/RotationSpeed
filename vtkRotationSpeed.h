/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkRotationSpeed.h

  Copyright (c) Ken Martin, Will Schrodeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

  =========================================================================*/
// Copyright 2014-2016 Etienne Tang

/**
 * @class   vtkRotationSpeed
 *
 * vtkRotationSpeed apply a rotation to the input dataset
 * at a specified rotation speed and from an initial rotated position.
 *
 * @warning
 *   ...
 *
 * @par Thanks:
 * Thanks to Mickael Philit
 */

#ifndef VTKROTATIONSPEED_H
#define VTKROTATIONSPEED_H

#include "vtkSetGet.h"

#include "vtkPointSetAlgorithm.h"

class vtkTransform;
class vtkInformation;
class vtkInformationVector;
class vtkDataSetAttributes;


class VTK_EXPORT vtkRotationSpeed : public vtkPointSetAlgorithm
{
public:
    static vtkRotationSpeed* New();
    vtkTypeMacro(vtkRotationSpeed, vtkPointSetAlgorithm);
    void PrintSelf(ostream &os, vtkIndent indent);

    vtkGetMacro(Omega, double)
    vtkSetMacro(Omega, double)

    vtkGetMacro(Theta0, double)
    vtkSetMacro(Theta0, double)

    vtkGetMacro(T0, double)
    vtkSetMacro(T0, double)

protected:

    double Omega;
    double Theta0;
    double T0;

    vtkTransform* Transform;

    double CurrentTimestep;

    vtkRotationSpeed();
    ~vtkRotationSpeed();

    virtual int RequestData(vtkInformation* request, vtkInformationVector** inVector, vtkInformationVector* outVector);
    // virtual int RequestDataObject(vtkInformation* request, vtkInformationVector** inVector, vtkInformationVector* outVector);
    virtual int ExecuteInformation(vtkInformation* request, vtkInformationVector** inVector, vtkInformationVector* outVector);
    virtual int ComputeInputUpdateExtent(vtkInformation* request, vtkInformationVector** inVector, vtkInformationVector* outVector);

    void RotateVectors(vtkDataSetAttributes* data);

private:
    vtkRotationSpeed(const vtkRotationSpeed &);
    bool operator=(const vtkRotationSpeed &);
};

#endif // VTKROTATIONSPEED_H
