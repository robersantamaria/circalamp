#!/bin/bash

export LC_ALL=C.UTF-8
export LANG=C.UTF-8
export FLASK_APP=mocks.py

echo "Starting mocks.py"
while true; do
  flask run --host=0.0.0.0 &
  PID=$!
  inotifywait -e modify -e move -e create -e delete -e attrib -r . --exclude '.*\.pyc.*' > /dev/null 2>&1
  echo "Restarting mocks.py"
  kill $PID 
done
