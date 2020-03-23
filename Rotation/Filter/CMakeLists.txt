set(classes
  vtkRotationSpeed
)

set(SERVER_MANAGER_XML_files
  RotationSpeed.xml
)

set(SERVER_MANAGER_SOURCES_files
  vtkRotationSpeed.cxx
)

set(SERVER_MANAGER_HEADERS_files
  vtkRotationSpeed.h
)

vtk_module_add_module(Rotation::vtkRotationSpeed
  SOURCES ${SERVER_MANAGER_SOURCES_files}
  HEADERS ${SERVER_MANAGER_HEADERS_files}
)

paraview_add_server_manager_xmls(
  XMLS ${SERVER_MANAGER_XML_files}
)

