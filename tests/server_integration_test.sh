#!/bin/bash

PORT_NO=8080
SERVER_URL="http://localhost:$PORT_NO"

# generate tmp config file
TMP_CONFIG="tmp_config"
echo "
http {
    server {
        listen $PORT_NO;
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
echo "Testing 200 response to valid GET request..."
OUTPUT_FILE=test.out
curl "$SERVER_URL" -sv &> "$OUTPUT_FILE"
cat "$OUTPUT_FILE" | grep "200 OK"

if [ $? == 0 ];
then
    echo "SUCCESS...200 OK in response"
else
    echo "FAILURE...200 OK not in response"
    kill_server
    exit 1
fi

rm "$OUTPUT_FILE"

# delete tmp config file
rm "$TMP_CONFIG"

kill_server
exit 0
