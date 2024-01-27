# How it works

1. run local server within network
2. run client on different computers within network

# Brainstorm
Client should connect to websocket server. On connect, client receives a cache of most recent messages.

On sending message, it is broadcasted to all connected clients, including sender.

