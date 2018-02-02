AM_DEFAULT_SOURCE_EXT = .cpp

check_PROGRAMS = TestNullFilter \
                 TestDelogoFilter \
                 TestDrawboxFilter \
                 TestFilterGenerator

TESTS = $(check_PROGRAMS)

AM_CPPFLAGS = -I../../src/filter-generator
LDADD = ../../src/filter-generator/libfilter-generator.a \
        $(BOOST_UNIT_TEST_FRAMEWORK_LIB)
