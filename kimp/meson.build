project(
    'ljpeg2000'
    , ['cpp']
    , default_options: 'cpp_std=c++20'
    , version: '0.0.1'
)

add_global_arguments('-Dabstract=', language: 'cpp')
add_global_arguments('-Dtrait=class', language: 'cpp')

jpeg2000_sources = [
    'src/color/mapper.cpp'
    , 'src/color/rgb.cpp'
    , 'src/color/yuv.cpp'
    
    , 'src/jp2/box.cpp'
    , 'src/jp2/coder.cpp'
    , 'src/jp2/codestream.cpp'
    , 'src/jp2/mapper.cpp'
    , 'src/jp2/raw.cpp'

    , 'src/utils/dwt.cpp'
    , 'src/utils/logging.cpp'
    , 'src/utils/membuf.cpp'
]

jpeg2000_dependencies = [
    dependency('spdlog')
    , dependency('pugixml')
    , dependency('fmt')
]

jpeg2000_include = include_directories('./include')
jpeg2000_so = shared_library(
    'jpeg2000'
    , jpeg2000_sources
    , dependencies: jpeg2000_dependencies
    , include_directories: jpeg2000_include
)

jpeg2000 = declare_dependency(
    link_with: jpeg2000_so
    , dependencies: jpeg2000_dependencies
    , include_directories: jpeg2000_include
)

subdir('./checker/')
