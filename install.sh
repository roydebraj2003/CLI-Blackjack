#!/bin/bash
echo "Downloading CLI-Blackjack ...."
git clone https://github.com/roydebraj2003/CLI-Blackjack.git ~/Blackjack
cd ~/Blackjack

echo "Installing required dependencies..."
sudo apt update && sudo apt install -y gcc make libpq-dev libsodium-dev

echo "Compiling Blackjack ..."
gcc -o Blackjack db.h db.c main.c -lpq -lsodium

echo "Starting ..."
./Blackjack
