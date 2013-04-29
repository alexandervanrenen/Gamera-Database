all: tester util uebung1 uebung2 uebung3

objDir:= bin/
srcDir:= src/

-include config.local

CXX ?= g++
cf = -g0 -O3 -Werror -Wall -Wextra -Wuninitialized --std=c++0x -I./src -I./libs/gtest/include
lf = -g0 -O3 --std=c++0x -I./src
build_dir = @mkdir -p $(dir $@)

# Fix for clang
ifeq (ccache clang,$(filter $(CXX),ccache clang))
	cf += -Qunused-arguments -fcolor-diagnostics
	lf += -libstd=libc++
endif

-include src/LocalMakefile
obj_files := $(addprefix $(objDir),$(src_files))

tester: libs/gtest $(obj_files) bin/test/tester.o
	$(CXX) -o $@ bin/test/tester.o $(lf) $(obj_files) libs/gtest/libgtest.a -pthread

util: libs/gtest $(obj_files) bin/test/util/tester.o
	$(CXX) -o $@ bin/test/util/tester.o $(lf) $(obj_files) libs/gtest/libgtest.a -pthread

uebung1: libs/gtest $(obj_files) bin/test/external_sort/tester.o
	$(CXX) -o $@ bin/test/external_sort/tester.o $(lf) $(obj_files) libs/gtest/libgtest.a -pthread
bench1: libs/gtest $(obj_files) bin/test/bench1.o
	$(CXX) -o $@ bin/test/bench1.o $(lfp) $(obj_files) libs/gtest/libgtest.a -pthread

uebung2: libs/gtest $(obj_files) bin/test/buffer_manager/tester.o
	$(CXX) -o $@ bin/test/buffer_manager/tester.o $(lf) $(obj_files) libs/gtest/libgtest.a -pthread

uebung3: libs/gtest $(obj_files) bin/test/segment_manager/tester.o
	$(CXX) -o $@ bin/test/segment_manager/tester.o $(lf) $(obj_files) libs/gtest/libgtest.a -pthread -fPIC $(ld)

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
	find . -name "tester" -type f -delete
	find . -name "util" -type f -delete
	find . -name "uebung1" -type f -delete
	find . -name "uebung2" -type f -delete
	find . -name "uebung3" -type f -delete
