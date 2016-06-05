#!/bin/bash

# Clear the entire directory, making it ready to deploy

echo "[virginia] Removing Virginia files, program files and outputs..."

rm virginia

# Insert here the removal instructions
rm program/*.beam
./clean.sh

echo "[virginia] Done"
