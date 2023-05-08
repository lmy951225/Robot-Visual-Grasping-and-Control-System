INCLUDE = -I./
objects = test_taike_dh.o taike_kin.o 
taike:	$(objects)
			g++	-o	taike	$(objects)
%.o:	%.cpp 
			g++ -c $<  $(INCLUDE)

# 伪目标
.PHONY:clean
clean:
			rm	*.o
			rm	taike