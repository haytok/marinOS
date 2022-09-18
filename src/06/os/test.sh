FILE=test
if test -f "$FILE"; then
    echo "$FILE exists,so delete this file."
    rm "$FILE"
fi

gcc -Wall -g -o test test.c
./test
