.PHONY: all clean

all:
	@for dir in 1 2 3 4 5 6 7 8 9 10; do \
        $(MAKE) -C $$dir; \
    done

clean:
	@for dir in 1 2 3 4 5 6 7 8 9 10; do \
        $(MAKE) clean -C $$dir; \
    done