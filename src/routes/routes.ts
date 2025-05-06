import express from 'express';
import path from 'path';
import fs from 'fs';
import { cppServerService } from '../services/cppServer';

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
  } catch (err) {
    console.error('Error in request handling:', err);
    res.status(500).send('Server error');
  }
});

router.get('/zen', (req, res) => {
  res.sendFile(path.join(__dirname, '../../public/zen.html'));
});

router.get('/:page', (req, res, next) => {
  const page = req.params.page;
  const htmlFilePath = path.join(__dirname, `../../public/${page}.html`);
  
  fs.access(htmlFilePath, fs.constants.F_OK, (err) => {
    if (err) {
      return next();
    }
    res.sendFile(htmlFilePath);
  });
});

export default router;
