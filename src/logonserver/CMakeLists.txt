########### next target ###############

SET(logonserver_SRCS
AuthCodes.h
AuthSocket.cpp
AuthSocket.h
Main.cpp
RealmList.cpp
RealmList.h
)

SET(logonserver_LINK_FLAGS "")

add_executable(logonserver ${logonserver_SRCS})
add_definitions(
-D_LOGON_REALM_CONFIG='"${CONF_DIR}/LogonServer.conf"'
)
IF (DO_MYSQL)
   SET(logonserver_LINK_FLAGS "-pthread ${logonserver_LINK_FLAGS}")
ENDIF(DO_MYSQL)
IF (DO_POSTGRESQL)
   SET(logonserver_LINK_FLAGS "-pthread ${logonserver_LINK_FLAGS}")
ENDIF(DO_POSTGRESQL)

IF (CMAKE_SYSTEM_NAME MATCHES "Darwin")
   SET(logonserver_LINK_FLAGS "-framework Carbon ${logonserver_LINK_FLAGS}")
ENDIF (CMAKE_SYSTEM_NAME MATCHES "Darwin")

SET_TARGET_PROPERTIES(logonserver PROPERTIES LINK_FLAGS "${logonserver_LINK_FLAGS}")

target_link_libraries(
logonserver
shared
framework
sockets
database
auth
config
zlib
gomp
${SSLLIB}
${MYSQL_LIBRARIES}
${POSTGRESQL_LIBRARIES}
${OSX_LIBS}
)

install(TARGETS logonserver DESTINATION bin)


########### install files ###############

install(FILES logonserver.conf.dist DESTINATION bin)


