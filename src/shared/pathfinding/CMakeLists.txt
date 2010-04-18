
########### next target ###############

SET(pathfinding_STAT_SRCS
    ChunkyTriMesh.h
    ChunkyTriMesh.cpp
    InputGeom.h
    InputGeom.cpp
    MeshLoaderObj.h
    MeshLoaderObj.cpp
    Detour/DetourCommon.cpp
    Detour/DetourCommon.h
    Detour/DetourNavMesh.cpp
    Detour/DetourNavMesh.h
    Detour/DetourNavMeshBuilder.cpp
    Detour/DetourNavMeshBuilder.h
    Detour/DetourNode.cpp
    Detour/DetourNode.h
    Recast/Recast.cpp
    Recast/Recast.h
    Recast/RecastArea.cpp
    Recast/RecastContour.cpp
    Recast/RecastFilter.cpp
    Recast/RecastLog.cpp
    Recast/RecastLog.h
    Recast/RecastMesh.cpp
    Recast/RecastMeshDetail.cpp
    Recast/RecastRasterization.cpp
    Recast/RecastRegion.cpp
    Recast/RecastTimer.cpp
    Recast/RecastTimer.h
)

add_library(pathfinding  STATIC ${pathfinding_STAT_SRCS})

