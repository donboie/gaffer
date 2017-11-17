##########################################################################
#
#  Copyright (c) 2017, Image Engine Design Inc. All rights reserved.
#
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions are
#  met:
#
#      * Redistributions of source code must retain the above
#        copyright notice, this list of conditions and the following
#        disclaimer.
#
#      * Redistributions in binary form must reproduce the above
#        copyright notice, this list of conditions and the following
#        disclaimer in the documentation and/or other materials provided with
#        the distribution.
#
#      * Neither the name of Image Engine Design Inc nor the names of
#        any other contributors to this software may be used to endorse or
#        promote products derived from this software without specific prior
#        written permission.
#
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
#  IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
#  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
#  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
#  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
#  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
#  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
#  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
#  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
#  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
#  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
##########################################################################

import GafferTest

import GafferVDB
import IECore
import GafferVDBTest
import os
import GafferScene

class LevelSetToMeshTest( GafferVDBTest.VDBTestCase ) :
	def setUp( self ) :
		GafferVDBTest.VDBTestCase.setUp( self )
		self.sourcePath = os.path.join( self.dataDir, "sphere.vdb" )
		self.sceneInterface = IECore.SceneInterface.create( self.sourcePath, IECore.IndexedIO.OpenMode.Read )

	def testCanConvertLevelSetToMesh( self ) :
		sphere = GafferScene.Sphere()
		meshToLevelSet = GafferVDB.MeshToLevelSet()
		meshToLevelSet["voxelSize"].setValue( 0.05 )
		meshToLevelSet["in"].setInput( sphere["out"] )

		obj = meshToLevelSet["out"].object( "sphere" )

		self.assertTrue( isinstance( obj, GafferVDB.VDBObject ) )

		self.assertEqual( obj.gridNames(), ['levelset'] )
		grid = obj.findGrid( "levelset" )

		levelSetToMesh = GafferVDB.LevelSetToMesh()
		levelSetToMesh["in"].setInput( meshToLevelSet["out"] )

		mesh = levelSetToMesh["out"].object( "sphere" )
		self.assertTrue( isinstance( mesh, IECore.MeshPrimitive) )

	def testChangingIsoValueUpdatesBounds ( self ) :
		sphere = GafferScene.Sphere()
		sphere["radius"].setValue( 5 )

		meshToLevelSet = GafferVDB.MeshToLevelSet()
		meshToLevelSet["voxelSize"].setValue( 0.05 )
		meshToLevelSet["exteriorBandwidth"].setValue( 4.0 )
		meshToLevelSet["interiorBandwidth"].setValue( 4.0 )
		meshToLevelSet["in"].setInput( sphere["out"] )

		levelSetToMesh = GafferVDB.LevelSetToMesh()
		levelSetToMesh["in"].setInput( meshToLevelSet["out"] )

		self.assertAlmostEqual( 5.0, levelSetToMesh['out'].bound( "sphere" ).max[0] )

		levelSetToMesh['isoValue'].setValue(0.5)
		self.assertAlmostEqual( 5.5, levelSetToMesh['out'].bound( "sphere" ).max[0] )

		levelSetToMesh['isoValue'].setValue(-0.5)
		self.assertAlmostEqual( 4.5, levelSetToMesh['out'].bound( "sphere" ).max[0] )

	def testIncreasingAdapativityDecreasesPolyCount( self ) :
		sphere = GafferScene.Sphere()
		sphere["radius"].setValue( 5 )

		meshToLevelSet = GafferVDB.MeshToLevelSet()
		meshToLevelSet["voxelSize"].setValue( 0.05 )
		meshToLevelSet["exteriorBandwidth"].setValue( 4.0 )
		meshToLevelSet["interiorBandwidth"].setValue( 4.0 )
		meshToLevelSet["in"].setInput( sphere["out"] )

		levelSetToMesh = GafferVDB.LevelSetToMesh()
		levelSetToMesh["in"].setInput( meshToLevelSet["out"] )

		levelSetToMesh['adaptivity'].setValue(0.0)
		self.assertEqual( 187514, len( levelSetToMesh['out'].object( "sphere" ).verticesPerFace ) )

		levelSetToMesh['adaptivity'].setValue(1.0)
		self.assertEqual( 2934, len( levelSetToMesh['out'].object( "sphere" ).verticesPerFace ) )
