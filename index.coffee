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

# No backend server required anymore - all logic is in WASM
serverStatus = {
  isConnected: true,
  startTime: Date.now(),
  lastConnectionTime: Date.now()
}

# Add server status endpoint for backward compatibility
app.get '/server-status', (req, res) ->
  status = {
    connected: true,
    uptime: Date.now() - serverStatus.startTime
  }
  res.json(status)

app.post '/ai', (req, res) ->
  # Server is no longer supported, AI computations should be done via WASM in the browser
  res.status(400).json({
    error: 'Server-side AI is no longer supported. Please use the WebAssembly implementation.'
  })

# Server status endpoint already defined above
