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

#include "IECore/Exception.h"
#include "IECore/MurmurHash.h"
#include "GafferVDB/VDBObject.h"
#include "GafferVDB/VDBUtil.h"

#include "openvdb/openvdb.h"

using namespace IECore;
using namespace GafferVDB;

IE_CORE_DEFINEOBJECTTYPEDESCRIPTION( VDBObject );

const unsigned int VDBObject::m_ioVersion = 0;

VDBObject::VDBObject()
{

}

VDBObject::VDBObject(CompoundObjectPtr grids)
: m_grids (grids)
{
}

VDBObject::~VDBObject()
{
}

void VDBObject::copyFrom( const Object *other, CopyContext *context )
{
	Object::copyFrom( other, context );

	const VDBObject *otherVDBObject = static_cast<const VDBObject *>( other );

	m_grids = otherVDBObject->m_grids->copy();

}

bool VDBObject::isEqualTo( const Object *other ) const
{
	return Object::isEqualTo( other );
}

void VDBObject::save( SaveContext *context ) const
{
	Object::save( context );
	throw IECore::NotImplementedException( "VDBObject::save" );
}

void VDBObject::load( LoadContextPtr context )
{
	Object::load( context );
	throw IECore::NotImplementedException( "VDBObject::load" );
}

void VDBObject::memoryUsage( Object::MemoryAccumulator &a ) const
{
	Object::memoryUsage( a );

	for (auto i  : m_grids->members())
	{
		a.accumulate( i.second.get() );
	};
}

void VDBObject::hash( MurmurHash &h ) const
{
	Object::hash( h );
	m_grids->hash( h );
}


VDBGrid::Ptr VDBObject::grid(const std::string& name) const
{
	const auto& it = m_grids->members().find( name );

	if (it == m_grids->members().end())
		return VDBGrid::Ptr();

	return IECore::runTimeCast<VDBGrid>( it->second );
}


std::vector<std::string> VDBObject::gridNames() const
{
	std::vector<std::string> outputGridNames;
	for(const auto &g : m_grids->members())
	{
		outputGridNames.push_back(runTimeCast<VDBGrid>(g.second)->grid()->getName());
	}
	return outputGridNames;
}

Imath::Box3f VDBObject::bound() const
{
	std::vector<std::string> names = gridNames();

	Imath::Box3f combinedBounds;

	for (const auto& name : names)
	{
		Imath::Box3f gridBounds = getBounds<float>( grid(name)->grid() );

		combinedBounds.extendBy( gridBounds );
	}

	return combinedBounds;
}

void VDBObject::render( IECore::Renderer *renderer ) const
{

}

void VDBObject::write(const std::string& path) const
{
	openvdb::io::File file( path );

	openvdb::GridCPtrVec grids;

	std::vector<std::string> names = gridNames();
	for (const auto& name : names)
	{
		grids.push_back(grid(name)->grid());
	}

	file.write(grids);
	file.close();
}