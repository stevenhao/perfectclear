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

app.get '/zen', (req, res) ->
  res.sendFile(__dirname + '/public/zen.html')
  
app.get '/debug', (req, res) ->
  res.sendFile(__dirname + '/public/debug.html')

# General route handler for any HTML file without the .html extension
fs = require('fs')
path = require('path')
app.get '/:page', (req, res, next) ->
  page = req.params.page
  htmlFilePath = path.join(__dirname, "public/#{page}.html")
  
  # Check if the HTML file exists
  fs.access htmlFilePath, fs.constants.F_OK, (err) ->
    if err
      # File doesn't exist, pass to the next middleware
      return next()
    # File exists, send it
    res.sendFile(htmlFilePath)

requests = {}
reqCnt = 0

port = process.argv[2] || 4445
backendHost = process.env.BACKEND_HOST || 'localhost'
net = require('net');
serverStatus = {
  isConnected: true,
  startTime: Date.now(),
  lastConnectionTime: Date.now()
}

client = net.createConnection {host: backendHost, port}, -> 
  # 'connect' listener
  console.log('connected to server!');
  serverStatus.isConnected = true
  serverStatus.lastConnectionTime = Date.now()

client.on 'data', (data) ->
  msg = JSON.parse(data.toString())
  print 'sending', msg
  requests[msg.reqid].send(msg.body)

# Add error and close event handlers
client.on 'error', (err) ->
  console.error('Connection error:', err)
  serverStatus.isConnected = false

client.on 'close', ->
  console.log('Connection to server closed')
  serverStatus.isConnected = false

# Add a reconnection mechanism
reconnectInterval = 5000 # 5 seconds
setInterval ->
  if !serverStatus.isConnected
    console.log('Attempting to reconnect...')
    try
      newClient = net.createConnection {host: backendHost, port}, -> 
        console.log('Reconnected to server!')
        client = newClient
        serverStatus.isConnected = true
        serverStatus.lastConnectionTime = Date.now()
        
        # Set up the data handler again
        client.on 'data', (data) ->
          try
            msg = JSON.parse(data.toString())
            print 'sending', msg
            if requests[msg.reqid]
              requests[msg.reqid].send(msg.body)
          catch e
            console.error('Error processing data:', e)
        
        # Set up error handlers again
        client.on 'error', (err) ->
          console.error('Connection error:', err)
          serverStatus.isConnected = false
        
        client.on 'close', ->
          console.log('Connection to server closed')
          serverStatus.isConnected = false
      
      newClient.on 'error', (err) ->
        console.error('Reconnection error:', err)
        # Don't crash on reconnection errors
    catch e
      console.error('Error during reconnection attempt:', e)
, reconnectInterval

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

# Add server status endpoint
app.get '/server-status', (req, res) ->
  status = {
    connected: serverStatus.isConnected,
    uptime: if serverStatus.isConnected then Date.now() - serverStatus.lastConnectionTime else 0
  }
  res.json(status)
