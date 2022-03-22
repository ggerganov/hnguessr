# HNGuessr

A daily game where you try to guess the top [Hacker News](https://news.ycombinator.com/news) story titles from yesterday.

Play online: https://hnguessr.ggerganov.com

![Peek 2022-03-22 20-36](https://user-images.githubusercontent.com/1991296/159552341-1f8abf8d-8eb2-4eca-8cde-9bc8aa4787b9.gif)

## Details

The stories are updated once per day at 12:00 UTC by fetching the top stories from the HN API. There is a 24 hours
moderation period where I choose which words are going to be unknown for the next round. In case I am unavailable during
this period, a random set of words will be selected. At 12:00 UTC the next day, the stories are published and available
for everyone to play.

If you want to play past rounds, you can use the following URL:

https://hnguessr.ggerganov.com/1/

Replace the "1" with the round that you would like to replay.
