#!/bin/bash

I=0
while [[ I -lt $1 ]] 
do
	curl --location --request GET 'http://localhost:8080/g-series-15-3579-laptop_service-manual_en-us.pdf' > /dev/null &
	I=$((I + 1));
done
