export interface ServerStatus {
  connected: boolean;
  uptime: number;
}

export interface AiRequest {
  board: {
    W: string;
    H: string;
    data: string;
  };
  pieces: string[];
  reqid: number;
  search_breadth?: number;
}

export interface AiResponse {
  body: {
    path: number[];
    happy: number;
    pathStrings: string[];
    board: string;
  };
  reqid: number;
}
