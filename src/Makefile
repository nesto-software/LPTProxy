CPPFLAGS="-I${STAGING_DIR}/usr/local/include/" "-I${STAGING_DIR}/local/include/"
LINKFLAGS="-L${STAGING_DIR}/usr/local/lib" "-L${STAGING_DIR}/local/lib"
CC=${TOOLCHAIN}-gcc 
CXX=${TOOLCHAIN}-g++
LDLIBS=-l:libwiringPi.so.2.60

lptproxy: main.c RetroPrinter_Capture.c rpleds.c
	$(CC) $(CPPFLAGS) $(LINKFLAGS) -Wall -Wno-null-character-ignored main.c RetroPrinter_Capture.c rpleds.c $(LDLIBS) -o lptproxy