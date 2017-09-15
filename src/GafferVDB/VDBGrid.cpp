//////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2015, Image Engine. All rights reserved.
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
#include "GafferVDB/VDBGrid.h"
#include "IECore/SimpleTypedData.h"



using namespace IECore;
using namespace GafferVDB;

IE_CORE_DEFINEOBJECTTYPEDESCRIPTION( VDBGrid );

const unsigned int VDBGrid::m_ioVersion = 0;

VDBGrid::VDBGrid( openvdb::GridBase::Ptr grid )
	:	m_grid( grid )
{
}

VDBGrid::~VDBGrid()
{
}

IECore::CompoundObjectPtr VDBGrid::metadata() const
{
	CompoundObjectPtr metadata = new CompoundObject();

	for (auto metaIt = m_grid->beginMeta(); metaIt != m_grid->endMeta(); ++metaIt)
	{
		openvdb::Metadata::Ptr ptr = metaIt->second;

		if (metaIt->second->typeName() == "string")
		{
			openvdb::TypedMetadata<openvdb::Name>::ConstPtr typedPtr = openvdb::DynamicPtrCast<openvdb::TypedMetadata<openvdb::Name> >(ptr);

			if (typedPtr)
			{
				StringDataPtr stringData = new StringData();
				stringData->writable() = typedPtr->value();
				metadata->members()[metaIt->first] = stringData;
			}
		}
		else if (metaIt->second->typeName() == "int64")
		{
			openvdb::TypedMetadata<openvdb::Int64>::ConstPtr typedPtr = openvdb::DynamicPtrCast<openvdb::TypedMetadata<openvdb::Int64> >(ptr);
			if (typedPtr)
			{
				Int64DataPtr intData = new Int64Data();
				intData->writable() = typedPtr->value();
				metadata->members()[metaIt->first] = intData;
			}
		}
		else if (metaIt->second->typeName() == "bool")
		{
			openvdb::TypedMetadata<bool>::ConstPtr typedPtr = openvdb::DynamicPtrCast<openvdb::TypedMetadata<bool> > (ptr);
			if (typedPtr)
			{
				BoolDataPtr data = new BoolData();
				data->writable() = typedPtr->value();
				metadata->members()[metaIt->first] = data;
			}

		}
		else if (metaIt->second->typeName() == "vec3i")
		{
			openvdb::TypedMetadata<openvdb::math::Vec3i>::ConstPtr typedPtr = openvdb::DynamicPtrCast<openvdb::TypedMetadata<openvdb::math::Vec3i> >(ptr);
			if (typedPtr)
			{
				V3iDataPtr data = new V3iData();
				data->writable() = Imath::V3i( typedPtr->value().x(), typedPtr->value().y(), typedPtr->value().z() );
				metadata->members()[metaIt->first] = data;
			}
		}
		else
		{
			StringDataPtr stringData = new StringData();
			stringData->writable() = "unsupported type: " + metaIt->second->typeName();
			metadata->members()[metaIt->first] = stringData;
		}
	}
	return metadata;
}

openvdb::GridBase::Ptr VDBGrid::grid()
{
	return m_grid;
}

openvdb::GridBase::ConstPtr VDBGrid::grid() const
{
	return m_grid;
}

void VDBGrid::copyFrom( const Object *other, CopyContext *context )
{
	Object::copyFrom( other, context );
	const VDBGrid *otherVDBGrid = static_cast<const VDBGrid *>( other );
	if( !otherVDBGrid->m_grid )
	{
		m_grid.reset();
	}
	else
	{
		m_grid = otherVDBGrid->m_grid->deepCopyGrid();
	}
}

bool VDBGrid::isEqualTo( const Object *other ) const
{
	if( !Object::isEqualTo( other ) )
	{
		return false;
	}

	const VDBGrid *otherVDBGrid = static_cast<const VDBGrid *>( other );
	return m_grid == otherVDBGrid->m_grid;
}

void VDBGrid::save( SaveContext *context ) const
{
	Object::save( context );
	throw IECore::NotImplementedException( "VDBGrid::save" );
}

void VDBGrid::load( LoadContextPtr context )
{
	Object::load( context );
	throw IECore::NotImplementedException( "VDBGrid::load" );
}

void VDBGrid::memoryUsage( Object::MemoryAccumulator &a ) const
{
	Object::memoryUsage( a );
	a.accumulate( m_grid->memUsage() );
}

void VDBGrid::hash( MurmurHash &h ) const
{
	Object::hash( h );
	throw IECore::NotImplementedException( "VDBGrid::hash" );
}
