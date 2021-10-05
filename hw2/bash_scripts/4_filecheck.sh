#!/bin/bash

file=Linux
if test -f "$file"; then
	echo "course"
else
	echo "very easy"
	echo "course is easy" > $file
fi
