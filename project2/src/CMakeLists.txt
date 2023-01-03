aux_source_directory(Test TEST)
aux_source_directory(Operators OPERATORS)
aux_source_directory(Storage STORAGE)
aux_source_directory(Utility UTILITY)

set(SOURCES "Main.cpp" ${TEST} ${OPERATORS} ${STORAGE} ${UTILITY})

add_executable(BetterCalculator ${SOURCES})
