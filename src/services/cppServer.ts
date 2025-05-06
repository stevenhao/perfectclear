import { spawn, ChildProcess } from 'child_process';
import { AiRequest, AiResponse } from '../types';
import * as path from 'path';
import * as fs from 'fs';

const PIPE_PATH = '/tmp/perfectclear_pipe';

export class CppServerService {
  private serverProcess: ChildProcess | null = null;
  private isConnected: boolean = false;
  private lastConnectionTime: number = 0;
  private requestCallbacks: Map<number, (response: any) => void> = new Map();
  private reqId: number = 0;

  constructor() {
    this.ensurePipeExists();
  }

  private ensurePipeExists(): void {
    if (!fs.existsSync(PIPE_PATH)) {
      try {
        require('child_process').execSync(`mkfifo ${PIPE_PATH}`);
      } catch (err) {
        console.error('Failed to create named pipe:', err);
        throw err;
      }
    }
  }

  public start(): void {
    try {
      this.serverProcess = spawn('./server.o', ['pipe'], {
        cwd: process.cwd(),
        stdio: ['pipe', 'pipe', 'pipe']
      });
      
      this.isConnected = true;
      this.lastConnectionTime = Date.now();

      const readStream = fs.createReadStream(PIPE_PATH);
      let buffer = '';

      readStream.on('data', (data) => {
        buffer += data.toString();
        
        let endIndex;
        while ((endIndex = buffer.indexOf('}\n')) !== -1) {
          const message = buffer.substring(0, endIndex + 1);
          buffer = buffer.substring(endIndex + 2);
          
          try {
            const response = JSON.parse(message) as AiResponse;
            const callback = this.requestCallbacks.get(response.reqid);
            if (callback) {
              callback(response);
              this.requestCallbacks.delete(response.reqid);
            }
          } catch (err) {
            console.error('Error parsing response:', err);
          }
        }
      });

      readStream.on('error', (err) => {
        console.error('Error reading from pipe:', err);
        this.isConnected = false;
        this.restart();
      });

      this.serverProcess.on('error', (err) => {
        console.error('C++ server process error:', err);
        this.isConnected = false;
        this.restart();
      });

      this.serverProcess.on('exit', (code) => {
        console.log(`C++ server process exited with code ${code}`);
        this.isConnected = false;
        this.restart();
      });
    } catch (err) {
      console.error('Failed to start C++ server:', err);
      this.isConnected = false;
    }
  }

  public restart(): void {
    if (this.serverProcess) {
      try {
        this.serverProcess.kill();
      } catch (err) {
        console.error('Error killing C++ server process:', err);
      }
    }
    
    setTimeout(() => this.start(), 5000);
  }

  public getStatus(): { connected: boolean; uptime: number } {
    return {
      connected: this.isConnected,
      uptime: this.isConnected ? Date.now() - this.lastConnectionTime : 0
    };
  }

  public sendRequest(request: Omit<AiRequest, 'reqid'>): Promise<any> {
    return new Promise((resolve, reject) => {
      if (!this.isConnected || !this.serverProcess) {
        reject(new Error('C++ server is not connected'));
        return;
      }

      const reqid = ++this.reqId;
      const fullRequest = { ...request, reqid };
      
      this.requestCallbacks.set(reqid, resolve);
      
      try {
        if (!this.serverProcess || !this.serverProcess.stdin) {
          throw new Error('Server process or stdin is not available');
        }
        this.serverProcess.stdin.write(JSON.stringify(fullRequest) + '\n');
      } catch (err) {
        this.requestCallbacks.delete(reqid);
        reject(err);
        this.isConnected = false;
        this.restart();
      }
    });
  }

  public stop(): void {
    if (this.serverProcess) {
      this.serverProcess.kill();
      this.serverProcess = null;
    }
    this.isConnected = false;
  }
}

export const cppServerService = new CppServerService();
