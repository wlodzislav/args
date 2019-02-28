CXX = clang++
CFLAGS = -std=c++2a

EXAMPLES = $(patsubst %.cpp, %, $(wildcard examples/*.cpp))
.PHONY: examples
examples: $(EXAMPLES)
	./examples/short-syntax -b -i12 -d1.5 --str str -v 0 -v 1 -v 2 -m a=A -m b=B
	@echo
	./examples/chain-syntax -b -i12 -d1.5 --str str -v 0 -v 1 -v 2 -m a=A -m b=B
	@echo
	./examples/containers-action -v 0 -v 1 -v 2 -m a=A -m b=B
	@echo
	./examples/errors --num asd
	@echo
	./examples/errors --bool=asd
	@echo
	./examples/errors -v asd
	@echo
	./examples/errors --mapk asd
	@echo
	./examples/errors --mapk a=1
	@echo
	./examples/errors --mapv a=str
	@echo
	./examples/errors --nop
	@echo
	./examples/errors --num
	@echo
	./examples/errors -fn
	@echo
	./examples/errors arg
	@echo
	./examples/errors cmd --cnum asd
	@echo
	./examples/errors cmd --cbool=asd
	@echo
	./examples/errors cmd -cv asd
	@echo
	./examples/errors cmd --cmapk asd
	@echo
	./examples/errors cmd --cmapk a=1
	@echo
	./examples/errors cmd --cmapv a=str
	@echo
	./examples/errors cmd --cnop
	@echo
	./examples/errors cmd --cnum
	@echo
	./examples/errors cmd -cfn
	@echo
	./examples/errors cmd arg
	@echo
	./examples/required-errors
	@echo
	./examples/required-errors -a
	@echo
	./examples/required-errors -a -b
	@echo
	./examples/required-errors -a -b arg1
	@echo
	./examples/required-errors -a -b arg1 rest cmd
	@echo
	./examples/required-errors -a -b arg1 rest cmd -c
	@echo
	./examples/required-errors -a -b arg1 rest cmd -c -d
	@echo
	./examples/required-errors -a -b arg1 rest cmd -c -d carg1

%: %.cpp args.h
	$(CXX) $(CFLAGS) -o $@ $<


.PHONY: test
test: test/test
	./test/test

test/test: test/test.cpp args.h
	$(CXX) $(CFLAGS) -o $@ test/test.cpp

.PHONY: clean
clean:
	rm ${EXAMPLES}
	rm test/test
