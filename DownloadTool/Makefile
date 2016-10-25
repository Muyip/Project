DLT_BIN=downloadtool
DLT_SRC=downloadtool.c

CC=g++


$(DLT_BIN):$(DLT_SRC)
	$(CC) -o $@ $^ -lpthread -g

.PYONY:clean
clean:
	rm -rf $(DLT_BIN) index.html WeChat_2.2.0.37_Setup.exe WeChat_2.2.0.37_Setup.exe.config index* .config
