##########################################################################
#
#  Copyright (c) 2018, Image Engine Design Inc. All rights reserved.
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
#      * Neither the name of John Haddon nor the names of
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

import unittest

import IECore

import Gaffer
import GafferScene
import GafferSceneTest

class RenderControllerTest( GafferSceneTest.SceneTestCase ) :

	def testConstructorAndAccessors( self ) :

		sphere = GafferScene.Sphere()
		context1 = Gaffer.Context()
		renderer = GafferScene.Private.IECoreScenePreview.Renderer.create(
			"OpenGL",
			GafferScene.Private.IECoreScenePreview.Renderer.RenderType.Interactive
		)
		controller = GafferScene.RenderController( sphere["out"], context1, renderer )

		self.assertTrue( controller.renderer().isSame( renderer ) )
		self.assertTrue( controller.getScene().isSame( sphere["out"] ) )
		self.assertTrue( controller.getContext().isSame( context1 ) )

		cube = GafferScene.Cube()
		context2 = Gaffer.Context()

		controller.setScene( cube["out"] )
		controller.setContext( context2 )

		self.assertTrue( controller.getScene().isSame( cube["out"] ) )
		self.assertTrue( controller.getContext().isSame( context2 ) )

	def testBoundUpdate( self ) :

		sphere = GafferScene.Sphere()
		group = GafferScene.Group()
		group["in"][0].setInput( sphere["out"] )

		renderer = GafferScene.Private.IECoreScenePreview.Renderer.create(
			"OpenGL",
			GafferScene.Private.IECoreScenePreview.Renderer.RenderType.Interactive
		)
		controller = GafferScene.RenderController( group["out"], Gaffer.Context(), renderer )
		controller.update()
		self.assertEqual(
			renderer.command( "gl:queryBound", {} ),
			group["out"].bound( "/" )
		)

		sphere["transform"]["translate"]["x"].setValue( 1 )
		controller.update()
		self.assertEqual(
			renderer.command( "gl:queryBound", {} ),
			group["out"].bound( "/" )
		)

	def testUpdateMatchingPaths( self ) :

		sphere = GafferScene.Sphere()
		group = GafferScene.Group()
		group["in"][0].setInput( sphere["out"] )
		group["in"][1].setInput( sphere["out"] )

		renderer = GafferScene.Private.IECoreScenePreview.Renderer.create(
			"OpenGL",
			GafferScene.Private.IECoreScenePreview.Renderer.RenderType.Interactive
		)
		controller = GafferScene.RenderController( group["out"], Gaffer.Context(), renderer )
		controller.setMinimumExpansionDepth( 3 )
		controller.update()

		def bound( path ) :

			renderer.option( "gl:selection", IECore.PathMatcherData( IECore.PathMatcher( [ path ] ) ) )
			return renderer.command( "gl:queryBound", { "selection" : True } )

		boundOrig = sphere["out"].bound( "/sphere" )
		self.assertEqual( bound( "/group/sphere" ), boundOrig )
		self.assertEqual( bound( "/group/sphere1" ), boundOrig )

		sphere["radius"].setValue( 2 )

		self.assertEqual( bound( "/group/sphere" ), boundOrig )
		self.assertEqual( bound( "/group/sphere1" ), boundOrig )

		controller.updateMatchingPaths( IECore.PathMatcher( [ "/group/sphere" ] ) )

		boundUpdated = sphere["out"].bound( "/sphere" )
		self.assertEqual( bound( "/group/sphere" ), boundUpdated )
		self.assertEqual( bound( "/group/sphere1" ), boundOrig )

		controller.update()

		self.assertEqual( bound( "/group/sphere" ), boundUpdated )
		self.assertEqual( bound( "/group/sphere1" ), boundUpdated )

	def testUpdateMatchingPathsAndInheritedTransforms( self ) :

		sphere = GafferScene.Sphere()
		group = GafferScene.Group()
		group["in"][0].setInput( sphere["out"] )
		group["in"][1].setInput( sphere["out"] )

		renderer = GafferScene.Private.IECoreScenePreview.Renderer.create(
			"OpenGL",
			GafferScene.Private.IECoreScenePreview.Renderer.RenderType.Interactive
		)
		controller = GafferScene.RenderController( group["out"], Gaffer.Context(), renderer )
		controller.setMinimumExpansionDepth( 3 )
		controller.update()

		def bound( path ) :

			renderer.option( "gl:selection", IECore.PathMatcherData( IECore.PathMatcher( [ path ] ) ) )
			return renderer.command( "gl:queryBound", { "selection" : True } )

		untranslatedBound = group["out"].bound( "/group/sphere" ) * group["out"].fullTransform( "/group/sphere" )
		self.assertEqual( bound( "/group/sphere" ), untranslatedBound )
		self.assertEqual( bound( "/group/sphere1" ), untranslatedBound )

		group["transform"]["translate"]["x"].setValue( 2 )
		translatedBound = group["out"].bound( "/group/sphere" ) * group["out"].fullTransform( "/group/sphere" )

		controller.updateMatchingPaths( IECore.PathMatcher( [ "/group/sphere" ] ) )

		self.assertEqual( bound( "/group/sphere" ), translatedBound )
		self.assertEqual( bound( "/group/sphere1" ), untranslatedBound )

		controller.update()

		self.assertEqual( bound( "/group/sphere" ), translatedBound )
		self.assertEqual( bound( "/group/sphere1" ), translatedBound )

	def testUpdateRemoveFromLightSet( self ) :

		sphere = GafferScene.Sphere()
		lightSet = GafferScene.Set()
		lightSet["in"].setInput( sphere["out"] )
		lightSet["name"].setValue( '__lights' )
		lightSet["paths"].setValue( IECore.StringVectorData( [ '/sphere' ] ) )

		renderer = GafferScene.Private.IECoreScenePreview.Renderer.create(
			"OpenGL",
			GafferScene.Private.IECoreScenePreview.Renderer.RenderType.Interactive
		)
		controller = GafferScene.RenderController( sphere["out"], Gaffer.Context(), renderer )
		controller.update()
		self.assertEqual(
			renderer.command( "gl:queryBound", {} ),
			lightSet["out"].bound( "/" )
		)

		controller.setScene( lightSet["out"] )
		controller.update()
		self.assertEqual(
			renderer.command( "gl:queryBound", {} ),
			lightSet["out"].bound( "/" )
		)

		# While doing this exact same thing worked the first time, there was a bug where
		# rendering geo that had previously been rendered in the lights pass would fail.
		controller.setScene( sphere["out"] )
		controller.update()
		self.assertEqual(
			renderer.command( "gl:queryBound", {} ),
			lightSet["out"].bound( "/" )
		)

if __name__ == "__main__":
	unittest.main()
