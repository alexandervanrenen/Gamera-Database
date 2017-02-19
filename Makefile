all: bin/tester bin/server bin/client bin/driver

# Define compile and link flags
-include config.local
CXX ?= g++
opt = -g3 -O0
#opt = -g0 -O3
cf = $(opt) -Wall -Wextra -Wuninitialized --std=c++0x -I./src -I. -I./libs/gtest/include -I./libs/zmq/include/ -I./libs/gflags/include/ -fPIC
lf = $(opt) --std=c++0x -ldl -lpthread

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
bin/database.so: libs src/query/parser/Parser.cpp $(src_files)
	$(build_dir) bin/gen bin/var
	$(CXX) -shared -o bin/database.so $(src_files) libs/zmq/libzmq.a $(lf)

# Build tester
bin/tester: libs bin/database.so $(test_files) build/test/tester.o
	$(build_dir) bin/gen bin/var
	$(CXX) -o bin/tester $(test_files) bin/database.so libs/gtest/libgtest.a $(lf)

# Build driver
bin/driver: libs bin/database.so build/driver.o
	$(build_dir) bin/gen bin/var
	$(CXX) -o bin/driver build/driver.o bin/database.so $(lf)

# Build server
bin/server: libs bin/database.so build/server.o
	$(build_dir) bin/gen bin/var
	$(CXX) -o bin/server build/server.o bin/database.so libs/zmq/libzmq.a $(lf)

# Build client
bin/client: libs build/client.o
	$(build_dir) bin/gen bin/var
	$(CXX) -o bin/client build/client.o libs/zmq/libzmq.a $(lf)

# Ensure latest parser version
src/query/parser/Parser.cpp: src/query/parser/Parser.leg
	./libs/greg-cpp/greg -o src/query/parser/Parser.cpp src/query/parser/Parser.leg

LIBS := libs/gtest libs/zmq libs/greg-cpp
libs: $(LIBS)

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
	git clone https://github.com/google/googletest.git;\
	mv googletest gtest-1.7.0;\
	cd gtest-1.7.0;\
	git checkout release-1.7.0;\
	mkdir -p build ;\
	cd build ;\
	cmake -G"Unix Makefiles" .. ;\
	make ;\
	ar -r libgtest.a libgtest_main.a
	mkdir -p libs/gtest/include/gtest
	mv libs/gtest-1.7.0/include/gtest/* libs/gtest/include/gtest
	mv libs/gtest-1.7.0/build/libgtest.a libs/gtest/
	rm -rf libs/gtest-1.7.0

# Build zmq
libs/zmq:
	$(build_dir)
	cd libs/ ;\
	wget http://download.zeromq.org/zeromq-3.2.3.tar.gz ;\
	tar -xf zeromq-3.2.3.tar.gz ;\
	cd zeromq-3.2.3 ;\
	./configure --enable-static --disable-shared --prefix ${PWD}/libs/zmq ;\
	make -j4 ;\
	make install ;\
	cd .. ;\
	rm zeromq-3.2.3.tar.gz ;\
	rm -rf zeromq-3.2.3 ;\
	rm -rf zmq/share ;\
	mv zmq/lib/libzmq.a zmq/ ;\
	rm -rf zmq/lib ;\
	mkdir zmq/include/zmq ;\
	mv zmq/include/*.h zmq/include/zmq/
	# now get c++ header files for zmq
	cd libs ;\
	git clone https://github.com/zeromq/cppzmq.git ;\
	mv cppzmq/zmq.hpp zmq/include/zmq/ ;\
	ruby -pi -e 'gsub("#include <zmq.h>", "#include \"zmq.h\"")' zmq/include/zmq/zmq.hpp ;\
	rm -rf cppzmqx

# Build greg
libs/greg-cpp:
	$(build_dir)
	cd libs/ ;\
	git clone https://github.com/alexandervanrenen/greg-cpp.git ;\
	cd greg-cpp ;\
	make

# Clean up =)
clean:
	rm -rf bin
	rm -rf build
	touch src/query/parser/Parser.cpp

# Clean up =)
clean-complete:
	rm -rf bin
	rm -rf build
	rm -rf libs
	rm -rf src/query/parser/Parser.cpp
