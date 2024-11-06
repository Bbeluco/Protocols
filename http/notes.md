As mentioned in this [link](https://developer.mozilla.org/en-US/docs/Web/HTTP/Overview) from Mozilla, HTTP is basically a protocol used by two entities (usually a server and a client) with the main goal of **fetching resources**.

A HTTP server occurs in parts with the client (also known as **user agent**), requesting resources piece by piece. When the first request finishes, it requests another, and so on. (This could be done async but it depends who the server is set to handle responses).

Many other devices can exist between client and server, these entities are called _proxies_, they aim to perform specific taks, such as: caching, authorization, load balancing, and so on.

***HTTP is stateless, not sessionless***
This means that HTTP has no built-in way to save user's current status in an application. However, it can save some session information by using _cookies_. As HTTP is a protocol primarily focused on serving resources to the user, cookies are used to only be a share information between the server and the client.The client sends its cookie to the server, which retrieves all information related to that cookie and then responds to the client.

***HTTP and connections***
HTTP has the following philosophy: _I need to be transported over a relaiable protocol_. As we all know, we have 2 main protocols in the transport layer, those are: TCP and UDP. By definition, UDP lacks the reliability features required by HTTP, so it's not suitable. TCP, on the other hand, is the better option in this case.

HTTP had a major issue in version HTTP/1.0: every time a resource was requested from the server, HTTP/1.0 would open a knew connection. It was as if each connection was exclusive to a single resource, and once that resource was downloaded, the connection was closed. To address this problem HTTP/1.1 were introduced, bringing the _pipelining_ and _persistent connections_ concepts. While HTTP still has room for improvement, researches are developing HTTP/2.0 to achieve more efficient communication.

***What can be controlled by HTTP?***
Caching, Resource from the same origin (CORS), Authentication (BASIC Auth, WWW-Authenticate), Proxy and tunneling and Sessions

# HTTP Flow
1. Open TCP connection
2. Send an HTTP message
3. Read response back
4. Close (or reuse) connection

***HTTP Messages***
Usually, HTTP messages has the following structure:

_Request_
``` 
GET / HTTP/1.1
HOST: developer.mozilla.org
Accept-Language: Pt-br
```

The first part of the message includes an HTTP **method** (such as GET, POST, etc.), the **path** of the resource to fetch, HTTP version, and headers.

_Response_
``` 
HTTP/1.1 200 OK
date: Tue, 18 Jun 2024 10:03:55 GMT
cache-control: public, max-age=3600
content-type: text/html
```

The response contains the following elements:
HTTP version, status, status message, headers and optional body

# Sockets
An important concept we need to know is **socket**. Socket is essentially an agreemnt on how to communicate with another computer over a network. Using a real-world example, socket is like have a door in a house (house in this context means the process running), your door is your way to the external world, similar to how a socket connects to the network.

-----------

# Coding
(04/11/2024)
To write our code in C we'll need some knowledge of sockets. Since we don't have this knoledge yet we'll start by learning how to implement basic socket communication in C and later we can develop a more robust version.

(05/11/2024)
As we saw in this [link](https://dev.to/jeffreythecoder/how-i-built-a-simple-http-server-from-scratch-using-c-739) we have a way to implement this custom HTTP based on socket approach, tha main steps are as follows:

1. Create a simple socket
2. Improve that socket to handle multiple connections, simulating a real-world scenario
3. Add a "filter" to accept only GET requests

The code in socket_base/server.c was created to provide response headers. When accessed via a browser it's considered a valid response, allowing it to be processed as a normal API response. The client.c code no longer work because it doesn't sent a valid METHOD + PATH + HTTP Version. The easiest way to test server code is by creating a NetCat communication and sending the expected request format.