#!/bin/bash

if [ $# -eq 0 ]; then
    echo "Usage: $0 <number of items>"
    exit 1
fi

if ! [[ $1 =~ ^[0-9]+$ ]]; then
    echo "Usage: $0 <number of items>"
    exit 1
fi

if [ $1 -lt 1 ]; then
    echo "Usage: $0 <number of items>"
    exit 1
fi

count=$1

items=$(curl -s https://hacker-news.firebaseio.com/v0/topstories.json?print=pretty | jq -c '.[]' - )

# get days since epoch
timestamp=$(date +%s)
days=$((timestamp / 86400))

echo $timestamp
echo $days

for i in $items ; do
    cur=$(curl -s https://hacker-news.firebaseio.com/v0/item/$i.json)

    echo $cur | jq -r .by
    echo $cur | jq -r .descendants
    echo $cur | jq -r .id
    echo $cur | jq -r .score
    echo $cur | jq -r .time
    echo $cur | jq -r .title
    echo $cur | jq -r .url

    count=$((count-1))
    if [ $count -eq 0 ] ; then
        break
    fi
done
