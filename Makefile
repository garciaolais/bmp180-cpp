all:
	g++ i2c-test.cpp -o i2c-test -ldl
test:
	./i2c-test
clean:
	rm i2c-test
