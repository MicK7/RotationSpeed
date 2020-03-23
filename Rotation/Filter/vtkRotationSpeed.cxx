/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkRotationSpeed.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
//  Copyright 2014-2016 Etienne Tang.

#include "vtkRotationSpeed.h"

#include "vtkObjectFactory.h"
#include "vtkTransform.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkPointSet.h"
#include "vtkDataSetAttributes.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkMath.h"

#include <iostream>
#include <list>
#include <string>

vtkStandardNewMacro(vtkRotationSpeed)

vtkRotationSpeed::vtkRotationSpeed()
{
    this->Omega = 0.0;
    this->Theta0 = 0.0;
    this->T0 = 0.0;

    this->Transform = vtkTransform::New();
}

vtkRotationSpeed::~vtkRotationSpeed()
{
    this->Transform->Delete();
}

void vtkRotationSpeed::PrintSelf(ostream &os, vtkIndent indent)
{
    os << "Rotation Speed Filter";
}

int vtkRotationSpeed::ExecuteInformation(vtkInformation* request, 
                                        vtkInformationVector** inVector,
                                        vtkInformationVector* outVector)
{
    vtkInformation* inInfo = inVector[0]->GetInformationObject(0);
    vtkInformation* outInfo = outVector->GetInformationObject(0);

    // We provide the output with the same timesteps as the input.
    // If you need more, use vtkTemporalShiftScale with the Periodic key
    if(inInfo->Has(vtkStreamingDemandDrivenPipeline::TIME_STEPS()))
    {
        int nTimesteps = inInfo->Length(vtkStreamingDemandDrivenPipeline::TIME_STEPS());
        double* timesteps = new double[nTimesteps];
        inInfo->Get(vtkStreamingDemandDrivenPipeline::TIME_STEPS(), timesteps);
        outInfo->Set(vtkStreamingDemandDrivenPipeline::TIME_STEPS(), timesteps, nTimesteps);
        delete[] timesteps;
    }
    else
    {
        std::cout << "Error: no input time information" << std::endl;
        return 0;
    }
    return 1;
}

int vtkRotationSpeed::ComputeInputUpdateExtent(vtkInformation* request,
                                               vtkInformationVector** inVector,
                                               vtkInformationVector* outVector)
{
    vtkInformation* inInfo = inVector[0]->GetInformationObject(0);
    vtkInformation* outInfo = outVector->GetInformationObject(0);

    // We simply forward the requested timestep to the input
    if(outInfo->Has(vtkStreamingDemandDrivenPipeline::UPDATE_TIME_STEP()))
    {
        this->CurrentTimestep = outInfo->Get(vtkStreamingDemandDrivenPipeline::UPDATE_TIME_STEP());
        std::cout << "Nouveau pas de temps demande "
                  << this->CurrentTimestep << std::endl;
        inInfo->Set(vtkStreamingDemandDrivenPipeline::UPDATE_TIME_STEP(), this->CurrentTimestep);
    }
    else
    {
        std::cout << "Error: no timestep has been requested by the filter output"
                  << std::endl;
        return 0;
    }
    return 1;
}

int vtkRotationSpeed::RequestData(vtkInformation *request,
                                  vtkInformationVector **inVector,
                                  vtkInformationVector *outVector)
{
    vtkInformation* inInfo = inVector[0]->GetInformationObject(0);
    vtkInformation* outInfo = outVector->GetInformationObject(0);
    vtkPointSet* outData = vtkPointSet::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));
    if(!outData)
    {
        std::cout << "Error: output data object has not been generated at the RequestData step"
                  << std::endl;
        return 0;
    }
    vtkPointSet* inData = vtkPointSet::SafeDownCast(inInfo->Get(vtkDataObject::DATA_OBJECT()));
    if(!inData)
    {
        std::cout << "Error: input data object has not been generated at the RequestData step"
                  << std::endl;
        return 0;
    }
    outInfo->Set(vtkDataObject::DATA_TIME_STEP(), this->CurrentTimestep);
    double theta = (this->Theta0
                    + (this->CurrentTimestep - this->T0)*this->Omega)
                    *180./vtkMath::Pi();
    this->Transform->Identity();
    this->Transform->RotateX(theta);

    outData->ShallowCopy(inData);

    // Rotate the points
    vtkPoints* newPts = vtkPoints::New();
    this->Transform->TransformPoints(inData->GetPoints(), newPts);
    outData->SetPoints(newPts);
    newPts->Delete();

    // Rotate the vectors
    this->RotateVectors(outData->GetPointData());
    this->RotateVectors(outData->GetCellData());

    return 1;
}

void vtkRotationSpeed::RotateVectors(vtkDataSetAttributes *data)
{
    // Apply the current transform to either a vtkPointData or a vtkCellData
    // First step: detect which of the variables are vectors
    std::list<std::string> names;
    for(int i = 0, n = data->GetNumberOfArrays(); i < n; i++)
    {
        vtkDataArray* ar = data->GetArray(i);
        if(ar->GetNumberOfComponents() == 3)
        {
            names.push_back(std::string(ar->GetName()));
        }
    }
    // Apply the rotation to the detected vectors
    for(auto name: names)
    {
        vtkDataArray* oldAr = data->GetArray(name.c_str());
        vtkDataArray* newAr = oldAr->NewInstance();
        newAr->SetNumberOfComponents(3);
        this->Transform->TransformVectors(oldAr, newAr);
        data->RemoveArray(name.c_str());
        newAr->SetName(name.c_str());
        data->AddArray(newAr);
        newAr->Delete();
    }
}
