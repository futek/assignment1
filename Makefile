name = ecg
compiler = gcc
flags = -Wall
objects = main.o sensor.o filter.o peaks.o

all: $(name)

$(name): $(objects)
	$(compiler) $(flags) $(objects) -o $(name)

%.o: %.c
	$(compiler) $(flags) -c $< -o $@

clean:
	rm -f $(name) $(objects)
