cmake_minimum_required (VERSION 3.12)

# Inserisce l’SDK (deve essere prima del progetto)
include(pico_sdk_import.cmake)

project(pico_examples C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

set(PICO_EXAMPLES_PATH ${PROJECT_SOURCE_DIR})

# Inizializza l'SDK
pico_sdk_init()

include(example_auto_set_url.cmake)

# Aggiunge l’esempio hello_world ovvero la cartella sua cartella che contiene il CMakeLists.txt creato in precedenza
add_sottodirectory (hello_world)

# Aggiunge le opzioni di compilazione
add_compile_options(-Wall -Wno-format  -Wno-unused-function  -Wno-maybe-uninitialized)
