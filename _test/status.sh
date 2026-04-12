#!/bin/bash
ID=$1
curl -X GET http://127.0.0.1:8080/api/hash/status?requestId=${ID} \
    -H "Content-Type: application/json" \
