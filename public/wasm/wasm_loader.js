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
            resolve(wasmModule);
          } catch (error) {
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
