import { CppServerService } from './cppServer';
import { spawn } from 'child_process';
import * as fs from 'fs';

jest.mock('child_process', () => ({
  spawn: jest.fn().mockReturnValue({
    on: jest.fn(),
    kill: jest.fn(),
    removeAllListeners: jest.fn(),
    stdin: { write: jest.fn() }
  }),
  execSync: jest.fn()
}));

jest.mock('fs', () => ({
  existsSync: jest.fn(),
  createReadStream: jest.fn().mockReturnValue({
    on: jest.fn(),
    removeAllListeners: jest.fn(),
    destroy: jest.fn()
  })
}));

describe('CppServerService', () => {
  let service: CppServerService;
  
  beforeEach(() => {
    jest.clearAllMocks();
    (fs.existsSync as jest.Mock).mockReturnValue(true);
    service = new CppServerService();
  });
  
  afterEach(() => {
    service.stop();
    jest.clearAllTimers();
  });
  
  test('should start C++ server process with pipe argument', () => {
    service.start();
    expect(spawn).toHaveBeenCalledWith('./server.o', ['pipe'], expect.anything());
  });
  
  test('should return connected status when server is running', () => {
    service.start();
    const status = service.getStatus();
    expect(status.connected).toBe(true);
    expect(status.uptime).toBeGreaterThanOrEqual(0);
  });
  
  test('should handle server process errors and restart', () => {
    const mockOn = jest.fn();
    (spawn as jest.Mock).mockReturnValueOnce({
      on: mockOn,
      kill: jest.fn(),
      removeAllListeners: jest.fn(),
      stdin: { write: jest.fn() }
    });
    
    service.start();
    
    const errorCallback = mockOn.mock.calls.find(call => call[0] === 'error')?.[1];
    if (errorCallback) {
      errorCallback(new Error('Test error'));
      expect(service.getStatus().connected).toBe(false);
    }
  });
  
  test('should properly clean up resources when stopped', () => {
    service.start();
    service.stop();
    
    const mockReadStream = fs.createReadStream('');
    const mockServerProcess = spawn('', []);
    
    expect(mockReadStream.removeAllListeners).toHaveBeenCalled();
    expect(mockReadStream.destroy).toHaveBeenCalled();
    expect(mockServerProcess.removeAllListeners).toHaveBeenCalled();
    expect(mockServerProcess.kill).toHaveBeenCalled();
  });
});
