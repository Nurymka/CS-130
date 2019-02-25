#!/bin/bash

PORT_NO=8080
SERVER_URL="http://localhost:$PORT_NO"
TIMEOUT=10
NUM_STATUS_CALLS=100

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

handler status {
  location /status;
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

OUTPUT_FILE=".tmp/multithreading_test.out"

# Test that a request to /sleep doesn't block other requests
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

# Test that on 10 calls to /status, threads don't step on each other
echo "Testing race conditions on /status calls..."
for i in `seq 1 $NUM_STATUS_CALLS`;
do
  curl "$SERVER_URL/status" &> /dev/null &
  pid=$!
  PID_LIST+=" $pid"
done

trap "kill $PID_LIST" SIGINT

wait $PID_LIST
curl "$SERVER_URL/status" -sv &> "$OUTPUT_FILE"

# -A1 returns the next line after match, so we can grep again for count of reqs
cat "$OUTPUT_FILE" | grep -A1 "/status" | grep "200: $NUM_STATUS_CALLS"
if [ $? == 0 ];
then
  echo "SUCCESS... /status calls returned $NUM_STATUS_CALLS request counts"
else
  echo "FAILURE... /status calls didn't return $NUM_STATUS_CALLS request counts"
  kill_server
  exit 1
fi

kill_server
exit 0
