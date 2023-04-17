#!/bin/bash

# stop the server
killall ipkcpd

# go through all files in tests directory
# and run them as input

# start the server in the background
./ipkcpd -m tcp -h localhost -p 3333 &
# wait for the server to start
sleep 1

# for all files
for file in tests/in/tcp/*;
do
    echo "Running test $file"
    # run the test, get the output
    output=$(./ipkcpc -m tcp -p 3333 -h localhost < "$file")
    # compare output with expected output
    diff <(echo "$output") "tests/out/tcp/$(basename "$file")"
done

# stop the server
killall ipkcpd

# start the server in the background
./ipkcpd -m udp -h localhost -p 4444 &
# wait for the server to start
sleep 1

# for all files
for file in tests/in/udp/*;
do
    echo "Running test $file"
    # run the test, get the output
    output=$(./ipkcpc -m udp -p 4444 -h localhost < "$file")
    # compare output with expected output
    diff <(echo "$output") "tests/out/udp/$(basename "$file")"
done

# stop the server
killall ipkcpd
