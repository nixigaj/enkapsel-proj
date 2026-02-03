-include .env

PIO_ENV ?= usb

.PHONY: upload run clean all

all: upload

upload:
	pio run -t upload -e $(PIO_ENV)

monitor:
	pio device monitor -e $(PIO_ENV)

run:
	pio run -t upload -t monitor -e $(PIO_ENV)

clean:
	rm -rf .pio
