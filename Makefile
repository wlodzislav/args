CXX = clang++
CFLAGS = -std=c++2a

EXAMPLES = $(patsubst %.cpp, %, $(wildcard examples/*.cpp))
.PHONY: examples
examples: $(EXAMPLES)

%: %.cpp args.h
	$(CXX) $(CFLAGS) -o $@ $< && ./$@


.PHONY: test
test: test/test
	./test/test

test/test: test/test.cpp args.h
	$(CXX) $(CFLAGS) -o $@ test/test.cpp

.PHONY: clean
clean:
	rm ${EXAMPLES}
	rm test/test
