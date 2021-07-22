project('tupl', 'cpp',
  default_options : ['cpp_std=c++2a','warning_level=3'],
  version : '0.1'
)

subprojects = ['c_array_support', 'IREPEAT']

c_array_support_dep = subproject('c_array_support').get_variable('c_array_support_dep')
IREPEAT_dep = subproject('IREPEAT').get_variable('IREPEAT_dep')

#IREPEAT_incs = IREPEAT_dep.get_variable(internal:'includes')

if meson.get_compiler('cpp').get_id() == 'msvc'
  extra_args = ['/std:c++latest','/Zc:preprocessor']
  prepro_args = ['-I.','-I../subprojects/IREPEAT','/Zc:preprocessor','/C','/EP']
elif meson.get_compiler('cpp').get_id() == 'clang'
  extra_args = ['-std=c++20']
  prepro_args = ['-I.','-I../subprojects/IREPEAT','-MMD','-nostdinc','-C','-E','-P','-Wno-c++20-compat']
else
  extra_args = []
  prepro_args = ['-I.','-I../subprojects/IREPEAT','-MMD','-nostdinc','-C','-E','-P']
endif

tupl_impl_header = custom_target('tupl_impl',
  input: 'tupl_impl_pre.hpp',
  output: 'tupl_impl.hpp',
  command: [meson.get_compiler('cpp').get_id(),
                            prepro_args, '-o', '@OUTPUT@', '@INPUT@'],
  depfile: 'tupl_impl.d',
  install: true,
  install_dir: meson.source_root()
)

tupl_impl_dep = declare_dependency(
  include_directories : include_directories('.'),
  compile_args : extra_args,
  sources : ['tupl.hpp', tupl_impl_header]
).partial_dependency(includes : true, compile_args : true, sources : true)

tupl_dep = declare_dependency(
  include_directories : include_directories('.'),
  compile_args : extra_args,
  sources : ['tupl.hpp', tupl_impl_header]
).partial_dependency(includes : true, compile_args : true, sources : true)

test('test tupl',
  executable('test_tupl', 'tupl_test.cpp',
             dependencies: [c_array_support_dep, IREPEAT_dep, tupl_dep],
             cpp_args : '-pedantic-errors')
)
