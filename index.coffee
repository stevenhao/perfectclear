express = require('express')
bodyParser = require('body-parser')

print = console.log.bind(console)
# App initialization
app = express()
app.use(bodyParser.json())
app.use(bodyParser.urlencoded({extended: true}))
port = process.env.PORT or 4444
app.listen(port)
print 'listening on ', port
app.on 'uncaughtException', (err) ->
  error(err);
  print("Node NOT Exiting...");

app.use(express.static(__dirname + '/bower_components/bootstrap/dist'));
app.use(express.static(__dirname + '/bower_components/jquery/dist'));
app.use(express.static(__dirname + '/public'));
app.use(express.static(__dirname + '/public/views'));
app.get '/', (req, res) ->
  res.send('hi')

requests = {}
reqCnt = 0

port = process.argv[2] || 4445
backendHost = process.env.BACKEND_HOST || 'localhost'
net = require('net');
client = null
serverConnected = false
serverUptime = 0

# Function to connect to the backend
connectToBackend = ->
  client = net.createConnection {host: backendHost, port}, -> 
    # 'connect' listener
    console.log('connected to server!')
    serverConnected = true
  
  client.on 'data', (data) ->
    msg = JSON.parse(data.toString())
    print 'sending', msg
    if requests[msg.reqid]
      requests[msg.reqid].send(msg.body)
  
  client.on 'error', (err) ->
    console.log('Connection error:', err.message)
    serverConnected = false
    serverUptime = null
    setTimeout(connectToBackend, 5000)
  
  client.on 'close', ->
    console.log('Connection closed')
    serverConnected = false
    serverUptime = null
    setTimeout(connectToBackend, 5000)

# Initial connection attempt
connectToBackend()

# Server uptime tracking
updateUptime = ->
  if serverConnected && client
    reqCnt += 1
    uptimeReq = {
      type: 'uptime',
      reqid: reqCnt
    }
    requests[reqCnt] = {
      send: (data) ->
        serverUptime = data.uptime_seconds
    }
    try
      client.write(JSON.stringify(uptimeReq))
    catch e
      console.log('Error sending uptime request:', e.message)
      serverUptime = null

# Update uptime every 5 seconds
setInterval(updateUptime, 5000)

app.get '/uptime', (req, res) ->
  if serverConnected && serverUptime != null
    res.json({ uptime_seconds: serverUptime })
  else
    res.json({ status: 'offline' })

app.post '/ai', (req, res) ->
  if req.body?
    msg = req.body
    reqCnt += 1
    msg.reqid = reqCnt # in theory, this is "atomic"
    requests[msg.reqid] = res
    print('/ai: got request', msg)
    msg.search_breadth = msg.search_breadth || 200
    client.write(JSON.stringify(msg));
  else
    res.send('bad request')
