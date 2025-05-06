interface Window {
  wasmAiMove: (
    board: any,
    pieces: string[],
    searchBreadth: number,
    callback: (result: any) => void
  ) => void;
  loadWasmModule: () => Promise<any>;
  zenMode?: boolean;
  Module?: any;
}
