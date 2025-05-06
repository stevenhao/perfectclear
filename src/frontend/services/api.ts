export interface ServerStatus {
  connected: boolean;
  uptime: number;
}

export interface AiRequest {
  board: any;
  pieces: string[];
  search_breadth?: number;
}

export interface AiResponse {
  path: number[];
  happy: number;
  pathStrings: string[];
  board: string;
  error?: string;
}

export async function fetchServerStatus(): Promise<ServerStatus> {
  const response = await fetch('/server-status');
  if (!response.ok) {
    throw new Error(`Failed to fetch server status: ${response.status}`);
  }
  return await response.json();
}

export async function requestAiMove(request: AiRequest): Promise<AiResponse> {
  if (window.wasmAiMove) {
    return new Promise((resolve, reject) => {
      window.wasmAiMove(
        request.board,
        request.pieces,
        request.search_breadth || 200,
        (result: AiResponse) => {
          if (result.error) {
            reject(new Error(result.error));
          } else {
            resolve(result);
          }
        }
      );
    });
  }

  const response = await fetch('/ai', {
    method: 'POST',
    headers: {
      'Content-Type': 'application/json'
    },
    body: JSON.stringify(request)
  });

  if (!response.ok) {
    throw new Error(`Failed to fetch AI move: ${response.status}`);
  }
  
  return await response.json();
}
