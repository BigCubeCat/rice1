#!/bin/bash
curl -X POST http://127.0.0.1:8080/api/hash/crack \
     -H "Content-Type: application/json" \
     -d @test_request.json
