
all:
	cmake --build build

run: all
	./build/src/main

web:
	cmake --build build-web/

serve: web
	python -m http.server --directory=build-web/src/
