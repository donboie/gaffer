import GafferTest

import GafferVDB
import IECore

class SceneInterfaceTest( GafferTest.TestCase ) :

	def testCanLoading( self ) :
		sceneInterface = IECore.SceneInterface.create("/home/donbo/journal/data/vdb/utahteapot.vdb", IECore.IndexedIO.OpenMode.Read)


	def testNoChildNames( self ):
		sceneInterface = IECore.SceneInterface.create("/home/donbo/journal/data/vdb/utahteapot.vdb", IECore.IndexedIO.OpenMode.Read)

		self.assertEqual(sceneInterface.childNames(), [])




