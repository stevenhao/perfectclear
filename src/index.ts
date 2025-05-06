import express from 'express';
import bodyParser from 'body-parser';
import path from 'path';
import routes from './routes/routes';
import { cppServerService } from './services/cppServer';

const app = express();
const port = process.env.PORT || 4444;

app.use(bodyParser.json());
app.use(bodyParser.urlencoded({ extended: true }));

app.use((err: Error, req: express.Request, res: express.Response, next: express.NextFunction) => {
  console.error(err);
  res.status(500).send('Server error');
});

if (process.env.NODE_ENV === 'production') {
  app.use(express.static(path.join(__dirname, '../dist')));
} else {
  app.use(express.static(path.join(__dirname, '../public')));
}

app.use('/', routes);

if (process.env.NODE_ENV === 'production') {
  app.get('*', (req, res) => {
    res.sendFile(path.join(__dirname, '../dist/index.html'));
  });
}

cppServerService.start();

app.listen(port, () => {
  console.log(`Listening on port ${port}`);
});

process.on('SIGINT', () => {
  console.log('Shutting down servers...');
  cppServerService.stop();
  process.exit();
});
