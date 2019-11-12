sample1 = the-hello-world-triangle/main.cpp
sample1_out = the-hello-world-triangle/app

all:
	/usr/local/bin/gcc-9 -I/usr/local/include -std=gnu++17 -o $(sample1_out) -lstdc++ -L/usr/local/lib -lglfw -lassimp -framework OpenGL -D__gl_h_ -DGL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED $(sample1)
