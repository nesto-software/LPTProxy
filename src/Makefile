CPPFLAGS="-I${STAGING_DIR}/usr/local/include/" "-I${STAGING_DIR}/local/include/"
LINKFLAGS="-L${STAGING_DIR}/usr/local/lib"
CC=${TOOLCHAIN}-gcc 
CXX=${TOOLCHAIN}-g++

lptproxy: main.c RetroPrinter_Capture.c rpleds.c
	$(CC) $(CPPFLAGS) $(LINKFLAGS) -Wall -o lptproxy main.c RetroPrinter_Capture.c rpleds.c