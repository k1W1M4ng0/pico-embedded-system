.PHONY: build

build:
	cmake -B build
	cmake --build build

mount:
	sudo mount /dev/sd*1 /mnt

monitor:
	sudo minicom -D /dev/ttyACM0 -b 115200
