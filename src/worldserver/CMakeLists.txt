
########### next target ###############

SET(worldserver_SRCS
CliRunnable.cpp
CliRunnable.h
Main.cpp
Master.cpp
Master.h
RASocket.cpp
RASocket.h
WorldRunnable.cpp
WorldRunnable.h
Soap/Soap.cpp
Soap/Soap.h
Soap/soapC.cpp
Soap/soapH.h
Soap/soapServer.cpp
Soap/soapStub.h
)

SET(worldserver_LINK_FLAGS "")

add_executable(worldserver${worldserver_SRCS})
add_definitions(
-D_WORLD_CORE_CONFIG='"${CONF_DIR}/WorldServer.conf"'
)
IF (DO_MYSQL)
    SET(worldserver_LINK_FLAGS "-pthread ${worldserver_LINK_FLAGS}")
ENDIF(DO_MYSQL)
IF (DO_POSTGRESQL)
    SET(worldserver_LINK_FLAGS "-pthread ${worldserver_LINK_FLAGS}")
ENDIF(DO_POSTGRESQL)


IF (CMAKE_SYSTEM_NAME MATCHES "Darwin")
    SET(worldserver_LINK_FLAGS "-framework Carbon ${worldserver_LINK_FLAGS}")
    SET(SCRIPT_LIB "")
ENDIF (CMAKE_SYSTEM_NAME MATCHES "Darwin")

SET_TARGET_PROPERTIES(worldserver PROPERTIES LINK_FLAGS "${worldserver_LINK_FLAGS}")

target_link_libraries(
worldserver
game
shared
zlib
xframework
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

install(TARGETS worldserver DESTINATION bin)


########### install files ###############

install(FILES  worldserver.conf.dist DESTINATION bin)

