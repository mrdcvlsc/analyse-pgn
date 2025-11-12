CMAKE_BUILD_DIR := build
CMAKE_CONFIG ?= Release

# choose compiler: default gcc. On Ubuntu you can pass COMPILER=clang to use clang.
COMPILER ?= gcc

all:
	@echo "Configuring and building with CMake (build dir: ${CMAKE_BUILD_DIR}, config: ${CMAKE_CONFIG}, compiler: ${COMPILER}, target: ${TARGET})"
	@{ \
	if [ "${TARGET}" = "windows" ]; then \
	  CMAKE_CC=gcc; CMAKE_CXX=g++; GENARGS="-G \"MinGW Makefiles\""; \
	else \
	  if [ "${COMPILER}" = "clang" ]; then CMAKE_CC=clang; CMAKE_CXX=clang++; else CMAKE_CC=gcc; CMAKE_CXX=g++; fi; \
	  GENARGS=""; \
	fi; \
	cmake -S . -B ${CMAKE_BUILD_DIR} -DCMAKE_BUILD_TYPE=${CMAKE_CONFIG} -DCMAKE_C_COMPILER=$${CMAKE_CC} -DCMAKE_CXX_COMPILER=$${CMAKE_CXX} $${GENARGS}; \
	cmake --build ${CMAKE_BUILD_DIR} --config ${CMAKE_CONFIG}; \
	}

test:
	@cmake --build ${CMAKE_BUILD_DIR} --config ${CMAKE_CONFIG} --target test

package:
	@cmake --build ${CMAKE_BUILD_DIR} --config ${CMAKE_CONFIG} --target package

win_config:
	cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -G "MinGW Makefiles"

win_build:
	cmake --build build --config Debug -- -j8

win_config_rel:
	cmake -S . -B build-release -DCMAKE_BUILD_TYPE=Release -G "MinGW Makefiles"

win_build_rel:
	cmake --build build-release --config Release -- -j8


clean:
	@echo "removing pgn-extract object files"
	@$(MAKE) -C dependencies/pgn-extract clean || true
	@echo "removing uci-analyse object files"
	@$(MAKE) -C dependencies/uci-analyser clean || true
	@echo "removing analyse-pgn binaries files"
	@rm -f ./bin/analyse ./bin/analyse/analyse ./bin/analyse/analyse.exe ./bin/pgn-extract/pgn-extract ./bin/pgn-extract/pgn-extract.exe ./apgn ./apgn.exe ./pgn_samples/first.analyzed.pgn ./pgn_samples/first.stats.txt || true
	@echo "Removing CMake build directory"
	@if [ -d "${CMAKE_BUILD_DIR}" ]; then rm -rf ${CMAKE_BUILD_DIR}; fi