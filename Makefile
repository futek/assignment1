name = ecg
compiler = gcc
flags = -Wall
objects = main.o sensor.o filter.o

all: $(name)

$(name): $(objects)
	$(compiler) $(flags) $(objects) -o $(name)

$(objects): %.o: %.c
	$(compiler) $(flags) -c $< -o $@

clean:
	rm -f $(name) $(objects)
