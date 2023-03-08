
all: cli ser

c: cli
	./client localhost 6667

s: ser
	./server localhost 6667

p:
	python proxy.py

clean:
	rm -rf client server __pycache__

cli:
	c++ client.cpp -o client

ser:
	c++ server.cpp -o server

info:
	@echo "run cmds in following order:\nmake s\nmake p\nmake c"
