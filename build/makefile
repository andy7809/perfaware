CXX=g++
CXXFLAGS=-std=c++17
COMP_ENHANCE_PATH=/Users/andrewwiedenmann/code/computer_enhance/perfaware
SHAREDI=$(COMP_ENHANCE_PATH)/sim86/shared
LIBI=/Users/andrewwiedenmann/code/perfaware/build

all: sim86lib sharedtest

sim86lib: libsim86mac.so

sharedtest: sharedlibtest.o

sharedlibtest.o: ../shared_library_test.cpp sim86lib
	$(CXX) $(CXXFLAGS) -I$(SHAREDI) -L$(LIBI) -o $(@D)/$@ -lsim86mac $<

libsim86mac.so: $(COMP_ENHANCE_PATH)/sim86/sim86_lib.cpp
	$(CXX) $(CXXFLAGS) -shared -o $(@D)/$@ -fPIC $<

clean:
	rm -rf *.o *.so