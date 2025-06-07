.PHONY: all clean server client

all: server client

# ===== 服务端配置 =====
SERVER_SRC = server/main.c server/database.c server/crypto.c server/user_auth.c server/friend.c server/online_users.c  server/audit_log.c
SERVER_OBJ = $(SERVER_SRC:.c=.o)
SERVER_HDR = server/database.h server/crypto.h common/protocol.h server/friend.h server/online_users.h  server/audit_log.h
SERVER_TARGET = server/server

SERVER_CFLAGS = -Wall -g -Iserver -Icommon \
-I/home/albertzhu/GmSSL-3.1.1-Linux/include \
-I/home/albertzhu/GmSSL-3.1.1-Linux/include/gmssl

SERVER_LDFLAGS = -L/home/albertzhu/GmSSL-3.1.1-Linux/lib -lgmssl -lsqlite3 -lpthread

server/%.o: server/%.c $(SERVER_HDR)
	$(CC) $(SERVER_CFLAGS) -c $< -o $@

$(SERVER_TARGET): $(SERVER_OBJ)
	$(CC) $(SERVER_OBJ) -o $@ $(SERVER_LDFLAGS)

server: $(SERVER_TARGET)

# ===== 客户端配置（Qt5）=====
CLIENT_SRC = client/main.cpp client/mainwindow.cpp
CLIENT_HDR = client/mainwindow.h common/protocol.h
CLIENT_UI = client/mainwindow.ui
CLIENT_UI_HDR = client/ui_mainwindow.h
CLIENT_MOC = client/moc_mainwindow.cpp
CLIENT_TARGET = client/client

QT_INC = $(shell pkg-config --cflags Qt5Widgets Qt5Network)
QT_LIB = $(shell pkg-config --libs Qt5Widgets Qt5Network)
UIC = uic
MOC = moc
CXXFLAGS = -std=c++11 -Wall -g -fPIC \
-Icommon \
-I/home/albertzhu/GmSSL-3.1.1-Linux/include \
-I/home/albertzhu/GmSSL-3.1.1-Linux/include/gmssl \
$(QT_INC)
CLIENT_LDFLAGS = -L/home/albertzhu/GmSSL-3.1.1-Linux/lib -lgmssl

$(CLIENT_UI_HDR): $(CLIENT_UI)
	$(UIC) $< -o $@

$(CLIENT_MOC): client/mainwindow.h
	$(MOC) $< -o $@

$(CLIENT_TARGET): $(CLIENT_SRC) $(CLIENT_UI_HDR) $(CLIENT_MOC)
	$(CXX) $(CXXFLAGS) $(CLIENT_SRC) $(CLIENT_MOC) -o $@ $(QT_LIB) $(CLIENT_LDFLAGS)

client: $(CLIENT_TARGET)

clean:
	rm -f server/*.o client/*.o $(SERVER_TARGET) $(CLIENT_TARGET) client/ui_mainwindow.h client/moc_mainwindow.cpp
