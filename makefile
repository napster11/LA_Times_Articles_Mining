CXX=c++
CXXFLAGS=-g -std=c++11  -pedantic

SRC=kcluster.cpp
OBJ=$(SRC:%.cpp=%.o)

kcluster: $(OBJ)
	$(CXX) -o kcluster $^

%.o: %.c
	$(CXX) $@ -c $<

preprocess :
	python data.py

clean:
	rm -f *.o