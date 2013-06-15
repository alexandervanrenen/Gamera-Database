all: bin/tester bin/server bin/client bin/driver

# Define compile and link flags
-include config.local
CXX ?= g++
#opt = -g3 -O0
opt = -g -O3
cf = $(opt) -Wall -Wextra -Wuninitialized --std=c++0x -I./src -I. -I./libs/gtest/include -I./libs/zmq/include/ -I./libs/gflags/include/ -fPIC
lf = $(opt) --std=c++0x -ldl -lpthread -lrt

# Object director
objDir:= build/
build_dir = @mkdir -p $(dir $@)

# Fix for clang
ifeq (ccache clang,$(filter $(CXX),ccache clang))
	cf += -Qunused-arguments -fcolor-diagnostics
	lf += -libstd=libc++
endif

# Get source file names
src_files := $(patsubst src/%,build/src/%, $(patsubst %.cpp,%.o,$(wildcard src/*.cpp src/*/*.cpp src/*/*/*.cpp)))
test_files := $(patsubst test/%,build/test/%, $(patsubst %.cpp,%.o,$(wildcard test/*.cpp test/*/*.cpp test/*/*/*.cpp)))

# Build database
bin/database.so: libs src/query_parser/Parser.cpp $(src_files)
	$(build_dir) bin/gen bin/var
	$(CXX) -shared -o bin/database.so $(src_files) libs/zmq/libzmq.a $(lf)

# Build tester
bin/tester: libs bin/database.so $(test_files) build/test/tester.o
	$(build_dir) bin/gen bin/var
	$(CXX) -o bin/tester $(test_files) bin/database.so libs/gtest/libgtest.a $(lf)

# Build driver
bin/driver: libs bin/database.so build/driver.o
	$(build_dir) bin/gen bin/var
	$(CXX) -o bin/driver build/driver.o bin/database.so libs/gflags/libgflags.a $(lf)

# Build server
bin/server: libs bin/database.so build/server.o
	$(build_dir) bin/gen bin/var
	$(CXX) -o bin/server build/server.o bin/database.so libs/zmq/libzmq.a libs/gflags/libgflags.a $(lf)

# Build client
bin/client: libs build/client.o
	$(build_dir) bin/gen bin/var
	$(CXX) -o bin/client build/client.o libs/zmq/libzmq.a libs/gflags/libgflags.a $(lf)

# Ensure latest parser version
src/query_parser/Parser.cpp: src/query_parser/Parser.leg
	./libs/greg-cpp/greg -o src/query_parser/Parser.cpp src/query_parser/Parser.leg

libs: libs/gtest libs/zmq libs/greg-cpp libs/gflags

# Command for building and keeping track of changed files 
$(objDir)%.o: %.cpp
	$(build_dir)
	$(CXX) -MD -c -o $@ $< $(cf)
	@cp $(objDir)$*.d $(objDir)$*.P; \
		sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
			-e '/^$$/ d' -e 's/$$/ :/' < $(objDir)$*.d >> $(objDir)$*.P; \
		rm -f $(objDir)$*.d

-include $(objDir)*.P
-include $(objDir)*/*.P
-include $(objDir)*/*/*.P
-include $(objDir)*/*/*/*.P

# Build gtest library
libs/gtest:
	$(build_dir)
	cd libs/ ;\
	wget -O gtest-1.6.0.zip https://googletest.googlecode.com/files/gtest-1.6.0.zip ;\
	unzip -q gtest-1.6.0.zip ;\
	cd gtest-1.6.0 ;\
	mkdir -p build ;\
	cd build ;\
	cmake -G"Unix Makefiles" .. ;\
	make ;\
	ar -r libgtest.a libgtest_main.a
	mkdir -p libs/gtest/include/gtest
	mv libs/gtest-1.6.0/include/gtest/* libs/gtest/include/gtest
	mv libs/gtest-1.6.0/build/libgtest.a libs/gtest/
	rm libs/gtest-1.6.0.zip
	rm -rf libs/gtest-1.6.0

# Build tbb library
libs/tbb:
	$(build_dir)
	cd libs/ ;\
	wget http://threadingbuildingblocks.org/sites/default/files/software_releases/source/tbb41_20130116oss_src.tgz ;\
	tar -xaf tbb41_20130116oss_src.tgz ;\
	cd tbb41_20130116oss ;\
	make tbb -j4 ;\
	rm build/*_debug/* -rf ;\
	cd .. ;\
	mkdir -p tbb ;\
	find . -name "libtbb*.*" -exec mv {} ./tbb/ \; ;\
	mv tbb41_20130116oss/include/ tbb/ ;\
	rm tbb41_20130116oss* -rf

# Build zmq
libs/zmq:
	$(build_dir)
	cd libs/ ;\
	wget http://download.zeromq.org/zeromq-3.2.3.tar.gz ;\
	tar -xaf zeromq-3.2.3.tar.gz ;\
	cd zeromq-3.2.3 ;\
	./configure --enable-static --disable-shared --prefix ${PWD}/libs/zmq ;\
	make -j4 ;\
	make install ;\
	cd .. ;\
	rm zeromq-3.2.3.tar.gz ;\
	rm zeromq-3.2.3 -rf ;\
	rm zmq/share -rf ;\
	mv zmq/lib/libzmq.a zmq/ ;\
	rm zmq/lib -rf ;\
	mkdir zmq/include/zmq ;\
	mv zmq/include/*.h zmq/include/zmq/
	# now get c++ header files for zmq
	cd libs ;\
	git clone https://github.com/zeromq/cppzmq.git ;\
	mv cppzmq/zmq.hpp zmq/include/zmq/ ;\
	sed -i "s/#include <zmq.h>/#include \"zmq.h\"/g" zmq/include/zmq/zmq.hpp ;\
	rm cppzmq -rf

# Build greg
libs/greg-cpp:
	$(build_dir)
	cd libs/ ;\
	git clone git@github.com:alexandervanrenen/greg-cpp.git ;\
	cd greg-cpp ;\
	make

# Build gflags
libs/gflags:
	$(build_dir)
	cd libs/ ;\
	wget http://gflags.googlecode.com/files/gflags-2.0-no-svn-files.tar.gz ;\
	tar -xaf gflags-2.0-no-svn-files.tar.gz ;\
	cd gflags-2.0 ;\
	./configure --enable-static --disable-shared --prefix ${PWD}/libs/gflags ;\
	make -j4 ;\
	make install ;\
	cd .. ;\
	rm gflags-2.0-no-svn-files.tar.gz ;\
	rm gflags-2.0 -rf ;\
	mv gflags/lib/libgflags.a gflags/ ;\
	rm gflags/bin -rf ;\
	rm gflags/include/google -rf ;\
	rm gflags/share -rf ;\
	rm gflags/lib -rf

# Clean up =)
clean:
	rm bin -rf
	rm build -rf
	touch src/query_parser/Parser.cpp

# Clean up =)
clean-complete:
	rm bin -rf
	rm build -rf
	rm libs -rf
	rm src/query_parser/Parser.cpp
