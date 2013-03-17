all: tester

objDir:= bin/
srcDir:= src/

-include config.local

CXX ?= g++
cf = -Werror -Wall -Wextra -Wuninitialized --std=c++0x -g0 -O3 -I./src -I./libs/gtest/include 
lf = -g0 -O3 --std=c++0x -I./src -I./libs/gtest/include
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

$(objDir)%.o: %.cpp
	$(build_dir)
	$(CXX) -MD -c -o $@ $< $(cf)
	@cp $(objDir)$*.d $(objDir)$*.P; \
		sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
			-e '/^$$/ d' -e 's/$$/ :/' < $(objDir)$*.d >> $(objDir)$*.P; \
		rm -f $(objDir)$*.d

-include $(objDir)*.P
-include $(objDir)*/*.P

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
	mkdir -p libs/gtest/include
	mv libs/gtest-1.6.0/include/gtest/* libs/gtest/include
	mv libs/gtest-1.6.0/build/libgtest.a libs/gtest/
	rm libs/gtest-1.6.0.zip
	rm -rf libs/gtest-1.6.0

clean:
	rm $(objDir) -rf
	find . -name "tester" -type f -delete
