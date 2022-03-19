#!/bin/bash

cd $(dirname $0)
cd data

if [ -f stories.txt ] ; then
    day=$(head -n 2 stories.txt | tail -n 1)
    issue=$(($day - 19070))

    echo "Updating web pages for day $day, issue $issue"

    cp -v top5  /var/www/html/hnguessr/
    cp -v top10 /var/www/html/hnguessr/
    cp -v top20 /var/www/html/hnguessr/

    mkdir /var/www/html/hnguessr/$issue
    cp -v top5  /var/www/html/hnguessr/$issue/
    cp -v top10 /var/www/html/hnguessr/$issue/
    cp -v top20 /var/www/html/hnguessr/$issue/
fi

echo "Fetching HN API ..."

../get-top.sh 20 > stories.txt

day=$(head -n 2 stories.txt | tail -n 1)
cp stories.txt stories-${day}.txt

echo "Got stories for day $day"
