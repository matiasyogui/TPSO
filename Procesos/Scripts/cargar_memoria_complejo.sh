#!/bin/sh

cd ../GameBoy/Debug

./gameboy broker catch_pokemon pikachu 9 3
./gameboy broker catch_pokemon squirtle 9 3

./gameboy broker caught_pokemon 10 1
./gameboy broker caught_pokemon 11 0

./gameboy broker catch_pokemon bulbasaur 1 7
./gameboy broker catch_pokemon charmander 1 7

./gameboy broker get_pokemon pichu
./gameboy broker get_pokemon raichu