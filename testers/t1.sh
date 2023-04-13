#!/bin/bash

if [[ "$1" == "D" ]]; then
    set -x
fi

SERVER="localhost"
PORT="6667"
CLIENT1_NICK="client_1"
CLIENT2_NICK="client_2"
PASSWORD="password"

#\nc $SERVER $PORT | tee /dev/fd/4 | tee /dev/fd/5 &
nc $SERVER $PORT > >(tee >(cat >&2) >&3) 3> >(tee >(cat >&2)) &

echo "PASS $PASSWORD" >&4
echo "USER userName_1 main@mail.edu test.com :myFakeName" >&4
echo "NICK $CLIENT1_NICK" >&4

echo "PASS $PASSWORD" >&5
echo "USER userName_2 new@mail.net real.com :myRealName" >&5
echo "NICK $CLIENT2_NICK" >&5

sleep 5

echo "JOIN #test" >&4
echo "JOIN #test" >&5
echo "PRIVMSG #test :Hello, world!" >&4

kill %1

