include_directories(
   ${LLVM_PROF_INCLUDE_DIRS}
   ${PROJECT_BINARY_DIR}
   ${LLVM_INCLUDE_DIRS} 
   )
link_directories(${LLVM_LIBRARY_DIRS})
add_definitions(-fno-rtti)
add_definitions(-Wall --std=c++11)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${LLVM_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${LLVM_CPP_FLAGS}")

add_library(CodeBlockPred SHARED
   CBCount.cpp
   CBCycle.cpp
   CBOperation.cpp
   VariedCBCycle.cpp
    )
target_link_libraries(CodeBlockPred
	${LLVM_DYNAMIC_LIBRARY}
	-L/home/wzzhang/usr/local/lib/
	)

if(LLVM_VERSION VERSION_EQUAL "3.4")
   # print Asm code for a bitcode

endif()
