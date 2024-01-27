# How it works

1. run local server within network (use `server.sh`)
2. run client on different computers within network (run `client.sh`)
_Make sure to edit `client/src/Main.cpp` to the right ip address based on which computer you hosted the server.

# Brainstorm
Client should connect to websocket server. On connect, client receives a cache of most recent messages.

On sending message, it is broadcasted to all connected clients, including sender.

![](https://i.imgur.com/wjnZKju.jpg)

# Todo
- [ ] figure out how to dockerize the server
- [ ] port-forward server through homelab reverse proxy
