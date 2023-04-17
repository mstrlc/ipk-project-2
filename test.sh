#!/bin/sh

# echo time
echo "Running ipkcpd tests at $(date)"
printf "\n"

# stop the server if something went wrong in the middle of the test
pkill ipkcpd

# go through all files in tests directory
# and run them as input

echo "-- TCP TESTS --"
# start the server in the background
./ipkcpd -m tcp -h localhost -p 3333 &
# wait for the server to start
sleep 1

# for all files
for file in tests/in/tcp/*;
do
    # printf running test with tab in front
    printf "\tRunning test %s\n" "$file"
    # run the test, get the output
    output=$(./ipkcpc -m tcp -p 3333 -h localhost < "$file")
    # compare output with expected output
    diff <(echo "$output") "tests/out/tcp/$(basename "$file")"
done
printf "\n"

# stop the server
pkill ipkcpd

# start the server in the background
echo "-- UDP TESTS --"
./ipkcpd -m udp -h localhost -p 4444 &
# wait for the server to start
sleep 1

# for all files
for file in tests/in/udp/*;
do
    printf "\tRunning test %s\n" "$file"
    # run the test, get the output
    output=$(./ipkcpc -m udp -p 4444 -h localhost < "$file")
    # compare output with expected output
    diff <(echo "$output") "tests/out/udp/$(basename "$file")"
done

# stop the server
pkill ipkcpd
