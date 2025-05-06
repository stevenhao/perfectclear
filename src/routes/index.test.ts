import request from 'supertest';
import express from 'express';
import bodyParser from 'body-parser';
import routes from './index';

const app = express();
app.use(bodyParser.json());
app.use(bodyParser.urlencoded({ extended: true }));
app.use('/', routes);

describe('API Routes', () => {
});
