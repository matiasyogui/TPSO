#!/bin/bash

cd ../GameBoy/Debug

for N in {1..3}
do

	./gameboy broker new_pokemon pikachu 2 3 4      			# tamaño del mensaje  = 24
	./gameboy broker appeared_pokemon pikachuu 3 4 6		# tamaño del mensaje  = 25
	./gameboy broker catch_pokemon pikachuu 3 5			# tamaño del mensaje  = 21
	./gameboy broker caught_pokemon 4 1						# tamaño del mensaje  = 8
	./gameboy broker get_pokemon pikachuu					# tamaño del mensaje  = 9

done
wait
