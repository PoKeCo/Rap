TARGET = rapunzel
OBJS = rapunzel.o
SRCS = rapunzel.c

CFLAGS =  -O3

CC = gcc

#INC = -I/usr/local/include/opencv
INC = -I/opt/ros/kinetic/include/opencv-3.3.1-dev

LIB = -L/lib -lopencv_contrib -lopencv_core -lopencv_features2d -lopencv_flann -lopencv_gpu -lopencv_highgui -lopencv_imgproc -lopencv_legacy -lopencv_ml -lopencv_objdetect -lopencv_ocl -lopencv_photo -lopencv_stitching -lopencv_superres -lopencv_ts -lopencv_video -lopencv_videostab
# -lopencv_nonfree  -lgstreamer-1.0 
#OPT = -lcxcore -lcv -lhighgui -lstdc++
OPT =

$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS) -lm $(INC) $(LIB) $(OPT)

.c.o:	
	$(CC) -c $(CFLAGS) $< $(INC)
clean:
	rm -f $(TARGET) *.o core *.core core.* *~

