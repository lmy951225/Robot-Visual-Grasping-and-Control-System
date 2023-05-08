INCLUDE = -I./
objects = serialTest.o serialPort.o 
close_open:	$(objects)
			g++	-o	close_open	$(objects)
%.o:	%.cpp 
			g++ -c $<  $(INCLUDE)

# 伪目标
.PHONY:clean
clean:
			rm	*.o