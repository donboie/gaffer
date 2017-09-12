import GafferTest

import GafferVDB
import IECore


class SceneInterfaceTest( GafferTest.TestCase ) :
	def setUp( self ) :
		GafferTest.TestCase.setUp( self )
		self.sceneInterface = IECore.SceneInterface.create( "/home/donbo/journal/data/vdb/utahteapot.vdb", IECore.IndexedIO.OpenMode.Read )

	def testCanLoading( self ) :
		self.assertEqual( self.sceneInterface.hasObject(), True )
		self.assertEqual( self.sceneInterface.fileName(), "/home/donbo/journal/data/vdb/utahteapot.vdb" )
		self.assertEqual( self.sceneInterface.pathAsString(), "/" )
		self.assertEqual( self.sceneInterface.name(), "" )
		self.assertEqual( self.sceneInterface.hasBound(), True )

	def testSingleVDBChildName( self ) :
		self.assertEqual( self.sceneInterface.childNames(), ["vdb"] )

	def testCanGetChildVDBLocation( self ):
		self.assertEqual( self.sceneInterface.hasChild("vdb"), True)
		childSceneInterface = self.sceneInterface.child("vdb")
		self.assertEqual( childSceneInterface.childNames(), [])

	def testIdentityTransform( self ) :
		transformData = self.sceneInterface.readTransform( 0.0 )
		transform = self.sceneInterface.readTransformAsMatrix( 0.0 )

		#todo check the transform

	def testCanReadBounds( self ) :
		bounds = self.sceneInterface.readBound( 0.0 )

		# print type( bounds )
		# print dir( bounds )
		#
		# print type( bounds.min )
		# print dir( bounds.min )

		# -50.055000305175781 -23.155000686645508 -30.854999542236328 48.055000305175781 23.155000686645508 30.854999542236328

		# print transformData
		# print transform
		# todo check the bounds
