#include "GafferVDB/VDBUtil.h"


namespace GafferVDB
{
Imath::Box3d getBounds( openvdb::GridBase::ConstPtr grid, float padding )
{
	openvdb::Vec3i min = grid->metaValue<openvdb::Vec3i>( grid->META_FILE_BBOX_MIN );
	openvdb::Vec3i max = grid->metaValue<openvdb::Vec3i>( grid->META_FILE_BBOX_MAX );

	openvdb::Vec3d offset = openvdb::Vec3d( padding );
	openvdb::BBoxd indexBounds = openvdb::BBoxd( min - offset, max + offset );
	openvdb::BBoxd worldBounds = grid->transform().indexToWorld( indexBounds );
	openvdb::Vec3d minBB = worldBounds.min();
	openvdb::Vec3d maxBB = worldBounds.max();

	return Imath::Box3d( Imath::V3f( minBB[0], minBB[1], minBB[2] ), Imath::V3f( maxBB[0], maxBB[1], maxBB[2] ) );
}
}

