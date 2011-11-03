#!/bin/bash

patch -p0 -i build_on_glibc_2.14.patch
patch -p0 -i dont_ask_for_pin_on_sign.patch
patch -p0 -i use_rpath_origin.patch
