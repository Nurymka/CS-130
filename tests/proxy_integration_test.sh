#!/bin/bash

PORT_NO_FRONT=8080
PORT_NO_BACK=8081
SERVER_URL_FRONT="http://localhost:$PORT_NO_FRONT"
SERVER_URL_BACK="http://localhost:$PORT_NO_BACK"

# generate tmp config file
mkdir .tmp/
TMP_CONFIG_FRONT=".tmp/tmp_config_front"
ROOT="$(pwd)"
echo "
port $PORT_NO_FRONT;

root $ROOT;

handler proxy {
  location /frontend;
  root $SERVER_URL_BACK;
}

" > "$TMP_CONFIG_FRONT"

TMP_CONFIG_BACK=".tmp/tmp_config_back"
ROOT="$(pwd)"
echo "
port $PORT_NO_BACK;

root $ROOT;

handler static {
  location /backend;
  root .tmp/static/;
}

" > "$TMP_CONFIG_BACK"

# Start the webserver
./server "$TMP_CONFIG_BACK" &> /dev/null &
SERVER_PID_BACK=$!
./server "$TMP_CONFIG_FRONT" &> /dev/null &
SERVER_PID_FRONT=$!

# Stops the webserver
kill_server() {
    rm -rf .tmp/
    kill -9 "$SERVER_PID_FRONT"
    kill -9 "$SERVER_PID_BACK"
}

# Run the tests
OUTPUT_FILE_FRONT=".tmp/test_front.out"
OUTPUT_FILE_BACK=".tmp/test_back.out"

echo "Generating test static files"
STATIC_FILE_CONTENT="THIS IS A TEST STATIC FILE"
mkdir .tmp/static
echo "$STATIC_FILE_CONTENT" > .tmp/static/static.txt

echo "Testing reverse proxy..."
curl "$SERVER_URL_BACK/backend/static.txt" -sv &> "$OUTPUT_FILE_BACK"
curl "$SERVER_URL_FRONT/frontend/backend/static.txt" -sv &> "$OUTPUT_FILE_FRONT"

cat "$OUTPUT_FILE_BACK" | grep "200 OK"

if [ $? == 0 ];
then
    echo "SUCCESS...200 OK in response for backend"
else
    echo "FAILURE...200 OK not in response for backend"
    kill_server
    exit 1
fi

cat "$OUTPUT_FILE_BACK" | grep "$STATIC_FILE_CONTENT"

if [ $? == 0 ];
then
    echo "SUCCESS...Static file was served on backend"
else
    echo "FAILURE...Static file was not served on backend"
    kill_server
    exit 1
fi

cat "$OUTPUT_FILE_FRONT" | grep "200 OK"

if [ $? == 0 ];
then
    echo "SUCCESS...200 OK in response for frontend"
else
    echo "FAILURE...200 OK not in response for frontend"
    kill_server
    exit 1
fi

cat "$OUTPUT_FILE_FRONT" | grep "$STATIC_FILE_CONTENT"

if [ $? == 0 ];
then
    echo "SUCCESS...Proxy served backend static file"
else
    echo "FAILURE...Proxy did not serve backend static file"
    kill_server
    exit 1
fi


kill_server
exit 0
