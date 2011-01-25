import os
import os.path
import platform


class ConfigurationError(Exception):
  def __init__(self, reason):
    Exception.__init__(self, "Configuration failed: %s" % reason)


class ProductBuilder:
  def __init__(self, enable_debug=True):
    self.enable_debug = enable_debug
    self.internal_deps = []
    self.internal_deps_names = []
    
    self.env = Environment(PATH = os.environ.get('PATH',''),
                           CXX = os.environ.get('CXX','g++'),
                           CC = os.environ.get('CC','gcc'),
                           LINKFLAGS = os.environ.get('LINKFLAGS',''),
                           CCFLAGS = os.environ.get('CCFLAGS',''),
                           CPPPATH = ['.', '#src', '#lib', '/usr/local/include'])
#self.env.Append(CXXFLAGS = ['-fvisibility-inlines-hidden', '-fvisibility=hidden'])
    self.env.Append(CXXFLAGS = [''])

    if not self.env.GetOption('clean'):
      conf = Configure(self.env)

      if conf.CheckCHeader('sys/epoll.h'):
        conf.env.Append(CPPDEFINES = ['-DHAVE_SYS_EPOLL_H'])
      if conf.CheckCHeader('mach/mach_time.h'):
        conf.env.Append(CPPDEFINES = ['-DHAVE_MACH_MACH_TIME_H'])

      if conf.CheckFunc('strlcpy'):
        conf.env.Append(CPPDEFINES = ['-DHAVE_STRLCPY'])
      if conf.CheckFunc('strlcat'):
        conf.env.Append(CPPDEFINES = ['-DHAVE_STRLCAT'])
      if conf.CheckFunc('prctl'):
        conf.env.Append(CPPDEFINES = ['-DHAVE_PRCTL'])
      if conf.CheckFunc('clock_get_time'):
        conf.env.Append(CPPDEFINES = ['-DHAVE_CLOCK_GET_TIME'])
      if conf.CheckFunc('mach_absolute_time'):
        conf.env.Append(CPPDEFINES = ['-DHAVE_MACH_ABSOLUTE_TIME'])
      if conf.CheckFunc('clock_gettime'):
        conf.env.Append(CPPDEFINES = ['-DHAVE_CLOCK_GETTIME'])

      if platform.system() == 'Linux':
        conf.env.Append(CPPDEFINES = ['-DOS_LINUX', '-DHAVE_EPOLL'])
      elif platform.system() == 'Darwin':
        conf.env.Append(CPPDEFINES = ['-DOS_MACOSX', '-DHAVE_WORKING_KQUEUE'])
      elif platform.system() == 'FreeBSD':
        conf.env.Append(CPPDEFINES = ['-DOS_BSD', '-DHAVE_WORKING_KQUEUE'])
      else:
        conf.env.Append(CPPDEFINES = ['-DOS_GENERIC'])

      self.env = conf.Finish()
    else:
      os.system('rm -rf build')

    if not self.enable_debug:
      self.env.Append(CCFLAGS = ['-O3', '-funroll-loops', '-ffast-math', '-ftree-vectorize'])
    else:
      self.env.Append(CCFLAGS = ['-O0', '-ggdb'], CPPDEFINES = ['-DENABLE_DEBUG'])

    self.env.Append(CCFLAGS = [
        '-Wall', 
        '-Wno-unknown-pragmas', 
        '-Wextra', 
        '-Wshadow', 
        '-Wpointer-arith', 
        '-Wcast-qual',
        '-Wcast-align', 
        '-Wwrite-strings', 
        '-Wconversion', 
        '-Wmissing-noreturn', 
        '-Wmissing-format-attribute',
        '-Wredundant-decls', 
        '-Winline'])


  def BuildLibrary(self, library_name, dir):
    target_dir = "#build/" + library_name
    name = target_dir + '/' + library_name
    if self.enable_debug:
      name += "-debug"
      
    VariantDir(target_dir, dir, duplicate=0);
    self.env.Append(LIBPATH=[target_dir])
    library = self.env.StaticLibrary(name, Glob(dir + '/*.cpp'))
    self.internal_deps += library
    if self.enable_debug:
      library_name += "-debug"
    self.internal_deps_names.append(library_name)
    Clean(library, target_dir)


  def BuildProgram(self, prog, dir, installdir, sources, system_libs):
    if not self.enable_debug:
      name = '#build/' + prog + '/' + prog
    else:
      name = '#build/' + prog + '/' + prog + '-debug'

    VariantDir('#build/' + prog, dir, duplicate=0);
    program = self.env.Program(name, sources, LIBS=self.internal_deps_names+system_libs)
    # self.env.Install(installdir, program);
    self.env.Alias('install', installdir);
    self.env.Requires(program, self.internal_deps)


helloworld_dir = '#demos/helloworld'
helloworld_sources = Glob(helloworld_dir + '/*.cpp')
# simple_helloworld_dir = '#demos/simple_helloworld'
# simple_helloworld_sources = Glob(simple_helloworld_dir + '/*.cpp')

system_libs = ['tbb', 'tbbmalloc', 'log4cpp', 'pthread']
if platform.system() == 'Linux':
  system_libs.append("rt")

builder = ProductBuilder(enable_debug=False)
builder.BuildLibrary('webserver', '#lib/webserver')
builder.BuildLibrary('threads', '#lib/threads')
builder.BuildLibrary('base', '#lib/base')
builder.BuildLibrary('clock', '#lib/clock')
builder.BuildLibrary('cstring', '#lib/cstring')
builder.BuildLibrary('io', '#lib/io')
builder.BuildLibrary('logger', '#lib/logger')
builder.BuildLibrary('sockets', '#lib/sockets')
builder.BuildProgram('helloworld', helloworld_dir, '#build/helloworld', helloworld_sources, system_libs)
#builder.BuildProgram('simple_helloworld', simple_helloworld_dir, '#build/simple_helloworld', simple_helloworld_sources, system_libs)
