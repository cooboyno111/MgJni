# build helloworld executable when user executes "make"
libTinyServer.so: mongoose.o TinyServer.o
	$(CC) -Wall -shared -o libTinyServer.so TinyServer.o mongoose.o -DMG_ENABLE_SSL -lcrypto -lssl
TinyServer.o: TinyServer.c
	$(CC) -Wall -fPIC -c TinyServer.c -o TinyServer.o -DMG_ENABLE_SSL -lcrypto -lssl
mongoose.o: mongoose.c
	$(CC) -Wall -fPIC -c mongoose.c -o mongoose.o -DMG_ENABLE_SSL -lcrypto -lssl

# remove object files and executable when user executes "make clean"
clean:
	rm *.o 
	rm *.so

