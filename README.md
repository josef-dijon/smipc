# smipc
[![CMake on multiple platforms](https://github.com/josef-dijon/smipc/actions/workflows/cmake-multi-platform.yml/badge.svg)](https://github.com/josef-dijon/smipc/actions/workflows/cmake-multi-platform.yml)

Shared memory IPC library

This is very much a work in progress project at the moment, so don't expect anything to build or work XD

The concept is to design an API description schema in json, the description is to be parsed by a tool that will generate service classes, serialisation classes, datatypes, documentation etc. from that decription. The goal is for multiservice systems, that one can define the API for each service, and then all serialisation and IPC code bolierplate is generated for the service.
