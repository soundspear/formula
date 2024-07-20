include(CMakeParseArguments)
include(CMakeDependentOption)

# ==================================================================================================

# Generates an index readable by formula::storage::LocalIndex class
# That index is a JSON file merging all the individual Formulas -that are JSON files themselves-

function(generate_formulas_index formulas_directory output_index)
    file(GLOB all_formula_files "${formulas_directory}/*")
    file(WRITE ${output_index} "{\n")

    set(first TRUE)
    foreach(formula IN LISTS all_formula_files)
        file(READ ${formula} json_content)
        string(JSON name GET "${json_content}" Name)
        if(NOT first)
            file(APPEND ${output_index} ",\n")
        else()
            set(first FALSE)
        endif()
        file(APPEND ${output_index} "  \"${name}\": ${json_content}")
    endforeach()

    file(APPEND ${output_index} "\n}\n")
endfunction()

# Called by `add_custom_command`
if (DEFINED CMAKE_ARGV3)
    generate_formulas_index(${CMAKE_ARGV3} ${CMAKE_ARGV4})
endif ()