#!/bin/sh

g++ -g -o transmitter transmitter.cxx
g++ -g -o receiver receiver.cxx
g++ -g -o simulator simulator.cxx -lmodbus
