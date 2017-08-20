#!/bin/bash

find ./ -name *.c > SourcePathbak

find ./ -name *.cpp >> SourcePathbak
find ./ -name *.cxx >> SourcePathbak
find ./ -name *.cc >> SourcePathbak
find ./ -name *.C >> SourcePathbak
find ./ -name *.c++ >> SourcePathbak
