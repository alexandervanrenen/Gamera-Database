all: tester util uebung1 uebung2 uebung3

objDir:= build/
srcDir:= src/

-include config.local

CXX ?= g++
cf = -g -O3 -Werror -Wall -Wextra -Wuninitialized --std=c++0x -I./src -I./libs/gtest/include
lf = -g -O3 --std=c++0x -I./src
build_dir = @mkdir -p $(dir $@)

# Fix for clang
ifeq (ccache clang,$(filter $(CXX),ccache clang))
	cf += -Qunused-arguments -fcolor-diagnostics
	lf += -libstd=libc++
endif

-include src/LocalMakefile
obj_files := $(addprefix $(objDir),$(src_files))

tester: libs/gtest $(obj_files) build/test/tester.o
	$(build_dir) bin
	$(CXX) -o bin/tester build/test/tester.o $(lf) $(obj_files) libs/gtest/libgtest.a -pthread

util: libs/gtest $(obj_files) build/test/util/tester.o
	$(build_dir) bin
	$(CXX) -o bin/util build/test/util/tester.o $(lf) $(obj_files) libs/gtest/libgtest.a -pthread

uebung1: libs/gtest $(obj_files) build/test/external_sort/tester.o
	$(build_dir) bin
	$(CXX) -o bin/uebung1 build/test/external_sort/tester.o $(lf) $(obj_files) libs/gtest/libgtest.a -pthread
sort: libs/gtest $(obj_files) build/test/external_sort/uebung1_abgabe.o
	$(build_dir) bin
	$(CXX) -o bin/sort build/test/external_sort/uebung1_abgabe.o $(lfp) $(obj_files) libs/gtest/libgtest.a -pthread

uebung2: libs/gtest $(obj_files) build/test/buffer_manager/tester.o
	$(build_dir) bin
	$(CXX) -o bin/uebung2 build/test/buffer_manager/tester.o $(lf) $(obj_files) libs/gtest/libgtest.a -pthread

uebung3: libs/gtest $(obj_files) build/test/segment_manager/tester.o
	$(build_dir) bin
	$(CXX) -o bin/uebung3 build/test/segment_manager/tester.o $(lf) $(obj_files) libs/gtest/libgtest.a -pthread -fPIC $(ld)

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

# build gtest
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

# build tbb	
libs/tbb:
	$(build_dir)
	cd libs/ ;\
	wget http://threadingbuildingblocks.org/sites/default/files/software_releases/source/tbb41_20130116oss_src.tgz ;\
	tar -xaf tbb41_20130116oss_src.tgz ;\
	cd tbb41_20130116oss ;\
	make tbb ;\
	rm build/*_debug/* -rf ;\
	cd .. ;\
	mkdir -p tbb ;\
	find . -name "libtbb*.*" -exec mv {} ./tbb/ \; ;\
	mv tbb41_20130116oss/include/ tbb/ ;\
	rm tbb41_20130116oss* -rf

clean:
	rm $(objDir) -rf
	rm $(objDir) -rf
