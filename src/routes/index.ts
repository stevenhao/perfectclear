import express from 'express';
import { cppServerService } from '../services/cppServer';
import { AiRequest } from '../types';

const router = express.Router();

router.get('/server-status', (req, res) => {
  res.json(cppServerService.getStatus());
});

router.post('/ai', (req, res) => {
  if (!req.body) {
    return res.send('bad request');
  }

  try {
    const msg = req.body;
    cppServerService.sendRequest({
      board: msg.board,
      pieces: msg.pieces,
      search_breadth: msg.search_breadth || 200
    }).then(response => {
      res.send(response.body);
    }).catch(err => {
      console.error('Error processing AI request:', err);
      res.status(500).send('Server error');
    });
    return;
  } catch (err) {
    console.error('Error in request handling:', err);
    res.status(500).send('Server error');
  }
});

export default router;
