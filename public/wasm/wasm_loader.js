let wasmModule = null;
let wasmLoaded = false;

async function initWasmModule() {
  if (wasmLoaded) return wasmModule;
  
  return new Promise((resolve, reject) => {
    const script = document.createElement('script');
    script.src = '/wasm/perfectclear.js';
    script.onload = () => {
      window.Module = {
        onRuntimeInitialized: async function() {
          try {
            await setupFileSystem(Module);
            Module.loadWasmData();
            wasmModule = Module;
            wasmLoaded = true;
            console.log("WASM module loaded successfully");
            
            window.addEventListener('beforeunload', () => {
              if (wasmModule) {
                try {
                  wasmModule.unloadWasmData();
                  console.log("WASM module unloaded");
                } catch (e) {
                  console.error("Error unloading WASM module:", e);
                }
              }
            });
            
            resolve(wasmModule);
          } catch (error) {
            console.error("Error initializing WASM module:", error);
            reject(error);
          }
        },
        print: (text) => console.log(text),
        printErr: (text) => console.error(text)
      };
    };
    script.onerror = reject;
    document.body.appendChild(script);
  });
}

async function callWasmEngine(data) {
  if (!wasmLoaded) {
    await initWasmModule();
  }
  
  const jsonString = JSON.stringify(data);
  const result = wasmModule.handleWasmRequest(jsonString);
  return JSON.parse(result);
}
