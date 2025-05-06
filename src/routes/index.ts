import express from 'express';
import { AiRequest } from '../types';

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

export default router;
