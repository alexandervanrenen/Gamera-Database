all: tester

buildDir:= bin/
srcDir:= src/
testDir:= test/

utility_inc:= -I../utility/include
utility_lib:= ../utility/libgda.a

cf :=-g3 -O0 -Werror -Wall -Wextra -Wuninitialized --std=c++11 $(utility_inc) -I./src
lf :=-g3 -O0 --std=c++0x -lpthread -I./src $(utility_inc)

ccache_use?= ccache
comp?= g++
comp:= $(ccache_use) $(comp)

buidl_folger=@mkdir -p $(dir $@)

# ===============================================
#   tester
# ===============================================

-include src/LocalMakefile
src_obj:= $(addprefix $(buildDir),$(src_files))

tester: $(src_obj) $(utility_lib)
	$(comp) -o $(buildDir)$@ test/tester.cpp $(src_obj) $(lf) -lgtest $(utility_lib)
	./bin/tester

# ===============================================
#   performance collector
# ===============================================

perf: $(src_obj) $(utility_lib)
	$(comp) -o $(buildDir)$@ test/perf.cpp $(src_obj) $(lf) -lgtest $(utility_lib)
	./bin/perf

# ===============================================
#   build individual files
# ===============================================

$(buildDir)%o: $(srcDir)%cpp $(srcDir)%hpp
	$(buidl_folger)
	$(comp) -c -o $@ $< $(cf)

# ====================================
#   clean up command
# ====================================

clean:
	rm $(buildDir) -rf && find . -maxdepth 1 -name 'tester' -delete

