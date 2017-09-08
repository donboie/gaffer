//////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2015, John Haddon. All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions are
//  met:
//
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//
//     * Neither the name of Image Engine Design nor the names of any
//       other contributors to this software may be used to endorse or
//       promote products derived from this software without specific prior
//       written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
//  IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
//  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
//  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
//  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
//  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
//  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
//  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
//  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
//  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
//  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//////////////////////////////////////////////////////////////////////////


#include "openvdb/openvdb.h"

#include "IECore/SceneInterface.h"

#include "GafferVDB/TypeIds.h"
#include "GafferVDB/VDBObject.h"

using namespace Imath;
using namespace IECore;
using namespace GafferVDB;

using namespace openvdb;
namespace
{

class VDBScene : public SceneInterface
{

	public :

		IE_CORE_DECLARERUNTIMETYPEDEXTENSION( VDBScene, VDBSceneTypeId, IECore::SceneInterface );

		VDBScene( const std::string &fileName, IndexedIO::OpenMode openMode )
		: SceneInterface(),
          m_fileName(fileName)
		{
			// todo check if it's invalid to call this multiple times or is a performance hazard
			openvdb::initialize();

			io::File file( fileName );
			file.open();

			m_grids = file.readAllGridMetadata();
			m_metaData = file.getMetadata();
		}

		~VDBScene() override
		{
		}

		std::string fileName() const override
		{
			return m_fileName;
		}

		Name name() const override
		{
			return Name("/");
		}

		void path( Path &p ) const override
		{
			p = Path();
		}

		////////////////////////////////////////////////////
		// Bounds
		////////////////////////////////////////////////////

		Imath::Box3d readBound( double time ) const override
		{
			return Box3d();
		}

		 void writeBound( const Imath::Box3d &bound, double time ) override
		 {
			 throw IECore::NotImplementedException("");
		 }

		////////////////////////////////////////////////////
		// Transforms
		////////////////////////////////////////////////////

		ConstDataPtr readTransform( double time ) const override
		{
			return nullptr;
		}

		Imath::M44d readTransformAsMatrix( double time ) const override
		{
			return M44d();
		}

		void writeTransform( const Data *transform, double time ) override
		{
			throw IECore::NotImplementedException("");
		}

		////////////////////////////////////////////////////
		// Attributes
		////////////////////////////////////////////////////

		bool hasAttribute( const Name &name ) const override
		{
			return false;
		}

		/// Fills attrs with the names of all attributes available in the current directory
		void attributeNames( NameList &attrs ) const override
		{
			attrs = {};
		}

		ConstObjectPtr readAttribute( const Name &name, double time ) const override
		{
			return nullptr;
		}

		void writeAttribute( const Name &name, const Object *attribute, double time ) override
		{
			throw IECore::NotImplementedException("");
		}

		////////////////////////////////////////////////////
		// Tags
		////////////////////////////////////////////////////

		bool hasTag( const Name &name, int filter = LocalTag ) const override
		{
			return false;
		}

		void readTags( NameList &tags, int filter = LocalTag ) const override
		{
			tags = {};
		}

		void writeTags( const NameList &tags ) override
		{
			throw IECore::NotImplementedException("");
		}

		////////////////////////////////////////////////////
		// Objects
		////////////////////////////////////////////////////

		bool hasObject() const override
		{
			return true;
		}

		ConstObjectPtr readObject( double time ) const override
		{
			VDBObject::Ptr ptr = new VDBObject(m_grids->at(0));
			return ptr;
		}

		PrimitiveVariableMap readObjectPrimitiveVariables( const std::vector<InternedString> &primVarNames, double time ) const override
		{
			return PrimitiveVariableMap();
		}

		void writeObject( const Object *object, double time ) override
		{
			throw IECore::NotImplementedException("");
		}

		////////////////////////////////////////////////////
		// Hierarchy
		////////////////////////////////////////////////////

		/// Convenience method to determine if a child exists
		bool hasChild( const Name &name ) const override
		{
			return false;
		}
		/// Queries the names of any existing children of path() within
		/// the scene.
		void childNames( NameList &childNames ) const override
		{
			childNames = NameList();
		}
		/// Returns an object for the specified child location in the scene.
		/// If the child does not exist then it will behave according to the
		/// missingBehavior parameter. May throw and exception, may return a NULL pointer,
		/// or may create the child (if that is possible).
		/// Bounding boxes will be automatically propagated up from the children
		/// to the parent as it is written.
		SceneInterfacePtr child( const Name &name, MissingBehaviour missingBehaviour = ThrowIfMissing ) override
		{
			if (missingBehaviour == ThrowIfMissing)
			{
				throw IECore::InvalidArgumentException("VDBSCene::child(): no child called \" + name.string()");
			}
			else if (missingBehaviour == CreateIfMissing)
			{
				throw IECore::InvalidArgumentException("VDBScene::child(): CreateIfMissing not supported");
			}

			return SceneInterfacePtr();
		}
		/// Returns a read-only interface for a child location in the scene.
		ConstSceneInterfacePtr child( const Name &name, MissingBehaviour missingBehaviour = ThrowIfMissing ) const override
		{
			if (missingBehaviour == ThrowIfMissing)
			{
				throw IECore::InvalidArgumentException("VDBSCene::child(): no child called \" + name.string()");
			}
			else if (missingBehaviour == CreateIfMissing)
			{
				throw IECore::NotImplementedException("VDBScene::child(): CreateIfMissing not supported");
			}

			return SceneInterfacePtr();
		}
		/// Returns a writable interface to a new child. Throws an exception if it already exists.
		/// Bounding boxes will be automatically propagated up from the children
		/// to the parent as it is written.
		SceneInterfacePtr createChild( const Name &name ) override
		{
			throw IECore::NotImplementedException("");
		}
		/// Returns a interface for querying the scene at the given path (full path).
		SceneInterfacePtr scene( const Path &path, MissingBehaviour missingBehaviour = ThrowIfMissing ) override
		{
			throw IECore::InvalidArgumentException("");
		}
		/// Returns a const interface for querying the scene at the given path (full path).
		ConstSceneInterfacePtr scene( const Path &path, MissingBehaviour missingBehaviour = ThrowIfMissing ) const override
		{
			throw IECore::InvalidArgumentException("");
		}

		////////////////////////////////////////////////////
		// Hash
		////////////////////////////////////////////////////

		void hash( HashType hashType, double time, MurmurHash &h ) const override
		{

		}

	private :

		static FileFormatDescription<VDBScene> g_description;

		openvdb::GridPtrVecPtr m_grids;
		openvdb::MetaMap::Ptr m_metaData;
		std::string m_fileName;

		Ptr m_parent;

};


SceneInterface::FileFormatDescription<VDBScene> VDBScene::g_description(".vdb", IndexedIO::Read );



} // namespace
