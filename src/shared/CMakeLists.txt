add_subdirectory(vmap)
add_subdirectory(Auth)
add_subdirectory(Config)
add_subdirectory(Database)
add_subdirectory(pathfinding)

########### next target ###############

SET(shared_STAT_SRCS
ByteBuffer.h
Common.cpp
Common.h
Errors.h
LockedQueue.h
Log.cpp
Log.h
MemoryLeaks.cpp
MemoryLeaks.h
Threading.cpp
Threading.h
Util.cpp
Util.h
WorldPacket.h
)

add_library(shared STATIC ${shared_STAT_SRCS})
target_link_libraries(
shared
${ACE_LIBRARY}
)
