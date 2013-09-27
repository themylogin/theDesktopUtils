from distutils.core import setup, Extension

setup(name="wallpaper", version="1.0", ext_modules=[Extension("wallpaper", libraries=["stdc++", "Imlib2", "X11"], sources=["wallpaper.cpp"], extra_compile_args=["-std=c++11"])])
