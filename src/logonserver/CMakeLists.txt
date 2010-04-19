########### next target ###############

SET(logonserver_SRCS
AuthCodes.h
AuthSocket.cpp
AuthSocket.h
Main.cpp
RealmList.cpp
RealmList.h
)

SET(LogonServer_LINK_FLAGS "")

add_executable(LogonServer ${LogonServer_SRCS})
add_definitions(
-D_LOGON_CONFIG='"${CONF_DIR}/LogonServer.conf"'
)
IF (DO_MYSQL)
    SET(LogonServer_LINK_FLAGS "-pthread ${LogonServer_LINK_FLAGS}")
ENDIF(DO_MYSQL)
IF (DO_POSTGRESQL)
   SET(LogonServer_LINK_FLAGS "-pthread ${LogonServer_LINK_FLAGS}")
ENDIF(DO_POSTGRESQL)

IF (CMAKE_SYSTEM_NAME MATCHES "Darwin")
    SET(LogonServer_LINK_FLAGS "-framework Carbon ${LogonServer_LINK_FLAGS}")
ENDIF (CMAKE_SYSTEM_NAME MATCHES "Darwin")

SET_TARGET_PROPERTIES(LogonServer PROPERTIES LINK_FLAGS "${LogonServer_LINK_FLAGS}")

target_link_libraries(
LogonServer
shared
framework
sockets
database
auth
config
zlib
${SSLLIB}
${MYSQL_LIBRARIES}
${POSTGRESQL_LIBRARIES}
${OSX_LIBS}
)

install(TARGETS LogonServer DESTINATION bin)

########### install files ###############

install(FILES  logonserver.conf.dist DESTINATION etc)

