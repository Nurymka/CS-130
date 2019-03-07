#!/bin/bash

PORT=8080

SERVER_URL="http://localhost:$PORT"

# generate tmp config file
TEMP_DIR=".tmp"
mkdir "$TEMP_DIR"

# copy over meme.db so we don't affect the actual db instance
mv meme.db orig-meme.db

CONFIG="$TEMP_DIR/config"
ROOT="$(pwd)"

echo "
port $PORT;

root $ROOT;

handler new_meme {
  location /meme/new;
}
" > "$CONFIG"

# Start the server
echo "Starting server..."
./server "$CONFIG" &> /dev/null &
SERVER_PID=$!

# Stops the server and clean up tmp files
kill_server() {
  # restore original meme.db back
  rm meme.db
  mv orig-meme.db meme.db

  rm -rf "$TEMP_DIR"
  kill -9 "$SERVER_PID"
}

# Run the tests
OUTPUT_FILE="$TEMP_DIR/test.out"

# Ensure there is no leftover file from previous/other runs
rm "$OUTPUT_FILE"

echo "Testing 100 concurrent requests to create memes..."
REQUEST_PIDS=()
for i in {1..100}
do
  curl "$SERVER_URL/meme/new" -X POST -d "img_path=IMG&top_text=TOP&bottom_text=BOTTOM" -s >> "$OUTPUT_FILE"; echo "" >> "$OUTPUT_FILE" &
  REQUEST_PIDS+=($!)
done

# Wait for requests to complete
for REQUEST_PID in $REQUEST_PIDS
do
  wait $REQUEST_PID
done

# Count number of memes created
NUM_IDS=`cat $OUTPUT_FILE | grep id | wc -l`

if [ $NUM_IDS == 100 ];
then
  echo "SUCCESS...100 memes created"
else
  echo "FAILURE...not all memes were created"
  kill_server
  exit 1
fi

kill_server
exit 0
