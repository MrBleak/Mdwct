CFLAGS=`pkg-config --cflags gtk+-2.0 gstreamer-0.10`  
LIBS=`pkg-config --libs gtk+-2.0 gstreamer-0.10`  
  
all: player  
  
player:   
    gcc -o MediaPlayer main.c main.h $(LIBS) $(CFLAGS) -l gstinterfaces-0.10  
  
clean:  
    rm *.o player  