import { defineConfig } from 'vite';
import react from '@vitejs/plugin-react';
import { resolve } from 'path';

export default defineConfig({
  plugins: [react()],
  build: {
    outDir: 'dist',
    emptyOutDir: true,
    sourcemap: true,
  },
  resolve: {
    alias: {
      '@': resolve(__dirname, './src')
    }
  },
  optimizeDeps: {
    exclude: ['perfectclear-wasm']
  },
  server: {
    proxy: {
      '/ai': {
        target: 'http://localhost:4444',
        changeOrigin: true
      },
      '/server-status': {
        target: 'http://localhost:4444',
        changeOrigin: true
      }
    }
  }
});
