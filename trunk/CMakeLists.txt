cmake_minimum_required (VERSION 2.8.7)

# Use this to make static binaries.
# SET(CMAKE_EXE_LINKER_FLAGS "-static")

if (USE_STDLIBCPP)
  set(LIBCPP libstdc++)
else()
  set(LIBCPP libc++)
endif()

# if(APPLE)
#    SET(CMAKE_OSX_DEPLOYMENT_TARGET "10.7")
#    set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++0x")
#    set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "${LIBCPP}")
# endif(APPLE)

project (chuffed CXX)

set (CMAKE_CXX_STANDARD 11)

# The version number.
set (chuffed_VERSION_MAJOR 0)
set (chuffed_VERSION_MINOR 9)
set (chuffed_VERSION_PATCH 0)

include_directories("chuffed")
include_directories("cpp-integration/protobuf")
include_directories(".")
include_directories(${CMAKE_BINARY_DIR})
if (WIN32)
  include_directories("cpp-integration/protobuf/config_win")
  add_definitions(-DYY_NO_UNISTD_H)
else()
  include_directories("cpp-integration/protobuf/config_unix")
endif()

SET_SOURCE_FILES_PROPERTIES(
chuffed/vars/int-var.c
chuffed/vars/int-var-el.c
chuffed/vars/modelling.c
chuffed/vars/int-var-sl.c
chuffed/vars/bool-view.c
chuffed/vars/int-var-ll.c
chuffed/parallel/master.c
chuffed/parallel/slave.c
chuffed/ldsb/ldsb.c
chuffed/globals/subcircuit.c
chuffed/globals/mddglobals.c
chuffed/globals/sym-break.c
chuffed/globals/linear-bool.c
chuffed/globals/linear-bool-decomp.c
chuffed/globals/well-founded.c
chuffed/globals/circuit.c
chuffed/globals/minimum.c
chuffed/globals/alldiff.c
chuffed/globals/template.c
chuffed/globals/directives.c
chuffed/globals/cumulative.c
chuffed/globals/cumulativeCalendar.c
chuffed/globals/disjunctive.c
chuffed/globals/regular.c
chuffed/globals/lex.c
chuffed/globals/table.c
chuffed/mdd/MDD.c
chuffed/mdd/mdd_prop.c
chuffed/mdd/mdd_to_lgraph.c
chuffed/mdd/MurmurHash3.c
chuffed/mdd/opcache.c
chuffed/mdd/weighted_dfa.c
chuffed/mdd/wmdd_prop.c
chuffed/mip/mip.c
chuffed/mip/recalc.c
chuffed/mip/simplex.c
chuffed/primitives/element.c
chuffed/primitives/bool.c
chuffed/primitives/linear.c
chuffed/primitives/arithmetic.c
chuffed/primitives/binary.c
chuffed/flatzinc/parser.tab.c
chuffed/flatzinc/registry.c
chuffed/flatzinc/flatzinc.c
chuffed/flatzinc/lexer.yy.c
chuffed/branching/branching.c
chuffed/core/init.c
chuffed/core/stats.c
chuffed/core/engine.c
chuffed/core/options.c
chuffed/core/sat.c
chuffed/core/conflict.c
chuffed/flatzinc/fzn_chuffed.c
PROPERTIES LANGUAGE CXX )

add_library(chuffed
chuffed/vars/int-var.c
chuffed/vars/int-var-el.c
chuffed/vars/modelling.c
chuffed/vars/int-var-sl.c
chuffed/vars/bool-view.c
chuffed/vars/int-var-ll.c
chuffed/parallel/master.c
chuffed/parallel/slave.c
chuffed/ldsb/ldsb.c
chuffed/globals/subcircuit.c
chuffed/globals/mddglobals.c
chuffed/globals/sym-break.c
chuffed/globals/linear-bool.c
chuffed/globals/linear-bool-decomp.c
chuffed/globals/well-founded.c
chuffed/globals/circuit.c
chuffed/globals/minimum.c
chuffed/globals/alldiff.c
chuffed/globals/template.c
chuffed/globals/directives.c
chuffed/globals/cumulative.c
chuffed/globals/cumulativeCalendar.c
chuffed/globals/disjunctive.c
chuffed/globals/regular.c
chuffed/globals/lex.c
chuffed/globals/table.c
chuffed/mdd/MDD.c
chuffed/mdd/mdd_prop.c
chuffed/mdd/mdd_to_lgraph.c
chuffed/mdd/MurmurHash3.c
chuffed/mdd/opcache.c
chuffed/mdd/weighted_dfa.c
chuffed/mdd/wmdd_prop.c
chuffed/mip/mip.c
chuffed/mip/recalc.c
chuffed/mip/simplex.c
chuffed/primitives/element.c
chuffed/primitives/bool.c
chuffed/primitives/linear.c
chuffed/primitives/arithmetic.c
chuffed/primitives/binary.c
chuffed/branching/branching.c
chuffed/core/init.c
chuffed/core/stats.c
chuffed/core/engine.c
chuffed/core/options.c
chuffed/core/sat.c
chuffed/core/conflict.c
chuffed/vars/int-var.h
chuffed/vars/vars.h
chuffed/vars/int-var-sl.h
chuffed/vars/int-var-ll.h
chuffed/vars/int-view.h
chuffed/vars/int-var-el.h
chuffed/vars/modelling.h
chuffed/vars/bool-view.h
chuffed/parallel/parallel.h
chuffed/support/vec.h
chuffed/support/misc.h
chuffed/support/heap.h
chuffed/support/sparse_set.h
chuffed/ldsb/ldsb.h
chuffed/globals/globals.h
chuffed/globals/mddglobals.h
chuffed/mdd/CFG.h
chuffed/mdd/circ_fns.h
chuffed/mdd/circutil.h
chuffed/mdd/CYK.h
chuffed/mdd/MDD.h
chuffed/mdd/mdd_prop.h
chuffed/mdd/mdd_to_lgraph.h
chuffed/mdd/MurmurHash3.h
chuffed/mdd/opcache.h
chuffed/mdd/opts.h
chuffed/mdd/sorters.h
chuffed/mdd/weighted_dfa.h
chuffed/mdd/wmdd_prop.h
chuffed/mip/mip.h
chuffed/mip/simplex.h
chuffed/primitives/primitives.h
chuffed/branching/branching.h
chuffed/core/sat.h
chuffed/core/engine.h
chuffed/core/sat-types.h
chuffed/core/options.h
chuffed/core/propagator.h

cpp-integration/connector.cpp
cpp-integration/message.pb.cpp

cpp-integration/protobuf/google/protobuf/compiler/importer.cc
cpp-integration/protobuf/google/protobuf/compiler/parser.cc
cpp-integration/protobuf/google/protobuf/descriptor.cc
cpp-integration/protobuf/google/protobuf/descriptor.pb.cc
cpp-integration/protobuf/google/protobuf/descriptor_database.cc
cpp-integration/protobuf/google/protobuf/dynamic_message.cc
cpp-integration/protobuf/google/protobuf/extension_set.cc
cpp-integration/protobuf/google/protobuf/extension_set_heavy.cc
cpp-integration/protobuf/google/protobuf/generated_message_reflection.cc
cpp-integration/protobuf/google/protobuf/generated_message_util.cc
cpp-integration/protobuf/google/protobuf/io/coded_stream.cc
cpp-integration/protobuf/google/protobuf/io/gzip_stream.cc
cpp-integration/protobuf/google/protobuf/io/printer.cc
cpp-integration/protobuf/google/protobuf/io/strtod.cc
cpp-integration/protobuf/google/protobuf/io/tokenizer.cc
cpp-integration/protobuf/google/protobuf/io/zero_copy_stream.cc
cpp-integration/protobuf/google/protobuf/io/zero_copy_stream_impl.cc
cpp-integration/protobuf/google/protobuf/io/zero_copy_stream_impl_lite.cc
cpp-integration/protobuf/google/protobuf/message.cc
cpp-integration/protobuf/google/protobuf/message_lite.cc
cpp-integration/protobuf/google/protobuf/reflection_ops.cc
cpp-integration/protobuf/google/protobuf/repeated_field.cc
cpp-integration/protobuf/google/protobuf/service.cc
cpp-integration/protobuf/google/protobuf/stubs/atomicops_internals_x86_gcc.cc
cpp-integration/protobuf/google/protobuf/stubs/atomicops_internals_x86_msvc.cc
cpp-integration/protobuf/google/protobuf/stubs/common.cc
cpp-integration/protobuf/google/protobuf/stubs/once.cc
cpp-integration/protobuf/google/protobuf/stubs/stringprintf.cc
cpp-integration/protobuf/google/protobuf/stubs/structurally_valid.cc
cpp-integration/protobuf/google/protobuf/stubs/strutil.cc
cpp-integration/protobuf/google/protobuf/stubs/substitute.cc
cpp-integration/protobuf/google/protobuf/text_format.cc
cpp-integration/protobuf/google/protobuf/unknown_field_set.cc
cpp-integration/protobuf/google/protobuf/wire_format.cc
cpp-integration/protobuf/google/protobuf/wire_format_lite.cc
)

add_library(chuffed_fzn
chuffed/flatzinc/parser.tab.c
chuffed/flatzinc/registry.c
chuffed/flatzinc/flatzinc.c
chuffed/flatzinc/lexer.yy.c
chuffed/flatzinc/parser.tab.h
chuffed/flatzinc/flatzinc.h
chuffed/flatzinc/ast.h
)

add_executable(fzn_chuffed chuffed/flatzinc/fzn_chuffed.c)
target_link_libraries(fzn_chuffed chuffed_fzn chuffed -lpthread)

add_custom_target(
  versionTarget
  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/update-version.sh ${CMAKE_BINARY_DIR}/version.h
)
add_dependencies(fzn_chuffed versionTarget)

INSTALL(TARGETS fzn_chuffed chuffed
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)

MACRO(INSTALL_HEADERS_WITH_DIRECTORY HEADER_LIST)

    FOREACH(HEADER ${${HEADER_LIST}})
        STRING(REGEX MATCH "(.\\\*)\\\[/\\\]" DIR ${HEADER})
        INSTALL(FILES ${HEADER} DESTINATION include/${DIR})
    ENDFOREACH(HEADER)

ENDMACRO(INSTALL_HEADERS_WITH_DIRECTORY)

INSTALL_HEADERS_WITH_DIRECTORY(
chuffed/vars/int-var.h
chuffed/vars/vars.h
chuffed/vars/int-var-sl.h
chuffed/vars/int-var-ll.h
chuffed/vars/int-view.h
chuffed/vars/int-var-el.h
chuffed/vars/modelling.h
chuffed/vars/bool-view.h
chuffed/parallel/parallel.h
chuffed/support/vec.h
chuffed/support/misc.h
chuffed/support/heap.h
chuffed/support/sparse_set.h
chuffed/ldsb/ldsb.h
chuffed/globals/globals.h
chuffed/globals/mddglobals.h
chuffed/mdd/CFG.h
chuffed/mdd/circ_fns.h
chuffed/mdd/circutil.h
chuffed/mdd/CYK.h
chuffed/mdd/MDD.h
chuffed/mdd/mdd_prop.h
chuffed/mdd/mdd_to_lgraph.h
chuffed/mdd/MurmurHash3.h
chuffed/mdd/opcache.h
chuffed/mdd/opts.h
chuffed/mdd/sorters.h
chuffed/mdd/weighted_dfa.h
chuffed/mdd/wmdd_prop.h
chuffed/mip/mip.h
chuffed/mip/simplex.h
chuffed/primitives/primitives.h
chuffed/branching/branching.h
chuffed/core/sat.h
chuffed/core/engine.h
chuffed/core/sat-types.h
chuffed/core/options.h
chuffed/core/propagator.h
)

INSTALL(DIRECTORY chuffed DESTINATION include
        PATTERN *.c EXCLUDE)
