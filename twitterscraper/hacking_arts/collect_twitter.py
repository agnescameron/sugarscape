from tweetfeels import TweetFeels

consumer_key = '3bBN3chrXoZgoe5tG4E942BA7'
consumer_secret = '1Kx9SUZb64bUEz4u8mMA0ncmHk1bMHESbiVfYhm1WlQvQprVb2'
access_token = '1908392192-sGDAihgfGOO4A80DFzjEUK39gciz6C9Fdo1k5yE'
access_token_secret = 'ASejV2ZBOfFK8Ox623Dg8e9q1BgYDSTg83jVd7pfB1XcN'

login = [consumer_key, consumer_secret, access_token, access_token_secret]

trump_feels = TweetFeels(login, tracking=['trump'])


from threading import Thread
import time

def print_feels(seconds=10):
    while go_on:
        time.sleep(seconds)
        print(f'[{time.ctime()}] Sentiment Score: {trump_feels.sentiment.value}')


go_on = True
t = Thread(target=print_feels)
trump_feels.start()
t.start()
