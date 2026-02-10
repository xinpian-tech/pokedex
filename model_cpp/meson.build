project(
  'pokedex_model',
  ['c', 'cpp'],
  version : '0.1',
  meson_version : '>= 1.3.0',
  default_options : [
    'warning_level=3',
    'cpp_std=c++20',
    'cpp_eh=none', # -fno-exceptions
    'cpp_rtti=false', # -fno-rtti
  ],
)

cc = meson.get_compiler('c')

softfloat = declare_dependency(
  include_directories : get_option('softfloat_include_dir'),
  dependencies : cc.find_library(
    'softfloat',
    dirs : get_option('softfloat_lib_dir'),
    required : true,
  )
)

pokedex_header = declare_dependency(
  include_directories : get_option('pokedex_include_dir'),
)

sources = [
  'pokedex_interface.cpp',
  'model/util_arith.cpp',
  'model/model.cpp',
  'model/do_execute.cpp',
]

version_script_file = 'version_script.ld'

exe = shared_library(
  'pokedex_model',
  sources,

  dependencies : [softfloat, pokedex_header],

  link_language : 'c', # avoid linking with libstdc++
  gnu_symbol_visibility : 'hidden',

  link_args: ['-Wl,--version-script,' + meson.current_source_dir() / version_script_file],
  link_depends: version_script_file,

  install : true,
)
