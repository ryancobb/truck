build:
	pio -f -c vim run

upload:
	pio -f -c vim run --target upload

clean:
	pio -f -c vim run --target clean

all:
	pio run
	pio run --target upload
	pio device monitor
