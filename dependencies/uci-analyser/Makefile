# Makefile for a UCI-based analyser.
#
# This file is part of uci-analyser: a UCI-based Chess Game Analyser
# Copyright (C) 2013-2017 David J. Barnes
#
# uci-analyser is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# uci-analyser is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with uci-analyser.  If not, see <http://www.gnu.org/licenses/>.
#
# David J. Barnes may be contacted as d.j.barnes@kent.ac.uk
# https://www.cs.kent.ac.uk/people/staff/djb/
# Available from: http://www.cs.kent.ac.uk/~djb/chessplag/

CC=g++
# CFLAGS=-c -g -Wall -O3 -D__unix__ # Linux Debug
# CFLAGS=-c -g -Wall -O3 -D__unix__ -DPRODUCTION # Linux
# CFLAGS= -std=c++17 -c -g -Wall -O3 # Windows Debug Eval
CFLAGS= -std=c++17 -c -g -Wall -O3 -DPRODUCTION
OBJS=analyse.o evaluation.o engine.o utils.o interpret.o
TARGET=analyse

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

$(TARGET) : $(OBJS)
	$(CC) -o $@ $(OBJS)

clean:
	rm -f $(OBJS) $(TARGET)

engine.o : engine.cpp engine.h
evaluation.o : evaluation.cpp evaluation.h utils.h interpret.hpp
analyse.o : analyse.cpp engine.h evaluation.h utils.h
utils.o : utils.cpp utils.h
interpret.o : interpret.cpp interpret.hpp
