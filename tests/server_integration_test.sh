#!/bin/bash

PORT_NO=8080
SERVER_URL="http://localhost:$PORT_NO"

# generate tmp config file
TMP_CONFIG="tmp_config"
echo "
http {
    server {
        listen $PORT_NO;
        location /echo {
            echo;
        }
        location /static {
            root /usr/src/projects/iceberg-webserver/file1/;
        }
    }
}
" > "$TMP_CONFIG"

# Start the webserver
./server tmp_config &> /dev/null &
SERVER_PID=$!

# Stops the webserver
kill_server() {
    kill -9 "$SERVER_PID"
}

# Run the tests
echo "Testing 200 response to valid Echo GET request..."
OUTPUT_FILE=test.out
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

echo "Testing 200 response to valid Static GET request..."
OUTPUT_FILE=test.out
curl "$SERVER_URL/static/index.html" -sv &> "$OUTPUT_FILE"
cat "$OUTPUT_FILE" | grep "200 OK"

if [ $? == 0 ];
then
    echo "SUCCESS...200 OK in response"
else
    echo "FAILURE...200 OK not in response"
    kill_server
    exit 1
fi

echo "Testing 404 response to valid Static GET request..."
OUTPUT_FILE=test.out
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

rm "$OUTPUT_FILE"

# delete tmp config file
rm "$TMP_CONFIG"

kill_server
exit 0
