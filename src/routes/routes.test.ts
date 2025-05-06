import request from 'supertest';
import express from 'express';
import bodyParser from 'body-parser';
import routes from './routes';

const app = express();
app.use(bodyParser.json());
app.use(bodyParser.urlencoded({ extended: true }));
app.use('/', routes);

describe('API Routes', () => {
  test('GET /zen should return zen page', async () => {
    const response = await request(app).get('/zen');
    expect(response.status).toBe(200);
  });
});
