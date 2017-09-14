//////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2015, John Haddon. All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions are
//  met:
//
//      * Redistributions of source code must retain the above
//        copyright notice, this list of conditions and the following
//        disclaimer.
//
//      * Redistributions in binary form must reproduce the above
//        copyright notice, this list of conditions and the following
//        disclaimer in the documentation and/or other materials provided with
//        the distribution.
//
//      * Neither the name of John Haddon nor the names of
//        any other contributors to this software may be used to endorse or
//        promote products derived from this software without specific prior
//        written permission.
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

#include "IECoreGL/CurvesPrimitive.h"
#include "IECoreGL/Group.h"

#include "GafferSceneUI/ObjectVisualiser.h"

#include "GafferVDB/VDBObject.h"

using namespace std;
using namespace Imath;
using namespace GafferSceneUI;
using namespace GafferVDB;

namespace
{


template<typename GridType>
void extract(typename GridType::ConstPtr grid, std::vector<V3f>& positions, std::vector<int>& vertsPerCurve, openvdb::Index64 depth = 3)
{
	using openvdb::Index64;

//	Index getDepth() const { return ROOT_LEVEL - mLevel; }
//	static Index getLeafDepth() { return LEAF_DEPTH; }

	openvdb::Vec3d ptn;
	openvdb::CoordBBox bbox;

	std::vector<V3f> boundPositions(8);

	for (typename GridType::TreeType::NodeCIter iter = grid->tree().cbeginNode(); iter; ++iter)
	{
		int boundIndex = 0;
		iter.getBoundingBox(bbox);

		if (iter.getDepth() != depth)
		{
			continue;
		}

		// Nodes are rendered as cell-centered
		const openvdb::Vec3d min(bbox.min().x()-0.5, bbox.min().y()-0.5, bbox.min().z()-0.5);
		const openvdb::Vec3d max(bbox.max().x()+0.5, bbox.max().y()+0.5, bbox.max().z()+0.5);

		// corner 1
		ptn = grid->indexToWorld(min);
		boundPositions[boundIndex++] = V3f(ptn[0], ptn[1], ptn[2]);

		// corner 2
		ptn = openvdb::Vec3d(min.x(), min.y(), max.z());
		ptn = grid->indexToWorld(ptn);
		boundPositions[boundIndex++] = V3f(ptn[0], ptn[1], ptn[2]);

		// corner 3
		ptn = openvdb::Vec3d(max.x(), min.y(), max.z());
		ptn = grid->indexToWorld(ptn);
		boundPositions[boundIndex++] = V3f(ptn[0], ptn[1], ptn[2]);

		// corner 4
		ptn = openvdb::Vec3d(max.x(), min.y(), min.z());
		ptn = grid->indexToWorld(ptn);
		boundPositions[boundIndex++] = V3f(ptn[0], ptn[1], ptn[2]);

		// corner 5
		ptn = openvdb::Vec3d(min.x(), max.y(), min.z());
		ptn = grid->indexToWorld(ptn);
		boundPositions[boundIndex++] = V3f(ptn[0], ptn[1], ptn[2]);

		// corner 6
		ptn = openvdb::Vec3d(min.x(), max.y(), max.z());
		ptn = grid->indexToWorld(ptn);
		boundPositions[boundIndex++] = V3f(ptn[0], ptn[1], ptn[2]);

		// corner 7
		ptn = grid->indexToWorld(max);
		boundPositions[boundIndex++] = V3f(ptn[0], ptn[1], ptn[2]);

		// corner 8
		ptn = openvdb::Vec3d(max.x(), max.y(), min.z());
		ptn = grid->indexToWorld(ptn);
		boundPositions[boundIndex++] = V3f(ptn[0], ptn[1], ptn[2]);

		for (size_t i = 0; i < 12; ++i)
		{
			vertsPerCurve.push_back(2);
		}

		positions.push_back(boundPositions[0]);
		positions.push_back(boundPositions[1]);

		positions.push_back(boundPositions[1]);
		positions.push_back(boundPositions[2]);

		positions.push_back(boundPositions[2]);
		positions.push_back(boundPositions[3]);

		positions.push_back(boundPositions[3]);
		positions.push_back(boundPositions[0]);

		//

		positions.push_back(boundPositions[4]);
		positions.push_back(boundPositions[5]);

		positions.push_back(boundPositions[5]);
		positions.push_back(boundPositions[6]);

		positions.push_back(boundPositions[6]);
		positions.push_back(boundPositions[7]);

		positions.push_back(boundPositions[7]);
		positions.push_back(boundPositions[4]);

		//

		positions.push_back(boundPositions[0]);
		positions.push_back(boundPositions[4]);

		positions.push_back(boundPositions[1]);
		positions.push_back(boundPositions[5]);

		positions.push_back(boundPositions[2]);
		positions.push_back(boundPositions[6]);

		positions.push_back(boundPositions[3]);
		positions.push_back(boundPositions[7]);



	}
}
class VDBVisualiser : public ObjectVisualiser
{

	public :

		typedef VDBObject ObjectType;

		VDBVisualiser()
			:	m_group( new IECoreGL::Group() )
		{

			m_group->getState()->add( new IECoreGL::Primitive::DrawWireframe( true ) );
			m_group->getState()->add( new IECoreGL::Primitive::DrawSolid( false ) );
			m_group->getState()->add( new IECoreGL::CurvesPrimitive::UseGLLines( true ) );
			m_group->getState()->add( new IECoreGL::WireframeColorStateComponent( Color4f( 0.06, 0.2, 0.56, 1 ) ) );
			m_group->getState()->add( new IECoreGL::CurvesPrimitive::GLLineWidth( 2.0f ) );

			IECore::V3fVectorDataPtr pData = new IECore::V3fVectorData;
			vector<V3f> &p = pData->writable();
			p.reserve( 6 );
			p.push_back( V3f( 0 ) );
			p.push_back( V3f( 1, 0, 0 ) );
			p.push_back( V3f( 0 ) );
			p.push_back( V3f( 0, 1, 0 ) );
			p.push_back( V3f( 0 ) );
			p.push_back( V3f( 0, 0, 1 ) );

			IECore::IntVectorDataPtr vertsPerCurve = new IECore::IntVectorData;
			vertsPerCurve->writable().resize( 3, 2 );

			IECoreGL::CurvesPrimitivePtr curves = new IECoreGL::CurvesPrimitive( IECore::CubicBasisf::linear(), false, vertsPerCurve );
			curves->addPrimitiveVariable( "P", IECore::PrimitiveVariable( IECore::PrimitiveVariable::Vertex, pData ) );
			m_group->addChild( curves );
		}

		virtual ~VDBVisualiser()
		{
		}

		virtual IECoreGL::ConstRenderablePtr visualise( const IECore::Object *object ) const
		{
			const VDBObject* vdbObject = IECore::runTimeCast<const VDBObject>(object);
			if ( !vdbObject )
			{
				return m_group;
			}

			std::vector<std::string> names = vdbObject->gridNames();

			if (names.empty())
			{
				return m_group;
			}

			GafferVDB::VDBGrid::Ptr grid = vdbObject->grid(names[0]);

			openvdb::GridBase::ConstPtr vdbgrid = grid->grid();

			IECoreGL::Group *rootGroup = new IECoreGL::Group();

			static std::vector<Color4f> colors = { Color4f( 0.56, 0.06, 0.2, 0.2 ), Color4f( 0.06, 0.56, 0.2, 0.2 ), Color4f( 0.06, 0.2, 0.56, 0.2 ), Color4f( 0.6, 0.6, 0.6, 0.2 ) };
			for (openvdb::Index64 depth = 0; depth < 4; ++depth)
			{
				IECoreGL::Group *group = new IECoreGL::Group();

				group->getState()->add( new IECoreGL::Primitive::DrawWireframe( true ) );
				group->getState()->add( new IECoreGL::Primitive::DrawSolid( false ) );
				group->getState()->add( new IECoreGL::CurvesPrimitive::UseGLLines( true ) );
				group->getState()->add( new IECoreGL::WireframeColorStateComponent( colors[depth] ) );
				group->getState()->add( new IECoreGL::CurvesPrimitive::GLLineWidth( 0.5f ) );

				IECore::V3fVectorDataPtr pData = new IECore::V3fVectorData;
				vector<V3f> &p = pData->writable();

				IECore::IntVectorDataPtr vertsPerCurve = new IECore::IntVectorData;
				vector<int> &topology = vertsPerCurve->writable();

				extract<openvdb::FloatGrid>( openvdb::GridBase::constGrid<openvdb::FloatGrid>( vdbgrid ), p, topology, depth);

				IECoreGL::CurvesPrimitivePtr curves = new IECoreGL::CurvesPrimitive( IECore::CubicBasisf::linear(), false, vertsPerCurve );
				curves->addPrimitiveVariable( "P", IECore::PrimitiveVariable( IECore::PrimitiveVariable::Vertex, pData ) );
				group->addChild( curves );

				rootGroup->addChild( group );
			}
			return rootGroup;
		}

	protected :

		static ObjectVisualiserDescription<VDBVisualiser> g_visualiserDescription;

		IECoreGL::GroupPtr m_group;

};

ObjectVisualiser::ObjectVisualiserDescription<VDBVisualiser> VDBVisualiser::g_visualiserDescription;

} // namespace
