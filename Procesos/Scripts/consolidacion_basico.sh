#!/bin/sh

cd ../GameBoy/Debug

./gameboy BROKER CAUGHT_POKEMON 1 1
./gameboy BROKER CAUGHT_POKEMON 2 0

./gameboy BROKER CATCH_POKEMON Pikachu 2 3
./gameboy BROKER CATCH_POKEMON Squirtle 5 2

./gameboy BROKER CATCH_POKEMON Onyx 4 5

./gameboy SUSCRIPTOR CAUGHT_POKEMON 10

./gameboy BROKER CATCH_POKEMON Charmander 4 5
