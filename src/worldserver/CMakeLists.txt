
########### next target ###############

SET(WorldServer_SRCS
CliRunnable.cpp
CliRunnable.h
Main.cpp
Master.cpp
Master.h
RASocket.cpp
RASocket.h
WorldRunnable.cpp
WorldRunnable.h
)

SET(WorldServer_LINK_FLAGS "")

add_executable(WorldServer ${WorldServer_SRCS})
add_definitions(
-D_WORLD_CONFIG='"${CONF_DIR}/WorldServer.conf"'
)
IF (DO_MYSQL)
    SET(WorldServer_LINK_FLAGS "-pthread ${WorldServer_LINK_FLAGS}")
ENDIF(DO_MYSQL)
IF (DO_POSTGRESQL)
    SET(WorldServer_LINK_FLAGS "-pthread ${WorldServer_LINK_FLAGS}")
ENDIF(DO_POSTGRESQL)

IF (CMAKE_SYSTEM_NAME MATCHES "Darwin")
    SET(WorldServer_LINK_FLAGS "-framework Carbon ${WorldServer_LINK_FLAGS}")
    SET(SCRIPT_LIB "")
ENDIF (CMAKE_SYSTEM_NAME MATCHES "Darwin")

SET_TARGET_PROPERTIES(WorldServer PROPERTIES LINK_FLAGS "${WorldServer_LINK_FLAGS}")

target_link_libraries(
WorldServer
game
shared
zlib
framework
sockets
database
auth
config
pathfinding
vmaps
g3dlite
${READLINE_LIBRARY}
${TERMCAP_LIBRARY}
gomp
${SCRIPT_LIB}
${MYSQL_LIBRARIES}
${POSTGRESQL_LIBRARIES}
${SSLLIB}
${ACE_LIBRARY}
${ZLIB}
${OSX_LIBS}
)

install(TARGETS WorldServer DESTINATION bin)

########### install files ###############

install(FILES  worldserver.conf.dist DESTINATION bin)

