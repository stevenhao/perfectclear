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

net = require('net');
client = net.createConnection {port: 4448}, -> 
  #'connect' listener
  console.log('connected to server!');

client.on 'data', (data) ->
  print 'got data', data.toString()

app.post '/ai', (req, res) ->
  if req.body?
    reqCnt += 1
    reqid = reqCnt # in theory, this is "atomic"
    requests[reqid] = res
    print('/ai: got request', req.body)
    client.write(JSON.stringify(req.body));
  else
    res.send('bad request')


module.exports = app
