## Settings for test subprojects


!isEmpty(BUILD_TESTS) {
##----------------------------------------------------------------------
## compile UnitTest if not yet done

system([ -e ../../ThirdParty/unittest-cpp-1.3/UnitTest++/libUnitTest++.a ] && echo "check libUnitTest++ ... ok" | grep ok ): UNITTEST_LIB_READY=true

isEmpty(UNITTEST_LIB_READY){
message("compile libUnitTest++")
system(cd ../../ThirdParty/unittest-cpp-1.3/UnitTest++/; ./build.pl)
}
##----------------------------------------------------------------------

LIBS += -L../../ThirdParty/unittest-cpp-1.3/UnitTest++ -lUnitTest++ 
INCLUDEPATH +=  ../../ThirdParty/unittest-cpp-1.3

}
