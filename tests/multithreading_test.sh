#!/bin/bash

PORT_NO=8080
SERVER_URL="http://localhost:$PORT_NO"
TIMEOUT=10

# generate tmp config
rm -rf .tmp/
mkdir .tmp/
TMP_CONFIG=".tmp/multithread_config"
ROOT="$(pwd)"
echo "
port $PORT_NO;

root $ROOT;

handler echo {
  location /echo;
}

handler sleep {
  location /sleep;
}
" > "$TMP_CONFIG"

# Start the webserver
./server "$TMP_CONFIG" &> /dev/null &
SERVER_PID=$!

# Stops the webserver
kill_server() {
  kill -9 "$SERVER_PID"
}

# Run the tests
OUTPUT_FILE=".tmp/multithreading_test.out"

echo "Testing multithreading support..."
curl "$SERVER_URL/sleep" &> /dev/null &

start=$(date +%s.%N)
curl "$SERVER_URL/echo" -sv &> "$OUTPUT_FILE"
end=$(date +%s.%N)

# An appropriate solution here is to use bc instead. Used python to avoid
# modifying the development env
secs_difference=$(python -c "print(int(${TIMEOUT} - (${end} - ${start})))")
echo $secs_difference >> "$OUTPUT_FILE"

cat "$OUTPUT_FILE" | grep "200 OK"
if [ $? == 0 ] && [ "$secs_difference" -gt 0 ] ;
then
  echo "SUCCESS... 200 OK in response didn't timeout"
else
  echo "FAILURE... 200 OK not in response, or the request timed out"
  kill_server
  exit 1
fi

kill_server
exit 1
