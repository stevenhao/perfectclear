import express from 'express';
import path from 'path';
import fs from 'fs';

const router = express.Router();

router.get('/server-status', (req, res) => {
  res.json({ 
    connected: true, 
    uptime: Date.now() 
  });
});

router.post('/ai', (req, res) => {
  res.status(400).json({
    error: 'Server-side AI is no longer supported. Please use the WebAssembly implementation.'
  });
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
