AM_DEFAULT_SOURCE_EXT = .cpp

check_PROGRAMS = TestNullFilter \
                 TestFilterGenerator

TESTS = $(check_PROGRAMS)

AM_CPPFLAGS = -I../../src
AM_LDFLAGS = -L../../src/filter-generator
LDADD = -lfilter-generator $(BOOST_UNIT_TEST_FRAMEWORK_LIB)

