
########### next target ###############

SET(auth_STAT_SRCS
   AuthCrypt.cpp
   AuthCrypt.h
   BigNumber.cpp
   BigNumber.h
   HMACSHA1.cpp
   HMACSHA1.h
   Sha1.cpp
   Sha1.h
   md5.c
   md5.h
   SARC4.cpp
   SARC4.h
)

add_library(auth STATIC ${auth_STAT_SRCS})
