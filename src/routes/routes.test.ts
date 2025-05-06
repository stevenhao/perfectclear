import request from 'supertest';
import express from 'express';
import bodyParser from 'body-parser';
import routes from './routes';
import { cppServerService } from '../services/cppServer';

jest.mock('../services/cppServer', () => ({
  cppServerService: {
    getStatus: jest.fn(),
    sendRequest: jest.fn()
  }
}));

const app = express();
app.use(bodyParser.json());
app.use(bodyParser.urlencoded({ extended: true }));
app.use('/', routes);

describe('API Routes', () => {
  test('GET /server-status should return server status', async () => {
    const mockStatus = { connected: true, uptime: 1000 };
    (cppServerService.getStatus as jest.Mock).mockReturnValue(mockStatus);
    
    const response = await request(app).get('/server-status');
    
    expect(response.status).toBe(200);
    expect(response.body).toEqual(mockStatus);
  });
  
  test('POST /ai should process AI requests', async () => {
    const mockRequest = {
      board: { W: '10', H: '20', data: '0000000000' },
      pieces: ['I', 'J', 'L']
    };
    
    const mockResponse = {
      body: {
        path: [1, 2, 3],
        happy: 1,
        pathStrings: ['a', 'b', 'c'],
        board: '0000000000'
      }
    };
    
    (cppServerService.sendRequest as jest.Mock).mockResolvedValue(mockResponse);
    
    const response = await request(app)
      .post('/ai')
      .send(mockRequest);
    
    expect(response.status).toBe(200);
    expect(response.body).toEqual(mockResponse.body);
    expect(cppServerService.sendRequest).toHaveBeenCalledWith(expect.objectContaining({
      board: mockRequest.board,
      pieces: mockRequest.pieces
    }));
  });
  
  test('POST /ai should handle errors', async () => {
    (cppServerService.sendRequest as jest.Mock).mockRejectedValue(new Error('Test error'));
    
    const response = await request(app)
      .post('/ai')
      .send({ board: {}, pieces: [] });
    
    expect(response.status).toBe(500);
  });
});
