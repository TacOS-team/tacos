#NBITER = 50000000
NBITER = 10

FICHIERS=testSet.out testMap.out testVector.out testPair.out testDeque.out testQueue.out testStack.out

all: clean $(addprefix std, $(FICHIERS)) $(FICHIERS)
	@rm -f *.out

noclean: clean $(addprefix std, $(FICHIERS)) $(FICHIERS)

# tests with standard stl lib
std%.out: %.cpp
	@g++ -o $@ $< -Wall -g -I../libtest/ -DNBITER=$(NBITER)
	@./$@
	@if [ $$? = 0 ]; then echo "\033[32m" $@ " ✔ \033[0m"; else echo "\033[31m" $@ " ✘ \033[0m"; fi

# tests with tacos stl lib
%.out: %.cpp
	@g++ -o $@ $< -Wall -g -I../libtest/ -I../../libs/stl/ -I../../libc/include/ -DNBITER=$(NBITER)
	@./$@
	@if [ $$? = 0 ]; then echo "\033[32m" $@ " ✔ \033[0m"; else echo "\033[31m " $@ " ✘ \033[0m"; fi

clean:
	@rm -f *.out