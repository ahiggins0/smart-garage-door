#!/usr/bin/env bash

export PATH="/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/local/games:/usr/games"


STATUS=$(curl -s 'http://192.168.0.15:8080/status' | jq -r '.status')

if [[ $STATUS == *"open"* ]]; then
  curl -s 'http://192.168.0.15:8080/trigger'
fi