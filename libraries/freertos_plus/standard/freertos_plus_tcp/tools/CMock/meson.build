#
# build script written by : Michael Brockus.
# github repo author: Mike Karlesky, Mark VanderVoord, Greg Williams.
#
# license: MIT
#
project('cmock', 'c',
    license: 'MIT',
    meson_version: '>=0.53.0',
    subproject_dir : 'vendor',
    default_options: ['werror=true', 'c_std=c11']
)

unity_dep = dependency('unity', fallback: ['unity', 'unity_dep'])

subdir('src')
cmock_dep = declare_dependency(link_with: cmock_lib, include_directories: cmock_dir)
