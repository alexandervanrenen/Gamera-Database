all: tester

buildDir:= bin/
srcDir:= src/
testDir:= test/

utility_inc:= -I../utility/include
utility_lib:= ../utility/libgda.a

cf :=-g3 -O0 -Werror -Wall -Wextra -Wuninitialized --std=c++11 $(utility_inc) -I./src
lf :=-g3 -O0 --std=c++0x $(engine_lib)-lpthread -I./src

ccache_use?= ccache
comp?= g++
comp:= $(ccache_use) $(comp)

$(buildDir):
	mkdir -p $(buildDir)

# ===============================================
#   tester
# ===============================================

-include src/LocalMakefile
src_obj:= $(addprefix $(buildDir),$(mokey_files))

tester: $(buildDir) $(src_obj) $(engine_lib) $(utility_lib)
	$(comp) -o $(buildDir)$@ test/tester.cpp $(src_obj) $(lf) -lgtest

# ===============================================
#   build individual files
# ===============================================

$(buildDir)%o: $(srcDir)%cpp $(srcDir)%hpp
	$(comp) -c -o $@ $< $(cf)

# ====================================
#   clean up command
# ====================================

clean:
	rm $(buildDir) -rf && find . -maxdepth 1 -name 'tester' -delete

