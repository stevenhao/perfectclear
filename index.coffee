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

# All logic is now in WASM, no server endpoints needed
