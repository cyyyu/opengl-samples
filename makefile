cc = /usr/local/bin/gcc-9 -I/usr/local/include -std=gnu++17 -lstdc++ -L/usr/local/lib -lglfw -lassimp -framework OpenGL -D__gl_h_ -DGL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED

sample1 = the-hello-world-triangle/main.cpp
sample1_out = the-hello-world-triangle/app
compile_sample1 = $(cc) $(sample1) -o $(sample1_out)

sample2 = texture/main.cpp
sample2_out = texture/app
compile_sample2 = $(cc) $(sample2) -o $(sample2_out)

all:
	$(compile_sample1) && $(compile_sample2)
