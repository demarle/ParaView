/*=========================================================================

Program:   ParaView
Module:    pvpython.cxx

Copyright (c) Kitware, Inc.
All rights reserved.
See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkInitializationHelper.h"
#include "vtkProcessModule.h"
#include "vtkPVConfig.h" // Required to get build options for paraview
#include "vtkPVPythonOptions.h"
#include "vtkToolkits.h" // For VTK_USE_MPI
#include "vtkMultiProcessController.h"

#include "vtkPVPythonInterpretor.h"
#include <vtkstd/vector>

namespace ParaViewPython {

  //---------------------------------------------------------------------------

  char* clone(const char* str)
    {
    char *newStr = new char[ strlen(str) + 1 ];
    strcpy(newStr, str);
    return newStr;
    }

  //---------------------------------------------------------------------------

  void ProcessArgsForPython( vtkstd::vector<char*> & pythonArgs,
                             const char *script,
                             int argc, char* argv[] )
    {
    pythonArgs.clear();
    pythonArgs.push_back(clone(argv[0]));
    if(script)
      {
      pythonArgs.push_back(clone(script));
      }
    else if (argc > 1)
      {
      pythonArgs.push_back(clone("-"));
      }
    for (int cc=1; cc < argc; cc++)
      {
      pythonArgs.push_back(clone(argv[cc]));
      }
    }

  //---------------------------------------------------------------------------
  int Run(int processType, int argc, char* argv[])
    {
    // Setup options
    vtkPVPythonOptions* options = vtkPVPythonOptions::New();
    vtkInitializationHelper::Initialize( argc, argv, processType, options );

//    vtkMultiProcessController *ctrl = vtkMultiProcessController::GetGlobalController();
//    if(ctrl->GetLocalProcessId() > 0 && !options->GetSymmetricMPIMode())
//      {
//      return 0;
//      }

    // Process arguments
    vtkstd::vector<char*> pythonArgs;
    ProcessArgsForPython(pythonArgs, options->GetPythonScriptName(), argc, argv);

    // Start interpretor
    vtkPVPythonInterpretor* interpretor = vtkPVPythonInterpretor::New();
    int ret_val = interpretor->PyMain(pythonArgs.size(), &*pythonArgs.begin());
    interpretor->Delete();

    // Free python args
    vtkstd::vector<char*>::iterator it = pythonArgs.begin();
    while(it != pythonArgs.end())
      {
      delete [] *it;
      ++it;
      }

    // Exit application
    vtkInitializationHelper::Finalize();
    options->Delete();
    return ret_val;
    }
}