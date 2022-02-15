all:
	pio -f -c vim run

upload:
	pio -f -c vim run --target upload

clean:
	pio -f -c vim run --target clean
