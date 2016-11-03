DLT_BIN=mythunder
DLT_SRC= download/mythunder.cpp download/download.cpp window/window.cpp pool/threadpool.cpp

CC=g++

$(DLT_BIN):$(DLT_SRC)
	$(CC) -o $@ $^ -lpthread -lncurses -g 


.PYONY:clean
clean:
	rm -rf $(DLT_BIN) index.html WeChat_2.2.0.37_Setup.exe WeChat_2.2.0.37_Setup.exe.config QQ_*  Chrome*
