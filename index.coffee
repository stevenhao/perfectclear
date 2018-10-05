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
net = require('net');
client = net.createConnection {port}, -> 
  # 'connect' listener
  console.log('connected to server!');

client.on 'data', (data) ->
  msg = JSON.parse(data.toString())
  print 'sending', msg
  requests[msg.reqid].send(msg.body)

app.post '/security', (req, res) ->
  print('got post on /security', req.body)
  res.send('lol hi')

app.post '/ai', (req, res) ->
  if req.body?
    msg = req.body
    reqCnt += 1
    msg.reqid = reqCnt # in theory, this is "atomic"
    requests[msg.reqid] = res
    print('/ai: got request', msg)
    client.write(JSON.stringify(msg));
  else
    res.send('bad request')


module.exports = app
