all: spotifycli

CC = g++
CXXFLAGS = -std=c++11
CFLAGS = -g -Werror -Wall -I /usr/local/opt/openssl@1.1/include
LDFLAGS = -L/usr/local/opt/openssl@1.1/lib -lssl -lcrypto -pthread

%: %.cpp
	$(CC) $(CFLAGS) $(CXXFLAGS) $(LDFLAGS) $< -o $@ -lcurl

clean:
	rm -f spotifycli *.o *~ core.* *.d