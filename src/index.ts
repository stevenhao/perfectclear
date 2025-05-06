import express from 'express';
import bodyParser from 'body-parser';
import path from 'path';
import routes from './routes/routes';

const app = express();
const port = process.env.PORT || 4444;

app.use(bodyParser.json());
app.use(bodyParser.urlencoded({ extended: true }));

app.use((err: Error, req: express.Request, res: express.Response, next: express.NextFunction) => {
  console.error(err);
  res.status(500).send('Server error');
});

app.use(express.static(path.join(__dirname, '../public')));

app.use('/', routes);
app.get('/', (req, res) => {
  res.send('hi');
});

declare global {
  namespace NodeJS {
    interface Global {
      serverStartTime: number;
    }
  }
}
global.serverStartTime = Date.now();

app.listen(port, () => {
  console.log(`Listening on port ${port}`);
});

process.on('SIGINT', () => {
  console.log('Shutting down server...');
  process.exit();
});
