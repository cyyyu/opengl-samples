cc = /usr/local/bin/gcc-9 -I/usr/local/include -std=gnu++17 -lstdc++ -L/usr/local/lib -lglfw -lassimp -framework OpenGL -D__gl_h_ -DGL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED

sample1 = the-hello-world-triangle/main.cpp
sample1_out = the-hello-world-triangle/app
compile_sample1 = $(cc) $(sample1) -o $(sample1_out)

sample2 = texture/main.cpp
sample2_out = texture/app
compile_sample2 = $(cc) $(sample2) -o $(sample2_out)

sample3 = camera/main.cpp
sample3_out = camera/app
compile_sample3 = $(cc) $(sample3) -o $(sample3_out)

sample4 = lighting/main.cpp
sample4_out = lighting/app
compile_sample4 = $(cc) $(sample4) -o $(sample4_out)

sample5 = model_loading/main.cpp
sample5_out = model_loading/app
compile_sample5 = $(cc) $(sample5) -o $(sample5_out)

sample6 = shadow/main.cpp
sample6_out = shadow/app
compile_sample6 = $(cc) $(sample6) -o $(sample6_out)

all:
	$(compile_sample1) ; $(compile_sample2) ; $(compile_sample3) ; $(compile_sample4) ; $(compile_sample5)

triangle: the-hello-world-triangle/*.* 
	$(compile_sample1)

texture: texture/*.*
	$(compile_sample2)

camera: camera/*.*
	$(compile_sample3)

lighting: lighting/*.*
	$(compile_sample4)

model: model_loading/*.*
	$(compile_sample5)

shadow: shadow/*.*
	$(compile_sample6)
