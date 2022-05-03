project('tupl', 'cpp',
  default_options : ['cpp_std=c++20'],
  version : '0.3'
)

c_array_support_dep = subproject('c_array_support').get_variable('c_array_support_dep')

tupl_dep = declare_dependency(
  dependencies: [c_array_support_dep],
  include_directories : include_directories('.'),
)

IREPEAT_dep = subproject('IREPEAT').get_variable('IREPEAT_dep')

subdir('tupl_dev')
