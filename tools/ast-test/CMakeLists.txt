set(LLVM_NO_RTTI 1)

set(LLVM_LINK_COMPONENTS)

add_clang_executable(ast-test
  ast-test.cpp
  )
target_link_libraries(ast-test
    clangFrontend
    clangSerialization
    clangSema
    clangAST
    clangParse
    clangLex
    clangBasic
    )
add_dependencies(ast-test clang-headers)
