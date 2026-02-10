project(
  'pokedex_model',
  ['c', 'cpp'],
  version : '0.1',
  meson_version : '>= 1.3.0',
  default_options : ['warning_level=3', 'cpp_std=c++20'],
)

include_directories = include_directories(
  get_option('pokedex_include_dir'),
  is_system : true,
)

sources = [
  'pokedex_interface.cpp',
  'model/model.cpp',
  'model/do_execute.cpp',
]

exe = shared_library(
  'pokedex_model',
  sources,

  include_directories: include_directories,
  dependencies : [],

  link_language : 'c',
  gnu_symbol_visibility : 'hidden',

  install : true,
)
