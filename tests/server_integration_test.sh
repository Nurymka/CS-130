#!/bin/bash

PORT_NO=8080
SERVER_URL="http://localhost:$PORT_NO"

# generate tmp config file
mkdir .tmp/
TMP_CONFIG=".tmp/tmp_config"
ROOT="$(pwd)"
echo "
port $PORT_NO;

root $ROOT;

handler echo {
  location /echo;
}

handler static {
  location /static;
  root .tmp/static/;
}

handler status {
  location /status;
}

" > "$TMP_CONFIG"

# Start the webserver
./server "$TMP_CONFIG" &> /dev/null &
SERVER_PID=$!

# Stops the webserver
kill_server() {
    rm -rf .tmp/
    kill -9 "$SERVER_PID"
}

# Run the tests
OUTPUT_FILE=".tmp/test.out"

echo "Testing 200 response to valid Echo GET request..."
curl "$SERVER_URL/echo" -sv &> "$OUTPUT_FILE"
cat "$OUTPUT_FILE" | grep "200 OK"

if [ $? == 0 ];
then
    echo "SUCCESS...200 OK in response"
else
    echo "FAILURE...200 OK not in response"
    kill_server
    exit 1
fi

echo "Generating test static files"
STATIC_FILE_CONTENT="THIS IS A TEST STATIC FILE"
mkdir .tmp/static
echo "$STATIC_FILE_CONTENT" > .tmp/static/static.txt

echo "Testing 200 response to valid Static GET request..."
curl "$SERVER_URL/static/static.txt" -sv &> "$OUTPUT_FILE"
cat "$OUTPUT_FILE" | grep "200 OK"

if [ $? == 0 ];
then
    echo "SUCCESS...200 OK in response"
else
    echo "FAILURE...200 OK not in response"
    kill_server
    exit 1
fi

cat "$OUTPUT_FILE" | grep "$STATIC_FILE_CONTENT"

if [ $? == 0 ];
then
    echo "SUCCESS...Static file was served"
else
    echo "FAILURE...Static file was not served"
    kill_server
    exit 1
fi

echo "Testing 404 response to valid Static GET request..."
curl "$SERVER_URL/static/file_does_not_exist" -sv &> "$OUTPUT_FILE"
cat "$OUTPUT_FILE" | grep "404 Not Found"

if [ $? == 0 ];
then
    echo "SUCCESS...404 Not Found in response"
else
    echo "FAILURE...404 Not Found not in response"
    kill_server
    exit 1
fi

echo "Testing 200 response to valid Status GET request..."
curl "$SERVER_URL/status" -sv &> "$OUTPUT_FILE"
cat "$OUTPUT_FILE" | grep "200 OK"

if [ $? == 0 ];
then
    echo "SUCCESS...200 OK in response"
else
    echo "FAILURE...200 OK not in response"
    kill_server
    exit 1
fi

kill_server
exit 0
