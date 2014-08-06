# -----------------------------------------------------------------------------
# ParaView Python - Path setup
# -----------------------------------------------------------------------------

import sys, time, os
pv_path = '/Builds/ParaView/devel/master_debug'
output_dir = '/tmp/composite-data-exploration'

if not os.path.exists(output_dir):
    os.makedirs(output_dir)

from paraview.simple import *
from paraview import data_exploration as wx

# LoadPlugin(pv_path + '/lib/libRGBZView.dylib')

resolution = 500
center_of_rotation = [0.0, 0.0, 0.0]
rotation_axis = [0.0, 0.0, 1.0]
distance = 50.0

wavelet = Wavelet()
Calculator2 = Calculator()
Calculator2.Function = 'coordsX'
Calculator2.ResultArrayName = 'X'

randomVectors = RandomVectors()
contour_values = [ 64.0, 90.6, 117.2, 143.8, 170.4, 197.0, 223.6, 250.2]
color_type = [('POINT_DATA', "BrownianVectors")]
luts = { "BrownianVectors": GetLookupTableForArray( "BrownianVectors", 1, RGBPoints=[0, 0.23, 0.299, 0.754, 0.5, 0.865, 0.865, 0.865, 1.0, 0.706, 0.016, 0.15], VectorMode='Magnitude', NanColor=[0.25, 0.0, 0.0], ColorSpace='Diverging', ScalarRangeInitialized=1.0 ) }
filters = [ wavelet ]
filters_description = [ {'name': 'Wavelet'} ]
color_by = [ color_type ]

for iso_value in contour_values:
    filters.append( Contour( Input=randomVectors, PointMergeMethod="Uniform Binning", ContourBy = ['POINTS', 'RTData'], Isosurfaces = [iso_value], ComputeScalars = 1 ) )
    color_by.append( color_type )
    filters_description.append({'name': 'iso=%s' % str(iso_value)})

# ---------------------------------------------------------------------------

title       = "Composite test"
description = "Because test are important"
analysis = wx.AnalysisManager( output_dir, title, description)

id = 'composite'
title = '3D composite'
description = "contour set"
analysis.register_analysis(id, title, description, '{theta}/{phi}/{filename}', wx.CompositeImageExporter.get_data_type())
fng = analysis.get_file_name_generator(id)

camera_handler = wx.ThreeSixtyCameraHandler(fng, None, [ float(r) for r in range(0, 360, 45)], [ float(r) for r in range(-60, 61, 45)], center_of_rotation, rotation_axis, distance)

# Data exploration ------------------------------------------------------------

exporter = wx.CompositeImageExporter(fng, filters, color_by, luts, camera_handler, [resolution,resolution], filters_description, 0, 0)
exporter.set_analysis(analysis)

# Processing ------------------------------------------------------------------

analysis.begin()
exporter.UpdatePipeline(0)
analysis.end()
